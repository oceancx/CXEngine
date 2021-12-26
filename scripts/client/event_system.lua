local handlers = {}

function event_system_fire_event(ev, ...)
    if handlers[ev] then
        handlers[ev](...)
    end
end

function event_system_register(ev, func)
    handlers[ev] = func
end

function event_system_unregister(ev)
    handlers[ev] = nil
end


local actor_events = {}
function actor_reg_event(actor, ev, func)
    local id = actor:GetID()
    actor_events[id] = actor_events[id] or {}
    local oldfunc = actor_events[id][ev]
    actor_events[id][ev] = func
    return oldfunc
end

function actor_unreg_event(actor, ev)
    local id = actor:GetID()
    if actor_events[id] then
        actor_events[id][ev] = nil
    end
end

function actor_fire_event(actor, ev, ...)
    local id = actor:GetID()
    if actor_events[id] then
        actor_events[id][ev](actor,...)
    end
end

