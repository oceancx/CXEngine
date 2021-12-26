
script_system_dofile('../combat/combat_context.lua')


script_system_dofile('../combat/skill.lua')
script_system_dofile('../combat/buff.lua')

if IsClient() then
    script_system_dofile('../combat/client/skill.lua')
end

function battle_get_state_name(state)
    if state == BATTLE_DEFAULT then
        return '战斗默认'        
    elseif state == BATTLE_PREPARE then
        return '战斗准备'        
    elseif state == BATTLE_START then
        return '战斗开始'
    elseif state == BATTLE_TURN_STAND_BY then
        return '战斗待命'
    elseif state == BTTALE_TURN_EXECUTE then
        return '战斗执行'
    elseif state == BTTALE_TURN_NEXT then
        return '战斗下一回合'
    elseif state == BATTLE_END then
        return '战斗结束'
    else
        return '战斗未知状态'
    end        
end


BattleMT = {}
function BattleMT:new(o)
    o = o or {
        id = utils_next_uid('battle'),
        actors = {},
		cmds = {},
        state = BATTLE_DEFAULT,
        turn = 0,
        type = BATTLE_TYPE_PVE
	}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function BattleMT:Serialize()
    local info = {}
    info.id = self.id
    info.actors = {}
    for i, actor in ipairs(self.actors) do
        if actor:IsNPC() then
            table.insert(info.actors, { 
                id = actor:GetID(), 
                team_type = actor:GetProperty(PROP_TEAM_TYPE),
                pos_id = actor:GetProperty(PROP_COMBAT_POS_ID),
                props =  actor:GetProperties()
            })
        else
            table.insert(info.actors, { 
                id = actor:GetID(), 
                team_type = actor:GetProperty(PROP_TEAM_TYPE),
                pos_id = actor:GetProperty(PROP_COMBAT_POS_ID)
            })
        end
    end

    info.state = self.state
    info.turn = self.turn
    return info
end


function BattleMT:Deserialize(info)
    self.id = info.id
    self.actors = {}
    for i,actor_info in ipairs(info.actors) do
        local actor = actor_manager_fetch_player_by_id(actor_info.id)
        if not actor then
            actor = actor_manager_create_actor(actor_info.id)
            actor:SetProperties(actor_info.props)
        end
        actor:SetProperty(PROP_TEAM_TYPE, actor_info.team_type)
        actor:SetProperty(PROP_COMBAT_POS_ID, actor_info.pos_id)
        
        cxlog_info('Deserialize', actor:GetID(), actor:GetName(), actor:GetProperty(PROP_TEAM_TYPE), actor:GetProperty(PROP_COMBAT_POS_ID))
        table.insert(self.actors, actor)
    end
    self.state = info.state
    self.turn = info.turn
    return self
end


function BattleMT:FindActor(actor_id)
    for i,actor in ipairs(self.actors) do
        if actor_id == actor:GetID() then
            return actor
        end
    end
end

function BattleMT:AddActor(actor, team_type, pos_i)
    cxlog_info('Battle:AddActor', self.id, actor:GetID(), actor:GetName(),team_type==TEAM_TYPE_ATTACKER and 'atk' or 'def', pos_i)

    for i,_actor_ in ipairs(self.actors) do
        if _actor_:GetID() == actor:GetID() then
            return
        end
        if _actor_:GetProperty(PROP_COMBAT_POS_ID) == pos_i 
            and _actor_:GetProperty(PROP_TEAM_TYPE) == team_type
        then
            return
        end
    end
    actor:SetProperty(PROP_TEAM_TYPE, team_type)
    actor:SetProperty(PROP_COMBAT_BATTLE_ID, self.id)
    actor:SetProperty(PROP_COMBAT_POS_ID, pos_i)
    table.insert(self.actors,actor)
end

function BattleMT:RemoveActor(actor)
    for i,_actor in ipairs(self.actors) do
        if actor:GetID() == _actor:GetID() then
            actor:SetProperty(PROP_COMBAT_BATTLE_ID, 0)
            actor:SetProperty(PROP_IS_COMBAT, false)
            table.remove(self.actors,i)
            break
        end
    end
end

function BattleMT:GetNotReadyActors()
    local actors = {}
    for i,_actor in ipairs(self.actors) do
        if not actor:GetProperty(PROP_TURN_READY) then
            table.insert(actors , _actor)
        end
    end
    return actors
end

function BattleMT:GetTurn()
    return self.turn
end

function BattleMT:AddCommand(actor, cmd)
    actor:SetProperty(PROP_TURN_READY, true)
	table.insert(self.cmds, cmd)
