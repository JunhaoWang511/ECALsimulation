#!/bin/bash
# 读入作业ID
job_id=$1
# ECALBIN 占位符由submit脚本在拷贝时替换为ECAL可执行文件的绝对路径
@ECAL_BIN@ -e "run.mac" -o "job_${job_id}.root" -n 1 -op -s "$job_id"
