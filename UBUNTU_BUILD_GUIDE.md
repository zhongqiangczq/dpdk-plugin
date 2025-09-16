# Ubuntu环境下DPDK插件构建指南

本指南详细说明如何在Ubuntu环境中构建Zeek DPDK插件，以验证Zeek 8.0.1与DPDK 23.11.4的兼容性。

## 系统要求

- Ubuntu 22.04 LTS 或 Ubuntu 24.04 LTS
- 至少4GB内存
- 20GB可用磁盘空间
- 网络连接（用于下载依赖包）

## 安装步骤

### 1. 更新系统并安装基础依赖

```bash
sudo apt update
sudo apt install -y build-essential cmake git pkg-config python3 python3-pip wget curl software-properties-common
```

### 2. 安装DPDK依赖

```bash
sudo apt install -y libnuma-dev libpcap-dev libssl-dev zlib1g-dev
```

### 3. 安装DPDK 23.11.4

```bash
# 安装meson和ninja构建工具
pip3 install meson ninja pyelftools

# 下载并编译DPDK
cd /tmp
wget https://fast.dpdk.org/rel/dpdk-23.11.4.tar.xz
tar xf dpdk-23.11.4.tar.xz
cd dpdk-23.11.4
meson setup build
cd build
ninja
sudo ninja install
sudo ldconfig
```

### 4. 安装Zeek依赖

```bash
sudo apt install -y bison flex libfl-dev libgeoip-dev libkrb5-dev libmaxminddb-dev libpcap-dev libssl-dev python3-dev swig zlib1g-dev
```

### 5. 安装Zeek 8.0.1

```bash
cd /tmp
wget https://download.zeek.org/zeek-8.0.1.tar.gz
tar xzf zeek-8.0.1.tar.gz
cd zeek-8.0.1
./configure --prefix=/opt/zeek
make -j$(nproc)
sudo make install
```

### 6. 设置环境变量

```bash
echo 'export PATH="/opt/zeek/bin:$PATH"' >> ~/.bashrc
echo 'export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"' >> ~/.bashrc
source ~/.bashrc
```

### 7. 验证安装

```bash
# 检查Zeek版本
zeek --version

# 检查DPDK版本
pkg-config --modversion libdpdk
```

### 8. 编译DPDK插件

```bash
# 克隆或复制项目代码到Ubuntu系统
# 进入项目目录
cd /path/to/dpdk-plugin

# 清理之前的构建
rm -rf build

# 开始编译
make
```

## 预期结果

如果编译成功，您应该看到：

1. 构建过程中没有错误
2. 在`build/`目录中生成了插件文件（.so和.tgz文件）
3. 所有兼容性修复都已验证

## 兼容性修复说明

本项目已包含以下Zeek 8.0.1与DPDK 23.11.4的兼容性修复：

### DPDK API更新
- 更新版本检查从21.11.0到23.11.4
- 修复`rte_eth_promiscuous_enable`函数返回值处理
- 更新结构体字段访问方式

### Zeek API更新
- 修复`RecordVal::GetField`到`GetFieldOrDefault`的API变化
- 更新`StringVal::CheckString`到`ToStdString`的方法调用
- 修复条件编译中的类型声明问题

## 故障排除

### 常见问题

1. **DPDK未找到**
   ```
   Package 'libdpdk' not found
   ```
   解决方案：确保DPDK正确安装并运行`sudo ldconfig`

2. **Zeek头文件未找到**
   ```
   zeek-config: command not found
   ```
   解决方案：确保Zeek安装路径在PATH中

3. **编译错误**
   检查所有依赖是否正确安装，特别是开发头文件包

### 调试模式编译

```bash
# 使用调试模式编译以获得更多信息
export CMAKE_BUILD_TYPE=Debug
make
```

## 测试验证

编译成功后，可以运行以下测试：

```bash
# 检查插件是否正确加载
zeek -N | grep DPDK

# 运行基础测试
cd tests
make
```

## 联系支持

如果遇到问题，请提供：
- Ubuntu版本：`lsb_release -a`
- DPDK版本：`pkg-config --modversion libdpdk`
- Zeek版本：`zeek --version`
- 完整的编译错误日志