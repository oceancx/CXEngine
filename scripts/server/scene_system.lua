
local npcs = { }

function scene_system_init()
    cxlog_info('scene_system_init')
   
    local npc_table = content_system_get_table('npc')
    
    for i, npc_templ in ipairs(npc_table) do
        local npc = actor_manager_create_actor(utils_next_uid('actor'))
        npc:SetProperty(PROP_ACTOR_TYPE,  ACTOR_TYPE_NPC)
        npc:SetProperty(PROP_NAME,  npc_templ.name)
        npc:SetProperty(PROP_AVATAR_ID,  npc_templ.avatar_id)
        npc:SetPos(npc_templ.pos.x, npc_templ.pos.y)
        npc:SetProperty(PROP_SCENE_ID, npc_templ.scene_id)
        table.insert(npcs, npc)
    end
end

function scene_system_update()

end

function scene_system_stop()
end


stub[PTO_C2S_CLICK_NPC] = function(req)
    local player = actor_manager_fetch_player_by_id(req.pid)
    local target = actor_manager_fetch_player_by_id(req.target)
    if player and target then
        net_send_message(req.pid, PTO_S2C_CLICK_NPC, cjson.encode(req))
    end
end 