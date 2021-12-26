local tActors = {}
local ACTOR_ID_COUNTER = 1

local actor_table = {}




function actor_manager_deinit()
    
end

function actor_manager_add_new(tid)

end

function actor_ev_on_click(actor, button, x, y)
	cxlog_info('ACTOR_EV_ON_CLICK',button,x,y)
	local player = actor_manager_fetch_local_player()
	if player == actor then
		cxlog_info('点到了自己')
		return
	end
    local msg = {}
    msg.pid = player:GetID()
    msg.target = actor:GetID()
    net_send_message(PTO_C2S_CLICK_NPC, cjson.encode(msg) )
end

local stub = net_manager_stub()

stub[PTO_S2C_CLICK_NPC] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
    local target = actor_manager_fetch_player_by_id(req.target)
    if player and target then
		player:SetTarget(target)
		if player:GetProperty(PROP_IS_COMBAT) then
			combat_system_actor_ev_on_click(target)
		else
			npc_on_show_dialog(player, target)
		end
	end
end

stub[PTO_S2C_CREATE_ACTOR] = function(req)
    local pid = req[tostring(PROP_ID)]
    local actor = actor_manager_create_actor(pid)
    actor:SetProperties(req)
    actor:ResetASM()

end




stub[PTO_S2C_SWITCH_SCENE] = function()
    -- PTO_C2S_SWITCH_SCENE
end


stub[PTO_C2C_PLAYER_ENTER] = function(req)
	for i,actor_info in ipairs(req.actors) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
		cxlog_info('create pid ', actor_info[tostring(PROP_ID)] )
        actor:SetProperties(actor_info)
        actor:ResetASM()
		-- actor_reg_event(actor, ACTOR_EV_ON_CLICK, actor_ev_on_click)
	end
	if req.local_pid then
		actor_manager_set_local_player(req.local_pid)
        local player = actor_manager_fetch_local_player()
        local scene_id = player:GetProperty(PROP_SCENE_ID)
        cxlog_info('scene_manager_switch_scene_by_id', scene_id)
        scene_manager_switch_scene_by_id(scene_id)

        local req = {}
	    req.pid = player:GetID()
		net_send_message(PTO_C2S_FETCH_TEAM, cjson.encode(req))
	end
end

stub[PTO_C2C_ACTOR_ENTER] = function(req)
    local player = actor_manager_fetch_local_player()
    if not player then return end
    
	for i,actor_info in ipairs(req.npcs) do
		local actor = actor_manager_create_actor(actor_info[tostring(PROP_ID)])
        actor:SetProperties(actor_info)
	end 
end

stub[PTO_S2C_DELETE_ACTOR] = function(req)
    actor_manager_destroy_actor(req.delete_pid)
end
