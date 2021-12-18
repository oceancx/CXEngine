
skill_table = {}

local skill_env = {
    __index = _ENV
}

function init_skills()
    local skill_template_tbl = content_system_get_table('skill')
    skill_table = skill_template_tbl
    for id,skill in pairs(skill_table) do
        local dir = vfs_get_luapath('../combat/skill/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},skill_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                skill.SkillOnStart = module.SkillOnStart
                skill.SkillOnEnd = module.SkillOnEnd
                skill.SkillOnHit = module.SkillOnHit
            else
                cxlog_info(fun,err)
            end
        end 
    end
end

function skill_init_by_templ(skill, templ)
    skill.name = templ.name
    skill.sub_type = templ.sub_type
    skill.atk_anim = templ.atk_anim
    skill.group_kill = templ.group_kill 	--是否群体攻击
    skill.combo = templ.combo				--是否连击
    skill.type = templ.type				
    skill.sound = templ.sound				
	skill.SkillOnStart = templ.SkillOnStart
	skill.SkillOnEnd = templ.SkillOnEnd
	skill.SkillOnHit = templ.SkillOnHit
end

function battle_get_group_kill_targets(battle, group_kill, main_target)
    assert(main_target~=nil)
	local targets = {}
	if not main_target:IsDead() then
		table.insert(targets, main_target)
	end

    if group_kill > 0 then
        local candidates = {}
        for i,actor in ipairs(battle.actors) do
            if actor:GetProperty(PROP_TEAM_TYPE) == main_target:GetProperty(PROP_TEAM_TYPE)
                and not actor:IsDead() 
                and actor:GetID() ~= main_target:GetID() then
                table.insert(candidates, actor)
            end
        end
        if #candidates > 0 then
            table.sort(candidates, function(a,b) return a:CalcSpeed() > b:CalcSpeed() end)
            for i=1,group_kill-1 do
                local which = math.random(1,#candidates)
                table.insert(targets, candidates[which])
                table.remove(candidates,which)
                if #candidates == 0 then
                    break
                end
            end
        end
    end

	return targets
end

function serialize_skill(skill)
    return ret
end

function deserialize_skill(skill)
    return ret
end

function skill_play_target_dead_fly(skill, behit_action, master, target)
    behit_action:Reset()
    behit_action:Play()
    behit_action:SetLoop(0)

    master:SetTarget(target)

    local last_x, last_y = target:GetPos()
    local last_dir = target:GetDir()
    local dx, dy = 0,0
    local dir_x ,dir_y = master:GetAttackVec()
    local fly_x ,fly_y = 0,0 
    behit_action:AddUpdateCallback(function()
        local actor = target
        local px, py = actor:GetPos()
        local avatar = actor:GetAvatar()
        if py - avatar:GetFrameKeyY() <= 0 then
            dir_y = -dir_y
        end

        if py - avatar:GetFrameKeyY() + avatar:GetFrameHeight()  >= 600 then
            dir_y = -dir_y
        end
        
        if avatar:IsFrameUpdate() then
            px = px + dir_x * 49
            py = py + dir_y * 49
            actor:SetCombatPos(px,py)
        end

        if avatar:IsGroupEndUpdate() then
            local dir = actor:GetDir()
            dir = math_next_dir4(dir)
            actor:SetDir(dir)
        end
        if px - avatar:GetFrameKeyX() < 0 then
            behit_action:RemoveUpdateCallback()
            behit_action:Stop()
            actor:SetCombatPos(last_x,last_y)
            actor:SetDir(last_dir)
            if skill.type == 'spell' then
                skill_target_end_counter(skill)
            elseif skill.type == 'atk' then
                skill.target_end = true        
            end
            combat_system_remove_from_battle(target)
        end

        if px - avatar:GetFrameKeyX() + avatar:GetFrameWidth() >= 800 then
            behit_action:RemoveUpdateCallback()
            behit_action:Stop()
            actor:SetCombatPos(last_x,last_y)
            actor:SetDir(last_dir)
            if skill.type == 'spell' then
                skill_target_end_counter(skill)
            elseif skill.type == 'atk' then
                skill.target_end = true        
            end
            combat_system_remove_from_battle(target)
        end
    end)
end

function on_attack_action_callback(attack_action)
    local skill = attack_action.skill
    local target = attack_action.target
    local atk_info = attack_action.atk_info
    atk_info.atk_counter = atk_info.atk_counter + 1
    local master = skill.master
    master:SetTarget(target)

    if master == target then
        cxlog_info('怎么打到自己了???')
        skill.target_end = true
        return
    end
    local behit_action = target:GetAvatar(ACTION_BEHIT)
    behit_action:Reset()
    behit_action:SetLoop(-1)
    behit_action:AddFrameCallback(1, function()
        local avatar = target:GetAvatar()
        local pack, was = res_decode(skill.atk_anim)
        local anim = animation_create(pack,was)
        local group_time = anim:GetGroupFrameTime()
        cxlog_info('behit_action:AddFrameCallback',pack,was,group_time)
        skill.anim = anim
        anim:SetLoop(-1)
        local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        anim:SetOffsetY(offy)  
        
        if skill.SkillOnHit then
            skill.SkillOnHit(skill, master, target, skill.group_kill_counter, atk_info.atk_counter)
        end
        target:PlaySound('behit')

        target:AddFrontAnim(anim)
        
        local damage = atk_info.hp_deltas[atk_info.atk_counter].target
        target:ShowBeatNumber(damage)
        -- actor:ShowBeatNumber(damage)

        attack_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))
        behit_action:Pause(math.floor(anim:GetGroupFrameTime()* 1000))

        local dir_x ,dir_y = skill.atk_dir_x, skill.atk_dir_y
        cxlog_info('behit_action:AddFrameCallback',pack,was,anim:GetGroupFrameTime())
        target:MoveOnScreenWithDuration(dir_x*24,dir_y*24, anim:GetGroupFrameTime() ,true)
    end)
    
    behit_action:AddStopCallback(function()
        if atk_info.atk_counter == atk_info.combo then
            if atk_info.life_state.target == ACTOR_LIFE_DEAD_FLY then                
                skill_play_target_dead_fly(skill, behit_action, master, target)
            elseif atk_info.life_state.target == ACTOR_LIFE_DEAD then
                local clps_action = target:GetAvatar(ACTION_CLPS)
                clps_action:Reset()
                clps_action:SetLoop(0, 1)                                
                clps_action:AddStartCallback(function(anim)
                    target:PlaySound('clps')
                end)               
                clps_action:AddFrameCallback(clps_action:GetGroupFrameCount(),  function()
                    skill.target_end = true
                end)
                target:PushAction(ACTION_CLPS)
                target:MoveActionToBack()
            else
                local dir_x ,dir_y = skill.atk_dir_x, skill.atk_dir_y
                target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
                skill.target_end = true   
            end
        else 
            local dir_x ,dir_y = skill.atk_dir_x, skill.atk_dir_y
            target:MoveOnScreenWithDuration(-dir_x*24,-dir_y*24,PERFRAME_TIME*2,true)
        end
    end)
    target:PushAction(ACTION_BEHIT)
    target:MoveActionToBack()
