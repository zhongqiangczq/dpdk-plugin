#!/bin/bash

set -e

echo "=== 本地编译测试脚本 ==="
echo "注意: 这个脚本用于在有DPDK和Zeek环境的系统上测试编译"
echo ""

# 检查必要的工具
echo "检查编译环境..."
if ! command -v cmake &> /dev/null; then
    echo "警告: cmake未找到"
fi

if ! command -v make &> /dev/null; then
    echo "警告: make未找到"
fi

if ! command -v pkg-config &> /dev/null; then
    echo "警告: pkg-config未找到"
fi

# 检查DPDK
echo "检查DPDK..."
if pkg-config --exists libdpdk; then
    echo "DPDK版本: $(pkg-config --modversion libdpdk)"
else
    echo "警告: DPDK未找到或未正确配置"
fi

# 检查Zeek
echo "检查Zeek..."
if command -v zeek-config &> /dev/null; then
    echo "Zeek版本: $(zeek-config --version)"
    echo "Zeek安装路径: $(zeek-config --zeek_dist)"
else
    echo "警告: Zeek未找到或未正确配置"
fi

echo ""
echo "开始编译..."

# 清理之前的构建
rm -rf build

# 尝试编译
if make; then
    echo ""
    echo "=== 编译成功! ==="
    echo "生成的文件:"
    find build -name "*.so" -o -name "*.tgz" 2>/dev/null || echo "未找到生成的插件文件"
else
    echo ""
    echo "=== 编译失败 ==="
    echo "请检查依赖项是否正确安装"
    exit 1
fi