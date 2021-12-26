script_system_dofile('../generator/parser_keys.lua')
resource_ini_files = 
{
    ["addon.wdf"]       =   "addon.wdf.ini",
    ["atom.wdf"]        =   "atom.wdf.ini",
    -- ["chat.wdf"]        =   "chat.wdf.ini",
    ["firework.wdf"]    =   "firework.wdf.ini",
    ["goods.wdf"]       =   "goods.wdf.ini",
    ["item.wdf"]        =   "item.wdf.ini",
    ["magic.wdf"]       =   "magic.wdf.ini",
    ["mapani.wdf"]      =   "mapani.wdf.ini",
    -- ["mhimage.wdf"]     =   "mhimage.wdf.ini",
    ["misc.wdf"]        =   "misc.wdf.ini",
    -- ["music.wdf"]       =   "music.wdf.ini",
    -- ["scene.wdf"]       =   "scene.wdf.ini",
    ["shape.wdf"]       =   "shape.wdf.ini",
    -- ["shape.wd1"]       =   "shape.wd1.ini",
    ["shape.wd2"]       =   "shape.wd2.ini",
    ["shape.wd3"]       =   "shape.wd3.ini",
    ["shape.wd4"]       =   "shape.wd4.ini",
    ["shape.wd5"]       =   "shape.wd5.ini",
    ["shape.wd6"]       =   "shape.wd6.ini",
    ["shape.wd7"]       =   "shape.wd7.ini",
    ["smap.wdf"]        =   "smap.wdf.ini",
    -- ["sound.wdf"]       =   "sound.wdf.ini",
    -- ["stock.wdf"]       =   "stock.wdf.ini",
    ["waddon.wdf"]      =   "waddon.wdf.ini",
    ["wzife.wdf"]       =   "wzife.wdf.ini",
    -- ["wzife.wd1"]       =   "wzife.wd1.ini",
}


resource_ini_tables = {}
function init_table_templates()
    for k,v in pairs(resource_ini_files) do
        if v ~= "" then
            local path = vfs_makepath('res/tables/'..v)
            -- cxlog_info("file:"..path)
            local str = utils_parse_tsv_file(path)
            -- cxlog_info(str)
            resource_ini_tables[k] = cjson.decode(str)
        end
    end
end




local avatar_weapon_table = {}
function _parse_90_weapon_row(strs,WASID)
    if #strs ~= 5 then return false end

    local _90weapon = strs[1]
    local _weapon_type = find_weapon_key(strs[2]) 
    local _name = strs[3]
    local _role = find_role_key(strs[4])
    local _action = find_action_key(strs[5]) 

    local nameID = string.format('%s-%s-090-%s',_role,_weapon_type,_name)
    local row = find_avatar_table_row(avatar_weapon_table,nameID)
    row.ID = nameID
    row.name =  _name
    row.type = _weapon_type
    row.role = _role
    row.level = '90'

    if _action then
        table.insert(row[_action],WASID)
        return true
    end
    return false
end


-- 9B6684AA	武器\刀_剑侠客_被击中
function _parse_weapon_row(strs,WASID)
    if #strs ~= 4 then return false end
    local _weapon_type = find_weapon_key(strs[2]) 
    if not _weapon_type then return false end
    
    local _role = find_role_key(strs[3])
    local _action = find_action_key(strs[4])

    local nameID = string.format('%s-%s-X-X',_role,_weapon_type)

    local row = find_avatar_table_row(avatar_weapon_table,nameID)        
    row.ID = nameID
    row.name = 'X'
    row.type = _weapon_type
    row.role = _role
    row.level = '0~60'
    
    if _action then
        table.insert(row[_action],WASID)
        return true
    end
    return false
end

