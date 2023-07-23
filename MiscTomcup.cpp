#include "VapourSynth4.h"
#include <memory>
#include <set>
#include <string>
#include <filesystem>
#include "csv.hpp"

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
	}
};


static const VSFrame* VS_CC filterGetFrame(int n, int activationReason, void* instanceData, void** frameData, VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi) {
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

static void VS_CC filterFree(void* instanceData, VSCore* core, const VSAPI* vsapi) {
	delete reinterpret_cast<SCDetectData*>(instanceData);
}

static void VS_CC filterCreate(const VSMap* in, VSMap* out, void* userData, VSCore* core, const VSAPI* vsapi) {
	auto data{ std::make_unique<SCDetectData>(vsapi) };

	data->node = vsapi->mapGetNode(in, "clip", 0, nullptr);

	const std::string_view scfile{vsapi->mapGetData(in, "scfile", 0, nullptr)};
	std::filesystem::path scfilepath{scfile};
	if (scfilepath.is_relative())
		scfilepath = std::filesystem::path(vsapi->getPluginPath(vsapi->getPluginByID("com.tomcup.SCDetect", core))).remove_filename().append(scfile);
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
	vsapi->createVideoFilter(out, "SCDetect", vi, filterGetFrame, filterFree, fmParallel, deps, 1, data.release(), core);
}

VS_EXTERNAL_API(void) VapourSynthPluginInit2(VSPlugin* plugin, const VSPLUGINAPI* vspapi) {
	vspapi->configPlugin("com.tomcup.SCDetect", "tomcup", "Mark the clip with PySceneDetect's output", VS_MAKE_VERSION(1, 1), VAPOURSYNTH_API_VERSION, 0, plugin);
	vspapi->registerFunction("SCDetect", "clip:vnode;scfile:data", "clip:vnode;", filterCreate, 0, plugin);
}