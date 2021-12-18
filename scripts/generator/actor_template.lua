
local actor_enum_typename_to_type ={
    bool = 'PROP_TYPE_BOOL',
    int  = 'PROP_TYPE_INT',
    uint64 = 'PROP_TYPE_UINT64',
    float = 'PROP_TYPE_FLOAT',
    str  = 'PROP_TYPE_STR',
    vec2 = 'PROP_TYPE_VEC2'
}

local actor_enums = {} 
local actor_enum_types = {} 
function gen_actor_enum()
    local path = vfs_makepath('common/src/actor/actor_enum.h')
    local f = io.open(path,'w')
    f:write('#pragma once\n')
    f:write('#include <lua.h>\n')
    
    f:write([[
enum EActorProp
{]]..'\n')
    for i,e in ipairs(actor_enums) do
        f:write('\t'..e)
        -- if i ~= #actor_enums then
            f:write(',')
        -- end
        f:write('\n')
    end
    f:write('\tPROP_COUNT\n')
    f:write('};'..'\n')


    f:write([[
enum EActorPropType
{]]..'\n')

    local sort_enums_types = utils_fetch_sort_keys(actor_enum_types)
    for i,k in ipairs(sort_enums_types) do
        f:write('\t'.. actor_enum_typename_to_type[k])
        if i~=#sort_enums_types then
            f:write(',')
        end
        f:write('\n')
    end
    f:write('};'..'\n')
    
    f:write([[void luaopen_actor_enum(lua_State* L);]]..'\n')
    f:flush()
    f:close()

    path = vfs_makepath('common/src/actor/actor_enum.cpp')
    f = io.open(path,'w')
    f:write('#include "actor_enum.h"\n');
    f:write([[void luaopen_actor_enum(lua_State* L) {
#define REG_ENUM(e) (lua_pushinteger(L, e), lua_setglobal(L, #e))]]..'\n')

    f:write('//ENUM_ACTOR_PROP\n')
    for i,e in ipairs(actor_enums) do
        f:write(string.format('\tREG_ENUM(%s);\n',e) )
    end
    f:write(string.format('\tREG_ENUM(PROP_COUNT);\n') )
    

    f:write('//ENUM_ACTOR_PROP_TYPE\n')
    for i,k in ipairs(sort_enums_types) do
        f:write(string.format('\tREG_ENUM(%s);\n',actor_enum_typename_to_type[k]) )
    end

    f:write([[#undef REG_ENUM
};]]..'\n')
    f:flush()
    f:close()
end

function solve()
    local tbl  = utils_parse_tsv('actor_template',{
        { name='name'},
        { name='type'},
        { name='def'},
        { name='sync', fmt='i', def=0},
    })    

    for i,row in ipairs(tbl) do
        table.insert(actor_enums,  row.name:upper())  
        actor_enum_types[row.type] = true
    end
    pcall(gen_actor_enum)  
    cxlog_info('ActorProp生成完成！')
end

solve()