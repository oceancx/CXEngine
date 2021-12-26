function read_npc_table()
    local tbl,cnames  = utils_parse_tsv(vfs_get_tsvpath('avatar_npc'),{
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
        { name = 'unknown' }
    })    

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.ID] = row
    end
    return ret,cnames
end

function init_summon_quality_table()
    local tbl,col_names = utils_parse_tsv(vfs_get_tsvpath('summon_quality'),{
        { name = 'name' },
        { name = 'take_level', fmt='i', def=0},
        { name = 'battle_level', fmt='i',def=0 },
        { name = 'atk_qual', fmt='i' },
        { name = 'def_qual', fmt='i' },
        { name = 'health_qual', fmt='i' },
        { name = 'magic_qual', fmt='i' },
        { name = 'speed_qual', fmt='i' },
        { name = 'dodge_qual', fmt='i' },
        { name = 'grow_coef', fmt='n' },
    })

    local ret = {}
    for i,row in ipairs(tbl) do
        ret[row.name] = row
    end
    return ret,col_names
end

function handle_avatar_npc()
    local avatar_npc_tbl,avatar_npc_cnames = read_npc_table()
    local summon_tbl, summon_cnames = init_summon_quality_table()
    local rows={}
    for id, row in pairs(avatar_npc_tbl) do
        if not summon_tbl[id] then
            table.insert(rows,row)
        end
    end
    -- table.sort(rows,function(a,b)
    --     return summon_tbl[a.ID].take_level < summon_tbl[b.ID].take_level
    -- end)

    utils_save_rows_to_tsv(vfs_get_tsvpath('test2'), avatar_npc_cnames,rows )
end


function output_names()
    local tbl,cnames  = utils_parse_tsv(vfs_get_tsvpath('scene_npc'),{
        {name = 'id'},
        {name = 'name'}
    },'name')

    local names = {}
    for i,row in ipairs(tbl) do
        table.insert(names,row.name)
        cxlog_info('name', row.name)
    end

end