# 1.apollo c语言客户端
## 1.1 前置依赖
- 依赖libcurl 
```shell
  yum -y install libcurl libcurl-devel
```
- 依赖json-c
```shell
  yum -y install json-c json-c-devel
```
> 源代码安装libcurl和json-c请参考其他教程。
## 1.2 编译
### 1.2.1 cygwin环境
window环境下使用cygwin编译,进入源代码根目录
```shell
  make DESTDIR=./cygwin all
```
安装到cygwin
```shell
  make DESTDIR=./cygwin install
```
其他指令
- uninstall 卸载安装。ls
- clean 清空源代码生成的库文件缓存。
### 1.2.2 linux环境
window环境下使用cygwin编译,进入源代码根目录
```shell
  make all
```
安装到cygwin
```shell
  make install
```
其他指令
- uninstall 卸载安装。ls
- clean 清空源代码生成的库文件缓存。
## 1.3调用
###1.3.1不带缓存的Http接口从Apollo读取配置
引用头文件。
```asm
#include <apollo/apolloclient.h>
```
获取某个apollo上namespace对应的键值对
```c
    apollo_env apolloEnv={
        meta:"192.168.1.65:16001",
        appId:"ms-apigateway",
        clusterName:"default",
        namespaceName:"application.yml"
    };
    Properties properties;
    getNoCacheProperty(apolloEnv,&properties);
```
其中Properties封装了返回键值对信息。
```asm
typedef struct {
    size_t len; /**键值对个数**/
    String* keys; /**键数组**/
    String* values; /**值数组**/
} Properties;
```
注意一下几点：
 1. property类型namespace，返回对应键值对结果。
 2. json和yaml返回一个长度，keys 固定为content。
 
###1.3.2应用感知配置更新
应用注册后，当配置中心发生变化后自动推送更新到回调函数。
```c
    apollo_env apolloEnv={
        meta:"192.168.1.65:16001",
        appId:"ms-apigateway",
        clusterName:"default",
        namespaceName:NULL
    };
   notification notifications={
        namespaceName:"application.yml",
        notificationId:-1
    };
   submitNotificationsAsync(apolloEnv,notifications,1,updateConfig);
```
定义回调函数updateConfig
```c
/**
  old表示此次更新旧配置，new表示新配置。
  第一次回调old是NULL。
**/
void updateConfig(Properties* old,Properties* new){
    //对应的处理逻辑
}
```