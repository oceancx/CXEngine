__battles__ = __battles__ or {}
battle = nil

local ActorMT = actor_get_metatable()
function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

local battle_commands = {}
local ACTOR_CLICK_MODE_ATTACK = 1
local ACTOR_CLICK_MODE_SPELL = 2
local ACTOR_CLICK_MODE  = ACTOR_CLICK_MODE_ATTACK

BattleBG = BattleBG or nil
combat_self_pos = combat_self_pos or {}
combat_enemy_pos = combat_enemy_pos or {}
function calc_combat_self_pos(ratio_x, ratio_y)
	return {
        { x = 465.0 * ratio_x, y = 315.0 * ratio_y },
		{ x = 405.0 * ratio_x, y = 345.0 * ratio_y },
		{ x = 525.0 * ratio_x, y = 285.0 * ratio_y },
		{ x = 345.0 * ratio_x, y = 375.0 * ratio_y },
		{ x = 585.0 * ratio_x, y = 255.0 * ratio_y },
		{ x = 415.0 * ratio_x, y = 275.0 * ratio_y },
		{ x = 355.0 * ratio_x, y = 305.0 * ratio_y },
		{ x = 475.0 * ratio_x, y = 245.0 * ratio_y },
		{ x = 295.0 * ratio_x, y = 335.0 * ratio_y },
		{ x = 535.0 * ratio_x, y = 215.0 * ratio_y },
	}
end

function calc_combat_enemy_pos(ratio_x, ratio_y)
	return {
        { x = 175.0 * ratio_x, y = 170.0 * ratio_y },   
		{ x = 115.0 * ratio_x, y = 200.0 * ratio_y },   
		{ x = 235.0 * ratio_x, y = 140.0 * ratio_y },   
		{ x = 55.0  * ratio_x, y = 230.0 * ratio_y },  
		{ x = 295.0 * ratio_x, y = 110.0 * ratio_y },   
        { x = 220.0 * ratio_x, y = 210.0 * ratio_y },   
		{ x = 160.0 * ratio_x, y = 240.0 * ratio_y },   
		{ x = 280.0 * ratio_x, y = 180.0 * ratio_y },   
		{ x = 100.0 * ratio_x, y = 270.0 * ratio_y },   
		{ x = 340.0 * ratio_x, y = 150.0 * ratio_y },   
	}
end

function combat_system_init()
    cxlog_info('combat_system_init')
    BattleBG = animation_create(ADDONWDF, 0xE3B87E0F)
    local ratio_x = game_get_width()/ 640 
	local ratio_y = game_get_height()/ 480
    combat_self_pos =  calc_combat_self_pos(ratio_x, ratio_y)
    combat_enemy_pos =  calc_combat_enemy_pos(ratio_x, ratio_y)
    init_skills()
    init_buffs()
end


local current_master
function combat_system_actor_ev_on_click(actor, button, x, y)
    if not battle or battle.state ~= BATTLE_TURN_STAND_BY then return end
    if not battle:InBattle(actor) then return end
    if not current_master then return end
    current_master:SetTarget(actor)

    if ACTOR_CLICK_MODE == ACTOR_CLICK_MODE_ATTACK then
        current_master:SetProperty(PROP_USING_SKILL,1)
    end
    local msg = {}
    msg.master = current_master:GetID()
    msg.target = actor:GetID()
    msg.skill_id = current_master:GetProperty(PROP_USING_SKILL)
    msg.battle_id = battle.id
    assert(msg.skill_id ~= 0 and msg.skill_id < 300)
    net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg))
    ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_ATTACK
    current_master:SetProperty(PROP_TURN_READY, true)
end

function combat_reset_actor(actor)
    actor:SetProperty(PROP_ASM_BUFF_ANIM, 0)
    actor:SetProperty(PROP_IS_COMBAT,false)
    actor:SetProperty(PROP_CAN_MOVE,true)
    actor:ClearBackAnim()
    actor:ClearFrontAnim()
    actor:StopMove()
    actor:ClearAction()
    actor:PushAction(ACTION_IDLE)   
end

