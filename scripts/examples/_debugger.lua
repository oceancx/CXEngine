--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

调试器demo 

用vscode打开项目目录，点击VSCode左侧栏的Debug按钮或者（Ctrl+Shift+D），左边栏会出现绿色三角形的启动按钮，点击按钮右边的launcher，
选择Debugger，然后点击绿色三角按钮，即可从vscode运行此文件。
]]

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')


local actor
function init()
    content_system_init()          
    actor = lua_create_actor(1)    
    actor:ResetASM()               

    actor:SetPos(100, 100)         
end

function update()
    if not actor then return end
    if imgui.IsMouseClicked(0) then
        local mx,my = input_manager_get_mouse_pos()
        local x,y = imgui.GetMousePos()
        print(x,y , mx, my)
        actor:MoveTo(mx,my) 
    end

    actor:Update()
    actor:Draw()
end

do
    local dbg_port = command_arg_opt_int('dbg_port', 10086)
    luadbg_listen(dbg_port,true)       --第一个参数是调试器监听端口，第二个是初始化网络环境。
    
    iw_init(800, 600)
    local ok, ret = pcall(init)
    if not ok then
        print(ret)
    end
    iw_render(update)
    iw_deinit()
end
