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
local directions = {
    "N",
    "S",
    "W",
    "E",
    "N_E",
    "N_W",
    "S_E",
    "S_W"
}

function export_role_avatar()
    local avatar_rol_tbl = content_system_get_table('avatar_role')
    for id,row in pairs(avatar_rol_tbl) do
        for i=0,action_system_get_action_size()-1 do
            local action = action_system_get_action_name(i)
            local dir = string.format("%sassets/unpack/avatar/role/%s/%s/role", vfs_get_workdir(), row.ID,action)
            local cmd = "mkdir -p \""..dir.."\""
            os.execute(cmd)
            if row[action]~="" then
                res_export_was_image(res_parse_resid(row[action]), dir)
            end
        end
    end
end


function export_npc_avatar()
    local tbl = content_system_get_table('avatar_scene_npc')
    for id,row in pairs(tbl) do
        for i=0,action_system_get_action_size()-1 do
            local action = action_system_get_action_name(i)
            if row[action] then
                local dir = string.format("%sassets/unpack/avatar/npc/%s/%s", 
                            vfs_get_workdir(), row.ID,action)
                local cmd = "mkdir -p \""..dir.."\""
                cmd = util_utf8_to_gb2312(cmd)
                os.execute(cmd)
    
                if row[action]~="" then
                    dir = util_utf8_to_gb2312(dir)
                    res_export_was_image(res_parse_resid(row[action]), dir)
                end
            end
        end
    end
end

function export_avatar_summon()
    local tbl = content_system_get_table('avatar_summon')
    for id,row in pairs(tbl) do
        for i=0,action_system_get_action_size()-1 do
            local action = action_system_get_action_name(i)
            if row[action] then
                local dir = string.format("%sassets/unpack/avatar/summon/%s/%s", 
                            vfs_get_workdir(), row.ID,action)
                local cmd = "mkdir -p \""..dir.."\""
                cmd = util_utf8_to_gb2312(cmd)
                os.execute(cmd)
    
                if row[action]~="" then
                    dir = util_utf8_to_gb2312(dir)
                    res_export_was_image(res_parse_resid(row[action]), dir)
                end
            end
        end
    end
end

function export_weapon_avatar()
    local weapon_tbl = content_system_get_table('avatar_weapon')
    for id,row in pairs(weapon_tbl) do
        for i=0,action_system_get_action_size()-1 do
            local action = action_system_get_action_name(i)
            local dir = string.format("%sassets/unpack/avatar/%s-%s/%s/%s", vfs_get_workdir(), row.role,row.type,action, row.name)
            local cmd = "mkdir -p \""..dir.."\""
            cmd = util_utf8_to_gb2312(cmd)
            os.execute(cmd)
            if row[action]~="" then
                print(cmd)
                dir = util_utf8_to_gb2312(dir)
                res_export_was_image(res_parse_resid(row[action]), dir)
            end
        end
    end 
end

function export_map()
    local tbl = content_system_get_table('scene')
    for id,row in pairs(tbl) do
        local map_id = row.map_id
        if map_id ~= 0 then
            res_export_map(map_id)
        end
        -- return
    end
end

function export_music()
    local tbl = content_system_get_table('scene')

    local dir = string.format("%sassets/unpack/bgm", vfs_get_workdir())
    local cmd = "mkdir -p \""..dir.."\""
    os.execute(cmd)

    for id,row in pairs(tbl) do
        if row.bgm ~= "" then
            local path = dir..'/' ..row.name ..'.mp3'
            path = util_utf8_to_gb2312(path)
            local resid = res_parse_resid(row.bgm)
            res_export_bgm(path,resid)
            -- return
        end
    end
end

function export_skill()
    -- local tbl = content_system_get_table('skill')
    -- for id,row in pairs(tbl) do
    --     if row.atk_anim~=0 then
    --         local dir = string.format("%sassets/unpack/avatar/skill/%s", 
    --             vfs_get_workdir(), row.name)
    --         local cmd = "mkdir -p \""..dir.."\""
    --         cmd = util_utf8_to_gb2312(cmd)
    --         os.execute(cmd)
    --         dir = util_utf8_to_gb2312(dir)
    --         -- print('row.atk_anim ' ..row.atk_anim)
    --         res_export_was_image(row.atk_anim, dir)
    --     end
    -- end

    -- if true then
    --     return
    -- end

    tbl = content_system_get_table('buff')
    for id,row in pairs(tbl) do
        if row.buff_anim~=0 then
            local dir = string.format("%sassets/unpack/avatar/buff/%s", 
                vfs_get_workdir(), row.name)
            local cmd = "mkdir -p \""..dir.."\""
            cmd = util_utf8_to_gb2312(cmd)
            os.execute(cmd)
            dir = util_utf8_to_gb2312(dir)
            res_export_was_image(row.buff_anim, dir)
        end
    end
end

function init()
    content_system_init()
    -- export_map()
    -- export_music()
    -- export_npc_avatar()
    -- export_avatar_summon()
    -- export_npc_avatar()
    export_skill()
   
end

function update()
     
end

do
    iw_init(800, 600)
    iw_set_font(vfs_get_workdir() .. '/assets/font/simsun.ttc',14)
    local ok, ret = pcall(init)
    if not ok then
        print(ret)
    end
    iw_render(update)
    iw_deinit()
end
