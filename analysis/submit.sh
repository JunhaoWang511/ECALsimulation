# 这个是单能量点批量提交ECAL光学模拟的脚本：按需修改run.mac后运行本脚本提交作业
# （与多能量点脚本submit_batch.sh相互独立，数据目录为./data/ECAL_<E>GeVe+/）
#!/bin/bash
dir_name="./data/ECAL_1GeVe+/"
batch_name="run.mac"
number_of_jobs=500
Comment="1GeV动能e+"
script_dir="$(cd "$(dirname "$0")" && pwd -P)"
# run.mac：优先取当前目录下的（修改后直接提交），其次取脚本所在目录的
mac_src="$batch_name"
[ -f "$mac_src" ] || mac_src="$script_dir/$batch_name"
# 检查目录是否存在，如果不存在则创建
if [ ! -d "$dir_name" ]; then
	echo "Directory $dir_name does not exist."
	mkdir -p "$dir_name"
fi
# 拷贝宏文件和提交脚本到数据目录
cp "$mac_src" "$dir_name/run.mac"
# ECAL可执行文件绝对路径：优先用脚本所在目录的，其次用上级build目录的
# （pwd -P解析物理路径，避免符号链接路径在计算节点上失效）
ecal_bin="$script_dir/ECAL"
[ -f "$ecal_bin" ] || ecal_bin="$(cd "$script_dir/.." && pwd -P)/ECAL"
sed -e "s|@ECAL_BIN@|${ecal_bin}|g" "$script_dir/process_batch.sh" >"$dir_name/process.sh"

# 切换到数据目录
pushd "$dir_name" >/dev/null
echo $Comment >comment.txt
chmod +x process.sh
# 批量提交作业
hep_sub process.sh -argu "%{ProcID}" -n $number_of_jobs
# 提交完成后返回上一层目录
popd >/dev/null
