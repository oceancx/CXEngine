--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example6 资源示例

]]
script_system_dofile('../share/enums.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')


function draw_avatar_info(sprite)
    local x, y = sprite:GetPos()
    imgui.Text('X ' .. math.floor(x) .. ' Y' .. math.floor(y))
    imgui.Text('w ' .. sprite:GetWidth() .. ' h ' .. sprite:GetHeight())
    imgui.Text('KeyX ' .. sprite:GetKeyX() .. ' KeyY ' .. sprite:GetKeyY())
    
    local curframe = sprite:GetCurrentFrame()
    imgui.Text('CurKeyX ' .. sprite:GetFrameKeyX(curframe) .. ' CurKeyY ' .. sprite:GetFrameKeyY(curframe))
    
    imgui.Text('PlayTime' .. ' ' .. string.format('%.2f', sprite:GetPlayTime()))
    imgui.Text('DirCnt' .. ' ' .. sprite:GetDirCnt())
    imgui.Text('TotalFrames' .. ' ' .. sprite:GetTotalFrames())
    imgui.Text('CurrentFrame' .. ' ' .. sprite:GetCurrentFrame())
    imgui.Text('GroupFrameCount' .. ' ' .. sprite:GetGroupFrameCount())
    imgui.Text('GroupCount' .. ' ' .. sprite:GetGroupCount())
    imgui.Text('dir ' .. sprite:GetDir() .. ' dt ' .. string.format('%.2f', sprite:GetFrameInterval()))
end


local role_anim
function init()
    role_anim = animation_create(SHAPEWDF, 0x49386FCE)--12-49386FCE  , SHAPEWDF = 12
    role_anim:SetPos(300, 80)
end

function update()
    role_anim:Update()
    
    imgui.SetCursorPos(300-55, 80)
    imgui.BeginGroup()
    draw_avatar_info(role_anim)
    imgui.EndGroup()
    
    role_anim:Draw()
    
    if imgui.Button('Play') then
        role_anim:Play()
    end
    
    if imgui.Button('导出was') then
        res_export_was(SHAPEWDF, 0x49386FCE, vfs_makepath('role_idle'))
    end
end

do
    luadbg_listen(10086, true)
    iw_init(800, 600)
    iw_set_font(vfs_get_workdir() .. '/assets/font/simsun.ttc',14)
    local ok, ret = pcall(init)
    if not ok then
        print(ret)
    end
    iw_render(update)
    iw_deinit()
end
