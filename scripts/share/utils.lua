function utils_fetch_sort_keys(tbl, sortfunc)
    local kset = {}
    for k, v in pairs(tbl) do table.insert(kset, k) end
    table.sort(kset, sortfunc)
    return kset
end

function utils_string_split(str, cut)
    str = str .. cut
    local pattern = '(.-)' .. cut
    local res = {}
    for w in string.gmatch(str, pattern) do
        table.insert(res, w)
        -- print(w)
    end
    return res
end

function utils_string_split_fixcnt(str, cut, fixcnt)
    local strs = utils_string_split(str, cut)
    while #strs > fixcnt do table.remove(strs, #strs) end
    while #strs < fixcnt do table.insert(strs, '') end
    assert(#strs == fixcnt, 'fixcnt is error')
    return strs
end

function utils_dump_table(t)
    if not t or type(t) ~= 'table' then return end

    local count = 1
    local next_layer = {}
    table.insert(next_layer, t)
    while true do
        if #next_layer == 0 then break end
        local next_t = table.remove(next_layer, 1)
        for k, v in pairs(next_t) do
            if type(v) == 'table' then
                if count > 5 then break end
                count = count + 1
                table.insert(next_layer, v)
            else
                print(k, v)
            end
        end
    end
end

function custom_gen_next_sortk_fn(tbl, keys)
    return function(t, st)
        st = st == nil and 1 or (st + 1)
        if st > #keys then return nil end
        return st, t[keys[st]], keys[st]
    end
end

function gen_next_sortk_fn(tbl)
    local keys = utils_fetch_sort_keys(tbl)
    return function(t, st)
        st = st == nil and 1 or (st + 1)
        if st > #keys then return nil end
        return st, t[keys[st]], keys[st]
    end
end

imgui.HorizontalLayout = function(tbl, next_fn, cb, wrap_w)
    local line_width = wrap_w or imgui.GetContentRegionAvail()
    local cx, cy = imgui.GetCursorPos()
    local layout_x = cx
    do
        local st, v, k
        while true do
            st, v, k = next_fn(tbl, st)
            if st == nil then break end
            if k == nil then k = st end
            cb(k, v)
            local iw, ih = imgui.GetItemRectSize()
            layout_x = layout_x + iw + 8
            if layout_x < line_width - iw - 8 then
                imgui.SameLine()
            else
                layout_x = cx
            end
        end
        if layout_x ~= cx then imgui.NewLine() end
    end
end

function imgui_std_horizontal_button_layout(tbl, next_fn, on_click)
    imgui.HorizontalLayout(tbl, next_fn, function(k, v)
        if imgui.Button(k) then on_click(k, v) end
    end)
end

function utils_save_rows_to_tsv(path, col_names, rows, def_row)
    local f = io.open(path, 'w+')
    for i, col_name in ipairs(col_names) do f:write(col_name .. '\t') end
    f:write('\n')

    if def_row then
        f:write('*')
        for i, val in ipairs(def_row) do f:write(val .. '\t') end
        f:write('\n')
    end

    for i, row in ipairs(rows) do
        for j, name in ipairs(col_names) do f:write(row[name] .. '\t') end
        f:write('\n')
    end
    f:close()
end

function utils_parse_tsv_to_rows(path)
    local tbl = {}
    local col_names = {}
    local index = 1
    for line in io.lines(path) do
        if index == 1 then
            for col, name in ipairs(utils_string_split(line, '\t')) do
                if name ~= '' then table.insert(col_names, name) end
            end
            -- cxlog_info('colname :'..cjson.encode(col_names))
        else
            if line ~= '' and not line:match('^%*') then
                local row = {}
                local vals = utils_string_split_fixcnt(line, '\t', #col_names)
                for i, col in ipairs(col_names) do
                    row[col] = vals[i]
                end
                table.insert(tbl, row)
            end
        end
        index = index + 1
    end
    return tbl, col_names
end

-- function utils_parse_tsv(path, columns)
--     local tbl , col_names = utils_parse_tsv_file_as_table(path, columns)
--     for idx,col in ipairs(columns) do
--         for irow, row in ipairs(tbl) do
--             local key = col.name
--             if col.def and row[key] == '' then
--                 row[key] = col.def
--             else
--                 if col.fmt then
--                     if col.fmt == 'i' then
--                         row[key] = math.tointeger(row[key])
--                     elseif col.fmt == 'n' then
--                         row[key] = tonumber(row[key])
--                     elseif col.fmt == 'pos' then
--                         local v = utils_string_split(row[key], ',')
--                         local pos = {}
--                         if #v == 2 then
--                             pos.x = math.tointeger(v[1])
--                             pos.y = math.tointeger(v[2])
--                         else
--                             pos.x = 0
--                             pos.y = 0
--                         end
--                         row[key] = pos
--                     elseif type(col.fmt)=='function' then
--                         row[key] = col.fmt(row[key])
--                     end
--                 end
--             end
--         end
--     end
--     return tbl, col_names
-- end
function utils_parse_tsv_old(path, columns)
    local tbl = {}
    local col_names = {}
    local col_indices = {}
    local parse_first = false
    for line in io.lines(path) do
        if not parse_first then
            for col, name in ipairs(utils_string_split(line, '\t')) do
                table.insert(col_names, name)
                col_indices[name] = col
            end
            parse_first = true
            -- cxlog_info('colname :'..cjson.encode(col_names))
        else
            if line ~= '' and not line:match('^%*') then
                local row = {}
                local vals = utils_string_split_fixcnt(line, '\t', #col_names)
                for idx, col in ipairs(columns) do
                    local key = col.name
                    if key then
                        local i = col_indices[key]
                        if col.def and vals[i] == '' then
                            row[key] = col.def
                        else
                            if not col.fmt then
                                row[key] = vals[i]
                            elseif col.fmt == 'i' then
                                row[key] = math.tointeger(vals[i])
                            elseif col.fmt == 'n' then
                                row[key] = tonumber(vals[i])
                            elseif col.fmt == 'pos' then
                                local v = utils_string_split(vals[i], ',')
                                local pos = {}
                                if #v == 2 then
                                    pos.x = math.tointeger(v[1])
                                    pos.y = math.tointeger(v[2])
                                else
                                    pos.x = 0
                                    pos.y = 0
                                end
                                row[key] = pos
                            elseif type(col.fmt) == 'function' then
                                row[key] = col.fmt(vals[i])
                            end
                        end
                    end
                end
                table.insert(tbl, row)
            end
        end
    end
    return tbl, col_names
end

function utils_parse_tsv_by_main_key(path, columns, main_key)
    local tbl, col_names = utils_parse_tsv(path, columns)
    local ret = {}
    for i, row in ipairs(tbl) do ret[row[main_key]] = row end
    return ret
end

function file_read_int4(file)
    local n = file:read(4)
    n = string.unpack('i4', n)
    return n
end

function decode_mypal(path)
    cxlog_info('decode..' .. path)
    local file = io.open(path, 'rb')
    local flag = file:read(4)
    flag = string.unpack('c4', flag)
    assert(flag == 'wpal', 'decode_mypal error ' .. path)

    local seg_num = file_read_int4(file)
    local color_segments = {}
    for i = 1, seg_num + 1 do
        local sep = file_read_int4(file)
        table.insert(color_segments, sep)
    end
    -- cxlog_info(color_segments, cjson.encode(color_segments))

    local color_schemes = {}
    color_schemes.segments = color_segments
    for i = 1, seg_num do
        local cnt = file_read_int4(file)
        local tbl = {}
        for index = 1, cnt do
            local mat = {}
            for row = 1, 3 do
                for col = 1, 3 do
                    local v = file_read_int4(file)
                    table.insert(mat, v)
                end
            end
            table.insert(tbl, mat)
        end
        table.insert(color_schemes, tbl)
    end
    file:close()

    return color_schemes
end

function get_pal_from_json(str)
    local pal = cjson.decode(str)
    local new_pal = {}
    for k, v in pairs(pal) do
        if k ~= 'segments' then
            table.insert(new_pal, v)
        else
            new_pal.segments = v
        end
    end
    return new_pal
end

function traversal_file(path, cb)
    local files, dirs = ks.disk_dir(path)

    for i, v in ipairs(files) do cb(v) end
    for i, v in ipairs(dirs) do
        local subpath = path .. '/' .. v
        traversal_file(subpath, cb)
    end
end

function sort_file_tree_info_once(node)
    if node.nodes then
        table.sort(node.nodes, function(a, b)
            local alua = a.key:match('%.lua') or a.key:match('%.tab')
            local blua = b.key:match('%.lua') or b.key:match('%.tab')
            -- ks.log_info(a.key, b.key, alua, blua)
            if alua ~= blua then
                if alua then
                    return false
                else
                    return true
                end
            else
                return a.key < b.key
            end
        end)
    end
end

function sort_file_tree_info(node)
    if node.nodes then
        table.sort(node.nodes, function(a, b)
            local alua = a.key:match('%.lua')
            local blua = b.key:match('%.lua')
            -- ks.log_info(a.key, b.key, alua, blua)
            if alua ~= blua then
                if alua then
                    return false
                else
                    return true
                end
            else
                return a.key < b.key
            end
        end)
        for ni, n in ipairs(node.nodes) do sort_file_tree_info(n) end
    end
end

function get_path_from_node(node)
    local dirs = {}
    local fname = node.key
    while node.p ~= nil do
        if node.p.key ~= '' then table.insert(dirs, 1, node.p.key) end
        node = node.p
    end
    local dir = table.concat(dirs, '/')
    local path = dir .. '/' .. fname
    -- ks.log_info('path '.. path)
    return path
end

function build_file_tree_info(filepaths)
    local root = {}
    root.key = ''
    root.p = nil
    root.nodes = nil

    for i, path in ipairs(filepaths) do
        filepaths[i] = path:gsub('\\', '/')
        local sps = utils_string_split(filepaths[i], '/')
        local cur = root
        for si, s in ipairs(sps) do
            if s:match('%.lua') or s:match('%.tab') then
                local node = {}
                node.key = s
                node.p = cur
                node.nodes = nil
                if cur.nodes == nil then cur.nodes = {} end
                table.insert(cur.nodes, node)
            else
                if cur.nodes == nil then cur.nodes = {} end
                local found_node = nil
                for ni, n in ipairs(cur.nodes) do
                    if n.key == s then
                        found_node = n
                        break
                    end
                end
                if not found_node then
                    local node = {}
                    node.key = s
                    node.p = cur
                    node.nodes = nil
                    node.lua_cnt = 0
                    table.insert(cur.nodes, node)
                    cur = node
                else
                    cur = found_node
                end
            end
        end
    end
    sort_file_tree_info_once(root)
    return root
end

local cur_selection = ''
function show_next_tree_node(node)
    for i, n in ipairs(node.nodes) do
        if n.nodes then
            if imgui.TreeNode(n.key .. '##' .. tostring(n)) then
                sort_file_tree_info_once(n)
                show_next_tree_node(n)
                imgui.TreePop()
            end
        else
            if imgui.Selectable(n.key .. '##' .. tostring(n),
                                cur_selection == tostring(n)) then
                cur_selection = tostring(n)
            end
        end
    end
end

function IsServer() return process_is_server() end

function IsClient() return not process_is_server() end

function prop_id_to_name(prop_id)
    local tbl = content_system_get_table('actor_template')
    return tbl[prop_id + 1].name
end

function read_database_file(path)
    local file = io.open(path, 'r')
    if not file then
        local fw = io.open(path, 'w')
        fw:close()
        file = io.open(path, 'r')
    end
    local data = file:read('a')
    file:close()
    if data ~= '' then return cjson.decode(data) end
end

function format_lua_path(path)
    if string.find(path, '@') == 1 then path = string.sub(path, 2) end
    local subpaths = {}
    path = path .. '/'
    for subpath in path:gmatch('(.-)[\\/]') do
        if #subpaths == 0 then subpath = subpath:lower() end
        if subpath == '..' then
            table.remove(subpaths)
        else
            table.insert(subpaths, subpath)
        end
    end
    path = table.concat(subpaths, '\\')
    print(path)
    return path
end

__uids__ = __uids__ or {}
function utils_next_uid(ns)
    if not __uids__[ns] then __uids__[ns] = os.time() % 0xFFFFFFFF end
    __uids__[ns] = __uids__[ns] + 1
    return __uids__[ns]
end

imgui.TextFormat = function(...) return imgui.Text(string.format(...)) end

imgui.InputTextEx = function(name, sb, width)
    if width then
        imgui.PushItemWidth(width)
        local res = imgui.InputText(name, sb)
        imgui.PopItemWidth()
    else
        return imgui.InputText(name, sb)
    end
end

imgui.DropInputText = function(name, sb, width)
    local res
    if width then
        imgui.PushItemWidth(width)
        res = imgui.InputText(name, sb)
        imgui.PopItemWidth()
    else
        res = imgui.InputText(name, sb)
    end

    if iw_is_dropped() and (imgui.IsItemHovered() or imgui.IsItemActive()) then
        local files = iw_get_drop_files()
        sb:reset(files[1])
        iw_set_dropped(false)
    end
    return res
end

utils_parse_tsv = function(name, columns)
    return utils_parse_tsv_file(vfs_get_tsvpath(name), columns)
end

function utils_get_action_res_id(avatar_type, id, action_id)
    local tbl
    local action_name = action_get_name(action_id)
    if avatar_type == AVATAR_TYPE_ROLE then
        tbl = content_system_get_table('avatar_role')
        cxlog_info('read action', AVATAR_TYPE_ROLE, id, action_name)
    elseif avatar_type == ACTOR_TYPE_NPC then
        tbl = content_system_get_table('avatar_scene_npc')
        if not tbl[id] then
            tbl = content_system_get_table('avatar_summon')
        end
        cxlog_info('read action', ACTOR_TYPE_NPC, id, action_name)
    elseif avatar_type == ACTOR_TYPE_SUMMON then
        tbl = content_system_get_table('avatar_summon')
        if not tbl[id] then
            tbl = content_system_get_table('avatar_scene_npc')
        end
        cxlog_info('read action', ACTOR_TYPE_SUMMON, id, action_name)
    elseif avatar_type == AVATAR_TYPE_WEAPON then
        tbl = content_system_get_table('avatar_weapon')
        cxlog_info('read action', AVATAR_TYPE_WEAPON, id, action_name)
    else
        cxlog_info('read action error', avatar_type, id, action_name)
        return
    end

    local idstr = tbl[id][action_name]
    local ids = utils_string_split(idstr, ',')
    if #ids > 1 then
        local which = math.random(1, #ids)
        local resstr = ids[which]
        local resid = res_parse_resid(resstr)
        return resid
    else
        local resid = res_parse_resid(idstr)
        return resid
    end
end

function get_local_ips()
    local f = io.popen('ipconfig')
    local msg = f:read('a')
    msg = util_gb2312_to_utf8(msg)
    f:close()

    local ips = {}
    msg:gsub('(.-)\n', function(match)
        if match:match('IPv4') then
            local tmp, ip = match:match('IPv4(.+): (.+)')
            cxlog_info(ip)
            table.insert(ips, ip)
        end
    end)
    return ips
end

function get_first_local_ip()
    local ips = get_local_ips()
    if #ips > 0 then
        return ips[1]
    else
        return ''
    end
end
