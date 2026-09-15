# 离线数据分析工具与批量数据处理流程

本目录包含两个独立的数据分析工具，以及一套完整的批量数据生产-合并-分析流程脚本。

## 一、数据分析工具

| 可执行文件 | 功能 | 输入 | 输出 |
|---|---|---|---|
| `GenerateWaveform` | 将光学模拟输出的 root 文件转换为含波形信息的 root 文件 | 模拟输出 `*.root` | `<输入名>_withwaveform.root` |
| `FitWaveform` | 拟合波形，得到时间、能量信息 | `<输入名>_withwaveform.root` | `fitresult.root`、`time_resolution.png` |

### 编译

在 `build/` 目录下执行：

```bash
cmake ..
make GenerateWaveform FitWaveform
```

cmake 会把本目录源码拷贝到 `build/analysis/`，可执行文件生成在 `build/` 下。

### 使用

输入文件支持相对路径和绝对路径，输出文件与输入文件位于同一目录。

```bash
# 1. 波形生成
./GenerateWaveform /path/to/ecal.root
# 生成 /path/to/ecal_withwaveform.root

# 2. 波形拟合
./FitWaveform /path/to/ecal_withwaveform.root
# 生成 /path/to/fitresult.root 和 /path/to/time_resolution.png
```

## 二、批量数据处理流程

通过 hepcondor 批量提交模拟作业，每个能量点事例总数约 5000。
支持两种提交方式：单能量点（`submit.sh`，便于按需修改 `run.mac`）和多能量点扫描
（`submit_batch.sh`，0.2~3.5 GeV 共 10 个点，单 run 事例数按 round(5/E) 分配，下限 2）。

### 流程脚本

| 脚本 | 功能 |
|---|---|
| `submit.sh` | 单能量点提交：拷贝修改好的 `run.mac` 到数据目录 `./data/ECAL_<E>GeVe+/` 并提交作业 |
| `submit_batch.sh` | 遍历能量点列表，从模板生成各能量点的 `run.mac` 和 `process.sh`，创建数据目录并用 `hep_sub` 提交作业 |
| `process_batch.sh` | 单个 condor 作业的运行脚本（含 `@ECAL_BIN@` 占位符，提交时被替换为 ECAL 绝对路径） |
| `run_template.mac` | 宏模板（含 `@ENERGY@`、`@NEVENTS@` 占位符），供 `submit_batch.sh` 生成各能量点的 `run.mac` |
| `merge_batch.sh` | 对每个能量点目录执行 `hadd ecal.root job*.root` 合并子文件，成功后删除子文件及 condor 日志 |
| `analysis_batch.sh` | 对每个能量点目录依次执行 `GenerateWaveform` 和 `FitWaveform` |

### 完整使用流程

所有命令在 centos7 容器内、`build/` 目录下执行：

```bash
cos    # 进入 centos7 容器
cd /path/to/ECALsimulation/build

# 1. 编译（cmake 会把本目录的脚本、宏拷贝到 build/，源码拷贝到 build/analysis/）
cmake .. && make -j4
```

多能量点扫描：

```bash
# 2. 提交全部能量点的模拟作业
./submit_batch.sh
# 作业状态查看: hep_q；数据目录: ./data/batch/ECAL_*GeVe+/

# 3. 等全部作业完成后，合并每个能量点的 job*.root -> ecal.root 并清理
./merge_batch.sh ./data/batch/

# 4. 批量波形生成与拟合
./analysis_batch.sh ./data/batch/
```

单能量点（便于灵活调整 `run.mac`）：

```bash
# 2. 修改 run.mac（粒子种类、能量、位置/时间分布等）后提交
./submit.sh
# 能量、事例数、目录名等参数在脚本开头设置；数据目录: ./data/ECAL_<E>GeVe+/

# 3. 合并与分析同多能量点流程
./merge_batch.sh ./data/
./analysis_batch.sh ./data/
```

目录参数支持相对路径和绝对路径。脚本均基于自身位置定位 ECAL 可执行文件、宏和运行脚本，
请在 `build/` 目录下执行。

### 数据目录结构

多能量点扫描的数据存放在 `data/batch/` 下，单能量点存放在 `data/` 下，两者目录内容相同：

```
data/batch/                  # 单能量点数据直接在 data/ 下
├── ECAL_0.2GeVe+/
│   ├── comment.txt          # 能量点说明
│   ├── run.mac              # 由模板生成或直接拷贝的宏
│   ├── process.sh           # 由 process_batch.sh 替换占位符后生成
│   ├── job_*.root           # 各 condor 作业输出（合并后被清理）
│   ├── process.sh.err/out.* # condor 日志（合并后被清理）
│   ├── ecal.root            # hadd 合并结果
│   ├── ecal_withwaveform.root
│   └── fitresult.root
├── ECAL_0.4GeVe+/
│   └── ...
└── ECAL_3.5GeVe+/
```

## 三、文件说明

- `common.h`：公共数据结构 `DATA_STRUCT`，与模拟输出 root 文件 `ecal` 树的分支对应。
- `GenerateWaveform.cpp`：`ConvoluteIRF`（光子时间分布与探测器响应卷积）、`add_noise`（叠加电子学噪声）、`convert_waveform`（主流程）。
- `FitWaveform.cpp`：`ReadTree`（读取 `ecal` 树）、`fit_single_waveform`（单波形参数化拟合）、`fit_waveform`（主流程，输出时间分辨图与拟合结果树）。