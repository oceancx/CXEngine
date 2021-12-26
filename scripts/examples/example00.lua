--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example0 最简单的教程 显示一个窗口
]]--
script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')




--update函数在iw_render里面被调用，游戏的循环是在底层的cpp代码里，底层每一帧会调用一次你传入给iw_render的lua函数。
function update()
    cxclient_update()
    if imgui.Button('CXClientCreate') then
        cxclient_create()
    end

    if imgui.Button('CXClientDestroy') then
        cxclient_destroy()
    end

    if imgui.Button('CXClientConnect') then
        cxclient_connect()
    end

    if imgui.Button('CXClientDisConnect') then
        cxclient_disconnect()
    end
    
    if imgui.Button('CXClientSendMsg') then
        cxclient_sendmsg(PTO_C2S_GM, '{"type":"account_info"}')
    end
end

do
    iw_init(800, 600)       --iw_init为c函数,初始化了一个800x600的窗口,lua第一行代码从这里执行.
    iw_render(update)       --iw_render为c函数,接受一个lua函数作为参数传入,iw_render执行后,c代码里就进入了游戏循环逻辑,每帧调用传入的lua函数一次.
    iw_deinit()             --iw_deinit为c函数,释放c里面分配的资源,程序走到这里,说明游戏循环已经被跳出.
end
