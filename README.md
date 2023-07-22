# vs-misc-tomcup
`PySceneDetect` v0.6.1

`VapourSynthx64` R63

# com.tomcup.SCDetect
项目受到 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 启发

专门对 [VapourSynth-RIFE-ncnn-Vulkan](https://github.com/HomeOfVapourSynthEvolution/VapourSynth-RIFE-ncnn-Vulkan) 中转场功能的改进，所以仅仅实现了  [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete)  中 `SCDetect` 函数的半边（`_SceneChangeNext`）

由于 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 中的 `SCDetect` 方法被 [VapourSynth-RIFE-ncnn-Vulkan](https://github.com/HomeOfVapourSynthEvolution/VapourSynth-RIFE-ncnn-Vulkan) 所使用，但其转场识别效果不理想。故使用 `PySceneDetect` 进行转场识别，根据其输出模仿 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 中的 `SCDetect` 方法对视频转场进行标识。

## 代码解释
1. 在该项目实现的过程中，由于 `PySceneDetect` 尚未发布正式版（v1），故该项目可能会在未来（您的现在）无法正常读取其输出的 `csv` 文件。
2. 根据实验，`VapourSynthx64` R63 以第 0 帧作为起始帧，`PySceneDetect` v0.6.1 以第 1 帧作为起始帧，故在代码实现中要对 `PySceneDetect` 的输出进行减 1 操作。 

# 感谢以下项目
[csv-parser](https://github.com/vincentlaucsb/csv-parser)

# 备注
开发者不积极维护此项目，如果发现问题，很可能无法及时回复。

如需联系，请使用邮箱。开发者多半会在节假日进行回复。（原因略）
