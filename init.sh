#!/bin/bash
[ ! -d ./sub ] && mkdir ./sub
[ ! -d ./gen ] && mkdir ./gen
[ ! -d ./gen/exec ] && mkdir ./gen/exec
[ ! -d ./gen/out ] && mkdir ./gen/out
chmod +x ./judge-core/judger_linux
chmod +x ./comp/comp.sh