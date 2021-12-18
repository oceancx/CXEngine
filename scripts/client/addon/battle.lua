
local ui_is_show_battle = false
function ui_toggle_show_battle()
    ui_is_show_battle = not ui_is_show_battle
    return ui_is_show_battle
end



local edit_actor_is_local_team_type = true
local select_team_type = 0
local select_team_place = 1
function draw_battle_actor_editor()
    -- 添加的actor 要分敌方 还是友方
    if imgui.RadioButton('友方', select_team_type == battle.local_team_type) then
        select_team_type = battle.local_team_type
    end
    imgui.SameLine()
    if imgui.RadioButton('敌方', select_team_type ~= battle.local_team_type) then
        if battle.local_team_type== TEAM_TYPE_ATTACKER then
            select_team_type = TEAM_TYPE_DEFENDER
        else
            select_team_type = TEAM_TYPE_ATTACKER
        end
    end

    for i=1,10 do
        if imgui.RadioButton(i..'##select_team_place', i == select_team_place) then
            select_team_place = i
        end
        imgui.SameLine()
    end
    imgui.NewLine()

    if imgui.Button('添加Actor') then
        local actor = actor_manager_create_actor(os.time())
        local pos
        local dir 
        if select_team_type == TEAM_TYPE_ATTACKER then
            pos = combat_self_pos[select_team_place]
            dir = DIR_NW
        else
            pos = combat_enemy_pos[select_team_place]
            dir = DIR_SE
        end
        actor:SetProperty(PROP_NAME, string.format('%d', actor:GetID()))
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:SetPos(pos.x,pos.y)
        actor:SetProperty(PROP_COMBAT_POS_ID, select_team_place)
        actor:SetDir(dir)
        actor:ClearAction()
        actor:PushAction(ACTION_BATIDLE)

        -- player:SetProperty(PROP_ACTOR_TYPE,ACTOR_TYPE_PLAYER)
        -- player:SetProperty(PROP_AVATAR_ID, '%s') 
        -- player:SetProperty(PROP_WEAPON_AVATAR_ID,'')
        actor:ResetASM()
        battle:AddActor(actor, select_team_type, select_team_place)        
    end
end

