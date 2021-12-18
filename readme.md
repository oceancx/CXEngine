# 关于这个项目：

CXEngine是一个方便大家使用lua+imgui做游戏的游戏引擎

### 已构建完成的库下载

https://pan.baidu.com/s/18S0OKkdUJIRutJ5MuaLEug 提取码：3ppy  

下载完成后，运行launcher.bat可以启动客户端，服务端，以及资源路径配置。

### 构建方法：

1. git clone --recursive https://github.com/oceancx/CXEngine

2. 构建external工程。用cmake.exe打开CXEngine/external/CMakeLists.txt，配置构建的binaries目录为CXEngine/external/build, vs版本请选择Visual Studio 15 2017 之后的64位版本，之后全部采用默认配置，点击生成，直到vs工程生成。然后用IDE打开工程，构建INSTALL目标，这个构建完成后会自动安装libs还有include文件到CXEngine/external/dist目录下

3. 构建CXEngineAll工程。用cmake.exe打开CXEngine/CMakeLists.txt，配置构建的binaries目录为CXEngine/build。 vs版本请选择Visual Studio 15 2017 之后的64位版本，之后全部采用默认配置，点击生成，直到vs工程生成。然后用IDE打开工程，构建CXEngineAll目标。成功构建完成后，CXEngine/bin/Debug或者CXEngine/bin/Release下会生成CXEngine.exe, CXServer.exe, cxlua.exe。

4. sh launcher.bat。即可打开launcher，从而启动服务端客户端或者配置资源目录。


### 源码介绍

externals目录下的库全是作者引用的外部库，外部的意思是作者并不打算对其做任何修改。每个引用的外部库都有自己的介绍。这里不再赘述。

internals目录下的库全是作者引用的内部库，意味着作者随时有可能对其进行修改。下面对内部库作一一介绍。

1. kbase-cmake和ezio-cmake，这两个库修改自https://github.com/kingsamchen/KBase 和 https://github.com/kingsamchen/ezio  其中ezio依赖kbase，ezio是一个小巧高效的异步网络库，c++14编写。

2. lua-cmake，luacjson-cmake，为标准的lua库的cmake版。

3. nlohmann c++json库，单.hpp文件包含

4. NESupport 梦幻西游资源读取库，单一.h&.cpp文件包含

5. cxlua 依赖lua,luacjson,ezio,nesupport,nlohmann,glm。cxlua包装了基础性的c++接口，让其可以被lua调用到。SimpleEngine，SimpleServer以及launcher都依赖cxlua。

6. luadebugger https://github.com/oceancx/luadebugger  这个目录是一个repo，提供了lua调试器，包含了vscode的lua调试器适配器，以及嵌入到被调试程序里的调试相关功能的支持（例如断点，步入，步出）

7. gsm-cmake，miniaudio-cmake 修改自https://github.com/dr-soft/miniaudio 使其支持播放相关音频文件。

8. imgui https://github.com/ocornut/imgui ，游戏必备，本项目也提供了imgui的lua binding生成。

9. nanovg 2D矢量图形库 https://github.com/memononen/nanovg

下面是CXEngine/下剩余目录的介绍

1. .vscode保存的是项目的lua调试器配置，关于本项目调试器的构建安装请看launcher里的说明
2. bin/目录 是默认的exe构建生成目录
3. common/目录下的.cpp/.h文件会同时参与CXEngine和CXServer的构建。
4. client/仅用于CXEngine的cpp/h
5. server/仅用于CXServer的cpp/h
6. data/梦幻西游资源存放目录
7. cxluax是launcher项目
8. res/目录下是一些其他资源，例如字体，图标，tsv行列表
9. scripts/是CXEngine用到的所有lua文件，scripts/share/目录下的lua文件共享于launcher/CXEngine/CXServer，scripts/client/仅CXEngine使用，scripts/server仅CXServer使用


CXEngine，CXServer，以及launcher主要逻辑都由lua实现，其lua入口为各自的main.lua文件。

enums_protocol.lua 定义了所有客户端服务端之间的协议


### 目前CXEngine开发介绍

<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/1.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/2.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/3.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/4.jpg" width="50%" height="50%">


ALT+E 可以给自己染色，更换形象，更换武器

ALT+W 可以编辑场景里玩家以及召唤兽的属性，包括玩家属性点，玩家级别，玩家技能等级，玩家门派，玩家修炼，召唤兽属性点，召唤兽等级，召唤兽修炼，玩家携带的召唤兽信息，以及玩家的队伍信息。

ALT+R 仅战斗中会显示/隐藏，可以编辑当前战斗中所有对象的战斗指令。攻击/法术/防御/逃跑等


 
欢迎对此项目感兴趣的朋友加Q群讨论：983630090


