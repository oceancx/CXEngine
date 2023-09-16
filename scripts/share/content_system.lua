local content_tables = {}
function content_system_get_table(name)
    return content_tables[name] 
end

function content_system_set_table(name, func, ...)
    content_tables[name] = perf_call('content_system_set_table '..name,func, ...)
end



function read_magic_table()
    -- cxlog_info('read_magic_table')
    local file = io.open(vfs_get_tablepath('magic.tsv') )
    local tbl = {}
    local col_names = {}
    local index = 1
    for line in file:lines() do
        -- cxlog_info('line..' ..line)
        if index == 1 then
            col_names = utils_string_split(line,'\t')
        elseif line[0]~='*' and line ~='' then
            local strs = utils_string_split(line,'\t')
            local row = {}
            row.ID = math.tointeger(strs[1]) 
            row.name = strs[2]
            local strs2 = utils_string_split(strs[3],'-') 
            local pack = tonumber(strs2[1],10)
            local wasid = tonumber(strs2[2],16)
            row.resid = (pack<<32)|wasid
            table.insert(tbl,row)
        end
        index = index+1
    end
    file:close()

    local tbl_by_name = {}
    for i,v in ipairs(tbl) do
        tbl_by_name[v.name] = v
    end
    return tbl_by_name
end

function read_map_table()
    local path = vfs_get_tsvpath('map')
    local tbl,col_names = utils_parse_tsv_to_rows(path)
    for i, row in ipairs(tbl) do
        row.ID = math.tointeger(math.ID)
    end
    return tbl
end
function read_npc_table()
    local tbl  = utils_parse_tsv('avatar_npc',{
        { name = 'ID'},
        { name = 'can_take', fmt='i', def =0},
        { name = 'idle' },
        { name = 'walk' },
        { name = 'sit' },
        { name = 'angry' },
        { name = 'sayhi' },
        { name = 'dance' },
        { name = 'salute' },
        { name = 'clps' },
        { name = 'cry' },
        { name = 'batidle' },
        { name = 'attack' },
        { name = 'cast' },
        { name = 'behit' },
        { name = 'runto' },
        { name = 'runback' },
        { name = 'defend' },
        { name = 'unknown' },
        { name = 'atk_key', fmt='i', def =0 }
    })    

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function read_actor_template()
    local tbl  = utils_parse_tsv('actor_template',{
        { name='name'},
        { name='type'},
        { name='def'},
        { name='sync', fmt='i', def=0},
    })    
    return tbl
end


