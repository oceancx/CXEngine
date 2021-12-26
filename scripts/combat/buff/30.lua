function BuffOnStart(buff, actor, target)
    buff.turn = 4
    buff.turn_count_down = 4
	if IsServer() then
		actor:SetProperty(PROP_COMBAT_SKILL_BAN_SPELL, true	)
		actor:SetProperty(PROP_COMBAT_SKILL_BAN_ATK, true)
	end
    if IsClient() then
        local anim = animation_create(23,0xCA8FDEAD)
        buff.anim = anim 
        anim:SetLoop(0)
        actor:AddBackAnim(anim)
    end
end

function BuffOnEnd(buff, actor, target)
	if IsServer() then
		actor:SetProperty(PROP_COMBAT_SKILL_BAN_SPELL, false)
		actor:SetProperty(PROP_COMBAT_SKILL_BAN_ATK, false)
	end
    if IsClient() then
        buff.anim:Stop()
    end
end
