
local BuffMT = {}
function BuffMT:new(o)
    o = o or {}
    self.__index = self 
    setmetatable(o, self)
    return o
end

local buff_env = {
    __index = _ENV
}

local buff_table = {}
function init_buffs()
    local buff_template_tbl = content_system_get_table('buff')
    buff_table = buff_template_tbl
    for id,buff in pairs(buff_table) do
        local dir = vfs_get_luapath('../combat/buff/')
        local path = dir..id..'.lua'
        local f = io.open(path)
        if f then
            local chunk = f:read('a')
            f:close()
            local module = setmetatable({},buff_env)
            local fun,err = load(chunk,'@'..path,'bt', module)
            if fun then
                fun()
                buff.BuffOnStart = module.BuffOnStart
                buff.BuffOnEnd = module.BuffOnEnd
                buff.BuffOnNextTurn = module.BuffOnNextTurn
            else
                cxlog_info(fun,err)
            end
        end 
    end
end

function init_buff_by_templ(buff, templ)
    buff.turn = templ.turn
    buff.turn_count_down = buff.turn
    buff.BuffOnStart = templ.BuffOnStart
    buff.BuffOnEnd = templ.BuffOnEnd
    buff.BuffOnNextTurn = templ.BuffOnNextTurn
end

local ActorMT = actor_get_metatable()
__actors_buffs__ = {}
function actor_get_buffs(actor)
    local pid = actor:GetID()
    if not __actors_buffs__[pid] then 
        __actors_buffs__[pid] = {}
    end
    return __actors_buffs__[pid]
end

function actor_add_buff(actor, buff_tid)
    local buff = {}
    buff.id = utils_next_uid('buff')
    buff.tid = buff_tid
    local buff_templ = buff_table[buff_tid]
    init_buff_by_templ(buff, buff_templ)

    local buffs = actor_get_buffs(actor)
    buffs[buff.id] = buff
    if buff.BuffOnStart then
        buff.BuffOnStart(buff, actor)
    end
end

function actor_remove_buff(actor, buff_id)
    local buffs = actor_get_buffs(actor)
    local buff = buffs[buff_id]
    if buff.BuffOnEnd then
        buff.BuffOnEnd(buff, actor)
    end
    buffs[buff.id] = nil
end

function actor_remove_buff_by_tid(actor, buff_tid)
    local buffs = actor_get_buffs(actor)
    for buff_id, buff in pairs(buffs) do
        if buff.tid == buff_tid then
            actor_remove_buff(actor, buff.id)
            break
        end
    end
end


function actor_buff_next_turn(actor)
    local buffs = actor_get_buffs(actor)
    for id, buff in pairs(buffs) do
        if buff.turn_count_down > 0 then
            buff.turn_count_down = buff.turn_count_down - 1
            if buff.turn_count_down == 0 then
                actor_remove_buff(actor, buff.id)
            else
                if buff.BuffOnNextTurn then
                    buff.BuffOnNextTurn(buff, actor, buff.turn - buff.turn_count_down)
                end
            end
        end
    end
end


function process_turn_buffs(battle)
    cxlog_info('process_turn_buffs ', battle)
    for i,actor in ipairs(battle.actors) do
        actor_buff_next_turn(actor)
    end
end