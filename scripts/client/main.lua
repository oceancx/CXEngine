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
DBG_PORT = command_arg_opt_int('dbg_port', 9601)

function init()
    luadbg_listen(DBG_PORT)

    iw_init(SCREEN_WIDTH,SCREEN_HEIGHT)   
    iw_set_font(vfs_get_workdir()..'/assets/font/simsun.ttc',14)

    content_system_init()
    timer_manager_init()
    resource_manager_init()
    sprite_renderer_init()
	combat_system_init()
    net_manager_init(SERVER_HOST, SERVER_PORT)

    load_all_addons()
end

function update()
    net_manager_update()
    timer_manager_update(window_system_get_dt())
    scene_manager_update()
    scene_manager_draw()
end


function deinit()
    net_manager_deinit()
    timer_manager_deinit()
    input_manager_deinit()
    resource_manager_deinit()
    scene_manager_deinit()
    actor_manager_deinit()

    iw_deinit()               
end

do
    init()                    
    iw_render(update)         
    deinit()   
end