end

function BattleMT:RandomSelectEnemy(self_actor)
    local enemies = {}
    for i, actor in ipairs(self.actors) do
        if self_actor:GetProperty(PROP_TEAM_TYPE) ~= actor:GetProperty(PROP_TEAM_TYPE) then
            table.insert(enemies, actor)
        end
    end
    return enemies[math.random(1,#enemies)]
end

function BattleMT:AutoCommand(actor)
    actor:SetProperty(PROP_TURN_READY, true)

    local rand = math.random(1,10)
    if rand <=5 then
        local cmd = {}
        local target = self:RandomSelectEnemy(actor)
        cmd.master = actor:GetID()
        cmd.target = target:GetID()
        cmd.skill_id = actor:GetRandomSkillID('atk')
        table.insert(self.cmds, cmd)
        return
    else
        local cmd = {}
        local target = self:RandomSelectEnemy(actor)
        cmd.master = actor:GetID()
        cmd.target = target:GetID()
        cmd.skill_id = actor:GetRandomSkillID('spell')
        table.insert(self.cmds, cmd)
    end
end

function BattleMT:PrepareBattle()
    self.state = BATTLE_PREPARE
    if IsServer() then
    else
        on_battle_prepare(self)
    end
end

function BattleMT:StartBattle()
	self.state = BATTLE_START
    self.turn = 0
    
    for i,actor in ipairs(self.actors) do
        actor:SetProperty(PROP_COMBAT_BATTLE_ID,self.id)
        actor:SetProperty(PROP_IS_COMBAT,true)
    end    

    if IsServer() then
        for i,actor in ipairs(self.actors) do
            actor:SetProperty(PROP_HP,actor:GetMaxHP())
        end
    else
        on_battle_start(self)
    end
    self:NextTurn()
end

function BattleMT:CheckAllIdle()
    local all_idle = true
    for i,actor in ipairs(self.actors) do
        if actor:GetActionID() ~= ACTION_BATIDLE and actor:GetActionID() ~= ACTION_IDLE then
            all_idle = false
            break
        end
    end
    return all_idle
end

    

function BattleMT:InBattle(__actor__)
    local actor_in_battle = false
    for i,bat_actor in ipairs(self.actors) do
        if bat_actor:GetID() == __actor__:GetID() then
            actor_in_battle = true
            break
        end
    end
    return actor_in_battle
end

function BattleMT:EndBattle()
	self.state = BATTLE_END
    cxlog_info('BATTLE_END')
    for i,actor in ipairs(self.actors) do
        actor:SetProperty(PROP_IS_COMBAT,false)
        actor:SetProperty(PROP_COMBAT_BATTLE_ID,0)
    end    


    if IsServer() then
        
    else
        for i,actor in ipairs(self.actors) do
            combat_reset_actor(actor)
        end
        
        animation_manager_clear()

        local player = actor_manager_fetch_local_player()
        local bgm = player:GetSceneBGMName()
        audio_manager_stop('10-8905924A')
        audio_manager_play(bgm,true)

        timer_manager_pause_timer('TimerWildBattle',false)
    end

    for i,actor in ipairs(self.actors) do
        if actor:IsNPC() then
            actor_manager_destroy_actor(actor:GetID())
        end
    end    
    self.actors = {}
end

function BattleMT:NextTurn()
    self.turn = self.turn + 1
	self.cmds = {}
    self.state = BATTLE_TURN_STAND_BY
    if IsServer() then
        for i,actor in ipairs(self.actors) do
            actor:SetProperty(PROP_TURN_READY,false)
            if actor:GetProperty(PROP_IS_AUTO_COMMAND) then
                self:AutoCommand(actor)
            end
        end
    end
    process_turn_buffs(self)
end


function BattleMT:CheckStandBy()
    local ready = true
	for i,actor in ipairs(self.actors) do
        if not actor:GetProperty(PROP_TURN_READY) then
            ready = false
            break
        end
    end
    return ready
end

function BattleMT:CheckEnd()
    local atk_all_dead = true
    local def_all_dead = true
    for i,actor in ipairs(self.actors) do
        if not actor:IsDead() then
            if actor:GetProperty(PROP_TEAM_TYPE) == TEAM_TYPE_ATTACKER then
                atk_all_dead = false
            else
                def_all_dead = false
            end
        end
    end
    return  atk_all_dead or def_all_dead
end


if IsServer() then
    script_system_dofile('../combat/server/combat_system.lua')
end

if IsClient() then
    script_system_dofile('../combat/client/combat_system.lua')
end
