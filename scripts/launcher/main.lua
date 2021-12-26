script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('window.lua')
CX_MSG_HEADER_LEN = 4



function net_send_message(pt, msg)
    cxlog_info('net_send_message', pt, msg)
	net_send_message_in_c(pt,msg)
end
 

function init()
	local dbg_port = command_arg_opt_int('dbg_port', 9400)
    luadbg_listen(dbg_port)
    launcher_init()
    iw_init(800, 600)
    iw_set_font(vfs_get_workdir() .. '/assets/font/simsun.ttc', 14)
    net_manager_init('127.0.0.1', 45000)
end

function update()
    launcher_update()
end


function deinit()
    net_manager_deinit()
    iw_deinit()
end

do
    init()
    iw_render(update)
    deinit()
end