-- 922E6CCB	人物\骨精灵_攻击_魔棒
--人物\骨精灵_倒地
local avatar_role_table = {}
function _parse_role_row(strs,WASID)
    if #strs < 3 then return false end
    local _role = find_role_key(strs[2])
    local _action = find_action_key(strs[3]) 
    if not _action then return false end
    if #strs == 3 then
        local weapon_types = role_weapon_config[_role]
        local ID1 = string.format('%s-%s',_role, weapon_types[1])
        local ID2 = string.format('%s-%s',_role, weapon_types[2])
        local row1 = find_avatar_table_row(avatar_role_table,ID1)        
        local row2 = find_avatar_table_row(avatar_role_table,ID2)        

        if #row1[_action] < 1 then
            row1.ID = ID1
            row1.role = _role
            row1.name = strs[2]
            row1.weapon_type = weapon_types[1]
            table.insert(row1[_action], WASID)

            row2.ID = ID2
            row2.role = _role
            row2.name = strs[2]
            row2.weapon_type = weapon_types[2]
            row2[_action] ={WASID} 
            return true
        end

        row2.ID = ID2
        row2.role = _role
        row2.name =  strs[2]
        row2.weapon_type = weapon_types[2]
        row2[_action] ={WASID} 
        return true
    elseif #strs == 4 then
        local _weapon_type = find_weapon_key(strs[4])
        if not _weapon_type then return false end
        local ID = string.format('%s-%s',_role, _weapon_type)
        local row = find_avatar_table_row(avatar_role_table,ID)        
        row.ID = ID
        row.role = _role
        row.name = strs[2]
        row.weapon_type = _weapon_type
        row[_action] = {WASID}
        return true
    else 
        return false
    end
end

-- 3101E857	NPC\万圣公主\行走
local avatar_npc_table = {}
function _parse_npc_row(strs,WASID)
    if #strs ~= 3 and #strs~=4 then return false end

    if #strs == 3 then
        local _name = strs[2]
        local _action = find_action_key(strs[3]) 
        local row = find_avatar_table_row(avatar_npc_table,_name)
        row.ID = _name
        if _action then
            table.insert(row[_action] , WASID)
            return true    
        end
    elseif #strs == 4 then
        local _name = strs[3]
        local _action = find_action_key(strs[4]) 
        local row = find_avatar_table_row(avatar_npc_table,_name)
        row.ID = _name
        if _action then
            table.insert(row[_action] , WASID)
            return true    
        end
    end 
    return false
end

-- 0B5E6653	BB\凤凰\施法
function _parse_bb_row(strs,WASID)
    if #strs ~= 3 then return false end
    local _name =  strs[2]
    local _action = find_action_key(strs[3]) 
    local row = find_avatar_table_row( avatar_npc_table ,_name)
    row.ID = _name
    if _action then
        table.insert(row[_action] , WASID)
        return true    
    end
    return false
end

function handle_weapon_multi_actions(tbl)
    local add_tbl = {}
    for k,v in pairs(tbl) do
        if v.level == '0~60' then
            local ID0 = string.format('%s-%s-000-X',v.role,v.type)
            local ID30 = string.format('%s-%s-030-X',v.role,v.type)
            local ID60 = string.format('%s-%s-060-X',v.role,v.type)
            local IDs= {ID0, ID30,ID60}
            for i=1,3 do
                local row = find_avatar_table_row(add_tbl, IDs[i])
                row.ID = IDs[i]
                row.role = v.role
                row.name = v.type..'-'.. tostring( (i-1)*30)
                row.type = v.type
                row.level = tostring((i-1)*30)
                
                for _, action in ipairs(action_affix_keys) do
                    if #v[action] > 3 then
                        cxlog_info('#v[action] > 3')
                    end
                    row[action] ={v[action][i] or ''} 
                end
            end
            v.delete = true
        end
    end

    for k,v in pairs(tbl) do
        if not v.delete then
            add_tbl[k] = v
        end
    end
    return add_tbl
end

function sort_and_convert_tbl_by_ID(tbl)
    local sort_tbl = {}
    for k,v in pairs(tbl) do
        table.insert(sort_tbl, v)
    end
    table.sort(sort_tbl,function(a,b)return a.ID<b.ID end)
    return sort_tbl
end

