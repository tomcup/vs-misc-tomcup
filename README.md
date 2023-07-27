# vs-misc-tomcup
`PySceneDetect` v0.6.1

`VapourSynthx64` R63

# com.tomcup.misc
项目受到 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 启发

## SCDetect
由于 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 中的 `SCDetect` 方法被 [VapourSynth-RIFE-ncnn-Vulkan](https://github.com/HomeOfVapourSynthEvolution/VapourSynth-RIFE-ncnn-Vulkan) 所使用，但其转场识别效果不理想。故使用 `PySceneDetect` 进行转场识别，根据其输出模仿 [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete) 中的 `SCDetect` 方法对视频转场进行标识。

专门对 [VapourSynth-RIFE-ncnn-Vulkan](https://github.com/HomeOfVapourSynthEvolution/VapourSynth-RIFE-ncnn-Vulkan) 中转场功能的改进，故 `SCDetect` 函数仅仅实现了  [vs-miscfilters-obsolete](https://github.com/vapoursynth/vs-miscfilters-obsolete)  中 `SCDetect` 函数的半边（`_SceneChangeNext`）

## SCProcess
由于自家 GPU 速度不行，而使用 `SCDetect` 函数时进行长时间插帧无法暂停，故专为 24 fps 转 60 fps 情况进行“后转场处理”。即对一个使用 `RIFE` 插件插完帧的视频进行后期转场处理。
1. 将 插帧后 转场的前帧 及 其后第一帧 改为 原视频转场的 前帧，将 插帧后 转场的前帧 的后第二帧 改为 原视频转场的 前帧。
2. 将 插帧后 转场的后帧 及 其前第一帧 改为 原视频转场的 后帧，将 插帧后 转场的后帧 的前第二帧 改为 原视频转场的 前帧。

### 运算流程
经测试，`RIFE` 插件进行 24 -> 60 fps 时，先将视频插帧至 120 fps，再将其降至 60 fps。转场处理时直接将转场前一帧重复 2 次输出。

经计算，原 24 fps 视频中序列（以 0 帧作为起始帧）为 **偶数** 的帧被保留，奇数帧在降帧时被删除。

遍历 `PySceneDetect` 中 `End Frame` 数据，遇偶数帧则 *2.5即为插帧后转场的前帧，遇奇数帧则 +1 再 *2.5 即为插帧后转场的后帧。

## 代码解释
1. 在该项目实现的过程中，由于 `PySceneDetect` 尚未发布正式版（v1），故该项目可能会在未来（您的现在）无法正常读取其输出的 `csv` 文件。
2. 经实验，`VapourSynthx64` R63 以第 0 帧作为起始帧，`PySceneDetect` v0.6.1 以第 1 帧作为起始帧，故在代码实现中要对 `PySceneDetect` 的输出进行减 1 操作。 

# 感谢以下项目
- [PySceneDetect](https://github.com/Breakthrough/PySceneDetect)
- [VapourSynth](http://www.vapoursynth.com/)
- [csv-parser](https://github.com/vincentlaucsb/csv-parser)
