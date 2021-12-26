
function gen_protocol()
    
    local path =  vfs_makepath('scripts/share/enums_protocol.lua')
    
    local module = {}
    setmetatable(module,{__index = _ENV})
    local func = loadfile(path,'bt',module)
    func()

    local protocol_h_path = vfs_makepath('common/src/protocol.h')
    local protocol_header = io.open(protocol_h_path,'w+')
    protocol_header:write([[#pragma once
#include <string>
#define  CX_MSG_HEADER_LEN 4

enum EProtocalType
{
]])    
    
    local ks = utils_fetch_sort_keys(module, function(a,b)
        return module[a] < module[b]
    end)    
    for i,k in ipairs(ks) do
        local v = module[k]
        if i~=#ks then
            protocol_header:write('\t'..k..'='..v..',\n')
        else
            protocol_header:write('\t'..k..'='..v..'\n')
        end
    end
    protocol_header:write([[};

void luaopen_protocol(lua_State* L);   
]])
    protocol_header:flush()
    protocol_header:close()
    
    local protocol_cpp_path = vfs_makepath('common/src/protocol.cpp')
    local protocol_cpp = io.open(protocol_cpp_path,'w+')
    protocol_cpp:write([[#include "protocol.h"

void luaopen_protocol(lua_State* L)
{
#define REG_ENUM(name)  (lua_pushinteger(L, name),lua_setglobal(L, #name))
]])
    for i,k in ipairs(ks) do
        local v = module[k]
        protocol_cpp:write(string.format('\tREG_ENUM(%s);\n',k))
    end
    protocol_cpp:write([[#undef REG_ENUM
	(lua_pushinteger(L, CX_MSG_HEADER_LEN), lua_setglobal(L, "CX_MSG_HEADER_LEN"));

}]])
    protocol_cpp:flush()
    protocol_cpp:close()

end

gen_protocol()