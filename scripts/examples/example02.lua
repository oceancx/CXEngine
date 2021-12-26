--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example2 imgui简单使用,控制动画的位置
]]
--
local anim = nil
function init()
    anim = animation_create(WADDONWDF, 0xCA8FDEAD)--0xCA8FDEAD为was在wdf里的hash值,CA8FDEAD可以在res\tables\ini\waddon.wdf.ini找到对应的动画名字
    anim:SetPos(200, 100)--设置动画在屏幕上的位置,屏幕的原点(0,0)在左上角
    anim:SetLoop(0)--设置动画的循环次数,0表示无限循环
    anim:Play()--动画创建后默认状态是STOP,STOP状态下调用动画Update,动画是不会更新的.调用Play会让动画进入PLAY状态,PLAY状态下调用动画Update,动画是会更新
end

function update()
    if not anim then return end
    
    local px, py = anim:GetPos()        --获取anim对象的位置
    imgui.Text('Pos x:' .. px .. ' y:' .. py)   --imgui.Text使用很简单 就是把字符串画在屏幕上
    
    if imgui.Button("W") then                   --imgui.Button返回true表示按钮被点击
        local x, y = anim:GetPos()
        anim:SetPos(x, y - 5)
    end
    
    if imgui.IsKeyPressed(string.byte('W')) then    --判断W键是否被按下
        local x, y = anim:GetPos()
        anim:SetPos(x, y - 5)
    end
    
    if imgui.Button("A") or imgui.IsKeyPressed(string.byte('A')) then
        local x, y = anim:GetPos()
        anim:SetPos(x - 5, y)
    end
    
    if imgui.Button("S") or imgui.IsKeyPressed(string.byte('S')) then
        local x, y = anim:GetPos()
        anim:SetPos(x, y + 5)
    end
    
    if imgui.Button("D") or imgui.IsKeyPressed(string.byte('D')) then
        local x, y = anim:GetPos()
        anim:SetPos(x + 5, y)
    end
    
    
    anim:Update()
    anim:Draw()
end

do
    iw_init(800, 600)
    init()
    iw_render(update)
    iw_deinit()
end
