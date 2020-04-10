# apollo c语言客户端
## 前置依赖
- 依赖libcurl 
```shell
  yum -y install libcurl libcurl-devel
```
- 依赖json-c
```shell
  yum -y install json-c json-c-devel
```
> 源代码安装libcurl和json-c请参考其他教程。
## 编译
### cygwin环境
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
### linux环境
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