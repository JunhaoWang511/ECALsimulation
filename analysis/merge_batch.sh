# 这个是批量合并脚本：对每个能量点目录执行 hadd 合并job*.root，然后清理子文件和condor日志
#!/bin/bash
# 用法: ./merge_batch.sh <能量点数据文件夹所在目录>
# 例如: ./merge_batch.sh ./data/batch/  或  ./merge_batch.sh /ustcfs/HICUser/jhwang/ECALsimulation/build/data/batch/
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

for dir in "$base_dir"/ECAL_*/; do
	# 跳过不匹配的项
	[ -d "$dir" ] || continue
	echo "=== 处理 $dir ==="
	pushd "$dir" >/dev/null
	# 检查是否有待合并的子文件
	if ! ls job*.root >/dev/null 2>&1; then
		echo "  无 job*.root，跳过"
		popd >/dev/null
		continue
	fi
	n_files=$(ls job*.root | wc -l)
	# 合并（-f强制覆盖已有的ecal.root），成功后才删除子文件和日志
	if hadd -f ecal.root job*.root; then
		echo "  合并完成: ${n_files} 个子文件 -> ecal.root"
		rm -f job*.root
		rm -f process.sh.err.* process.sh.out.*
		echo "  已删除子文件及condor日志"
	else
		echo "  错误: hadd 失败，保留原始文件以便排查"
	fi
	popd >/dev/null
done
echo "全部完成"