local select_actor = nil
function ui_show_battle()
    if not ui_is_show_battle then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    local battle = player:GetBattle()
    if not battle then return end
    imgui.Begin('战斗编辑器')
        if imgui.CollapsingHeader('CMD') then
            if imgui.Button('战斗重载') then
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
                imgui.End()
                return
            end            

            imgui.SameLine()
            if imgui.Button('战斗测试') then
                local player = actor_manager_fetch_local_player()
                if player then
                    local msg = {}
                    msg.pid = player:GetID()
                    net_send_message(PTO_C2S_COMBAT_CREATE, cjson.encode(msg))
                end
            end
            imgui.SameLine()
            if imgui.Button('结束战斗') then
                combat_system_end_battle()
            end
        end

        if imgui.CollapsingHeader('Battle状态') then
            imgui.Text('Battle：'..battle_get_state_name(battle.state))

            imgui.Text('敌方')
            for i,actor in ipairs(battle.actors) do
                if battle.local_team_type ~= actor:GetProperty(PROP_TEAM_TYPE) then
                    if imgui.Button(actor:GetProperty(PROP_NAME)) then
                        select_actor = actor
                    end
                    imgui.SameLine()
                end
            end
            imgui.NewLine()
            imgui.Text('友方')
            for i,actor in ipairs(battle.actors) do
                if battle.local_team_type == actor:GetProperty(PROP_TEAM_TYPE) then
                    if imgui.Button(actor:GetProperty(PROP_NAME)) then
                        select_actor = actor
                    end
                    imgui.SameLine()
                end
            end
            imgui.NewLine()

            if select_actor then
                local actor = select_actor
                imgui.Text(actor:GetName())
                imgui.SameLine()
                imgui.Text(string.format('HP:%.f/%.f',actor:GetProperty(PROP_HP), actor:GetMaxHP()))
                imgui.SameLine()
                imgui.Text(string.format('MP:%.0f/%.0f',actor:GetProperty(PROP_MP),actor:GetMaxMP()))
                if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
                    imgui.SameLine()
                    imgui.Text(string.format('SP:%.0f/%.0f',actor:GetProperty(PROP_SP), 150))
                end
                
                if actor:GetProperty(PROP_TURN_READY) then
                    imgui.Text('准备完成')
                    if imgui.Button('取消准备') then
                        net_manager_actor_dostring(actor:GetID(),[[
                            actor:SetProperty(PROP_TURN_READY, false)
                        ]])
                    end
                else
                    imgui.Text('准备中')
                    imgui.Text('选择目标') 
               
                    imgui.HorizontalLayout(battle.actors,next,function(k,v)
                        if imgui.Button(v:GetProperty(PROP_NAME)..'##BESetTarget') then
                            actor:SetTarget(v)
                        end
                    end)
                    local target = actor:GetTarget()
                    if target and actor ~= target then
                        imgui.Text('目标:'..target:GetName())
                    end

                    if imgui.Button('攻击##BattleE') then
                        if target then
                            local msg = {}
                            msg.master = actor:GetID()
                            msg.target = target:GetID()
                            msg.skill_id = 1
                            msg.battle_id = battle.id
                            assert(msg.skill_id ~= 0 and msg.skill_id < 300)
                            net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg))
                        end
                    end
                    imgui.SameLine()

                    if imgui.Button('法术##BattleE') then
                        if target then
                            imgui.OpenPopup('PopupBattleSpellSelector')
                        end
                    end
                    if imgui.BeginPopup('PopupBattleSpellSelector') then
                        local skill_tbl = content_system_get_table('skill')
                        local school = actor:GetProperty(PROP_SCHOOL)
                        local school_skill = {}
                        for id,row in pairs(skill_tbl) do
                            if row.school == school then
                                school_skill[id] = row
                            end
                        end
                        imgui.HorizontalLayout(school_skill,next,function(k,v) 
                            if imgui.Button(v.name..'##'..v.ID) then
                                local msg = {}
                                msg.master = actor:GetID()
                                msg.target = target:GetID()
                                msg.skill_id = v.ID
                                msg.battle_id = battle.id
                                assert(msg.skill_id ~= 0 and msg.skill_id < 300)
                                net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg))

                                imgui.CloseCurrentPopup()
                            end
                        end)
                        imgui.EndPopup('PopupBattleSpellSelector')
                    end

                    imgui.SameLine()
                    if imgui.Button('特技##BattleE') then
    
                    end
                    imgui.SameLine()
                    if imgui.Button('道具##BattleE') then
        
                    end
                    imgui.SameLine()
                    if imgui.Button('防御##BattleE') then
                        local msg = {}
                        msg.master = actor:GetID()
                        msg.skill_id = 264
                        msg.battle_id = battle.id
                        net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
                    end
                    imgui.SameLine()
                    if imgui.Button('自动##BattleE') then
                        local msg = {}
                        msg.master = actor:GetID()
                        msg.skill_id = 270
                        msg.battle_id = battle.id
                        net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
                    end
                    imgui.SameLine()
                    if imgui.Button('召唤##BattleE') then
    
                    end
                    imgui.SameLine()
                    if imgui.Button('召还##BattleE') then
            
                    end
                    imgui.SameLine()
                    if imgui.Button('捕捉##BattleE') then
            
                    end
                    imgui.SameLine()
                    if imgui.Button('逃跑##BattleE') then
                        actor:SetProperty(PROP_USING_SKILL,268)
                        local msg = {}
                        msg.master = actor:GetID()
                        msg.skill_id = 268
                        msg.battle_id = battle.id
                        net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
                    end
                end
            end
        end
        if imgui.CollapsingHeader('Actor编辑') then
            draw_battle_actor_editor()
        end
    imgui.End()
end