end

function skill_target_end_counter(skill)
    skill.target_counter = skill.target_counter - 1
    if skill.target_counter == 0 then
        skill.target_end = true
    end
end


function skill_create_spell_anim(skill, effect, target)
    local resid = skill.atk_anim 
    local pack, was = res_decode(resid)
    local anim = animation_create(pack,was)
    skill.anim = anim
    anim:SetLoop(-1)
    if skill.sub_type == SKILL_SUBTYPE_HEAL then
        anim:SetOffsetY(-20)
    elseif skill.sub_type == SKILL_SUBTYPE_SEAL then
        anim:SetOffsetY(-30)
    end
    
    if skill.sub_type ~= SKILL_SUBTYPE_DEFAULT then
        anim:AddStopCallback(function()
            if skill.SkillOnHit then
                skill.SkillOnHit(skill, master, target, target_i, skill.spell_combo_counter)
            end
            if skill.sub_type == SKILL_SUBTYPE_HEAL then
                local hp_delta = effect.hp_deltas[skill.spell_combo_counter].target
                target:ShowBeatNumber(hp_delta)
            end
            skill_target_end_counter(skill)
        end)
    else
        anim:AddFrameCallback(anim:GetGroupFrameCount()/2, function()
            if skill.SkillOnHit then
                skill.SkillOnHit(skill, master, target, target_i, skill.spell_combo_counter)
            end
            target:PlaySound('behit')
            local hp_delta = effect.hp_deltas[skill.spell_combo_counter].target
            target:ShowBeatNumber(hp_delta)
        end)
    end
    target:AddFrontAnim(anim)
    return anim
