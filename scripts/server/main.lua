

stub = {}

luadbg_listen(9500)

script_system_dofile('../share/enums.lua')
script_system_dofile('../share/enums_protocol.lua')
script_system_dofile('../share/vfs.lua')
script_system_dofile('../share/utils.lua')
script_system_dofile('../share/content_system.lua')
script_system_dofile('../share/actor_metatable.lua')

script_system_dofile('server.lua')
script_system_dofile('actor_system.lua')
script_system_dofile('scene_system.lua')
script_system_dofile('team_system.lua')
script_system_dofile('../combat/combat_system.lua')

script_system_dofile('gm.lua')

function server_reload()
    cxlog_info('server_reload')

    script_system_dofile('../share/enums.lua')
    script_system_dofile('../share/enums_protocol.lua')
    script_system_dofile('../share/vfs.lua')
    script_system_dofile('../share/utils.lua')
    script_system_dofile('../share/content_system.lua')
    script_system_dofile('../share/actor_metatable.lua')

    script_system_dofile('actor_system.lua')
    script_system_dofile('../combat/combat_system.lua')

    script_system_dofile('team_system.lua')


    content_system_init()
    combat_system_init()
end

function on_script_system_init()
    content_system_init()
    combat_system_init()
    scene_system_init()
    read_account_database()
    actors_on_load()
end

local prop_templ_tbl
function is_prop_sync(prop_id)
    if not prop_templ_tbl then
        prop_templ_tbl = content_system_get_table('actor_template')
    end
    return prop_templ_tbl[prop_id+1].sync ~= 0 
end

function on_script_system_update()
    game_server_update()   
    scene_system_update()
    combat_system_update()


    local players = actor_manager_fetch_all_players()
    local dirty_props = {}

    local push_dirty_props = function(actor)
        local pid = actor:GetID()
        local props = actor:GetDirtyProps()
        for i,prop_id in ipairs(props) do
            if is_prop_sync(prop_id) then
                table.insert(dirty_props, {pid, prop_id, actor:GetProperty(prop_id)})
            end
        end
        actor:ClearDirty()
    end
    for _, p in ipairs(players) do
        if p:IsDirty() then
            push_dirty_props(p)
        end
        local summons = p:GetSummons()
        for __, summon in ipairs(summons) do
            if summon:IsDirty() then
                push_dirty_props(summon)
            end
        end
    end
    if #dirty_props > 0 then
        cxlog_info('sync dirty props', #dirty_props) 
        net_send_message_to_all_players(PTO_S2C_SYNC_PROPS, cjson.encode(dirty_props))
    end

    return true
end

function on_script_system_deinit()
    scene_system_stop()
    game_server_stop()
end

stub[PTO_C2S_DOSTRING] = function(req)
    local func, err = load(req.code,'@client','bt',_ENV)
    if func then
        func()
    else
        cxlog_info('PTO_C2S_DOSTRING', err)
    end
end 

stub[PTO_C2S_PLAYER_DOSTRING] = function(req)
    local pid = req.pid
    local player = actor_manager_fetch_player_by_id(pid)
    if not player then
        cxlog_info('PTO_C2S_PLAYER_DOSTRING not found player!!!')
        return 
    end
    local env = { player = player}
    setmetatable(env,{__index = _ENV})
    local func, err = load(req.code,'@player_dostring','bt',env)
    if func then
        local ok,ret = pcall(func)
        if not ok then
            cxlog_info(ret)
        end
    else
        cxlog_info('PTO_C2S_PLAYER_DOSTRING', err)
    end
end 


stub[PTO_C2S_ACTOR_DOSTRING] = function(req)
    local pid = req.pid
    local actor = actor_manager_fetch_player_by_id(pid)
    if not actor then
        cxlog_info('PTO_C2S_ACTOR_DOSTRING not found actor!!!')
        return 
    end
    local env = { actor = actor}
    setmetatable(env,{__index = _ENV})
    local func, err = load(req.code,'@actor_dostring','bt',env)
    if func then
        local ok,ret = pcall(func)
        if not ok then
            cxlog_info(ret)
        end
    else
        cxlog_info('PTO_C2S_ACTOR_DOSTRING', err)
    end
end 



function game_server_dispatch_message(pt)
    local type = pt:ReadAsInt()
    local js = pt:ReadAllAsString()
    print('game_server_dispatch_message' ,proto_name(type) , js)
    local req = cjson.decode(js)
    if stub[type] then
        stub[type](req,js)
    end
end

