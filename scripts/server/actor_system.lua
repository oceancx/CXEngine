
function actors_on_load()
    cxlog_info('actors_on_load')
    local path = vfs_get_workdir() .. '/res/storage/actor.data'
	local db = read_database_file(path)
	if db then
		for i,v in ipairs(db) do
            local pid = v[tostring(PROP_ID)]
            local actor = actor_manager_create_actor(pid)
            actor:SetProperties(v)
            actor:SetProperty(PROP_ID, pid)
		end
	end
end

function actors_on_save()
    local actor_infos = {}
    local __actors__ = actor_manager_fetch_all_actors()
    for pid, actor in pairs(__actors__) do
        table.insert(actor_infos, actor:GetProperties())
    end
    table.sort(actor_infos, function(a,b) return a[PROP_ID] < b[PROP_ID] end)
    	
	local path = vfs_get_workdir() .. '/res/storage/actor.data'
	local fw = io.open(path,'w')
    if not fw then return end
	fw:write(cjson.encode(actor_infos))
	fw:close()
end

stub[PTO_C2C_SAVE_ACTORS] = function()
    actors_on_save()
end

stub[PTO_C2C_LOGIN] = function(req)
    local req_player = actor_manager_fetch_player_by_id(req.pid)
    if not req_player then
        req_player = actor_manager_create_actor(req.pid)
        req_player:SetProperty(PROP_NAME,math.tointeger(req.pid))
        req_player:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_PLAYER)
        local scenes = content_system_get_table('scene')
        local scene_id = req_player:GetProperty(PROP_SCENE_ID)
        req_player:SetPos(scenes[scene_id].birth_pos.x,scenes[scene_id].birth_pos.y )
    end
    
    local players = actor_manager_fetch_all_players()    
    for i,p in ipairs(players) do
        local pid = p:GetID()
        if pid == req.pid then
            local actors_props = {}
            for _,actor in ipairs(players) do
                table.insert(actors_props, actor:GetProperties())
            end
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode({local_pid = req.pid, actors = actors_props}))
        else
            net_send_message(pid,PTO_C2C_PLAYER_ENTER, cjson.encode({ actors = { req_player:GetProperties() }}))
        end
    end
    
    local actors_props = {}
    local actors = actor_manager_fetch_all_actors()   
    for i,actor in ipairs(actors) do
        if actor:IsNPC() or actor:IsSummon() then
            table.insert(actors_props, actor:GetProperties())
        end
    end
    net_send_message(req.pid, PTO_C2C_ACTOR_ENTER, cjson.encode({ npcs = actors_props}))
end


stub[PTO_C2S_CREATE_SUMMON] = function(req)
    local pid = utils_next_uid('actor')
    local actor = actor_manager_create_actor(pid)
    actor:SetProperties(req.props)
    actor:SetProperty(PROP_ID, pid)
    actor:SetProperty(PROP_TEAM_ID, 0)
    actor:SetProperty(PROP_ACTOR_TYPE, ACTOR_TYPE_SUMMON)
    actor:SetProperty(PROP_IS_AUTO_COMMAND, false)

    cxlog_info('create summon ' ,actor:GetID(), actor:GetProperty(PROP_AVATAR_ID))
    local owner = actor_manager_fetch_player_by_id(req.owner)
    if owner then
        owner:AddSummon(actor)
    end
    __summons__[pid] = actor


    local summon_infos = {}
    for pid, summon in pairs(__summons__) do
        table.insert(summon_infos, summon:GetProperties())
    end
    
    net_send_message(req.owner, PTO_S2C_FETCH_SUMMON_RESP, 
        cjson.encode(summon_infos))

end

 

stub[PTO_C2S_FETCH_SUMMON] = function(req)
    local summon_infos = {}
    for pid, summon in pairs(__summons__) do
        table.insert(summon_infos, summon:GetProperties())
    end
    
    net_send_message(req.pid, PTO_S2C_FETCH_SUMMON_RESP, 
        cjson.encode(summon_infos))
end

stub[PTO_C2C_MOVE_TO_POS] = function(req,js)
    local player = actor_manager_fetch_player_by_id(req.pid)
    if not player then return end
    player:SetPos(req.x,req.y)
    net_send_message_to_all_players(PTO_C2C_MOVE_TO_POS,js)
end


stub[PTO_C2C_CHAT] = function(req,js)
    net_send_message_to_all_players(PTO_C2C_CHAT,js)
end

stub[PTO_C2S_CREATE_PLAYER] = function(req, js)
    local props = cjson.decode(js)
    local pid = props[tostring(PROP_ID)]
    local actor = actor_manager_fetch_player_by_id(pid)
    actor:SetProperties(props)
end

stub[PTO_C2S_CREATE_ACTOR] = function(req)
    local pid = utils_next_uid('actor')
    local actor = actor_manager_create_actor(pid)
    actor:SetProperties(req.props)
    actor:SetProperty(PROP_ID, pid)
    actor:SetProperty(PROP_NAME, pid)

    net_send_message(req.pid,PTO_S2C_CREATE_ACTOR, 
        cjson.encode(actor:GetProperties()))
end

stub[PTO_C2S_DELETE_ACTOR] = function(req)
    -- local actor = actor_manager_fetch_player_by_id(req.pid)
    -- if actor:IsPlayer() then
    -- end
    actor_manager_destroy_actor(req.delete_pid)
    net_send_message(req.pid, PTO_S2C_DELETE_ACTOR, cjson.encode(req))
end

local ActorMT = actor_get_metatable()
function ActorMT:AddSummon(summon)
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    uids_str = '[]'
    local uids = cjson.decode(uids_str) 
    table.insert(uids, summon:GetID())
    summon:SetProperty(PROP_SUMMON_HAS_OWNER, true) 
    summon:SetProperty(PROP_SUMMON_OWNER, self:GetID()) 
    self:SetProperty(PROP_SUMMON_UIDS, cjson.encode(uids))
end

function ActorMT:GetSummon()
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    local uids = cjson.decode(uids_str) 
    if #uids == 0 then return end
    local actor = actor_manager_fetch_player_by_id(uids[1])
    return actor
end

function ActorMT:GetSummons()
    local uids_str = self:GetProperty(PROP_SUMMON_UIDS) 
    local uids = cjson.decode(uids_str) 
    if #uids == 0 then return {} end
    local summons = {}
    for i,uid in ipairs(uids) do
        local actor = actor_manager_fetch_player_by_id(uid)
        table.insert(summons, actor)
    end
    return summons
end