
script_system_dofile('../generator/parser_keys.lua')


function parse_sound_tsv()
  
    local role_groups = {}
	local summon_groups = {}
	local last_group_name 
	for line in io.lines([[E:\Github\SimpleEngine\res\tables\sound.tsv]]) do
		cxlog_info('line', line)
		local group_name = line:match('%[(.+)%]')
        if group_name then
            last_group_name = group_name 
        else
            local actionstr, idstr = line:match('(.+)=(.+)')
            if actionstr and idstr then
                local id = '21-'.. idstr:match('0x(.+)')

                if role_sound_keys[last_group_name] then
                    local action_str,weapon_str = actionstr:match('(.+)_(.+)')
                    local action_key =  find_action_key(action_str)
                    local weapon_key = find_weapon_key(weapon_str)
                    local role_key = find_role_key(last_group_name)

                    local id_key = role_key ..'-'..weapon_key
                    if not role_groups[id_key] then
                        role_groups[id_key] = {}
                    end
                    role_groups[id_key][action_key] = id
                else
                    local action = find_action_key(actionstr)
                    local id_key = find_summon_key(last_group_name)
                    if not summon_groups[id_key] then
                        summon_groups[id_key] = {}
                    end
                    summon_groups[id_key][action] = id
                end
            end
		end
    end

    local role_path = [[E:\Github\SimpleEngine\res\tables\sound_role.tsv]]
    local role_tbl,role_tbl_cnames = utils_parse_tsv(role_path,{
        { name = 'ID' },
        { name = 'name' },
        { name = 'role' },
        { name = 'weapon_type' },
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
    })


    local rows = {}
    for id_key,v in pairs(role_groups) do
        local row = {}
        for c, name in ipairs(role_tbl_cnames) do
            row[name] = ''
            for _k,_v in pairs(v) do
                if name==_k then
                    row[name] = _v
                    break
                end
            end
        end
        row['ID'] = id_key
        table.insert(rows, row)
    end
    table.sort(rows, function(a,b)
        return a.ID < b.ID
    end)
    utils_save_rows_to_tsv(role_path, role_tbl_cnames,rows)


    local summon_path = [[E:\Github\SimpleEngine\res\tables\sound_summon.tsv]]
  
    local summon_tbl,summon_tbl_cnames = utils_parse_tsv(summon_path,{
        { name = 'ID' },
        { name = 'can_take' },
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


    rows = {}
    for id_key,v in pairs(summon_groups) do
        local row = {}
        for c, name in ipairs(summon_tbl_cnames) do
            row[name] = ''
            for _k,_v in pairs(v) do
                if name==_k then
                    row[name] = _v
                    break
                end
            end
        end
        row['ID'] = id_key
        table.insert(rows, row)
    end
    utils_save_rows_to_tsv(summon_path, summon_tbl_cnames,rows)


end