end

function skill_cast_spell(battle, skill)
    skill.target_counter = #skill.effects
    skill.caster_end = false
    skill.target_end = false
    skill.spell_combo_counter = skill.spell_combo_counter + 1

    audio_manager_play(skill.sound)
    
    local master = skill.master
    local cast_action = master:GetAvatar(ACTION_CAST)
    cast_action:Reset()
    cast_action:SetLoop(-1)
    cast_action:AddStartCallback(function(anim)
        master:PlaySound('cast') 
    end)
    cast_action:AddFrameCallback(cast_action:GetGroupFrameCount()/2,function()
        for target_i,effect in ipairs(skill.effects) do    
            local target = battle:FindActor(effect.target_id)
            if skill.spell_combo_counter <= effect.combo then
                if skill.sub_type ~= SKILL_SUBTYPE_DEFAULT then
                    skill_create_spell_anim(skill, effect, target) 
                else
                    local behit_action = target:GetAvatar(ACTION_BEHIT)
                    behit_action:Reset()
                    behit_action:SetLoop(1)
                    behit_action:AddFrameCallback(1, function()
                        skill_create_spell_anim(skill, effect, target)
                        behit_action:Pause(math.floor(skill.anim:GetGroupFrameTime()* 1000))
                    end)

                    behit_action:AddStopCallback(function()
                        if effect.combo==skill.spell_combo_counter then
                            if effect.life_state.target == ACTOR_LIFE_DEAD_FLY then
                                skill_play_target_dead_fly(skill, behit_action, master, target)
                            elseif effect.life_state.target == ACTOR_LIFE_DEAD then
                                local clps_action = target:GetAvatar(ACTION_CLPS)
                                clps_action:Reset()
                                clps_action:SetLoop(0, 1)      
                                clps_action:AddStartCallback(function(anim)
                                    target:PlaySound('clps')
                                end)               
                                clps_action:AddFrameCallback(clps_action:GetGroupFrameCount(),  function()
                                    skill_target_end_counter(skill)
                                end)
                                target:PushAction(ACTION_CLPS)
                                target:MoveActionToBack()
                            else
                                skill_target_end_counter(skill)
                            end
                        else
                            skill_target_end_counter(skill)
                        end
                    end)
                    target:PushAction(ACTION_BEHIT)
                    target:MoveActionToBack()
                end
            else
                skill_target_end_counter(skill)
            end
        end
    end)
    cast_action:AddStopCallback(function()
        skill.caster_end = true  
    end)
    master:PushAction(ACTION_CAST)
    master:MoveActionToBack()
end


