#!/bin/bash

# 获取传入的参数
program_name=$1
additional_args="${@:2}"

# 根据传入的程序名调用相应的 C++ 程序
case $program_name in
    "display")
        ./build/display_monitor/display "/work/configs.yml"
        ;;
    "server")
        ./build/rpc_manager/server/server 
        ;;
    "monitor")
        ./build/test_monitor/src/monitor "/work/configs.yml"
        ;;
    # 添加更多程序的处理逻辑
    *)
        echo "Unknown program: $program_name"
        ;;
esac
