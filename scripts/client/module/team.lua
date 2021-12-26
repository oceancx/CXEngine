

TeamMT = {}
function TeamMT:new(o)
    o = o or {
        id = id,
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



__teams__ = __teams__ or {}

local ActorMT = actor_get_metatable()

function ActorMT:HasTeam()
	local team_id = self:GetProperty(PROP_TEAM_ID)
	return team_id ~= 0
end

function ActorMT:GetTeam()
   	local team_id = self:GetProperty(PROP_TEAM_ID)
    return __teams__[team_id]
end

function ActorMT:IsTeamLeader()
    local team = self:GetTeam()
    if not team then return false end
    return team.leader == self:GetID() 
end

local stub = net_manager_stub()
stub[PTO_S2C_FETCH_TEAM] = function(resp)
	__teams__ = {}	
	for i,team_info in ipairs(resp) do
		__teams__[team_info.id] = TeamMT:new(team_info) 
	end
end

stub[PTO_S2C_TEAM_CREATE] = function(resp)
	__teams__[resp.team.id] = TeamMT:new(resp.team)
end

function ActorMT:CreateTeam()
	local req = {}
	req.pid = self:GetID()
	net_send_message(PTO_C2S_TEAM_CREATE, cjson.encode(req))
end

stub[PTO_S2C_TEAM_DISMISS] = function(resp)
	__teams__[resp.team_id] = nil
end


stub[PTO_S2C_TEAM_ADD_MEMBER] = function(resp)
	__teams__[resp.team.id] = TeamMT:new(resp.team)
end

stub[PTO_S2C_TEAM_REMOVE_MEMBER] = function(resp)
	__teams__[resp.team.id] = TeamMT:new(resp.team)
end

function ActorMT:DismissTeam()
	local req = {}
	req.pid = self:GetID()
	net_send_message(PTO_C2S_TEAM_DISMISS, cjson.encode(req))
end

function ActorMT:AddTeamMember(actor)
	local team = self:GetTeam()
	if not team then return end
	local req = {}
	req.team_id = team.id
	req.member_id = actor:GetID()
	net_send_message(PTO_C2S_TEAM_ADD_MEMBER, cjson.encode(req))
end

function ActorMT:OrganizeTeam(actor)
	local team = self:GetTeam()
	local member_id = actor:GetID()
	if not team then
		team = actor:GetTeam()
		member_id = self:GetID()
	end
	if not team then return end
	local req = {}
	req.team_id = team.id
	req.member_id = member_id
	net_send_message(PTO_C2S_TEAM_ADD_MEMBER, cjson.encode(req))
end


function ActorMT:RemoveTeamMember(actor)
	local req = {}
	req.team_id = team.id
	req.member_id = actor:GetID()
	net_send_message(PTO_C2S_TEAM_REMOVE_MEMBER, cjson.encode(req))
end


function ActorMT:LeaveTeam()
	local team = self:GetTeam()
	if not team then return end
	local req = {}
	req.team_id = team.id
	req.member_id = self:GetID()
	net_send_message(PTO_C2S_TEAM_REMOVE_MEMBER, cjson.encode(req))
end
