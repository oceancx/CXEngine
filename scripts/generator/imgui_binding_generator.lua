function remove_empty_lines(content) 
    local t = {}
    local line_count = 1
    for line in content:gmatch('(.-)\n') do
        -- print(line_count, line)
        line_count = line_count + 1
        if line:match('[^%s]') then
            table.insert(t,line)
        end
    end
    local new_content = table.concat(t,'\n')

    -- local path = vfs_makepath('scripts/client/remove_lines_file.txt') 
    -- local output_file = io.open(path,'w')
    -- output_file:write(new_content)
    -- output_file:close()
    -- print(new_content)
    return  new_content
end

imgui_header_separate_flags = {
    { '',                                   'skip'},
    { [[struct ImDrawChannel;]],            'parse struct'},
    { [[typedef int ImGuiCol;]],            'parse typedef'},
    { [[struct ImVec2]],                    'parse ImVec2'},
    { [[struct ImVec4]],                    'parse ImVec4'},
    { [[namespace ImGui]],                  'parse ImGuiAPI'},
    { [[enum ImGuiWindowFlags_]],           'parse enum blocks'},
    { [[#define IMGUI_PAYLOAD_TYPE_COLOR_3F     "_COL3F"]],     'skip'},
    { [[enum ImGuiDataType_]],              'parse enum blocks'},
    { [[struct ImNewDummy {};]],                'skip'},
    { [[struct ImGuiStyle]],                'skip'},
    { [[#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS]],     'skip'},
    { [[struct ImGuiOnceUponAFrame]],                  'skip'},
    { [[#ifdef IMGUI_USE_BGRA_PACKED_COLOR]],     'skip'},
    { [[struct ImColor]],                           'skip'},
    { [[#ifndef ImDrawCallback]],          'skip'},
    { [[enum ImDrawCornerFlags_]],          'parse enum blocks'},
    { [[struct ImDrawList]],                'parse ImDrawList'},
    { [[struct ImDrawData]],                'skip'},
    { [[enum ImFontAtlasFlags_]],           'parse enum blocks'},
    { [[struct ImFontAtlas]],               'skip'},
    { [[enum ImGuiViewportFlags_]],         'parse enum blocks'},
    { [[struct ImGuiViewport]],             'skip'},
    { [[#endif]] ,                          ''}
}

local imgui_enums = {}
local imgui_apis = {}
local imgui_typedefs = {}
local imgui_structs = {}

function parse_struct(content)
    for line in content:gmatch('(.-)\n') do
        if line:find('struct') then
            local last_word 
            for word in line:gmatch('([^%s;]+)') do
                last_word = word
            end
            if last_word then
                table.insert(imgui_structs, last_word)
            end
        end
    end
    -- for i,struct_str in ipairs(imgui_structs) do
        -- print(i, struct_str)
    -- end
end

local imgui_unsupported_types = 
{
    ImGuiContext = true,
    ImGuiInputTextCallback  = true,
    ImGuiStorage = true,
    ImGuiViewport = true,
    ImFontAtlas = true,
    ImGuiDockFamily = true,
    ImGuiStyle = true,
    ImFont = true,
    ImGuiSizeCallback = true,
    ['...'] = true, 
    va_list = true
}

local TypeProtoMT = {}
function TypeProtoMT:IsFunc()
    local cnt = 0
    self:ToString():gsub('@',function(cap)
        cnt  = cnt + 1 
        return cap
    end)
    return cnt == 2
end

function TypeProtoMT:IsVoid()
    return  self.type:find('void')
end

function TypeProtoMT:IsSupported()
    return (not self:IsFunc()) and( not self:IsVoid()  )
end

function TypeProtoMT:IsRef()
    return self.decor=='&'
end

function TypeProtoMT:IsPtr()
    return self.decor=='*' 
end

function TypeProtoMT:Type()
    local ret = {}
    if self.final and self.final ~= '' then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor and self.decor ~= '' then
        table.insert(ret,self.decor)
    end
    if self.is_array then
        table.insert(ret,'[')
        if self.array_size then
            table.insert(ret,tostring(self.array_size))
        end
        table.insert(ret,']')
    end
    return table.concat(ret)
end

function TypeProtoMT:ToString()
    local ret = {}
    if self.final and self.final ~= '' then
        table.insert(ret, 'const ')
    end
    table.insert(ret,self.type)
    if self.decor and self.decor ~='' then
        table.insert(ret,self.decor)
    end
    table.insert(ret,' ')
    table.insert(ret,self.name)
    if self.is_array then
        if self.array_size > 0 then
            table.insert(ret,string.format('[%d]', self.array_size ))
        else
            table.insert(ret,string.format('[]'))
        end
    end
    return table.concat(ret)
end
TypeProtoMT.__index = TypeProtoMT
function type_proto_create(name, type, decor, final, is_array, array_size)
    local type_proto = {
        name    = name, 
        type    = type,
        final   = final or '',
        decor   =  decor,
        is_array   = is_array,
        array_size = array_size,
        func    = nil,
        def     = nil,
        proto_type = '',
    }
    setmetatable(type_proto, TypeProtoMT)
    return type_proto
end

local FuncProtoMT = {}
function FuncProtoMT:WrapName()
    local tbl = {}
    if self.namespace then
        table.insert(tbl, self.namespace..'_')    
    end
    table.insert(tbl, self.name)
    
    if #self.args > 0 then
        table.insert(tbl, '_'..#self.args..'_')
        for i,arg in ipairs(self.args) do
            if arg.type == 'ImVec2' then
                table.insert(tbl, 'v2')
            elseif arg.type == 'ImVec4' then
                table.insert(tbl, 'v4')
            elseif arg:Type() == 'const char*' then
                table.insert(tbl, 's')
            elseif arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int' then
                if arg.type:find('unsigned') then
                    table.insert(tbl, 'I')
                else
                    table.insert(tbl, 'i')
                end
            elseif arg.type =='float' or arg.type =='double' then
                if arg.type:find('unsigned') then
                    table.insert(tbl, 'N')
                else
                    table.insert(tbl, 'n')
                end
            elseif arg.type == 'bool' then
                table.insert(tbl, 'b')
            else
                table.insert(tbl, 'u')
            end
            if arg.decor=='*' and arg:Type() ~= 'const char*' then
                table.insert(tbl, 'p')
            end
        end
    end
    return table.concat(tbl)
end

function FuncProtoMT:ToString()
    local tstr = {}
    if self.ret then
        table.insert(tstr, self.ret:ToString())
        table.insert(tstr, ' ')
    end

    if self.name~='' then
        table.insert(tstr, self.name)
    end
    table.insert(tstr, '(')
    if #self.args > 0 then
        for i,arg in ipairs(self.args) do
            table.insert(tstr, arg:ToString())
            if i~= #self.args then
                table.insert(tstr, ',')
            end
        end
    end
    table.insert(tstr, ');')
    return table.concat(tstr)
end
local imgui_api_ignore_fnames={
    ['GetStyle'] = true,
    ['ShowStyleEditor'] = true,
    ['StyleColorsDark'] = true,
    ['StyleColorsClassic'] = true,
    ['StyleColorsLight'] = true,
    ['CreateContext'] = true,
    ['DestroyContext'] = true,
    ['GetCurrentContext'] = true,
    ['SetCurrentContext'] = true,
    ['GetIO'] = true,
    ['GetDrawData'] = true,
    ['GetWindowDrawList'] = true,
    ['GetWindowViewport'] = true,
    ['SetNextWindowSizeConstraint'] = true,
    ['PushFont'] = true,
    ['PopFont'] = true,
    ['GetFont'] = true,
    ['Combo'] = true,
    ['DragScalar'] = true,
    ['DragScalarN'] = true,
    ['SliderScalar'] = true,
    ['SliderScalarN'] = true,
    ['VSliderScalar'] = true,
    ['InputScalar'] = true,
    ['InputScalarN'] = true,
    ['ListBox'] = true,
    ['PlotLines'] = true,
    ['PlotHistogram'] = true,
    ['DockSpace'] = true,
    ['DockSpaceOverViewport'] = true,
    ['SetNextWindowDockId'] = true,
    ['SetNextWindowDockFamily'] = true,
    ['SetDragDropPayload'] = true,
    ['EndDragDropSource'] = true,
    ['BeginDragDropTarget'] = true,
    ['AcceptDragDropPayload'] = true,
    ['EndDragDropTarget'] = true,
    ['GetDragDropPayload'] = true,
    ['GetOverlayDrawList'] = true,
    ['GetDrawListSharedData'] = true,
    ['GetStateStorage'] = true,
    ['SetStateStorage'] = true,
    ['SetAllocatorFunctions'] = true,
    ['MemAlloc'] = true,
    ['MemFree'] = true,
    ['GetPlatformIO'] = true,
    ['GetMainViewport'] = true,
    ['RenderPlatformWindowsDefault'] = true,
    ['FindViewportByPlatformHandle'] = true
}

local imgui_api_unsupported_types = {
    'void*',        --translate void* to int 
    'void',
    '...',          --translate ... into unsupported , translate const char* fmt, va_list args to const char* string
    'const char* const items[]',
    'items_getter'
}   
function FuncProtoMT:CalcSupported()
    if imgui_api_ignore_fnames[self.name] then
        self.supported = false
        return 
    end
    local supported = true
    if self.args then
        for i,arg in ipairs(self.args) do
            -- print('//arg'..i, arg:Type(), arg.name, '=', arg.def or '')
            supported = arg:IsSupported() and (
                    arg.type == 'ImVec2' or
                    arg.type == 'ImVec4' or
                    arg:Type() == 'const char*' or
                    (arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int')  or
                    (arg.type =='float' or arg.type =='double') or
                    arg.type == 'bool'
                )
            if not supported then
                break
            end
        end
    end
    self.supported = supported
end

FuncProtoMT.__index = FuncProtoMT
function func_proto_create(name)
    local func_proto = {
        name = name,
        args = {},
        ret  = nil,
        raw_args = '',
        brace_repls = {},
        proto_type= '',
    }
    setmetatable(func_proto, FuncProtoMT)
    return func_proto
end

function parse_type(str)
    if str =='...' then
        return type_proto_create('', '...', '')
    end
    local final
    local fs,fe = str:find('^%s*const')
    if fs then
        final = 'const'
        str = str:sub(fe+1)
    end

    local unsigned
    fs,fe = str:find('^%s*unsigned')
    if fs then
        unsigned = 'unsigned'
        str = str:sub(fe+1)
    end

    local type, decor, name = str:gmatch('([%w_]+)%s*([&*])%s*([@%w_]+)')()
    if not decor then
        type, name = str:gmatch('([%w_]+)%s+([@%w_]+)')()
    end
    if unsigned then
        type = unsigned..' '..type
    end
    
    
    -- print('final', final, 'type', type, 'decor', decor, 'name', name)
    fs, fe = str:find(name)
    str = str:sub(fe+1)
    local is_array = false 
    local array_size = 0
    if str:find('%[%d*%]') then
        is_array = true
        local sz = str:gmatch('%[(%d+)%]')()
        if sz then
            array_size = math.tointeger(sz)
        end
    end

    return type_proto_create(name, type, decor, final, is_array, array_size)
end

function parse_funcargs_cap(args, brace_repls)
    args = args..','
    local all_args = {}
    for arg_block in args:gmatch('(.-),') do
        arg_block = arg_block:gsub('@', ' @', 1)    

        if arg_block:find('%.%.%.') then
            local arg = parse_type('...')
            table.insert(all_args, arg)
        else
            local equal_left
            local equal_right
            local l,r = arg_block:find('=') 
            if l then
                equal_left = arg_block:sub(1,l-1)
                equal_right = arg_block:sub(r+1)
            else 
                equal_left = arg_block
            end
            
            local arg = parse_type(equal_left)
            if equal_right then
                equal_right = equal_right:gsub('(@%d+)',function(cap)
                    local index = cap:gmatch('(%d+)')()
                    return brace_repls[math.tointeger(index)]
                end)
                equal_right = equal_right:gsub(' ','')
                    arg.def = equal_right
                end
            table.insert(all_args, arg)
        end
    end
    return all_args
end

function parse_typedef(content)
    for line in content:gmatch('(.-)\n') do
        -- print('parse_typedef', line)
        if line:find('typedef') and line:find('ImS64')==nil and line:find('ImU64')==nil then
            if line:find('typedef.+%b()%s*%b();') then
                local ret_type, ret_dec, fname_cap, args_cap = line:gmatch('typedef%s*([%w_]+)%s*([&*]?)%s*(%b())(%b());')()
                local fname = fname_cap:gmatch('[*]%s*([%w_]+)')()
                -- print('find function', ret_type, ret_dec, fname, args)            
            else
                local words = {} 
                for word in line:gmatch('([^%s;]+)') do
                    table.insert(words, word)
                end
                if #words > 2 then
                    local first = words[1]
                    local last = words[#words]
                    table.remove(words,1)
                    table.remove(words,#words)
                    imgui_typedefs[last] = table.concat(words,' ')    
                end
            end
        end
    end
end

function parse_imvec2(content)
    imgui_typedefs['ImVec2'] = 'ImVec2';
end

function parse_imvec4(content)
    imgui_typedefs['ImVec4'] = 'ImVec4';
end


   
function parse_imgui_api(content, namespace)
    for line in content:gmatch('.-\n') do
        if line:find('IMGUI_API') then
            local rconst, rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+(const%s+)([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            if not rconst then
                rconst = ''
                rtype, rdec, fname, args = line:gmatch('IMGUI_API%s+([%w_]*)%s*([*&]?)%s*([%w_]+)(%b())')()
            end
            
            local proto = func_proto_create(fname)
            proto.ret =  type_proto_create('',rtype, rdec, rconst) 
            proto.raw_args = args
            proto.namespace = namespace
            if args ~= '()' then
                local brace_repls = {}  
                args = args:sub(2,#args-1)
                args = args:gsub('(%b())',function(cap)
                    table.insert(brace_repls, cap)
                    return '@'..#brace_repls
                end)
                proto.brace_repls = brace_repls
                proto.args  = parse_funcargs_cap(args, brace_repls)
            end
            proto:CalcSupported()
            table.insert(imgui_apis, proto)
        end
    end   
end

function parse_enum_blocks(content)
    imgui_enums = imgui_enums or {}
    for enum_name, enum_block in content:gmatch('enum%s*([%w_]+).-(%b{});') do
        -- print('enum_name', enum_name)
        imgui_enums[enum_name] =imgui_enums[enum_name] or {}
        local pause_parse = false
        for line in enum_block:gmatch('(.-)\n') do
            if line:find('#if[n]?def') then
                pause_parse = true
            elseif line:find('#endif') then
                pause_parse = false
            else
                if not pause_parse and line:find(enum_name) then
                    line = line:gsub('%s','')
                    
                    local name = line:gmatch('([%w_]+)%s*')()
                    table.insert(imgui_enums[enum_name],name)
                end
            end
        end
    end
end

 --[[
    函数生成算法:
    1. c函数原型里, 每有一个arg, 就对应于一个lua的check 或者 toxxx 语句
    2. 读出这些args后, 调用imguiAPI, 得到返回值
    3. 用c的返回类型转换成lua的pushxxx
    4. 打完收工
]]
function hook_write_fname(prop_name)
    if prop_name == 'Combo2' then
        return 'Combo'
    end
    return prop_name
end

function output_imguiapis(cximgui_path)
    local cximgui_file = io.open(cximgui_path,'w')
    cximgui_file:write([[#include "cximgui.h"]]..'\n') 
    cximgui_file:write([[#include <imgui.h>]]..'\n') 
    cximgui_file:write([[#include <string.h>]]..'\n') 
    local unsupported_func = {}
    for i,proto in ipairs(imgui_apis) do
        cximgui_file:write('//'..proto:ToString()..'\n')
        if not proto.supported then
            cximgui_file:write('//UnSupported '.. proto.name..'\n')
            table.insert(unsupported_func, proto:WrapName())
        else
            local call_api_args = {}
            local fun_impl = {}
            function fun_impl:write_line(fmt, ...) 
                table.insert(fun_impl,string.format(fmt,...))
            end
            fun_impl:write_line('int cximgui_%s(lua_State* L){', proto:WrapName());
            if #proto.args > 0 then
                fun_impl:write_line('\tint __argi__ = 1;');
            end
            local ret_args = {}
            for i,arg in ipairs(proto.args) do
                if (arg:IsRef() and arg.final=='') 
                or (arg:IsPtr() and arg.final=='') 
                or (arg.is_array and arg.final=='') then
                    table.insert(ret_args, arg)
                end
                if arg.type == 'ImVec2' then
                    if arg.def and not arg:IsPtr() then
                        fun_impl:write_line('\t%s %s_def = %s;', arg.type, arg.name, arg.def)
                        fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                        fun_impl:write_line( '\t%s.x = (float)luaL_optnumber(L,__argi__  ,%s_def.x);' ,arg.name, arg.name)
                        fun_impl:write_line( '\t%s.y = (float)luaL_optnumber(L,__argi__+1,%s_def.y);' ,arg.name, arg.name)
                        local wline = '\tif( _tmp_.x != _tmp__def.x || _tmp_.y != _tmp__def.y  ) __argi__+=2;'
                        wline = wline:gsub('_tmp_',arg.name)
                        fun_impl:write_line( wline)
                    else
                        fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                        fun_impl:write_line( '\t%s.x = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                        fun_impl:write_line( '\t%s.y = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                    end
                    
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif arg.type == 'ImVec4' then
                    if arg.def  and not arg:IsPtr() then
                        fun_impl:write_line('\t%s %s_def = %s;', arg.type, arg.name, arg.def)
                        fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                        fun_impl:write_line( '\t%s.x = (float)luaL_optnumber(L,__argi__  ,%s_def.x);' ,arg.name, arg.name)
                        fun_impl:write_line( '\t%s.y = (float)luaL_optnumber(L,__argi__+1,%s_def.y);' ,arg.name, arg.name)
                        fun_impl:write_line( '\t%s.z = (float)luaL_optnumber(L,__argi__+2,%s_def.y);' ,arg.name, arg.name)
                        fun_impl:write_line( '\t%s.w = (float)luaL_optnumber(L,__argi__+3,%s_def.y);' ,arg.name, arg.name)
                        local wline = '\tif( _tmp_.x != _tmp__def.x || _tmp_.y != _tmp__def.y ||  _tmp_.z != _tmp__def.z || _tmp_.w != _tmp__def.w) __argi__+=4;'
                        wline = wline:gsub('_tmp_',arg.name)
                        fun_impl:write_line( wline)
                    else
                        fun_impl:write_line('\t%s %s;', arg.type, arg.name)
                        fun_impl:write_line( '\t%s.x = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                        fun_impl:write_line( '\t%s.y = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                        fun_impl:write_line( '\t%s.z = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                        fun_impl:write_line( '\t%s.w = (float)lua_tonumber(L,__argi__++);' ,arg.name)
                    end

                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                elseif arg:Type() == 'const char*' then
                    if arg.def then
                        fun_impl:write_line( '\t%s %s = luaL_optstring(L, __argi__++, %s);' ,arg:Type(), arg.name, arg.def)
                        -- fun_impl:write_line( '\tif( %s != %s ) __argi__++;' ,arg.name, arg.def)
                    else
                        fun_impl:write_line( '\t%s %s = lua_tostring(L, __argi__++);' ,arg:Type(), arg.name)
                    end
                    table.insert(call_api_args, arg.name)
                elseif (arg.type == 'int' or arg.type =='unsigned int' or arg.type =='unsigned short' or imgui_typedefs[arg.type] == 'int' or imgui_typedefs[arg.type] == 'unsigned int')  then
                    if arg.is_array then
                        fun_impl:write_line('\t%s %s[%d];' ,arg.type, arg.name, arg.array_size)
                        for i=0,arg.array_size-1 do
                            fun_impl:write_line( '\t%s[%d] = (%s)lua_tointeger(L, __argi__++);', arg.name, i, arg.type)
                        end
                        table.insert(call_api_args, arg.name)
                    else
                        if arg.def then
                            fun_impl:write_line( '\t%s %s = (%s)luaL_optinteger(L, __argi__++, %s);' ,arg.type, arg.name, arg.type, arg.def)
                            -- fun_impl:write_line( '\tif( %s != %s ) __argi__++;' ,arg.name, arg.def)
                        else
                            fun_impl:write_line( '\t%s %s = (%s)lua_tointeger(L, __argi__++);' ,arg.type, arg.name, arg.type)
                        end                    
                        local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                        table.insert(call_api_args, call_arg)
                    end
                elseif (arg.type =='float' or arg.type =='double') then
                    if arg.is_array then
                        fun_impl:write_line('\t%s %s[%d];' ,arg.type, arg.name, arg.array_size)
                        for i=0,arg.array_size-1 do
                            fun_impl:write_line( '\t%s[%d] = (%s)lua_tonumber(L, __argi__++);', arg.name, i, arg.type)
                        end
                        table.insert(call_api_args, arg.name)
                    else
                        if arg.def then
                            fun_impl:write_line( '\t%s %s = (%s)luaL_optnumber(L, __argi__++, %s);' ,arg.type, arg.name , arg.type, arg.def)
                            -- fun_impl:write_line( '\tif( %s != %s ) __argi__++;' ,arg.name, arg.def)
                        else
                            fun_impl:write_line( '\t%s %s = (%s)lua_tonumber(L, __argi__++);' ,arg.type, arg.name , arg.type)
                        end                                        
                        
                        local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                        table.insert(call_api_args, call_arg)
                    end
                    
                elseif arg.type == 'bool' then
                    if arg.def then
                        fun_impl:write_line( '\t%s %s = lua_isboolean(L, __argi__) ? lua_toboolean(L, __argi__++) : (bool)%s;' ,arg.type, arg.name , arg.def)
                    else
                        fun_impl:write_line( '\t%s %s = lua_toboolean(L, __argi__++);' ,arg.type, arg.name)
                    end                                        
                    local call_arg = arg:IsPtr() and ('&'..arg.name) or arg.name
                    table.insert(call_api_args, call_arg)
                else
                    print('error')
                    break
                end 
            end
            if proto.supported then
                local ret = proto.ret
                local ret_cnt = 1
                if ret.type =='void' then
                    ret_cnt  = 0
                    if proto.namespace == 'ImDrawList' then
                        fun_impl:write_line( '\tImGui::GetOverlayDrawList()->%s(%s);', proto.name, table.concat(call_api_args,','))    
                    else
                        fun_impl:write_line( '\tImGui::%s(%s);', proto.name, table.concat(call_api_args,','))    
                    end
                else
                    if proto.namespace == 'ImDrawList' then
                        fun_impl:write_line( '\t%s __ret__ = ImGui::GetOverlayDrawList()->%s(%s);', ret:Type(), proto.name, table.concat(call_api_args,','))    
                    else
                        fun_impl:write_line( '\t%s __ret__ = ImGui::%s(%s);', ret:Type(), proto.name, table.concat(call_api_args,','))    
                    end
                    
                    if ret.type == 'ImVec2' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.x);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.y);')
                        ret_cnt = 2
                    elseif ret.type == 'ImVec4' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.x);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.y);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.z);')
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__.w);')
                        ret_cnt = 4
                    elseif ret:Type() == 'const char*' then
                        fun_impl:write_line( '\tlua_pushstring(L, __ret__);')
                    elseif ret.type== 'int' or ret.type =='unsigned int' or ret.type =='unsigned short' or imgui_typedefs[ret.type] == 'int'   or imgui_typedefs[ret.type] == 'unsigned int' then
                        fun_impl:write_line( '\tlua_pushinteger(L, __ret__);')
                    elseif ret.type =='float' or ret.type =='double' then
                        fun_impl:write_line( '\tlua_pushnumber(L, __ret__);')
                    elseif ret.type == 'bool' then
                        fun_impl:write_line( '\tlua_pushboolean(L, __ret__);')
                    else 
                        ret_cnt = 0
                    end
                end

                for i,ret_arg in ipairs(ret_args) do
                    if ret_arg.type == 'ImVec2' then
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.x);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.y);', ret_arg.name)
                        ret_cnt = ret_cnt + 2
                    elseif ret_arg.type == 'ImVec4' then
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.x);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.y);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.z);', ret_arg.name)
                        fun_impl:write_line( '\tlua_pushnumber(L, %s.w);', ret_arg.name)
                        ret_cnt = ret_cnt + 4
                    elseif ret_arg.type== 'int' or ret.type =='unsigned int' or ret.type =='unsigned short' or imgui_typedefs[ret.type] == 'int'  or imgui_typedefs[ret.type] == 'unsigned int' then
                        if ret_arg.is_array then
                            for i=0,ret_arg.array_size-1 do
                                fun_impl:write_line( '\tlua_pushinteger(L, %s[%d]);', ret_arg.name,i)
                            end
                            ret_cnt = ret_cnt + ret_arg.array_size
                        else
                            fun_impl:write_line( '\tlua_pushinteger(L, %s);', ret_arg.name)
                            ret_cnt = ret_cnt + 1
                        end
                    elseif ret_arg.type =='float' or ret.type =='double' then
                        if ret_arg.is_array then
                            for i=0,ret_arg.array_size-1 do
                                fun_impl:write_line( '\tlua_pushnumber(L, %s[%d]);', ret_arg.name,i)
                            end
                            ret_cnt = ret_cnt + ret_arg.array_size
                        else
                            fun_impl:write_line( '\tlua_pushnumber(L, %s);', ret_arg.name)
                            ret_cnt = ret_cnt + 1
                        end
                    elseif ret_arg.type == 'bool' then
                        fun_impl:write_line( '\tlua_pushboolean(L, %s);', ret_arg.name)
                        ret_cnt = ret_cnt + 1
                    end
                end
                fun_impl:write_line( '\treturn %d;', ret_cnt)
                table.insert(fun_impl, '};')
                
                local imp = table.concat(fun_impl,'\n')
                cximgui_file:write(imp..'\n')
                cximgui_file:write(''..'\n')
            end
        end
    end
    cximgui_file:write('\n//total func', #imgui_apis, 'unSupported', #unsupported_func..'\n')


    cximgui_file:write('luaL_Reg cximgui_methods[] = {'..'\n')
    local last_name = ''
    local name_identifier = 2
    for i,proto in ipairs(imgui_apis) do
        if proto.supported then
            if last_name ~= proto.name then
                name_identifier = 2
                cximgui_file:write(string.format('\t{"%s",%s},', hook_write_fname(proto.name),'cximgui_'.. proto:WrapName()) ..'\n')   
            else
                cximgui_file:write(string.format('\t{"%s",%s},', hook_write_fname(proto.name..name_identifier),'cximgui_'.. proto:WrapName()) ..'\n')   
                name_identifier = name_identifier+1
            end
        end
        last_name = proto.name
    end 
    cximgui_file:write('\t{ NULL, NULL }'..'\n')
    cximgui_file:write('};'..'\n')

    cximgui_file:write([[

struct CXIMStrBuf {
    char* str;
    uint32_t size;
};

int cximgui_strbuf_reset(lua_State* L) {
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, 1);
    const char* str = lua_tostring(L, 2);
    size_t len = strlen(str);
    strcpy(buf->str, str);
    buf->str[len] = '\0';
    return 0;
}

int cximgui_strbuf_str(lua_State* L) {
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, 1);
    lua_pushstring(L, buf->str);
    return 1;
}

int cximgui_strbuf_size(lua_State* L) {
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, 1);
    lua_pushinteger(L, buf->size);
    return 1;
}

luaL_Reg cximgui_strbuf_methods[] = {
    { "str", cximgui_strbuf_str },
    { "size", cximgui_strbuf_size },
    { "reset", cximgui_strbuf_reset },
    { NULL,NULL }
};

int cximgui_strbuf_destroy(lua_State* L) {
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, 1);
    delete[] buf->str;
    return 0;
}

int cximgui_strbuf_create(lua_State* L) {
    const char* str = lua_tostring(L, 1);
    size_t len = strlen(str);
    uint32_t size = (uint32_t)lua_tointeger(L, 2);
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_newuserdata(L, sizeof(CXIMStrBuf));
    luaL_setmetatable(L, "mt_cximgui_strbuf");
    buf->str = new char[size];
    strcpy(buf->str, str);
    buf->str[len] = '\0';
    buf->size = size;
    return 1;
}

int cximgui_InputText_3_sui(lua_State* L) {
    int __argi__ = 1;
    const char* label = lua_tostring(L, __argi__++);
    CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, __argi__++);
    ImGuiInputTextFlags extra_flags = (ImGuiInputTextFlags)luaL_optinteger(L, __argi__, 0);
    if (extra_flags != 0) __argi__++;
    bool __ret__ = ImGui::InputText(label, buf->str, buf->size, extra_flags);
    lua_pushboolean(L, __ret__);
    return 1;
};

int cximgui_InputTextMultiline_4_suv2i(lua_State* L) {
	int __argi__ = 1;
	const char* label = lua_tostring(L, __argi__++);
	CXIMStrBuf* buf = (CXIMStrBuf*)lua_touserdata(L, __argi__++);
	ImVec2 size_def = ImVec2(0, 0);
	ImVec2 size;
	size.x = (float)luaL_optnumber(L, __argi__, size_def.x);
	size.y = (float)luaL_optnumber(L, __argi__ + 1, size_def.y);
	if (size.x != size_def.x || size.y != size_def.y) __argi__ += 2;
	ImGuiInputTextFlags extra_flags = (ImGuiInputTextFlags)luaL_optinteger(L, __argi__, 0);
	if (extra_flags != 0) __argi__++;
	bool __ret__ = ImGui::InputTextMultiline(label, buf->str, buf->size, size, extra_flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

int cximgui_TextColored_2_is(lua_State* L) {
	int __argi__ = 1;
	int color = (int)lua_tointeger(L, __argi__++);
	const char* s = lua_tostring(L, __argi__++);
	ImGui::TextColored(ImColor(color), s);
	return 0;
};

int cximgui_DockSpace_3_iv2i(lua_State*L) {
	int __argi__ = 1;
	int id = (int)lua_tointeger(L, __argi__++);
	ImVec2 size_def = ImVec2(0, 0);
	ImVec2 size;
	size.x = (float)luaL_optnumber(L, __argi__, size_def.x);
	size.y = (float)luaL_optnumber(L, __argi__ + 1, size_def.y);
	if (size.x != size_def.x || size.y != size_def.y) __argi__ += 2;
	ImGuiDockNodeFlags  flags = (ImGuiDockNodeFlags)luaL_optinteger(L, __argi__, 0);
	if (flags != 0) __argi__++; 
		
	ImGui::DockSpace(id, size, flags);
	return 0;
}

int cximgui_GetMainViewport(lua_State*L) {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	lua_pushinteger(L, viewport->ID);
	lua_pushinteger(L, (int)viewport->Pos.x);
	lua_pushinteger(L, (int)viewport->Pos.y);
	lua_pushinteger(L, (int)viewport->Size.x);
	lua_pushinteger(L, (int)viewport->Size.y);
	return 5;
}

int cximgui_ListBox_5_spipsii(lua_State* L) {
	int __argi__ = 1;
	const char* label = lua_tostring(L, __argi__++);
	int current_item = (int)lua_tointeger(L, __argi__++);
	
	int len =(int)luaL_len(L, __argi__);
	char** items = new char*[len];
	int i = 0;
	lua_pushnil(L);
	while (lua_next(L, __argi__) != 0) {
		size_t str_len = 0;
		const char* str = lua_tolstring(L, -1, &str_len);
		items[i] = new char[str_len + 1];
		strcpy(items[i], str);
		i++;
		lua_pop(L, 1);
	}
	__argi__++;
	int height_in_items = (int)luaL_optinteger(L, __argi__, -1);
	if (height_in_items != -1)__argi__++;

	bool __ret__ = ImGui::ListBox(label, &current_item, (const char* const*)items, len, height_in_items);
	for (int i = 0; i < len; i++) {
		delete[] items[i];
	}
	delete[] items;

	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, current_item);
	return 2;
}

int cximgui_clipper_list(lua_State*L)
{
	int size = (int)lua_tointeger(L, 1);
	ImGuiListClipper clipper(size);
	while (clipper.Step())
	{
		int ps = clipper.DisplayStart; 
		int pe = clipper.DisplayEnd;
		
		lua_pushvalue(L, 2);
		lua_pushinteger(L, ps);
		lua_pushinteger(L, pe);
		lua_pcall(L, 2, 0, 0);
	}
	return 0;
}

int cximgui_keys_mod(lua_State*L)
{
	const char* which = lua_tostring(L, 1);
	ImGuiIO& io = ImGui::GetIO();
	if (strcmp(which, "Ctrl") == 0) {
		lua_pushboolean(L, io.KeyCtrl);
	}
	else if (strcmp(which, "Shift") == 0) {
		lua_pushboolean(L, io.KeyShift);
	}
	else if (strcmp(which, "ALT") == 0) {
		lua_pushboolean(L, io.KeyAlt);
	}
	else if (strcmp(which, "Super") == 0) {
		lua_pushboolean(L, io.KeySuper);
	}
	return 1;
}

int cximgui_PlotLines_8_stisnnv2i(lua_State* L) {
	int __argi__ = 1;
	const char* label = lua_tostring(L, __argi__++);
	int table_index = __argi__++;
	int values_count = luaL_len(L, table_index);
	float* values = new float[values_count];
	for (int i = 0; i < values_count; i++)
	{
		lua_geti(L, table_index, i + 1);
		values[i] = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	int values_offset = (int)luaL_optinteger(L, __argi__++, 0);
	const char* overlay_text = luaL_optstring(L, __argi__++, NULL);
	float scale_min = (float)luaL_optnumber(L, __argi__++, FLT_MAX);
	float scale_max = (float)luaL_optnumber(L, __argi__++, FLT_MAX);
	ImVec2 graph_size_def = ImVec2(0, 0);
	ImVec2 graph_size;
	graph_size.x = (float)luaL_optnumber(L, __argi__, graph_size_def.x);
	graph_size.y = (float)luaL_optnumber(L, __argi__ + 1, graph_size_def.y);
	if (graph_size.x != graph_size_def.x || graph_size.y != graph_size_def.y) __argi__ += 2;
	int stride = (int)luaL_optinteger(L, __argi__++, sizeof(float));
	ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	delete[]values;
	return 0;
};

int cximgui_PlotHistogram_8_sntisnnv2i(lua_State* L) {
	int __argi__ = 1;
	const char* label = lua_tostring(L, __argi__++);
	int table_index = __argi__++;
	int values_count = luaL_len(L, table_index);
	float* values = new float[values_count];
	for (int i = 0; i < values_count; i++)
	{
		lua_geti(L, table_index, i + 1);
		values[i] = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	int values_offset = (int)luaL_optinteger(L, __argi__++, 0);
	const char* overlay_text = luaL_optstring(L, __argi__++, NULL);
	float scale_min = (float)luaL_optnumber(L, __argi__++, FLT_MAX);
	float scale_max = (float)luaL_optnumber(L, __argi__++, FLT_MAX);
	ImVec2 graph_size_def = ImVec2(0, 0);
	ImVec2 graph_size;
	graph_size.x = (float)luaL_optnumber(L, __argi__, graph_size_def.x);
	graph_size.y = (float)luaL_optnumber(L, __argi__ + 1, graph_size_def.y);
	if (graph_size.x != graph_size_def.x || graph_size.y != graph_size_def.y) __argi__ += 2;
	int stride = (int)luaL_optinteger(L, __argi__++, sizeof(float));
	ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	delete[]values;
	return 0;
};



luaL_Reg cximgui_extra_methods[] = {
	{ "CreateStrbuf", cximgui_strbuf_create },
{ "DestroyStrbuf", cximgui_strbuf_destroy },
{ "InputText",cximgui_InputText_3_sui },
{ "InputTextMultiline",cximgui_InputTextMultiline_4_suv2i },
{ "Text", cximgui_TextUnformatted_2_ss },
{ "TextColored", cximgui_TextColored_2_is },
{ "DockSpace", cximgui_DockSpace_3_iv2i },
{ "GetMainViewport", cximgui_GetMainViewport },
{ "ListBox", cximgui_ListBox_5_spipsii },
{ "ClipperList", cximgui_clipper_list },
{ "KeysMod", cximgui_keys_mod},
{ "PlotLines", cximgui_PlotLines_8_stisnnv2i},
{ "PlotHistogram", cximgui_PlotHistogram_8_sntisnnv2i},
{ NULL,NULL }
};

void luaopen_cximgui(lua_State* L) {

#define REG_IMGUI_ENUM(name)  (lua_pushinteger(L, name),lua_setglobal(L, #name))
#include "cximgui_enums.inl"
#undef REG_IMGUI_ENUM

    if (luaL_newmetatable(L, "mt_cximgui_strbuf")) {
        luaL_setfuncs(L, cximgui_strbuf_methods, 0);
        lua_setfield(L, -1, "__index");
    }
    else {
        std::cout << "associate cximgui_strbuf error!" << std::endl;
    }

    if (luaL_newmetatable(L, "mt_cximgui")) {
        luaL_setfuncs(L, cximgui_methods, 0);
        luaL_setfuncs(L, cximgui_extra_methods, 0);
        lua_setfield(L, -1, "__index");
    }
    else {
        std::cout << "associate cximgui error!" << std::endl;
    }

    lua_newtable(L);
    luaL_setmetatable(L, "mt_cximgui");
    lua_setglobal(L, "imgui");
}
]]..'\n')
    cximgui_file:close()
    os.rename( cximgui_path, vfs_makepath('common/src/imgui/cximgui.cpp'))
end

function parse_imgui_header(path)
    local file = io.open(path)
    local content = file:read('a')
    content = content:gsub('//.-\n','\n')   --去掉注释
    content = content:gsub('/%*.-%*/','')   
    content = remove_empty_lines(content)    
    
    imgui_typedefs['size_t'] = 'unsigned int'
    local parsed_skip_file = io.open(vfs_makepath('scripts/client/parsed_skip_file.txt') ,'w')
    
    imgui_apis = {}
    for i=1, #imgui_header_separate_flags-1 do
        local begin_str = imgui_header_separate_flags[i][1]
        local end_str = imgui_header_separate_flags[i+1][1]
        local parse_flag = imgui_header_separate_flags[i][2]
        local s = begin_str=='' and 1 or content:find(begin_str)
        local e = content:find(end_str)
        local sub = content:sub(s,e-1)
        content = content:sub(e)

        if parse_flag=='skip' then
        elseif parse_flag =='parse struct' then
            parse_struct(sub)
        elseif parse_flag =='parse typedef' then
            parse_typedef(sub)
        elseif parse_flag =='parse ImVec2' then
            parse_imvec2(sub)
        elseif parse_flag =='parse ImVec4' then
            parse_imvec4(sub)
        elseif parse_flag =='parse ImGuiAPI' then
            parse_imgui_api(sub)
        elseif parse_flag == 'parse ImDrawList' then
            parse_imgui_api(sub, 'ImDrawList')
        elseif parse_flag =='parse enum blocks' then
            parse_enum_blocks(sub)
        end
        parsed_skip_file:write('\nsub :'..parse_flag.. '\n'..sub)
    end
    parsed_skip_file:close()
    
    output_imguiapis(vfs_makepath('common/src/imgui/cximgui_new.cpp'))
    print('output_imguiapis done!')
end


parse_imgui_header(vfs_makepath('internals/imgui/include/imgui.h') )

function output_imgui_enums(path)
    
    local file = io.open(path, 'w')
    local enum_macro = "#define REG_IMGUI_ENUM(name)  (lua_pushinteger(L, name),lua_setglobal(L, #name))"
    local sorted_keys  = {}
    for k,v in pairs(imgui_enums) do
        table.insert(sorted_keys, k)
    end
    table.sort(sorted_keys)
    for _,key in ipairs(sorted_keys) do
        for i, name in ipairs(imgui_enums[key]) do
            -- print(name)
            file:write( string.format('REG_IMGUI_ENUM(%s);\n',name) )
        end
    end
    file:close()

    os.rename(path, vfs_makepath('common/src/imgui/cximgui_enums.inl'))
end

output_imgui_enums(vfs_makepath('common/src/imgui/cximgui_enums_new.inl'))
print('output_imgui_enums done!')



