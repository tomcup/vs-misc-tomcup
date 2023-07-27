#include "VapourSynth4.h"
#include <memory>
#include <set>
#include <map>
#include <string>
#include <filesystem>
#include "csv.hpp"

namespace tomcup::plugin::sc {
	static std::string identifier{"com.tomcup.misc"};
}

namespace tcp = tomcup::plugin;

struct SCDetectData {
private:
	const VSAPI* vsapi;
public:
	VSNode* node{ nullptr };
	std::set<int> scdata;

	explicit SCDetectData(const VSAPI* vsapi) noexcept : vsapi(vsapi) {

	}

	~SCDetectData() {
		vsapi->freeNode(node);
		scdata.clear();
	}
};

struct SCProcessData {
private:
	const VSAPI* vsapi;
public:
	VSNode* originNode{ nullptr };
	VSNode* processedNode{ nullptr };
	std::map<int, int> scdata;

	explicit SCProcessData(const VSAPI* vsapi) noexcept : vsapi(vsapi) {

	}

	~SCProcessData() {
		vsapi->freeNode(originNode);
		vsapi->freeNode(processedNode);
		scdata.clear();
	}

};

template<typename T>
static void VS_CC filterFree(void* instanceData, VSCore* core, const VSAPI* vsapi) {
	delete reinterpret_cast<T*>(instanceData);
}

static const VSFrame* VS_CC SCDetectGetFrame(int n, int activationReason, void* instanceData, void** frameData, VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi) {
	SCDetectData* d = static_cast<SCDetectData*>(instanceData);

	if (activationReason == arInitial) {
		vsapi->requestFrameFilter(n, d->node, frameCtx);
	}
	else if (activationReason == arAllFramesReady) {
		const VSFrame* frame = vsapi->getFrameFilter(n, d->node, frameCtx);

		VSFrame* dst = vsapi->copyFrame(frame, core);
		VSMap* rwprops = vsapi->getFramePropertiesRW(dst);
		vsapi->mapSetInt(rwprops, "_SceneChangeNext", d->scdata.contains(n), maReplace);

		vsapi->freeFrame(frame);

		return dst;
	}

	return nullptr;
}

static void VS_CC SCDetectCreate(const VSMap* in, VSMap* out, void* userData, VSCore* core, const VSAPI* vsapi) {
	auto data{ std::make_unique<SCDetectData>(vsapi) };

	data->node = vsapi->mapGetNode(in, "clip", 0, nullptr);

	const std::string_view scfile{vsapi->mapGetData(in, "scfile", 0, nullptr)};
	std::filesystem::path scfilepath{scfile};
	if (scfilepath.is_relative())
		scfilepath = std::filesystem::path(vsapi->getPluginPath(vsapi->getPluginByID(tcp::sc::identifier.c_str(), core))).remove_filename().append(scfile);
	const std::filesystem::directory_entry scfileEntry{scfilepath};

	const VSVideoInfo* vi = vsapi->getVideoInfo(data->node);
	try {
		if (!scfileEntry.exists())
			throw std::runtime_error(std::string("target scfile not exists: ") + scfilepath.generic_string());
		csv::CSVReader reader(scfilepath.generic_string());
		for (csv::CSVRow& row : reader) {
			data->scdata.insert(row["End Frame"].get<int>() - 1);
		}
	}
	catch (const std::runtime_error& e) {
		vsapi->mapSetError(out, (std::string("SCDetect: ") + e.what()).c_str());
		return;
	}

	VSFilterDependency deps[] = { {data->node, rpGeneral} };
	vsapi->createVideoFilter(out, "SCDetect", vi, SCDetectGetFrame, filterFree<SCDetectData>, fmParallel, deps, 1, data.release(), core);
}

