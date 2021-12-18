###使用说明：
1. git clone --recursive git@github.com:oceancx/luadebugger.git
2. 首先用cmake.exe生成vs工程, config，config，generate，一步到位
3. 编译luadbg工程，会生成luadbg.lib, luadbg.dll
4. 手动把生成的luadbg.lib, luadbg.dll拷贝到 luadebugger/test目录里
5. 修改luadbgtest工程的main.cpp，里面的路径改成相对于自己电脑的，编译luadbgtest工程，成功luadebugger/test目录下会有luadbgtest.exe
6. 编译vsdbgadapter工程，成功luadebugger/vscode目录下会有vsdbgadapter.exe
7. 用VS Code打开luadebugger/vscode目录，点击左上角的Extension，成功会弹出一个安装了luadbg VS Code插件的VS Code进程。
8. 在新开的VS Code进程窗口里点击左上角的红点，会出弹窗提示你生成launch.json, 选择CXLua
9. 成功后VS Code左上角的绿色箭头处会有Attach/Launch的选项
10. 运行luadbgtest.exe后，点击Attach，即可调试

###整合luadbg到自己的lua项目：
1. 编译luadbg工程，生成luadbg.lib, luadbg.dll
2. 自己的工程根据情况添加以下代码, 没有注释的是你的代码

    #define LUADBG_DLL          //add this line for luadbg 
    #include <luadbg.h>         //add this line for luadbg 
    
    int main(int argc ,char** argv)
    {
        lua_State * L = luaL_newstate();
        luaL_openlibs(L); 
        ...
        luaopen_luadbg(L);      //add this line for luadbg 
        return 0;
    }

3. 自己的工程的include目录里要能找到luadbg.h, 链接的库要指定 luadbg.lib的绝对路径，luadbg.lib和luadbg.dll要在同一目录
4. 自己的工程lua代码里要根据情况添加下面代码，简单来说，就是lua代码运行开始的时候调用luadbg_start指定一个端口来开启luadbg server监听vscode的消息，你游戏update的时候，调用luadbg_update来poll vscode传来的消息。
    
    luadbg_start(9527) -- C调用完luaopen_luadbg即可调用
    ...

    function game_update()
        luadbg_update() 
        ...
    end

    function game_stop()
        ...
        luadbg_stop()
        ...
    end

5. 运行你的工程，然后vscode来Attach即可。
6. luadbg要求你工程里的dofile或者loadfile必须要传入文件的绝对路径，最好大小写也一致，如果vscode断点不能跳转到你工程里的lua文件，请检查是否符合此要求。
7. luadbg依赖ezio，因此你的工程需要满足win64和C++14。。。

feel free to bug me~


VS Code插件本地打包 安装 https://code.visualstudio.com/api/working-with-extensions/publishing-extension