function write_avater_file(path, header_keys, av_table)
    cxlog_info('write_avater_file', path)
    local col_names = {}
    for i,k in ipairs(header_keys) do table.insert(col_names, k) end
    for i,k in ipairs(action_affix_keys) do table.insert(col_names, k) end

    local file = io.open(path, 'w')
    for i,name in ipairs(col_names) do
        file:write(name..'\t')
    end
    file:write('\n*\n')
    
    local av_rows = sort_and_convert_tbl_by_ID(av_table)
    for i,v in ipairs(av_rows) do
        for ci,key in ipairs(col_names) do
            if v[key] then
                if type(v[key]) =='table' then
                    for _i, _v in ipairs(v[key]) do
                        if _i == 1 then
                            file:write( _v)
                        else
                            file:write( ','.._v)
                        end
                    end
                    file:write('\t')
                else
                    file:write(v[key]..'\t')
                end
            end
        end
        file:write('\n')
    end
    file:close()
end

function write_avater_npc_table(path, header_keys, tbl)
    cxlog_info('write_avater_npc_table', path)
    local col_names = {}
    for i,k in ipairs(header_keys) do table.insert(col_names, k) end
    for i,k in ipairs(action_affix_keys) do table.insert(col_names, k) end

    local file = io.open(path, 'w')
    for i,name in ipairs(col_names) do
        file:write(name..'\t')
    end
    file:write('\n*\n')
    
    local av_rows = sort_and_convert_tbl_by_ID(tbl)
    for i,v in ipairs(av_rows) do
        for ci,key in ipairs(col_names) do
            if v[key] then
                if type(v[key]) =='table' then
                   for i,item in ipairs(v[key]) do
                        if i==1 then
                            file:write(tostring(item))
                        else
                            file:write(','..tostring(item))
                        end
                   end
                    file:write('\t')
                else
                    file:write(v[key]..'\t')
                end
            end
        end
        file:write('\n')
    end
    file:close()
end

function write_parsed_error_file(path , tbl)
    local file = io.open(path ,'w')
    cxlog_info('write_parsed_error_file', path)
    local sorted_key = {}
    for k,v in pairs(tbl) do
        table.insert(sorted_key,k)
    end
    table.sort(sorted_key)
    for i,k in ipairs(sorted_key) do
        local v = tbl[k]
        file:write('['..k..']'..'\n')
        for i,row in ipairs(v) do
            for rk,rv in pairs(row) do
                file:write( tostring(rv)..'\t')
            end
            file:write('\n')
        end
    end
    file:close()
end

function parse_shape_wdf(content, errtbl)
    for i,row in ipairs(content) do
        local ok = false
        local strs = utils_str_split(row.name,'\\_')
        if strs[1] == '90级武器' then
            ok = _parse_90_weapon_row(strs,SHAPEWDF..'-'..row.ID)
        elseif strs[1] == 'BB' then
            ok = _parse_bb_row(strs,SHAPEWDF..'-'..row.ID)
        elseif strs[1] == 'NPC' then
            ok = _parse_npc_row(strs,SHAPEWDF..'-'..row.ID)
        elseif strs[1] == '人物' then 
            ok = _parse_role_row(strs,SHAPEWDF..'-'..row.ID)
        elseif strs[1] == '武器' then 
            ok = _parse_weapon_row(strs,SHAPEWDF..'-'..row.ID)
        end
        if not ok then
            table.insert(errtbl, row)
        end
    end
end

function handle_shape_wd2_row(strs, RESID)
    if strs[1] =='NPC' or 
        strs[1] == '三周年怪' or
        strs[1] =='儿童' or
        strs[1] == '召唤兽' or
        strs[1] == '婚庆' or
        strs[1] == '战神山' or
        strs[1] == '神兽' then
        local _action = find_action_key(strs[3])
        if not _action then return false end
        local _name = strs[2]
        local row = find_avatar_table_row(avatar_npc_table,_name)
        row.ID = _name
        table.insert(row[_action], RESID)
        return true
    end
    return false
end

function handle_shape_wd3_row(strs, RESID)
    if strs[1]=='125新宠' or 
        strs[1]=='NPC' or 
        strs[1]=='飞升NPC' or 
        strs[1]=='飞升新宠' then
        local _action = find_action_key(strs[3])
        if not _action then return false end
        local _name = strs[2]
        local row = find_avatar_table_row(avatar_npc_table,_name)
        row.ID = _name
        table.insert( row[_action] ,RESID)
        return true
    elseif strs[1] =='BB' then
        local _action = find_action_key(strs[4])
        if not _action then return false end
        local _name = strs[3]
        local row = find_avatar_table_row(avatar_npc_table,_name)
        row.ID = _name
        table.insert( row[_action] ,RESID)
        return true
    else
        local weapon_type = find_weapon_key(strs[1])
        if weapon_type then
            if #strs~=4 then return false end
            local _action = find_action_key(strs[4])
            if not _action then return false end
            local _name = strs[2]
            local _role = find_role_key(strs[3]) 
            local _id = _role..'-'..weapon_type..'-120-'.._name
            local row = find_avatar_table_row(avatar_weapon_table,_id)
            row.ID=_id 
            row.name = _name
            row.type = weapon_type
            row.role=_role
            row.level = '120'
            table.insert(row[_action], RESID)
            return true
        end
    end
