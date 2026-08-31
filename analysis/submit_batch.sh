# 这个是批量提交ECAL光学模拟的脚本（多能量点），与单能量点提交脚本submit.sh相互独立
#!/bin/bash
# 脚本所在目录（源码analysis/或构建目录），用于定位依赖的宏和运行脚本
# 定义数据存储目录名称
data_dir="batch"
script_dir="$(cd "$(dirname "$0")" && pwd -P)"
total_events=5000
energies=(0.2 0.4 0.6 0.8 1.0 1.5 2.0 2.5 3.0 3.5)

for E in "${energies[@]}"; do
	# 单run事例数：round(5/E)，最低为2
	n_per_run=$(awk -v e="$E" 'BEGIN{n=int(5.0/e+0.5); if(n<2)n=2; print n}')
	# job数：向上取整
	n_jobs=$(((total_events + n_per_run - 1) / n_per_run))

	dir_name="./data/${data_dir}/ECAL_${E}GeVe+"
	Comment="${E}GeV动能e+"

	# 检查目录是否存在，如果不存在则创建
	if [ ! -d "$dir_name" ]; then
		echo "Directory $dir_name does not exist."
		mkdir -p "$dir_name"
	fi
	# 从模板生成该能量的run.mac（替换能量和事例数占位符），并拷贝提交脚本到数据目录
	sed -e "s|@ENERGY@|${E}|g" \
		-e "s|@NEVENTS@|${n_per_run}|g" \
		"$script_dir/run_template.mac" >"$dir_name/run.mac"
	# ECAL可执行文件绝对路径：优先用脚本所在目录的，其次用上级build目录的
	# （pwd -P解析物理路径，避免符号链接路径在计算节点上失效）
	ecal_bin="$script_dir/ECAL"
	[ -f "$ecal_bin" ] || ecal_bin="$(cd "$script_dir/.." && pwd -P)/ECAL"
	sed -e "s|@ECAL_BIN@|${ecal_bin}|g" "$script_dir/process_batch.sh" >"$dir_name/process.sh"
	# 切换到数据目录
	pushd "$dir_name" >/dev/null
	echo $Comment >comment.txt
	chmod +x process.sh
	echo "Submitting ${E} GeV: ${n_jobs} jobs x ${n_per_run} events/run (total ~${total_events})"
	# 批量提交作业
	hep_sub process.sh -argu "%{ProcID}" -n $n_jobs
	# 提交完成后返回上一层目录
	popd >/dev/null
done
