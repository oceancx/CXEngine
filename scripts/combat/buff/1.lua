function BuffOnStart(buff, actor, turn)
    if IsClient() then
        local anim = animation_create(23,0xCA8FDEAD)
        buff.anim = anim 
        anim:SetLoop(0)
    -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        actor:AddBackAnim(anim)
    end
end

function BuffOnEnd(buff, actor, target)
    if IsClient() then
        buff.anim:Stop()
    end
end