function combat_system_draw()
    if not battle then return end
    BattleBG:Draw()

    local drawActors = {}
    for i,actor in ipairs(battle.actors) do
        table.insert(drawActors,actor)
    end
    table.sort(drawActors,function(a,b) return a:GetY() < b:GetY() end)

    for i,actor in ipairs(drawActors) do
        actor:Draw()
    end
    animation_manager_draw()
end


function combat_system_imgui_update()
    if not battle or battle.state ~= BATTLE_TURN_STAND_BY then return end

    local player = actor_manager_fetch_local_player()
    if not player:GetProperty(PROP_TURN_READY) then
        current_master = player
    else
        local summon = player:GetSummon()
        if summon then
            if not summon:GetProperty(PROP_TURN_READY) then
                current_master = summon
            else
                current_master = nil
            end
        else
            current_master = nil
        end
    end

    if current_master then
        if current_master:IsPlayer() then
            local vid,x,y  = imgui.GetMainViewport()
            imgui.SetNextWindowPos(x+640,y+100)
            imgui.Begin('Menu',menu_show)

            if imgui.Button('法术##player') then
                imgui.OpenPopup('SpellSelector')
                ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_SPELL
            end
            if imgui.BeginPopup('SpellSelector') then
                local skill_tbl = content_system_get_table('skill')
                local school = current_master:GetProperty(PROP_SCHOOL)
                local school_skill = {}
                for id,row in pairs(skill_tbl) do
                    if row.school == school then
                        school_skill[id] = row
                    end
                end
                imgui.HorizontalLayout(school_skill,next,function(k,v) 
                    if imgui.Button(v.name..'##'..v.ID) then
                        local current_master = actor_manager_fetch_local_player()
                        current_master:SetProperty(PROP_USING_SKILL, v.ID)
                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup('SpellSelector')
            end

            if imgui.Button('特技##player') then
    
            end

            if imgui.Button('道具##player') then

            end

            if imgui.Button('防御##player') then
                local msg = {}
                msg.master = current_master:GetID()
                msg.skill_id = 264
                msg.battle_id = battle.id
                net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
            end
            if imgui.Button('召唤##player') then
    
            end
            if imgui.Button('召还##player') then
    
            end
    
            if imgui.Button('捕捉##player') then
    
            end

            if imgui.Button('逃跑##player') then
                current_master:SetProperty(PROP_USING_SKILL,268)

                local msg = {}
                msg.master = current_master:GetID()
                msg.skill_id = 268
                msg.battle_id = battle.id
                net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
            end

        
            imgui.End()
        else
            local vid,x,y  = imgui.GetMainViewport()
            imgui.SetNextWindowPos(x+640,y+100)
            imgui.Begin('Menu2',menu_show)

            if imgui.Button('法术##summon') then
                imgui.OpenPopup('SpellSelector')
                ACTOR_CLICK_MODE = ACTOR_CLICK_MODE_SPELL
            end
            if imgui.BeginPopup('SpellSelector') then
                local skill_tbl = content_system_get_table('skill')
                local school = current_master:GetProperty(PROP_SCHOOL)
                local school_skill = {}
                for id,row in pairs(skill_tbl) do
                    if row.school == school then
                        school_skill[id] = row
                    end
                end
                imgui.HorizontalLayout(school_skill,next,function(k,v) 
                    if imgui.Button(v.name..'##'..v.ID) then
                        local current_master = actor_manager_fetch_local_player()
                        current_master:SetProperty(PROP_USING_SKILL, v.ID)
                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup('SpellSelector')
            end


            if imgui.Button('道具##player') then

            end

            if imgui.Button('防御##player') then
                local msg = {}
                msg.master = current_master:GetID()
                msg.skill_id = 264
                msg.battle_id = battle.id
                net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
            end

            if imgui.Button('逃跑##player') then
                current_master:SetProperty(PROP_USING_SKILL,268)

                local msg = {}
                msg.master = current_master:GetID()
                msg.skill_id = 268
                msg.battle_id = battle.id
                net_send_message(PTO_C2S_COMBAT_CMD, cjson.encode(msg) )
            end
        
            imgui.End()
        end
    end

    for i,actor in ipairs(battle.actors) do
        local x,y,w,h = actor:GetAvatarRect()
        imgui.SetCursorPos(x,y-10)
        local max_hp = actor:GetMaxHP()
        local hp = actor:GetHP()
        imgui.TextFormat('%.f/%.f',hp,max_hp)
    end
end

local stub = net_manager_stub()

stub[PTO_S2C_COMBAT_CREATE] = function(resp)
    ui_renderer_clear()
    
    battle = BattleMT:new()
    battle:Deserialize(resp.battle)

    __battles__[battle.id] = battle
    
    local player = actor_manager_fetch_local_player()
    for i, actor in ipairs(battle.actors) do
        if player:GetID() == actor:GetID() then
            battle.local_team_type = actor:GetProperty(PROP_TEAM_TYPE)
            battle:PrepareBattle()
            return
        end
    end
end

stub[PTO_S2C_COMBAT_START] = function(resp)
    battle = BattleMT:new()
    battle:Deserialize(resp.battle)

    __battles__[battle.id] = battle
    
    local player = actor_manager_fetch_local_player()
    for i, actor in ipairs(battle.actors) do
        if player:GetID() == actor:GetID() then
            battle.local_team_type = actor:GetProperty(PROP_TEAM_TYPE)
            battle:StartBattle()
            return
        end
    end
end 

stub[PTO_S2C_COMBAT_EXECUTE] = function(all_skills)
    battle_commands = {}
    for i,cskill in ipairs(all_skills) do
        local skill = cskill_to_skill(cskill)
        skill.state = SKILL_STATE_DEFAULT
        table.insert(battle_commands,skill)
    end
    battle.state = BTTALE_TURN_EXECUTE
end

function reset_actor_pos(self)
    local init_actor = function(actor, pos, dir)
        actor:SetCombatPos(pos.x,pos.y)
        actor:SetDir(dir)
        actor:ClearAction()
        actor:SetProperty(PROP_IS_COMBAT,true)
        actor:PushAction(ACTION_BATIDLE)
    end

    for i,actor in ipairs(self.actors) do
        local pos_i = actor:GetProperty(PROP_COMBAT_POS_ID)
        if actor:GetProperty(PROP_TEAM_TYPE) == battle.local_team_type then
            local pos = combat_self_pos[pos_i]
            init_actor(actor, pos, DIR_NW)
        else
            local pos =  combat_enemy_pos[pos_i]
            init_actor(actor, pos, DIR_SE)
        end
    end
end


function on_battle_prepare(self)
    reset_actor_pos(self)
end


function on_battle_start(self)
    reset_actor_pos(self)
    local player = actor_manager_fetch_local_player()
    player:StopMove()
    local bgm = player:GetSceneBGMName()
    audio_manager_stop(bgm)
    audio_manager_play('10-8905924A', true)
end


function combat_system_update()
    if battle then
        if battle.state == BTTALE_TURN_EXECUTE then
            if #battle_commands > 0 then
                local skill = battle_commands[1]
                if skill.state == SKILL_STATE_DEFAULT then
                    on_using_skill(battle, skill)
                elseif skill.state == SKILL_STATE_START then
                    on_using_skill_update(battle, skill)
                elseif skill.state == SKILL_STATE_END then
                    table.remove(battle_commands,1)    
                end

                local player = actor_manager_fetch_local_player()
                if not battle:FindActor(player:GetID()) then
                    battle:EndBattle()
                    return
                end
            else 
                if battle:CheckEnd() then
                    battle:EndBattle()
                    return
                else
                    battle:NextTurn()
                end
            end
        end

        for i,actor in ipairs(battle.actors) do
            actor:Update()
        end
        animation_manager_update()
    end
end

function combat_system_get_battle()
    return battle
end

function combat_system_remove_from_battle(_actor_)
    if not battle then return end

    combat_reset_actor(_actor_)
    battle:RemoveActor(_actor_)
end

stub[PTO_S2C_COMBAT_END_BATTLE] = function(resp)
    battle:EndBattle()
end 

function combat_system_end_battle()
    if not battle then return end
    local player = actor_manager_fetch_local_player()
    net_send_message(PTO_C2S_COMBAT_END_BATTLE, cjson.encode( { pid = player:GetID() , battle_id = battle.id}) ) 
end

