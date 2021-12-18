__battles__ = __battles__ or {}

local ActorMT = actor_get_metatable()

function ActorMT:GetBattle()
	local battle_id = self:GetProperty(PROP_COMBAT_BATTLE_ID)
	return __battles__[battle_id]
end

function combat_system_add_team_by_actor(battle, actor, team_type)
	if actor:HasTeam() then
		local team = actor:GetTeam()
		for i,mem in ipairs(team:GetMembers()) do
			battle:AddActor(mem, team_type, i)
		end

		for i,mem in ipairs(team:GetMembers()) do
			if mem:IsPlayer() then
				local summon = mem:GetSummon()
				if summon then
					battle:AddActor(summon, team_type, i+5)
				end
			end
		end
	else
		battle:AddActor(actor,team_type,1)
		if actor:IsPlayer() then
			local summon = actor:GetSummon()
			if summon then
				battle:AddActor(summon, team_type,6)
			end
		end
	end
end

function combat_system_create_pve_battle(player)
	local scene_id = player:GetProperty(PROP_SCENE_ID)
	local scene_name = player:GetSceneName()
	local scene_monster_tbl = content_system_get_table('scene_monster')
	local avatar_summon = content_system_get_table('avatar_summon')
	if not scene_monster_tbl[scene_name] then return end
	local candidates = scene_monster_tbl[scene_name].monster
	if not candidates then return end
	local monsters = {}
	for i,monster in ipairs(candidates) do
		if avatar_summon[monster] then
			table.insert(monsters,monster)
		end
	end
	if #monsters == 0 then return end

	local battle = BattleMT:new()
	combat_system_add_team_by_actor(battle, player, TEAM_TYPE_ATTACKER)	
	local team = player:GetTeam()
	local enemy_count = 0
	if team then
		local team_people = #team:GetMembers()
		if team_people == 1 then
			enemy_count =  math.random(1,3)
		elseif team_people == 2 then
			enemy_count =  math.random(2,4)
		elseif team_people == 3 then
			enemy_count =  math.random(3,6)
		elseif team_people == 4 then
			enemy_count =  math.random(5,8)
		elseif team_people == 5 then
			enemy_count =  math.random(7,10)
		end
	else
		enemy_count = math.random(1,3)
	end

	local player_lv = player:GetProperty(PROP_LV) 
	for i=1,enemy_count do
		local monster_name = monsters[math.random(1,#monsters)] 

		local actor = actor_manager_create_actor(utils_next_uid('actor'))
		actor:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_NPC)
		actor:SetProperty(PROP_NAME,  monster_name)
		actor:SetProperty(PROP_AVATAR_ID,  monster_name)
		actor:SetProperty(PROP_LV,  player_lv)
		actor:ClearAssignPoints()
		actor:ApplySummonQual(monster_name)
		actor:SetProperty(PROP_ASSIGN_HEALTH, player_lv)
		actor:SetProperty(PROP_ASSIGN_MAGIC, player_lv)
		actor:SetProperty(PROP_ASSIGN_FORCE, player_lv)
		actor:SetProperty(PROP_ASSIGN_STAMINA , player_lv)
		actor:SetProperty(PROP_ASSIGN_AGILITY, player_lv)
		actor:SetProperty(PROP_SCENE_ID, player:GetProperty(PROP_SCENE_ID))
		actor:SetProperty(PROP_IS_AUTO_COMMAND, true)

		actor:SetProperty(PROP_COMBAT_POS_ID,-1)
        actor:SetProperty(PROP_TEAM_TYPE,-1)

		if math.random(1,10) == 1 then
			actor:Variation()
		end
		battle:AddActor(actor, TEAM_TYPE_DEFENDER, i)
	end
	__battles__[battle.id] = battle
	return battle
		
end

function combat_system_create_battle(atk_actor, def_actor)
	local battle = BattleMT:new()
	local team = atk_actor:GetTeam()
	if team then
		if team:HasMember(def_actor) then
			return 
		end
	end

	combat_system_add_team_by_actor(battle, atk_actor, TEAM_TYPE_ATTACKER)
	combat_system_add_team_by_actor(battle, def_actor, TEAM_TYPE_DEFENDER)
	
	__battles__[battle.id] = battle
	return battle
end

function combat_system_remove_battle(battle_id)
	__battles__[battle.id]  = nil
end

function combat_system_send_message(battle, proto, msg)
	for i, actor in pairs(battle.actors) do
		if actor:IsPlayer() then
			net_send_message(actor:GetID(), proto, cjson.encode(msg))
		end
	end
end

stub[PTO_C2S_COMBAT_CREATE] = function(req)
	local actor = actor_manager_fetch_player_by_id(req.pid)
	local battle = BattleMT:new()
	combat_system_add_team_by_actor(battle, actor, TEAM_TYPE_ATTACKER)
	
	__battles__[battle.id] = battle
	
	battle:PrepareBattle()
	local resp = req
	resp.battle = battle:Serialize()
	combat_system_send_message(battle, PTO_S2C_COMBAT_CREATE, resp)
end

stub[PTO_C2S_COMBAT_START] = function(req)
	local atk = actor_manager_fetch_player_by_id(req.atk)
	local def = actor_manager_fetch_player_by_id(req.def)
	local battle = combat_system_create_battle(atk,def)
	if not battle then return end
	battle:StartBattle()
	local resp = req
	resp.battle = battle:Serialize()
	combat_system_send_message(battle, PTO_S2C_COMBAT_START,resp)
end

stub[PTO_C2S_COMBAT_PVP_START] = function(req)
	
end

stub[PTO_C2S_COMBAT_PVE_START] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
	if not player then return end

	local battle = combat_system_create_pve_battle(player)
	if not battle then return end
	battle:StartBattle()
	local resp = req
	resp.battle = battle:Serialize()
	combat_system_send_message(battle, PTO_S2C_COMBAT_START,resp)
end

function handle_turn_commands(battle)
-----先记录当前actors里面玩家的pid，因为战斗处理有可能移除掉---------
	local send_pids = {}
	for i,actor in ipairs(battle.actors) do
		if actor:IsPlayer() then
			table.insert(send_pids, actor:GetID())
		end
	end
---------------------------------------------------------------------	

	table.sort(battle.cmds, function(a,b)
		local pa = battle:FindActor(a.master)
		local pb = battle:FindActor(b.master)
		if pa and pb then
			return pa:CalcSpeed() > pb:CalcSpeed()
		else
			return true
		end
	end)
	
	local all_skills = {}
	for i,cmd in ipairs(battle.cmds) do
		local skill_info = process_turn_command(battle,cmd.master,cmd.target,cmd.skill_id)
		if skill_info then
			table.insert(all_skills, skill_info)
		end
	end

	for i, pid in ipairs(send_pids) do
		net_send_message(pid, PTO_S2C_COMBAT_EXECUTE, cjson.encode(all_skills))
	end
end

stub[PTO_C2S_COMBAT_CMD] = function(req)
	local battle = __battles__[req.battle_id]
	if not battle then return cxlog_info('battle not exist!') end
	if battle.state ~= BATTLE_TURN_STAND_BY then return cxlog_info('battle is not in standby') end
	
	local master = battle:FindActor(req.master)
	if master:GetProperty(PROP_TURN_READY) then return end
	battle:AddCommand(master, req)

	if battle:CheckStandBy() then
		battle.state = BTTALE_TURN_EXECUTE
		handle_turn_commands(battle)

		if battle:CheckEnd() then
			battle:EndBattle()
		else
			battle:NextTurn()
		end
	end
end

stub[PTO_C2S_COMBAT_END_BATTLE] = function(req) 
	local battle = __battles__[req.battle_id]
	if not battle then return end
	local actor = battle:FindActor(req.pid)
	
	for i,actor in ipairs(battle.actors) do
		if actor:IsPlayer() then
			local msg = {}
			net_send_message(actor:GetID(), PTO_S2C_COMBAT_END_BATTLE, cjson.encode(msg))
		end
	end
	battle:EndBattle()
end

function combat_system_battle_on_actor_leave(pid)
	local actor = actor_manager_fetch_player_by_id(pid)
	local battle = actor:GetBattle()
	if battle then
		battle:EndBattle()
	end
end

function combat_system_init()
	cxlog_info('combat_system_init')
	init_skills()
	init_buffs()
end

function combat_system_update()
	if not __battles__ then return end
	local push_dirty_props = function(dirty_props, actor)
        local pid = actor:GetID()
        local props = actor:GetDirtyProps()
        for i,prop_id in ipairs(props) do
            if is_prop_sync(prop_id) then
                table.insert(dirty_props, {pid, prop_id, actor:GetProperty(prop_id)})
            end
        end
        actor:ClearDirty()
    end
	for id,battle in pairs(__battles__) do
		local dirty_props = {}
		for i, actor in pairs(battle.actors) do
			if not actor:IsPlayer() and actor:GetSummonOwner() == nil then
				if actor:IsDirty() then
					push_dirty_props(dirty_props, actor)
				end
			end
		end
		if #dirty_props > 0 then
			cxlog_info('sync dirty props', #dirty_props) 
			for i,actor in pairs(battle.actors) do
				if actor:IsPlayer() then
					net_send_message(actor:GetID(),PTO_S2C_SYNC_PROPS, cjson.encode(dirty_props))
				end
			end
		end
	end
end