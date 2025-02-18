# istatus

一个轻量级的系统状态监控工具，用于收集系统资源使用情况并通过MQTT协议进行数据上报。

## 功能特性

- 实时监控系统资源使用情况
  - CPU使用率（用户态、系统态、空闲）
  - 磁盘使用情况（总容量、已用、剩余、使用率）
  - 网络流量统计（发送/接收字节数、数据包数）
- 支持MQTT协议数据上报
  - 可配置MQTT服务器连接参数
  - 支持QoS服务质量控制
  - 自动重连机制
- 灵活的配置文件支持
  - 支持INI格式配置文件
  - 可自定义监控区域和地址信息
  - 可配置网络接口

## 安装

### 依赖

- Eclipse Paho MQTT C Client Library
- GCC编译器
- Make工具

### 编译安装

```bash
# 编译
make

# 安装到系统
sudo make install

# 使用debug模式编译
make DEBUG=1
```

## 配置

配置文件示例 (config.ini):

```ini
[mqtt]
host=localhost
port=1883
keepalive=600
topic=status

[app]
area=嵩山
address=192.168.101.100

[network]
interface=en0
```

## 使用方法

```bash
# 使用指定配置文件运行
istatus -c /path/to/config.ini

# 查看版本信息
istatus -v

# 查看帮助信息
istatus -h
```

## 数据格式

系统状态数据以JSON格式上报，示例：

```json
{
  "timestamp":"2024-01-01 12:00:00",
  "area":"嵩山",
  "address":"192.168.101.100",
  "system_status":{
    "disk":{
      "total":1000000,
      "used":500000,
      "free":500000,
      "percent":50.00
    },
    "network":{
      "bytes_sent":1000,
      "bytes_recv":2000,
      "packets_sent":100,
      "packets_recv":200
    },
    "cpu":{
      "user":10.5,
      "system":5.2,
      "idle":84.3
    }
  }
}
```

---
that's all