static const VSFrame* VS_CC SCProcessGetFrame(int n, int activationReason, void* instanceData, void** frameData, VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi) {
	SCProcessData* d = static_cast<SCProcessData*>(instanceData);

	auto it{ d->scdata.find(n) };
	if (it == d->scdata.end()) {
		if (activationReason == arInitial) {
			vsapi->requestFrameFilter(n, d->processedNode, frameCtx);
		}
		else if (activationReason == arAllFramesReady) {
			return vsapi->getFrameFilter(n, d->processedNode, frameCtx);
		}
	}
	else {
		if (activationReason == arInitial) {
			vsapi->requestFrameFilter(it->second, d->originNode, frameCtx);
		}
		else if (activationReason == arAllFramesReady) {
			return vsapi->getFrameFilter(it->second, d->originNode, frameCtx);
		}
	}

	return nullptr;
}

static void VS_CC SCProcessCreate(const VSMap* in, VSMap* out, void* userData, VSCore* core, const VSAPI* vsapi) {
	auto data{ std::make_unique<SCProcessData>(vsapi) };

	data->originNode = vsapi->mapGetNode(in, "originClip", 0, nullptr);
	data->processedNode = vsapi->mapGetNode(in, "processedClip", 0, nullptr);

	const VSVideoInfo* vi{ vsapi->getVideoInfo(data->processedNode) };

	const std::string_view scfile{vsapi->mapGetData(in, "originscfile", 0, nullptr)};
	std::filesystem::path scfilepath{scfile};
	if (scfilepath.is_relative())
		scfilepath = std::filesystem::path(vsapi->getPluginPath(vsapi->getPluginByID(tcp::sc::identifier.c_str(), core))).remove_filename().append(scfile);
	const std::filesystem::directory_entry scfileEntry{scfilepath};

	try {
		const VSVideoInfo* _vi{ vsapi->getVideoInfo(data->originNode) };
		if (_vi->height != vi->height && _vi->width != vi->width) {
			throw std::runtime_error("height or width is different between origin and processed video!");
		};

		if (!scfileEntry.exists())
			throw std::runtime_error(std::string("target scfile not exists: ") + scfilepath.generic_string());
		csv::CSVReader reader(scfilepath.generic_string());
		for (csv::CSVRow& row : reader) {
			int target{ row["End Frame"].get<int>() - 1 };
			if (target % 2 == 0) {
				int _target{ static_cast<int>(target * 2.5f) };
				data->scdata.emplace(_target, target);
				data->scdata.emplace(_target + 1, target);
				data->scdata.emplace(_target + 2, target + 1);
			}
			else {
				int _target{ static_cast<int>((target + 1) * 2.5f) };
				data->scdata.emplace(_target, target + 1);
				data->scdata.emplace(_target - 1, target + 1);
				data->scdata.emplace(_target - 2, target);
			}
		}
	}
	catch (const std::runtime_error& e) {
		vsapi->mapSetError(out, (std::string("SCProcess: ") + e.what()).c_str());
		return;
	}

	VSFilterDependency deps[] = { {data->originNode, rpGeneral}, {data->processedNode, rpGeneral} };
	vsapi->createVideoFilter(out, "SCProcess", vi, SCProcessGetFrame, filterFree<SCProcessData>, fmParallel, deps, 2, data.release(), core);
}

VS_EXTERNAL_API(void) VapourSynthPluginInit2(VSPlugin* plugin, const VSPLUGINAPI* vspapi) {
	vspapi->configPlugin(tcp::sc::identifier.c_str(), "tomcup", "Mark the clip with PySceneDetect's output", VS_MAKE_VERSION(1, 1), VAPOURSYNTH_API_VERSION, 0, plugin);
	vspapi->registerFunction("SCDetect", "clip:vnode;scfile:data", "clip:vnode;", SCDetectCreate, nullptr, plugin);
	vspapi->registerFunction("SCProcess", "originClip:vnode;processedClip:vnode;originscfile:data;", "clip:vnode;", SCProcessCreate, nullptr, plugin);
}