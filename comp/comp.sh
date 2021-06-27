#!/bin/bash
SUB_PATH=$5
ANS_PATH=$9
echo $SUB_PATH
echo $ANS_PATH
# 判断文件是否存在
if [ ! -f $SUB_PATH ]; then
  exit -1
elif [ ! -f $SUB_PATH ]; then
  exit -1
fi

# 文件对比
diff -q -b "$SUB_PATH" "$ANS_PATH"