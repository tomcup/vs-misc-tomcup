#include "VapourSynth4.h"
#include "VSHelper4.h"
#include <memory>
#include <vector>
#include "csv.hpp"

struct SCDetectData {
private:
    const VSAPI* vsapi;
public:
    VSNode* node;
    std::vector<int> scdata;

    explicit SCDetectData(const VSAPI* vsapi) noexcept : vsapi(vsapi) {

    }

    ~SCDetectData() {
        vsapi->freeNode(node);
    }
};


static const VSFrame* VS_CC filterGetFrame(int n, int activationReason, void* instanceData, void** frameData, VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi) {
    SCDetectData* d = (SCDetectData*)instanceData;

    if (activationReason == arInitial) {
        vsapi->requestFrameFilter(n, d->node, frameCtx);
    }
    else if (activationReason == arAllFramesReady) {
        const VSFrame* frame = vsapi->getFrameFilter(n, d->node, frameCtx);

        /* your code here... */

        return frame;
    }

    return nullptr;
}

static void VS_CC filterFree(void* instanceData, VSCore* core, const VSAPI* vsapi) {
    delete reinterpret_cast<SCDetectData*>(instanceData);
}

static void VS_CC filterCreate(const VSMap* in, VSMap* out, void* userData, VSCore* core, const VSAPI* vsapi) {
    auto data{ std::make_unique<SCDetectData>(vsapi) };

    data->node = vsapi->mapGetNode(in, "clip", 0, nullptr);

    csv::CSVReader reader(vsapi->mapGetData(in, "scfile", 0, nullptr));
    for (csv::CSVRow& row : reader) {
        data->scdata.push_back(row["Start Frame"].get<int>());
    }

    VSFilterDependency deps[] = { {data->node, rpGeneral} }; /* Depending the the request patterns you may want to change this */
    vsapi->createVideoFilter(out, "SCDetect", vsapi->getVideoInfo(data->node), filterGetFrame, filterFree, fmParallel, deps, 1, data.release(), core);
}

//////////////////////////////////////////
// Init

VS_EXTERNAL_API(void) VapourSynthPluginInit2(VSPlugin* plugin, const VSPLUGINAPI* vspapi) {
    vspapi->configPlugin("com.tomcup.SCDetect", "misc", "Mark the clip with PySceneDetect's output", VS_MAKE_VERSION(0, 1), VAPOURSYNTH_API_VERSION, 0, plugin);
    vspapi->registerFunction("SCDetect", "clip:vnode;scfile:data[]", "clip:vnode;", filterCreate, nullptr, plugin);
}