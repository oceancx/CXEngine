
transports =  transports or {}
scene_list = scene_list or {}
	
function on_scene_manager_init()
    local scene_tbl = content_system_get_table('scene')
    for id, row in pairs(scene_tbl) do
        scene_manager_add_custom_scene(id, row.name, row.map_id)

        if row.script ~= '' then
            local path = vfs_get_luapath(row.script)
            local module = {
                exports = {},
                env = _ENV
            } 
            scene_list[row.name] = module

            setmetatable(module,{ __index = function(t,k)
                local env = rawget(t, "env")
                local v = env[k] ; if v then return v end
                local exports = rawget(t, "exports")
                local v = rawget(exports, k); if v then return v end 
            end})
    
            local fun,err = loadfile(path,'bt',module)
            if fun then
                fun()
            else
                cxlog_info(fun,err)
            end
        end
    end
end

function on_scene_manager_uninit_scene(name)
    local scene_id = scene_manager_get_current_scene_id() 
    local scene_tbl = content_system_get_table('scene')
    audio_manager_stop(scene_tbl[scene_id].bgm)
    timer_manager_remove_timer('TimerWildBattle')
end

function on_scene_manager_init_scene(name)
    cxlog_info('on_scene_manager_init_scene', name)
    local scene_id = scene_manager_get_current_scene_id() 
    local transport_tbl = content_system_get_table('transport')

    local scene_tbl = content_system_get_table('scene')
    local scene_monster_tbl = content_system_get_table('scene_monster')
    audio_manager_play(scene_tbl[scene_id].bgm, true)

    for ID, v in pairs(transports) do
        if v.anim then
            animation_destroy(v.anim)
        end
    end
    transports = {}
    for ID, v in pairs(transport_tbl) do
        if v.scene == scene_id then
            v.player_outside = false
            transports[ID] = v
        end
        v.anim = animation_create(MAPANIWDF,0x7F4CBC8C)
        -- v.anim:SetPos(v.pos.x + offx,v.pos.y + offy)
        v.anim:SetFrameInterval(0.128)
        v.anim:Reset()
        v.anim:SetLoop(0)
        v.anim:Play()
    end

    local player = actor_manager_fetch_local_player()
    if player then
        player:ClearAction()
        player:PushAction(ACTION_IDLE)
        player:StopMove()
    end
    if scene_list[name] then
        scene_list[name].OnSceneInit() 
    end
    
    if scene_monster_tbl[name] and not name:match('大雁塔%d层') then
        timer_manager_add_timer('TimerWildBattle',15000,function()
            local player = actor_manager_fetch_local_player()
            if not player:GetProperty(PROP_SETTING_WILD_BATTLE) then return end
            if player:IsCombat() then
                timer_manager_end_timer('TimerWildBattle')
                timer_manager_pause_timer('TimerWildBattle',true)
                return
            else
                if player:IsMoving() then
                    local req = {}
                    req.pid = player:GetID()
                    net_send_message(PTO_C2S_COMBAT_PVE_START, cjson.encode(req))
                end
            end
        end, true)    
    end
end

function on_scene_manager_update(name)
    on_game_imgui_update()

    if scene_list[name] then
        scene_list[name].OnSceneUpdate()
    end

    local player = actor_manager_fetch_local_player()
    if player and not player:IsCombat() then
        local x,y = player:GetPos()
        for ID,trans in pairs(transports) do
            if math_get_distance(x,y,trans.pos.x ,trans.pos.y) <= 100 then
                if trans.player_outside then
                    local transport_tbl = content_system_get_table('transport')
                    local to_trans = transport_tbl[trans.to_station]

                    net_manager_player_dostring(string.format([[ 
                        player:SetProperty(PROP_SCENE_ID, %d)
                        player:SetPos(%f, %f)  
                    ]], to_trans.scene,to_trans.pos.x, to_trans.pos.y))


                    player:SetProperty(PROP_SCENE_ID,to_trans.scene)
                    player:SetPos(to_trans.pos.x, to_trans.pos.y) 
                
                    scene_manager_switch_scene_by_id(to_trans.scene)   
                    trans.player_outside = false
                    break 
                end
            else
                trans.player_outside = true
            end
        end
    end
    for k,v in pairs(transports) do
        if v.anim then
            v.anim:Update()
        end
    end
end

function on_scene_manager_draw(name)
    local offx,offy = scene_get_map_offset()
    for k,v in pairs(transports) do
        if v.anim then
            -- + v.anim:GetWidth()/2
            -- + v.anim:GetHeight()/2
            v.anim:SetPos(v.pos.x + offx ,v.pos.y + offy )
            v.anim:Draw()
        end
    end
    if ui_show_options_on_draw then
        ui_show_options_on_draw()
    end
    if scene_list[name] then
        scene_list[name].OnSceneDraw()
    end
                
end

function scene_manager_reload(name)
    on_scene_manager_uninit_scene(name)
    on_scene_manager_init_scene(name)
end

local KEY_RELEASE_MOVE_AMOUT = 30
function fix_input_manager_mouse_pos()
    local wx,wy = imgui.GetWindowPos()
    local vid,x,y  = imgui.GetMainViewport()
    input_manager_set_window_pos(wx-x,wy-y)
end

function on_game_imgui_update()
    local name = scene_manager_get_current_scene_name()
    fix_input_manager_mouse_pos()
    
    if scene_list[name] then
        if scene_list[name].OnSceneImGuiUpdate then
            scene_list[name].OnSceneImGuiUpdate()
        end
    end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    if player:IsCombat() then
        combat_system_imgui_update()
    end

    local actors = actor_manager_fetch_all_actors()
    for i,actor in ipairs(actors) do
        if actor:GetProperty(PROP_SHOW_BOUNDINGBOX) then
            actor:DrawBoundingBox()
        end
        if actor:GetProperty(PROP_SHOW_AVATAR_INFO) then
            local x ,y  = actor:GetPos()
            local avatar = actor:GetAvatar()
            if not avatar then return end
            imgui.SetCursorPos(x-55,y+50)
            imgui.BeginGroup()
            actor:DrawAvatarInfo()
            imgui.EndGroup()
        end
    end
    addon_manager_imgui_update()
end

 