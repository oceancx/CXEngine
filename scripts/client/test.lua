SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
DefaultMapID = 1001

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile 'actor_metatable.lua'

script_system_dofile('net_manager.lua')

script_system_dofile('../combat/combat_system.lua')
script_system_dofile 'scene_manager.lua'
script_system_dofile 'actor_manager.lua'
script_system_dofile('event_system.lua')
script_system_dofile('input_manager.lua')

script_system_dofile('addon_manager.lua')

script_system_dofile('module/team.lua')
script_system_dofile('module/dialog.lua')
script_system_dofile('module/summon.lua')


SERVER_HOST = command_arg_opt_str('host','127.0.0.1')
SERVER_PORT = command_arg_opt_int('port', 45000)
DBG_PORT = command_arg_opt_int('dbg_port', 9600)

function update()
    
end
do
	at_exit_manager_init()
    io_service_context_init()
    luadbg_listen(DBG_PORT)
    iw_init(SCREEN_WIDTH,SCREEN_HEIGHT)
    iw_set_font(vfs_get_workdir()..'/assets/font/simsun.ttc',14)
    iw_render(update)
    iw_deinit()
end