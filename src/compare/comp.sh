#!/bin/bash 
# 判断文件是否存在
if [ ! -f $1 ]; then
  exit -1
elif [ ! -f $2 ]; then
  exit -1
fi

# 文件对比
diff -q -b "$1" "$2"