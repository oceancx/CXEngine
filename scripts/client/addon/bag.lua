local ui_is_show_bag = false
local to_scene_id = 1
function ui_toggle_show_bag()
    ui_is_show_bag = not ui_is_show_bag
    return ui_is_show_bag
end

local COLOR_SCHEMES 
local COLOR_SCHEMES_SELECT_MAP = {}
function update_color_schemes_select_map()
    COLOR_SCHEMES_SELECT_MAP = {}
    for i=1,#COLOR_SCHEMES.segments-1 do
        COLOR_SCHEMES_SELECT_MAP[i] = 1
    end
end

function fetch_role_keys(tbl)
    local role_keys = {}
    for k, v in pairs(tbl) do
        table.insert(role_keys,k)
    end
    table.sort(role_keys)
    return role_keys
end


function fetch_weapon_keys(tbl, avatar_key)
    local weapon_keys = {}
    local strs= utils_string_split(avatar_key,'-')
    local role_key , weapon_key  = strs[1], strs[2]
    for k, v in pairs(tbl) do
        if role_key == v.role and weapon_key == v.type then
            table.insert(weapon_keys,k)
        end
    end
    table.sort(weapon_keys)
    return weapon_keys
end

local PlayerNameSB = imgui.CreateStrbuf('test',256)
local ToSceneFilterSB = imgui.CreateStrbuf('',256)
local LocalPlayerDebugButtons = {
    
    {   '变异',function(player)
        player:Variation()
    end
    },
    {   '野外遇怪',function(player)
        net_manager_player_dostring(string.format([[ 
            player:SetProperty(PROP_SETTING_WILD_BATTLE, %s)
        ]],  not player:GetProperty(PROP_SETTING_WILD_BATTLE)))
    end
    },
    {   'BGM',function(player)
        audio_manager_toggle_bgm()
        net_manager_player_dostring([[ 
            player:SetProperty(PROP_SETTING_BGM, %s)
        ]],  audio_manager_is_bgm_on())
    end
    },
    {   'HUD',function(player)
            ui_toggle_show_hud()
        end
    },{
        '客户端重载', function(player)
            local scene_name = player:GetSceneName()
            
            script_system_dofile('../share/enums.lua')
            script_system_dofile('actor_metatable.lua')
            script_system_dofile('../share/actor_metatable.lua')
            script_system_dofile('../share/utils.lua')

            script_system_dofile('input_manager.lua')

            script_system_dofile('scene_manager.lua')
         

            script_system_dofile('actor_manager.lua')
          
            script_system_dofile('addon_manager.lua')
            load_all_addons()
            script_system_dofile('module/team.lua')
            script_system_dofile('module/dialog.lua')
    
            script_system_dofile('../combat/combat_system.lua')
            combat_system_init()

            scene_manager_reload(scene_name)
            game_map_reset_map_offset()

            actor_manager_clear_all()

            game_server_on_connection(true)
            collectgarbage()
        end
    },{
        '服务端重载', function(player)
            local msg = {code = [[
                server_reload()
            ]]} 
            net_send_message(PTO_C2S_DOSTRING, cjson.encode(msg) ) 
        end
    },{
        '刷新数据库', function(player)
            net_send_message(PTO_C2C_SAVE_ACCOUNT_DATABASE,cjson.encode({}))
            net_send_message(PTO_C2C_SAVE_ACTORS,cjson.encode({}))
        end
    },{
        '刷新角色', function(player)
            local player = actor_manager_fetch_local_player()
            if player then
                net_send_message(PTO_C2S_CREATE_PLAYER,cjson.encode(player:GetProperties()))
            end
        end
    },{
        '创建队伍', function(player)
            local player = actor_manager_fetch_local_player()
            if player then
                player:CreateTeam()
            end
        end
    },{
        '离开队伍', function(player)
            local player = actor_manager_fetch_local_player()
            if player then
                player:DismissTeam()
            end
        end
    },{
        '结束战斗', function(player)
            combat_system_end_battle()
        end
    },{
        'Say', function(player)
            player:Say('what the fuck')
        end
    },{ 
        'SetName', function(player)
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_NAME, %s)
            ]], PlayerNameSB:str() ))
        end
    },{ 
        'SetScene', function(player)
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_SCENE_ID, %d)
            ]], scene_manager_get_current_scene_id() ))
        end
    },{ 
        'BoundingBox', function(player)
            local show = player:GetProperty(PROP_SHOW_BOUNDINGBOX) 
            player:SetProperty(PROP_SHOW_BOUNDINGBOX , not show)
        end
    },{ 
        'AvatarInfo', function(player)
            local show = player:GetProperty(PROP_SHOW_AVATAR_INFO) 
            player:SetProperty(PROP_SHOW_AVATAR_INFO , not show)
        end
    },{ 
        '升级', function(player)
            local anim = animation_create(ADDONWDF,0x9B3AF4E5) 
            anim:SetLoop(-1)
            player:AddFrontAnim(anim)
        end
    },{
        '拉取召唤兽',function(player) 
            local msg = {}
            msg.pid = player:GetID()
            net_send_message(PTO_C2S_FETCH_SUMMON, cjson.encode(msg)) 
        end
    },{
        '满血',function(player) 
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_HP, %d) 
            ]], 10000 ))
        end
    },{
        '升级',function(player) 
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_LV, 100) 
                player:SetProperty(PROP_HP,1000) 

                  
            ]]))
        end
    },{
        '同步位置',function(player) 
            game_map_reset_map_offset()
            net_manager_player_dostring(string.format([[ 
                player:SetProperty(PROP_POS, {305,441}) 
                player:SetProperty(PROP_SCENE_ID, %d) 
            ]], scene_manager_get_current_scene_id()))
        end
    },{
        '重连服务器',function(player) 
            net_manager_reconnect()
        end
    },{
        '测试封印公式',function(player) 
            
        end
    },{
        '加速',function(player) 
            player:SetProperty(PROP_MOVE_VELOCITY, 750) 
        end
    },{
        '恢复',function(player) 
            player:SetProperty(PROP_MOVE_VELOCITY, 150) 
        end
    },{
        '屏幕上移动',function(player) 
            local x,y = player:GetPos()
            player:SetPos(x,y-10)
        end
    },{
        '屏幕下移动',function(player) 
            local x,y = player:GetPos()
            player:SetPos(x,y+10)
        end
    },{
        '屏幕左移动',function(player) 
            local x,y = player:GetPos()
            player:SetPos(x-10,y)
        end
    },{
        '屏幕右移动',function(player) 
            local x,y = player:GetPos()
            player:SetPos(x+10,y)
        end
    },{
        '创建传送点',function(player) 
            local x,y = player:GetPos()
            scene_write_id = scene_write_id  or 0
            local scenes  = content_system_get_table('scene')
            local line = string.format('%d\t%s=>%s\t%d\t%d,%d\n',
                scene_write_id,
                player:GetSceneName(),
                scenes[to_scene_id].name,
                player:GetProperty(PROP_SCENE_ID),
                x,y
            )
            scene_write_id = scene_write_id + 1
            cxlog_info('line ', line)
            local path = vfs_makepath('res/tables/transport.txt')
            local f = io.open(path,'a+')
            f:write(line)
            f:close()
        end
    },{
        '设置传送到的场景',function(player) 
            imgui.OpenPopup('PopupSetToTransportScene')
        end
    },{
        '传送到的场景',function(player) 
            imgui.OpenPopup('PopupTransportScene')
        end
    },{
        '重载战斗脚本',function(player) 
            script_system_dofile('../share/enums_protocol.lua')
            script_system_dofile('actor_metatable.lua')
            script_system_dofile('../share/actor_metatable.lua')
            script_system_dofile('../combat/combat_system.lua')
            combat_system_init()
            net_manager_player_dostring(string.format([[ 
                script_system_dofile('../share/enums_protocol.lua')
                script_system_dofile('../share/actor_metatable.lua')
                script_system_dofile('../combat/combat_system.lua')
                combat_system_init()
            ]]))
        end
    },{
        '战斗测试',function(player) 
            local msg = {}
            msg.pid = player:GetID()
            net_send_message(PTO_C2S_COMBAT_CREATE, cjson.encode(msg))
            player:StopMove()
        end
    }
}

