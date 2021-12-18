script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('window.lua')
CX_MSG_HEADER_LEN = 4 

function net_reconnect()
	if not cx_client:IsConnected() then
		cx_client:Cancel()
		cx_client:Connect()
	end
end

function net_send_message(pt, msg)
	cxlog_info('net_send_message',pt ,msg)
	local buf = ezio_buffer_create()
	buf:WriteInt(pt)
	buf:WriteString(msg)
	local cnt = buf:readable_size()
	buf:PrependInt(cnt)
	shared_netq:push_back(1,buf,buf:readable_size())
	ezio_buffer_destroy(buf)
end

local show_demo = false

do
	at_exit_manager_init()
	io_service_context_init()
	local dbg_port = command_arg_opt_int('dbg_port', 9400)
    luadbg_listen(dbg_port)   

	iw_init(800,600)
	iw_set_font(vfs_get_workdir()..'/res/font/simsun.ttc',14)

	event_loop = ez_event_loop_create()
	cx_client = ez_tcp_client_create(event_loop, '127.0.0.1',45000,'Launcher')
	shared_netq = net_thread_queue_create()
	cx_client:set_on_connection(function(conn)
		-- cxlog_info('conn-connected : ', conn:connected())
		if conn:connected() then
			cx_client_on_connected()
		else
			cx_client_on_disconnected()
		end
	end)
	cx_client:set_on_message(function(conn, buf, ts)
		while buf:readable_size() >= CX_MSG_HEADER_LEN do
			local len = buf:PeekAsInt()
			if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
				buf:Consume(CX_MSG_HEADER_LEN)
				shared_netq:push_back(0, buf, buf:readable_size())
				buf:Consume(len)
			else
				break
			end
		end
	end)

	cx_client:Connect()
	cx_client:set_auto_reconnect(true)
	
	launcher_init()
	timer_manager_init()
	event_loop:RunTaskEvery(function()
		if iw_should_close() then 
			event_loop:Quit()
			timer_manager_deinit()
			return
		end
		iw_begin_render()
		timer_manager_update(16)
		launcher_update()
		iw_end_render()
	end,16)
	event_loop:Run()
	iw_deinit()
end