function init_skill_template_table()
    local tbl  = utils_parse_tsv('skill',{
        { name='ID', fmt='i',def=0},
        { name='name'},
        { name='sound'},
        { name='type'},
        { name='sub_type',fmt='i',def=0},
        { name='school',fmt='i',def=0},
        { name='combo', fmt='i', def=0},
        { name='atk_anim', fmt='res', def=0 },
        { name='group_kill', fmt='i', def=0},
        { name='cast_anim', fmt='i', def=0},
        { name='act_turn', fmt='i', def=0}
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_buff_template_table()
    local tbl  = utils_parse_tsv('buff',{
        { name='ID', fmt='i'},
        { name='name'},
        { name='type',def=0},
        { name='buff_anim', fmt='res', def=0},
        { name='turn', fmt='i', def=0},
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_school_templ_table()
    local tbl = utils_parse_tsv('school',{
        {name = 'ID' , fmt = 'i'},
        {name = 'name'}
    })
    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_equip_templ_table()
    local tbl = utils_parse_tsv('equip',{
        {name = 'name'},
        {name = 'type'},
        {name = 'base_target', fmt = 'n' , def = 0},
        {name = 'lv_accum_target', fmt = 'n' , def = 0},
        {name = 'base_damage', fmt = 'n' , def = 0},
        {name = 'lv_accum_damage', fmt = 'n' , def = 0},
        {name = 'base_defend', fmt = 'n' , def = 0},
        {name = 'lv_accum_defend', fmt = 'n' , def = 0},
        {name = 'base_hp', fmt = 'n' , def = 0},
        {name = 'lv_accum_hp', fmt = 'n' , def = 0},
        {name = 'base_mp', fmt = 'n' , def = 0},
        {name = 'lv_accum_mp', fmt = 'n' , def = 0},
        {name = 'base_spiritual', fmt = 'n' , def = 0},
        {name = 'lv_accum_spiritual', fmt = 'n' , def = 0},
        {name = 'base_agile', fmt = 'n' , def = 0},
        {name = 'lv_accum_agile', fmt = 'n' , def = 0}
    })
    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.type] = row
    end
    return ret
end

local function prop_name_to_id(name)
    local v = load(string.format('do return %s end',name:upper()))()
    return v
end

function init_npc_table()
    local tbl,col_names = utils_parse_tsv('npc',{
        { name = 'ID' ,fmt='i'},
        { name = 'scene_id', fmt='i', def =0  },
        { name = 'name' },
        { name = 'avatar_id' },
        { name = 'pos', fmt='pos' },
        { name = 'dir', fmt='i', def =0 },
    })
    return tbl
end

function init_summon_quality_table()
    local tbl,col_names = utils_parse_tsv('summon_quality',{
        { name = 'name' },
        { name = 'take_level' },
        { name = 'battle_level' },
        { name = 'atk_qual', fmt='i' , def=0 },
        { name = 'def_qual', fmt='i' , def=0 },
        { name = 'health_qual', fmt='i' , def=0 },
        { name = 'magic_qual', fmt='i' , def=0 },
        { name = 'speed_qual', fmt='i' , def=0 },
        { name = 'dodge_qual', fmt='i' , def=0 },
        { name = 'grow_coef', fmt='n' , def=0 },
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.name] = row
    end
    return ret
end

function init_scene_table()
    local tbl,col_names = utils_parse_tsv('scene',{
        { name = 'ID', fmt='i', def=0},
        { name = 'name'},
        { name = 'entry_pos'},
        { name = 'map_id', fmt='i',def=0 },
        { name = 'bgm' },
        { name = 'script'},
        { name = 'birth_pos', fmt='pos' },
        { name = 'transports' }
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        if row.name == '' then break end
        ret[row.ID] = row
    end
    return ret
end

function init_transport_table()
    local tbl,col_names = utils_parse_tsv('transport',{
        { name = 'ID', fmt='i' },
        { name = 'name'},
        { name = 'scene', fmt='i' },
        { name = 'pos', fmt='pos'},
        { name = 'to_station', fmt='i',def=1 }
    })
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end


function init_summon_sound() 
    local tbl,col_names = utils_parse_tsv('sound_summon',nil)
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_role_sound() 
    local tbl,col_names = utils_parse_tsv('sound_role',nil)
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function init_summon_variation()
    local tbl,col_names = utils_parse_tsv('summon_variation',nil)
    local ret = {}
    for i, row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret
end

function read_avatar_role()
    local ret = utils_parse_tsv_by_main_key('avatar_role',nil,'ID')
    for k, row in pairs(ret) do
        if row.idle == '' and row.batidle ~='' then
            row.idle = row.batidle
        end
        if row.batidle =='' and row.idle~='' then
            row.batidle = row.idle
        end
    end
    return ret
end

function read_avatar_weapon()
    local ret = utils_parse_tsv_by_main_key('avatar_weapon',nil,'ID')
    for k, row in pairs(ret) do
        if row.idle == '' and row.batidle ~='' then
            row.idle = row.batidle
        end
        if row.batidle =='' and row.idle~='' then
            row.batidle = row.idle
        end
    end
    return ret
end

function read_avatar_scene_npc_table()
    local ret = utils_parse_tsv_by_main_key('avatar_scene_npc',nil,'ID')
    for k, row in pairs(ret) do
        if row.idle == '' and row.batidle ~='' then
            row.idle = row.batidle
        end
        if row.batidle =='' and row.idle~='' then
            row.batidle = row.idle
        end
    end
    return ret
end

function read_avatar_summon_table()
    local ret = utils_parse_tsv_by_main_key('avatar_summon',nil,'ID')
    for k, row in pairs(ret) do
        if row.idle == '' and row.batidle ~='' then
            row.idle = row.batidle
        end
        if row.batidle =='' and row.idle~='' then
            row.batidle = row.idle
        end
    end
    return ret
end

function read_scene_monster()
    local tbl,col_names = utils_parse_tsv('scene_monster')
    local ret = {}
    for i,row in ipairs(tbl) do
        local ss = utils_string_split(row.monster,',')
        row.monster = {}
        for i,s in ipairs(ss) do
            table.insert(row.monster, s)
        end
        ret[row.name] = row
    end
    return ret
end

function perf_call(name, f,...)
    local now = time_now()
    local ret = table.pack(f(...))
    local dt = time_now() - now
    cxlog_info(string.format('%s:\t%dms',name,dt))
    return table.unpack(ret)
end

function content_system_init()
    cxlog_info('content_system_init')
    
    content_system_set_table('avatar_role',  read_avatar_role) 
    content_system_set_table('avatar_weapon',  read_avatar_weapon) 
    content_system_set_table('avatar_npc',  read_npc_table) 
    content_system_set_table('avatar_scene_npc',  read_avatar_scene_npc_table) 
    content_system_set_table('avatar_summon',  read_avatar_summon_table) 
    content_system_set_table('scene_monster', read_scene_monster)
    content_system_set_table('magic',  read_magic_table) 
    content_system_set_table('maps',  read_map_table) 
    content_system_set_table('actor_template',  read_actor_template) 
    content_system_set_table('skill',  init_skill_template_table) 
    content_system_set_table('buff',  init_buff_template_table) 
    content_system_set_table('school',  init_school_templ_table) 
    content_system_set_table('equip',  init_equip_templ_table) 
    content_system_set_table('npc',  init_npc_table) 
    content_system_set_table('summon_quality',  init_summon_quality_table) 
    content_system_set_table('scene',  init_scene_table) 
    content_system_set_table('transport',  init_transport_table) 
    content_system_set_table('summon_sound',  init_summon_sound) 
    content_system_set_table('role_sound',  init_role_sound) 
    content_system_set_table('summon_variation',  init_summon_variation) 
end