function ui_show_bag()
    if not ui_is_show_bag then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('Window')

    if imgui.CollapsingHeader('命令##CMD', ImGuiTreeNodeFlags_DefaultOpen) then
        imgui.InputText("玩家名字", PlayerNameSB)
         
        local player = actor_manager_fetch_local_player()
        imgui_std_horizontal_button_layout(LocalPlayerDebugButtons,function(t,k) 
            local nk,v = next(t,k)
            return nk,v, nk and v[1]
        end,function(k,v)
            v[2](player)
        end)

        if imgui.BeginPopup('PopupSetToTransportScene') then
            imgui.InputText("Filter", ToSceneFilterSB)
            imgui.NewLine()
            local scenes  = content_system_get_table('scene')
            imgui.HorizontalLayout(scenes,next,function(k,v) 
                if ToSceneFilterSB:str()=='' 
                    and (v.name:match('帮派') 
                    or v.name:match('长安_')
                    or v.name:match('地板')
                    or v.name:match('宅')
                    or v.name:match('社区')
                    or v.name:match('店')
                    or v.name:match('坐标')
                    or v.name:match('%.')
                    ) 
                then 
                        return 
                end
                
                if v.name:match(ToSceneFilterSB:str()) and imgui.Button(v.name) then
                    to_scene_id = v.ID
                    imgui.CloseCurrentPopup()
                end
            end)
            imgui.EndPopup()
        end

        if imgui.BeginPopup('PopupTransportScene') then
            imgui.InputText("Filter", ToSceneFilterSB)
            imgui.NewLine()
            local scenes  = content_system_get_table('scene')
            imgui.HorizontalLayout(scenes,next,function(k,v) 
                if ToSceneFilterSB:str()=='' 
                    and (v.name:match('帮派') 
                    or v.name:match('长安_')
                    or v.name:match('地板')
                    or v.name:match('宅')
                    or v.name:match('社区')
                    or v.name:match('店')
                    or v.name:match('坐标')
                    or v.name:match('%.')
                    ) 
                then 
                        return 
                end
                if v.name:match(ToSceneFilterSB:str()) and imgui.Button(v.name) then
                    to_scene_id = v.ID
                    net_manager_player_dostring(string.format([[ 
                        player:SetProperty(PROP_SCENE_ID, %d)
                        player:SetPos(%f, %f)  
                    ]],to_scene_id,400,300))

                    player:SetProperty(PROP_SCENE_ID, to_scene_id)
                    player:SetPos(400,300) 
                    scene_manager_switch_scene_by_id(to_scene_id)   
                    imgui.CloseCurrentPopup()
                end
            end)
            imgui.EndPopup()
        end

        
    end

    if imgui.CollapsingHeader('染色') then
        if COLOR_SCHEMES then
            for i=1,#COLOR_SCHEMES.segments-1 do
                imgui.BeginGroup()
                local title = COLOR_SCHEMES.segments[i]..'-'..COLOR_SCHEMES.segments[i+1]
                imgui.AlignTextToFramePadding()
                imgui.Text(title)
                
                for mat_i,mat in ipairs(COLOR_SCHEMES[i]) do
                    local selected = imgui.RadioButton(mat_i..'##'..title ,mat_i == COLOR_SCHEMES_SELECT_MAP[i])
                    if selected then
                        COLOR_SCHEMES_SELECT_MAP[i] = mat_i
                        --选择矩阵 重新load actor的avatar
                        local new_pal = {}
                        new_pal.segments = COLOR_SCHEMES.segments
                        for seg_i=1,#COLOR_SCHEMES.segments-1 do
                            local mat_i = COLOR_SCHEMES_SELECT_MAP[seg_i]
                            local seg_mats = COLOR_SCHEMES[seg_i]
                            local mat  = seg_mats[mat_i]
                            local seg_pal = {}
                            seg_pal.from = COLOR_SCHEMES.segments[seg_i]
                            seg_pal.to = COLOR_SCHEMES.segments[seg_i+1]
                            seg_pal.mat = mat
                            table.insert(new_pal, seg_pal)
                        end
                        cxlog_info('new_pal  '.. cjson.encode(new_pal))
                        player:ChangePalMatrix(new_pal)

                        net_manager_actor_dostring(player:GetID(),string.format([=[
                            actor:SetProperty(PROP_PAL_MATRIX, %q)
                        ]=], cjson.encode(new_pal)))
                    end
                end
                                
                imgui.EndGroup()
                if i ~= #COLOR_SCHEMES.segments-1 then
                    imgui.SameLine()
                end
            end
        end
        
        local files = vfs_list_files(vfs_get_tablepath('wasee_pal'))
        local filenames = {}
        for k,f in pairs(files) do
            local  name = f:match('wasee_pal/(.+)%.')
            name = util_gb2312_to_utf8(name)
            table.insert(filenames,name)
        end

        imgui.HorizontalLayout(filenames,next,function(k,v)
            if imgui.Button(v) then
                select_pal = files[k]
                COLOR_SCHEMES = decode_mypal(select_pal)
                update_color_schemes_select_map()
                cxlog_info('...',cjson.encode(COLOR_SCHEMES))
                cxlog_info('...',cjson.encode(COLOR_SCHEMES_SELECT_MAP))
            end
        end)
    end
    
    if imgui.CollapsingHeader('角色形象') then
        local avatar_role_tbl = content_system_get_table('avatar_role')    
        local role_keys = fetch_role_keys(avatar_role_tbl)
        imgui_std_horizontal_button_layout(avatar_role_tbl,gen_next_sortk_fn(avatar_role_tbl), function(k,v)
            net_manager_player_dostring(string.format([[
                player:SetProperty(PROP_ACTOR_TYPE,ACTOR_TYPE_PLAYER)
                player:SetProperty(PROP_AVATAR_ID, '%s') 
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
            ]],k))
        end)
    end

    if imgui.CollapsingHeader('角色武器') then
        local avatar_weapon_tbl =  content_system_get_table('avatar_weapon')    
        local avatar_key = player:GetProperty(PROP_AVATAR_ID)
        local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
        imgui_std_horizontal_button_layout(avatar_weapon_tbl,custom_gen_next_sortk_fn(avatar_weapon_tbl,keys), function(k,v)
            net_manager_player_dostring(string.format([[
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'%s')
            ]],k))
        end)

        if imgui.Button('DropWeapon') then
            net_manager_player_dostring([[
                player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
            ]])
        end
    end
    if imgui.CollapsingHeader('门派') then
        local school = content_system_get_table('school')
        imgui.HorizontalLayout(school,next,function(k,v) 
            if imgui.Button(v.name..'##'..v.ID) then
                net_manager_player_dostring(string.format([[ player:SetProperty(PROP_SCHOOL, %d) ]],v.ID))
            end
        end)
    end

    imgui.End()
end