end

function parse_shape_wd2(content, errtbl)
    for i,row in ipairs(content) do
        local ok  = false
        local strs = utils_str_split(row.name,'\\_')
        ok = handle_shape_wd2_row(strs,SHAPEWD2..'-'..row.ID)
        if not ok then
            table.insert(errtbl, row)
        end
    end
end
function parse_shape_wd3(content,errtbl)
    for i,row in ipairs(content) do
        local ok  = false
        local strs = utils_str_split(row.name,'\\_')
        ok = handle_shape_wd3_row(strs,SHAPEWD3..'-'..row.ID)
        if not ok then
            table.insert(errtbl, row)
        end
    end
end

function generate_avatar_role_tsv()
    local parse_err_table = {}
    for k,v in pairs(resource_ini_files) do
        parse_err_table[k] = {}
        if k == "shape.wdf" or k=='shape.wd2' or k=='shape.wd3' then
            local path = vfs_makepath('res/tables/'..v)
            local content = utils_parse_tsv_file_as_table(path,false)
            if k == "shape.wdf" then
                parse_shape_wdf(content,parse_err_table[k])
            elseif k == 'shape.wd2' then
                parse_shape_wd2(content,parse_err_table[k])
            elseif k == 'shape.wd3' then
                parse_shape_wd3(content,parse_err_table[k])
            end
        end
    end

    avatar_weapon_table =  handle_weapon_multi_actions(avatar_weapon_table)
    write_avater_file(vfs_makepath("res/tables/avatar_weapon.tsv"),
            {'ID', 'name','type','role','level'},avatar_weapon_table)

    write_avater_npc_table(vfs_makepath("res/tables/avatar_npc.tsv"),
            {'ID'},avatar_npc_table)
    
    write_avater_file(vfs_makepath('res/tables/avatar_role.tsv'),
            {'ID','name','role','weapon_type'},avatar_role_table)

    write_parsed_error_file(vfs_makepath('res/tables/parse_err.tsv'),parse_err_table)
    
end


-- init_table_templates()
-- generate_avatar_role_tsv()

function new_shapewdf_update_npc_avatar(tbl,type,code,resid)

end

