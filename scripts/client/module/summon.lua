
__summons__ = __summons__ or {}


stub[PTO_S2C_FETCH_SUMMON_RESP] = function(resp)
    for i, info in ipairs(resp) do
        local pid = info[tostring(PROP_ID)]
        local actor = lua_create_actor(pid)
        actor:SetProperties(info)
        actor:SetProperty(PROP_ID, pid)

        -- cxlog_info('summon ' , i, actor:GetID(), actor:GetProperty(PROP_AVATAR_ID))
        __summons__[pid] = actor
    end
end


local ActorMT = actor_get_metatable()

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


