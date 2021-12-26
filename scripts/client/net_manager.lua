stub = stub or {}

function net_send_message(proto,msg)
	cxlog_info('net_send_message', proto_name(proto),msg)
	return net_send_message_in_c(proto,msg)
end

function on_player_send_chat_message(msg)
	local player = actor_manager_fetch_local_player()
	player:Say(msg)
	clear_chat_text_cache()

	local req = {}
	req.pid = player:GetID()
	req.msg = msg
	net_send_message(PTO_C2C_CHAT, cjson.encode(req))
end

stub[PTO_C2C_CHAT] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
	if not player:IsLocal() then
		player:Say(req.msg)
	end
end

stub[PTO_C2C_MOVE_TO_POS] = function(req)
	local player = actor_manager_fetch_player_by_id(req.pid)
	if not player:IsLocal() then
		player:MoveTo(req.x,req.y)
	end
end

stub[PTO_S2C_SYNC_PROPS] = function(req)
	for i, dirty_prop in ipairs(req) do
		local pid = dirty_prop[1]
		local p = actor_manager_fetch_player_by_id(pid)
		if p then
			p:SetProperty(dirty_prop[2] ,dirty_prop[3])
			cxlog_info(' p ',p, ' propid ', prop_id_to_name( dirty_prop[2]) ,dirty_prop[3])
		end
	end
end


stub[PTO_S2C_GM] = function(msg)
	if msg.type  == 'exit' then
		os.exit(0)
	end
end	



function game_dispatch_message(pt)
	local type = pt:ReadAsInt()
	local js = pt:ReadAllAsString()
	local req = cjson.decode(js)
	
	cxlog_info('game_dispatch_message', proto_name(type), cjson.encode(req))
	if stub[type] then
		stub[type](req)
	end
end

USER_ACCOUNT = command_arg_opt_str('user', 'yzxy')
USER_PASSWORD = command_arg_opt_str('pass', '123456')
function game_server_on_connection(connected)
	if connected then
		local msg = {}
		msg.account = USER_ACCOUNT
		msg.password = USER_PASSWORD
		msg.name = 'yzxy'
		msg.scene_id = -105
		msg.role_id = 1
		msg.weapon_id = 40
		msg.x = 306
		msg.y = 466
		net_send_message(PTO_C2C_LOGIN, cjson.encode(msg))
	else
		os.execute('exit()')
	end
end

function game_server_on_disconnect()

end



function net_manager_stub()
	return stub
end


function net_manager_player_dostring(fmt, ...)
	local code = string.format(fmt, ...)
	local player = actor_manager_fetch_local_player()
	if player then
		local req = {
			pid = player:GetID(),
			code = code
		}
		net_send_message(PTO_C2S_PLAYER_DOSTRING,cjson.encode(req))
	end
end


function net_manager_actor_dostring(pid, fmt, ...)
	local code = string.format(fmt, ...)
	local req = {
		pid = pid,
		code = code
	}
	net_send_message(PTO_C2S_ACTOR_DOSTRING,cjson.encode(req))
end