function new_shapewdf_update_weapon_avatar(tbl,code,resid)
    local strs = utils_string_split(code,' ')
    assert(#strs == 3, 'code:..'..code..' resid..'..resid)
    local level, weapon_name = string.match(strs[1],'(%d+)(.+)') 
    level = ''.. math.tointeger(level)
    local weapon_type = find_weapon_key(weapon_name)
    local role = find_role_key(strs[2])  
    local action = find_action_key(strs[3])  
    -- cxlog_info(role, level, weapon_name,weapon_type,action,resid)
    for k,v in pairs(tbl) do
        if v.role == role and v.level == level then
            -- cxlog_info('tbl row..'..cjson.encode(v))
            if math.tointeger(level) < 90 and v.type == weapon_type then
                if v[action] then
                    v[action] = resid
                    return true
                end
            else
                if weapon_name==v.name then
                    if v[action] then
                        v[action] = resid
                        return true
                    end
                end
            end
        end
    end
    return false
end

function new_shapewdf_update_role_avatar(tbl,code,resid)
    local strs = utils_string_split(code,' ')
    assert(#strs < 4, 'code:..'..code..' resid..'..resid)

    local role = find_role_key(strs[1])
    local action = find_action_key(strs[2])
    if #strs==2 then
        for ID,v in pairs(tbl) do
            if ID:match(role) then
                if v[action]~=resid then
                    v[action] = resid
                end
            end
        end
        return true
    elseif #strs==3 then
        local weapon_type = find_weapon_key(strs[3])
        local id = role..'-'..weapon_type 
        if tbl[id] then
            tbl[id][action] = resid
        end
        return true
    end
    return false
end

function simple_read_avatar_tbl(path)
    local tbl = utils_parse_tsv_to_rows(vfs_makepath(path))
    local new_tbl = {}
    for i,v in ipairs(tbl) do
        new_tbl[v.ID] = v
    end
    return new_tbl
end

function new_shape_wdf_write_avater_file(path, header_keys, av_table)
    cxlog_info('new_shape_wdf_write_avater_file', path)
    local col_names = {}
    for i,k in ipairs(header_keys) do table.insert(col_names, k) end
    for i,k in ipairs(action_affix_keys) do table.insert(col_names, k) end

    local file = io.open(path, 'w')
    for i,name in ipairs(col_names) do
        file:write(name..'\t')
    end
    file:write('\n*\n')
    
    local av_rows = sort_and_convert_tbl_by_ID(av_table)
    for i,v in ipairs(av_rows) do
        for ci,key in ipairs(col_names) do
            if v[key] then
                if type(v[key]) =='table' then
                    for _i, _v in ipairs(v[key]) do
                        if _i == 1 then
                            file:write( _v)
                        else
                            file:write( ','.._v)
                        end
                    end
                    file:write('\t')
                else
                    file:write(v[key]..'\t')
                end
            end
        end
        file:write('\n')
    end
    file:close()
end

function parse_second_shape_wdf_ini()
    local avatar_role_tbl = simple_read_avatar_tbl('res/tables/avatar_role.tsv')
    local avatar_weapon_tbl = simple_read_avatar_tbl('res/tables/avatar_weapon.tsv')
    local avatar_npc_tbl = simple_read_avatar_tbl('res/tables/avatar_npc.tsv')

    local parse_err_tbl = {}
    do
        local current_type = ''
        for line in io.lines(vfs_get_tablepath('second/shape.wdf.parsed.ini')) do
            if line:match('=') then
                local ok = false
                local strs = utils_string_split(line,'=')
                local code = strs[1]
                local resid = SHAPEWDF..'-'..strs[2]
                if current_type == 'npc' then
                    ok = new_shapewdf_update_npc_avatar(avatar_npc_tbl,'npc',code,resid)
                elseif current_type=='battle_npc' then
                    ok = new_shapewdf_update_npc_avatar(avatar_npc_tbl,'battle_npc',code,resid)
                elseif current_type=='pet' then
                    ok =new_shapewdf_update_npc_avatar(avatar_npc_tbl,'pet',code,resid)
                elseif current_type=='item' then
                elseif current_type=='weapon' then
                    ok =new_shapewdf_update_weapon_avatar(avatar_weapon_tbl,code,resid)
                elseif current_type=='role' then
                    ok =new_shapewdf_update_role_avatar(avatar_role_tbl,code,resid)
                elseif current_type =='zodiac' then
                    ok = false
                end
                if not ok then
                    table.insert(parse_err_tbl[current_type],line)
                end
            elseif line~='' then 
                current_type = line
                parse_err_tbl[current_type] = {}
            end
        end

        -- new_shape_wdf_write_avater_file(vfs_makepath('res/tables/avatar_role.tsv'),
        --     {'ID','name','role','weapon_type'},avatar_role_tbl)
        -- if parse_err_tbl['role'] then
        --     for i, v in ipairs(parse_err_tbl['role'] ) do
        --         cxlog_info('error : ' .. v)
        --     end        
        -- end

        new_shape_wdf_write_avater_file(vfs_makepath('res/tables/avatar_weapon.tsv'),
             {'ID', 'name','type','role','level'},avatar_weapon_tbl)
        if parse_err_tbl['weapon'] then
            for i, v in ipairs(parse_err_tbl['weapon'] ) do
                cxlog_info('error : ' .. v)
            end        
        end
        
    end
    
--    local avatar_npc_path = vfs_makepath('res/tables/avatar_npc.tsv')
--    local avatar_weapon_path = vfs_makepath('res/tables/avatar_weapon.tsv')
end

parse_second_shape_wdf_ini()