function skill_cast_atk(battle, skill)
    local master = skill.master
    if not master then return end
    skill.group_kill_counter = skill.group_kill_counter + 1
    skill.caster_end = false
    skill.target_end = false

    audio_manager_play(skill.sound)
    
    master:ClearAction()
    master:PushAction(ACTION_BATIDLE)
    local atk_info = skill.effects[skill.group_kill_counter]
    local target_id = atk_info.target_id
    local target = battle:FindActor(target_id)
    master:SetTarget(target)
    master:FaceTo(target)
    skill.atk_dir_x, skill.atk_dir_y = master:GetAttackVec()

    local runto_action = master:GetAvatar(ACTION_RUNTO)
    runto_action:Reset()
    runto_action:SetLoop(-1)
    runto_action:SetFrameInterval(PERFRAME_TIME)
    local runto_x, runto_y = calc_run_to_pos(master,target)
    runto_action:AddStartCallback(function(anim)
        master:MoveOnScreenWithDuration(runto_x,runto_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
    end)
    master:PushAction(ACTION_RUNTO)

    local attack_action = master:GetAvatar(ACTION_ATTACK)
    attack_action:Reset()
    attack_action:SetLoop(atk_info.combo)
    attack_action.skill = skill
    atk_info.atk_counter = 0
    attack_action.atk_info = atk_info
    attack_action.target = target
    
    local key_frame = master:GetAtkKeyFrame()
    attack_action:AddStartCallback(function(anim)
        master:PlaySound('attack')
    end)
    attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
    attack_action:AddLoopCallback(function(anim, counter)
        if counter < atk_info.combo then
            master:PlaySound('attack')
            attack_action:AddFrameCallback(key_frame, on_attack_action_callback)
        end
    end)
    master:PushAction(ACTION_ATTACK)

    if skill.sub_type ~= 3 or skill.group_kill_counter == #skill.effects then
        local runback_action = master:GetAvatar(ACTION_RUNBACK)
        runback_action:Reset()
        runback_action:SetLoop(-1)
        runback_action:SetFrameInterval(PERFRAME_TIME)
        runback_action:AddStartCallback(function(anim)
            master:ReverseDir()
            local x,y = master:GetPos()
            anim.to_x = skill.origin_x - x
            anim.to_y = skill.origin_y - y
            master:MoveOnScreenWithDuration(anim.to_x,anim.to_y,anim:GetGroupFrameTime()-PERFRAME_TIME,true)
        end)

        runback_action:AddStopCallback(function()
            master:ReverseDir()
            skill.caster_end = true     
        end)

        master:PushAction(ACTION_RUNBACK)
    else
        attack_action:AddStopCallback(function(anim, counter)
            skill.caster_end = true     
        end)
    end
    
    master:MoveActionToBack()
end

function skill_cast_flee(battle, skill)
    if not skill.success then 
        skill.state = SKILL_STATE_END
        return 
    end
    local master = skill.master
    master:ReverseDir()

    cxlog_info(master:GetName()..'使用了逃跑')

    master:ClearAction()

    if master:GetProperty(PROP_TEAM_TYPE) == battle.local_team_type then
        local px , py = master:GetPos()
        local dx = 800-px
        local dy = 600-py
        master:MoveOnScreenWithDuration(dx,dy,1,false)
    else
        local px , py = master:GetPos()
        local dx = 0-px
        local dy = 0-py
        master:MoveOnScreenWithDuration(dx,dy,1,false)
    end
    

    master:PushAction(ACTION_WALK)
end

function skill_take_effect_on_target(skill, effect, master, target, target_i, hit_i,...)
    cxlog_info(string.format('%s正在对%s第%d次使用技能【%s】',master:GetName(), target:GetName(), hit_i, skill.name))
    if skill.type == 'atk' then
        local hp_delta = master:GetAttackDamage(target, false , false,0 ,1) -- target, is_critical, is_combo, combo_coef, actor_type_coef
        target:ModifyHP(-hp_delta)
        table.insert(effect.hp_deltas, {target = -hp_delta})
        
        cxlog_info(string.format('并造成了%.2f伤害', hp_delta))
    elseif skill.type =='spell' then
        if skill.sub_type == SKILL_SUBTYPE_SEAL or skill.sub_type == SKILL_SUBTYPE_AUXI then
                
        elseif skill.sub_type == SKILL_SUBTYPE_HEAL then
            local hp_delta = master:GetSpellDamage(target)
            target:ModifyHP(hp_delta)
            table.insert(effect.hp_deltas, {target = hp_delta})

            cxlog_info(string.format('并恢复了%.2fHP', hp_delta))
        elseif skill.sub_type == SKILL_SUBTYPE_DEFAULT then
            local hp_delta = master:GetSpellDamage(target)
            target:ModifyHP(-hp_delta)
            table.insert(effect.hp_deltas, {target = -hp_delta})

            cxlog_info(string.format('并造成了%.2f伤害', hp_delta))
        end
    end
    if skill.SkillOnHit then
        skill.SkillOnHit(skill, master, target ,target_i, hit_i)
    end
end


function init_cskill(skill)
    local cskill = {}
    cskill.id = skill.id
    cskill.tid = skill.tid
    cskill.turn = skill.turn
    cskill.master = skill.master:GetID()
    cskill.targets = {}
    cskill.effects = {}
    return cskill
end

function check_life_state(actor)
    if actor:IsDead() then
        if actor:GetProperty(PROP_ACTOR_TYPE) ~= ACTOR_TYPE_PLAYER then
            return ACTOR_LIFE_DEAD_FLY
        else
            return ACTOR_LIFE_DEAD
        end
    else
        return ACTOR_LIFE_ALIVE
    end
end

function cskill_to_skill(skill)
    skill.master = battle:FindActor(skill.master)
    local skill_templ = skill_table[skill.tid]
    skill_init_by_templ(skill, skill_templ)
    return skill
end

function on_using_flee_skill(battle, skill)
    if IsServer() then
        local suc = math.random(1,10) <= 5 or true
        if suc then
            battle:RemoveActor(skill.master)
        end
        local cskill = init_cskill(skill)
        cskill.success = suc 
        return cskill
    else
        skill_cast_flee(battle, skill)
    end
end

function base_using_skill(battle, skill)
    if IsServer() then
        local cskill = init_cskill(skill)
        local master = skill.master

        if skill.SkillOnStart then
            skill.SkillOnStart(skill, master)
        end
        cxlog_info(string.format('%s开始使用技能【%s】',master:GetName() , skill.name))

        local targets = battle_get_group_kill_targets(battle, skill.group_kill, skill.target)
        for target_i, target in ipairs(targets) do
            local target_id = target:GetID()
            table.insert(cskill.targets, target_id)
            
            local effect = {}
            effect.target_id = target_id
            effect.hp_deltas = {}
            skill.max_hit_cnt = skill.combo > 0 and skill.combo or 1
            for hit_i=1, skill.max_hit_cnt do
                effect.combo = hit_i
                skill_take_effect_on_target(skill, effect, master, target, target_i, hit_i)
                if master:IsDead() or target:IsDead() then
                    if master:IsDead() then
                        cxlog_info(string.format('造成了%s死亡',master:GetName()))
                    end
                    if target:IsDead() then
                        cxlog_info(string.format('造成了%s死亡',target:GetName()))
                    end
                    break
                end
            end
            local master_life_state = check_life_state(master)
            if master_life_state==ACTOR_LIFE_DEAD_FLY then
                battle:RemoveActor(master)
            end
            local target_life_state = check_life_state(target)
            if target_life_state==ACTOR_LIFE_DEAD_FLY then
                battle:RemoveActor(target)
            end
            effect.life_state = {master = master_life_state, target=target_life_state}
            table.insert(cskill.effects, effect)
        end
        if skill.SkillOnEnd then
            skill.SkillOnEnd(skill, master)
        end
        cskill.success = skill.success
        return cskill
    else
        if #skill.effects==0 then 
            skill.state = SKILL_STATE_END
            return 
        end
        
        local master = skill.master
        if skill.SkillOnStart then
            skill.SkillOnStart(skill, master)
        end

        if skill.type =='atk' then
            skill.group_kill_counter = 0
            skill.origin_x, skill.origin_y = master:GetPos()
            skill_cast_atk(battle, skill)
        elseif skill.type == 'spell' then
            skill.spell_combo_counter = 0
            skill.spell_combo = 0
            for i, effect in ipairs(skill.effects) do
                skill.spell_combo = math.max(effect.combo)
            end
        
            skill_cast_spell(battle, skill)
        end
    end
end

if IsClient() then
    function on_using_skill_update(battle, skill)
        if skill.type == 'atk' then
            if skill.caster_end and skill.target_end then
                if skill.group_kill_counter == #skill.effects then
                    skill.state = SKILL_STATE_END
                    if skill.SkillOnEnd then
                        skill.SkillOnEnd(skill, skill.master)
                    end
                else
                    skill_cast_atk(battle, skill)
                end
            end
        elseif skill.type == 'spell' then
            if skill.caster_end and skill.target_end then
                if skill.spell_combo_counter == skill.spell_combo then
                    skill.state = SKILL_STATE_END
                    if skill.SkillOnEnd then
                        skill.SkillOnEnd(skill, skill.master)
                    end
                else
                    skill_cast_spell(battle, skill)
                end
            end
        elseif skill.type == 'flee' then
            local master = skill.master
            local px, py = master:GetPos()
            if master:GetProperty(PROP_TEAM_TYPE) == battle.local_team_type then
                if px >= 800 then
                    skill.state = SKILL_STATE_END
                    combat_reset_actor(master)
                    battle:RemoveActor(master)
                end
            else
                if px <= 0 then
                    skill.state = SKILL_STATE_END
                    combat_reset_actor(master)
                    battle:RemoveActor(master)
                end
            end
        end
    end
end

function on_using_skill(battle, skill)
    if skill.state ~= SKILL_STATE_DEFAULT then return end

    skill.state = SKILL_STATE_START
    if skill.type == 'atk' or skill.type == 'spell' then
        return base_using_skill(battle,skill)
    elseif skill.type == 'flee' then
        return on_using_flee_skill(battle, skill)
    elseif skill.type == 'idle' then
        return
    end
end

function process_turn_command(battle, master_id, target_id, skill_id)
	local master = battle:FindActor(master_id)
	if not master or master:IsDead() then return end

	local skill = {}
    skill.id = utils_next_uid('skill')
    skill.lv = master:GetProperty(PROP_LV)
	skill.tid = skill_id
	skill.master = master
	skill.state = SKILL_STATE_DEFAULT
    skill.success = true
    local target = battle:FindActor(target_id)
    --target有可能为空，因为某些actor可能dead 或者逃跑
    if not target then
        local targets = {}
        for i,actor in ipairs(battle.actors) do
            if actor:GetProperty(PROP_TEAM_TYPE) ~= master:GetProperty(PROP_TEAM_TYPE) then
                table.insert(targets, actor)
            end
        end
        if #targets == 0 then 
            cxlog_info('process_turn_command target找不到')
            return 
        end
        target = targets[math.random(1,#targets)]
	end
    skill.target = target
	skill.turn = battle.turn

	skill.templ = skill_table[skill_id]
	skill_init_by_templ(skill, skill.templ)			

    if skill_id == 1 then
        if master:GetProperty(PROP_COMBAT_SKILL_BAN_ATK) then
            cxlog_info('ban atk') 
            return 
        end
    elseif skill.type == 'atk' or skill.type == 'spell' then
        if master:GetProperty(PROP_COMBAT_SKILL_BAN_SPELL) then
            cxlog_info('ban spell') 
           return 
        end
    end

	return on_using_skill(battle, skill)
end


