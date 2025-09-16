#!/bin/bash

set -e

echo "=== Ubuntu环境下DPDK插件构建脚本 ==="
echo ""

# 检查Docker是否安装
if ! command -v docker &> /dev/null; then
    echo "错误: Docker未安装，请先安装Docker"
    exit 1
fi

echo "1. 构建Ubuntu Docker镜像..."
docker build -f Dockerfile.ubuntu -t dpdk-plugin-ubuntu .

echo ""
echo "2. 在Ubuntu容器中编译DPDK插件..."
docker run --rm -v "$(pwd):/workspace" dpdk-plugin-ubuntu

echo ""
echo "=== 构建完成 ==="
echo "如果编译成功，插件文件应该在 build/ 目录中"
echo "可以运行以下命令查看生成的文件:"
echo "  ls -la build/"
echo ""
echo "如果需要进入容器进行调试，可以运行:"
echo "  docker run -it --rm -v \"$(pwd):/workspace\" dpdk-plugin-ubuntu /bin/bash"