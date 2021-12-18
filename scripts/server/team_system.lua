
TeamMT = {}
function TeamMT:new(o)
    o = o or {
        id = utils_next_uid('team'),
		members = {},
        leader,
	}
    self.__index = self 
    setmetatable(o, self)
    return o
end

function TeamMT:GetID()
    return self.id
end

function TeamMT:AddMember(actor)
    local mem_id = actor:GetID()
    for i,_mem_id in ipairs(self.members) do
        if mem_id == _mem_id then
            return 
        end
    end
    table.insert(self.members, mem_id)
    actor:SetProperty(PROP_TEAM_ID, self.id)
end

function TeamMT:RemoveMember(actor)
    for i,_mem_id in ipairs(self.members) do
        if _mem_id == actor:GetID() then
            table.remove(self.members, i)
            actor:SetProperty(PROP_TEAM_ID, 0)
            return 
        end
    end
end

function TeamMT:HasMember(actor)
    for i,_mem_id in ipairs(self.members) do
        if _mem_id == actor:GetID() then
            return true
        end
    end
    return false
end

function TeamMT:SetLeader(actor)
    for i,_mem_id in ipairs(self.members) do
        if _mem_id == actor:GetID() then
            self.leader = _mem_id
            return
        end
    end
end

function TeamMT:GetMembers()
    local actors = {}
    for i,id in ipairs(self.members) do
        local actor = actor_manager_fetch_player_by_id(id)
        table.insert(actors,actor)
    end
    return actors
end

function TeamMT:GetLeader()
    return self.leader
end

function TeamMT:Serialize()
    local team_info = {}
    team_info.id = self.id
    team_info.members = self.members
    team_info.leader = self.leader
    return team_info
end

__teams__ = __teams__ or {}
function team_system_create_team(actor)
    local team = TeamMT:new()
    team:AddMember(actor)
    team:SetLeader(actor)
    __teams__[team:GetID()] = team
    return team
end

function team_system_get_team(actor)
    local team_id = actor:GetProperty(PROP_TEAM_ID)
    return __teams__[team_id]
end

function team_system_dismiss_team(actor)
    if actor:IsTeamLeader() then
        local team = actor:GetTeam()
        for i, mem_id in ipairs(team.members) do
            local actor = actor_manager_fetch_player_by_id(mem_id)
            actor:SetProperty(PROP_TEAM_ID, 0)
        end
        __teams__[team.id] = nil
    end
end


function team_system_leave_team(actor)
    if actor:IsTeamLeader() then
        team_system_dismiss_team(actor)
    else
        local team = actor:GetTeam()
        team:RemoveMember(actor)
    end
end

function team_system_on_disconnect(player)
    if not player then return end
    local team = player:GetTeam()
    if not team then return end
    team:RemoveMember(player)
    cxlog_info(player:GetName()..'离开了队伍 '..team.id)
    local resp = {
        team = team:Serialize()
    }
    net_send_message_to_all_players(PTO_S2C_TEAM_REMOVE_MEMBER, cjson.encode(resp))
end

local ActorMT = actor_get_metatable()

function ActorMT:HasTeam()
    return team_system_get_team(self) ~= nil 
end

function ActorMT:GetTeam()
    return team_system_get_team(self)
end

function ActorMT:IsTeamLeader()
    local team = self:GetTeam()
    if not team then return false end
    return team.leader == self:GetID() 
end

function ActorMT:CreateTeam()
    return team_system_create_team(self)
end

function ActorMT:DismissTeam()
    team_system_dismiss_team(self)
end

stub[PTO_C2S_FETCH_TEAM] = function(req)
    local team_infos = {}
    for k,v in pairs(__teams__) do
        table.insert(team_infos, v:Serialize())
    end

    cxlog_info('PTO_C2S_FETCH_TEAM', cjson.encode(team_infos))
    net_send_message(req.pid, PTO_S2C_FETCH_TEAM, cjson.encode(team_infos))
end

stub[PTO_C2S_TEAM_CREATE] = function(req)
    local actor = actor_manager_fetch_player_by_id(req.pid) 
    local team = actor:CreateTeam()
    cxlog_info(actor:GetName()..'创建了队伍 '..team.id)
    local resp = {
        pid = req.pid,
        team = team:Serialize()
    }
    net_send_message_to_all_players(PTO_S2C_TEAM_CREATE, cjson.encode(resp))
end

stub[PTO_C2S_TEAM_DISMISS] = function(req)
    local actor = actor_manager_fetch_player_by_id(req.pid) 
    local team = actor:GetTeam()
    local team_id = team.id
    actor:DismissTeam()
    cxlog_info(actor:GetName()..'解散了队伍 '..team.id)
    req.team_id = team_id
    net_send_message_to_all_players(PTO_S2C_TEAM_DISMISS, cjson.encode(req))
end

stub[PTO_C2S_TEAM_ADD_MEMBER] = function(req)
    local team_id = req.team_id
    local team = __teams__[team_id]
    local mem_actor = actor_manager_fetch_player_by_id(req.member_id)
    team:AddMember(mem_actor)
    cxlog_info(mem_actor:GetName()..'加入了队伍 '..team.id)
    local resp = {
        team = team:Serialize()
    }
    net_send_message_to_all_players(PTO_S2C_TEAM_ADD_MEMBER, cjson.encode(resp))
end

stub[PTO_C2S_TEAM_REMOVE_MEMBER] = function(req)
    local team_id = req.team_id
    local team = __teams__[team_id]
    local mem_actor = actor_manager_fetch_player_by_id(req.member_id)
    
    if mem_actor:IsTeamLeader() then
        mem_actor:DismissTeam()
        cxlog_info(mem_actor:GetName()..'解散了队伍 '..team_id)
        req.team_id = team_id
        net_send_message_to_all_players(PTO_S2C_TEAM_DISMISS, cjson.encode(req))
    else
        team:RemoveMember(mem_actor)
        cxlog_info(mem_actor:GetName()..'离开了队伍 '..team.id)
        local resp = {
            team = team:Serialize()
        }
        net_send_message_to_all_players(PTO_S2C_TEAM_REMOVE_MEMBER, cjson.encode(resp))
    end
end

