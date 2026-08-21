# 离线数据分析工具

本目录包含两个独立的数据处理工具：

| 可执行文件 | 功能 | 输入 | 输出 |
|---|---|---|---|
| `GenerateWaveform` | 将光学模拟输出的 root 文件转换为含波形信息的 root 文件 | 模拟输出 `*.root` | `<输入名>_withwaveform.root` |
| `FitWaveform` | 拟合波形，得到时间、能量信息 | `<输入名>_withwaveform.root` | `fitresult.root`、`time_resolution.png` |

## 编译

在 `build/` 目录下执行：

```bash
cmake ..
make GenerateWaveform FitWaveform
```

cmake 会把本目录源码拷贝到 `build/analysis/`，可执行文件生成在 `build/` 下。

## 使用

输入文件支持相对路径和绝对路径，输出文件与输入文件位于同一目录。

```bash
# 1. 波形生成
./GenerateWaveform /path/to/ecal.root
# 生成 /path/to/ecal_withwaveform.root

# 2. 波形拟合
./FitWaveform /path/to/ecal_withwaveform.root
# 生成 /path/to/fitresult.root 和 /path/to/time_resolution.png
```

## 文件说明

- `common.h`：公共数据结构 `DATA_STRUCT`，与模拟输出 root 文件 `ecal` 树的分支对应。
- `GenerateWaveform.cpp`：`ConvoluteIRF`（光子时间分布与探测器响应卷积）、`add_noise`（叠加电子学噪声）、`convert_waveform`（主流程）。
- `FitWaveform.cpp`：`ReadTree`（读取 `ecal` 树）、`fit_single_waveform`（单波形参数化拟合）、`fit_waveform`（主流程，输出时间分辨图与拟合结果树）。