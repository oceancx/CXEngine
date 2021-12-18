function BuffOnStart(buff, actor, target)
    if IsClient() then
        local anim = animation_create(23,0xD9463A0C)
        anim:SetLoop(0)
        -- local offy = -avatar:GetFrameKeyY() + avatar:GetFrameHeight() / 2.0
        anim:SetOffsetY(10)
        buff.anim = anim 
        actor:AddBackAnim(anim)
    end
end

function BuffOnEnd(buff, actor, target)
    if IsClient() then
        buff.anim:Stop()
    end
end
