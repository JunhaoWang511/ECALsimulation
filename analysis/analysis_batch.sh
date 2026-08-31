# 这个是批量处理数据脚本：对每个能量点目录执行:
# 1. GenerateWaveform dirtofile/ecal.root
# 2. FitWaveform dirtofile/ecal_withwaveform.root
#!/bin/bash
# 用法: ./analysis_batch.sh <能量点数据文件夹所在目录>
# 例如: ./analysis_batch.sh ./data/batch/  或  ./analysis_batch.sh /ustcfs/HICUser/jhwang/ECALsimulation/build/data/batch/
if [ -z "$1" ]; then
	echo "用法: $0 <能量点数据文件夹所在目录>"
	exit 1
fi
base_dir="$1"
# 检查目录是否存在
if [ ! -d "$base_dir" ]; then
	echo "错误: 目录 $base_dir 不存在"
	exit 1
fi
# 判断可执行文件是否存在（优先用脚本所在目录的，其次用build根目录的）
script_dir="$(cd "$(dirname "$0")" && pwd -P)"
gen_bin="$script_dir/GenerateWaveform"
fit_bin="$script_dir/FitWaveform"
[ -f "$gen_bin" ] || gen_bin="$script_dir/../GenerateWaveform"
[ -f "$fit_bin" ] || fit_bin="$script_dir/../FitWaveform"
if [ ! -f "$gen_bin" ]; then
	echo "错误: 可执行程序 GenerateWaveform 不存在"
	exit 1
fi
if [ ! -f "$fit_bin" ]; then
	echo "错误: 可执行程序 FitWaveform 不存在"
	exit 1
fi
for dir in "$base_dir"/ECAL_*/; do
	# 跳过不匹配的项
	[ -d "$dir" ] || continue
	echo "=== 处理 $dir ==="
	# 检查是否有ecal.root
	if [ ! -f $dir/ecal.root ]; then
		echo "  无 ecal.root，跳过"
		continue
	fi
	# 如果没有ecal_withwaveform.root，则生成ecal_withwaveform.root
	if [ ! -f $dir/ecal_withwaveform.root ]; then
		echo "  产生波形"
		"$gen_bin" $dir/ecal.root
	fi
	# 如果有ecal_withwaveform.root，则进行参数拟合
	if [ -f $dir/ecal_withwaveform.root ]; then
		echo "  拟合波形"
		"$fit_bin" $dir/ecal_withwaveform.root
	fi
done
echo "全部完成"
