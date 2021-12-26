
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, actor, hit_actor)
    -- hit_actor:AddBuff(1,skill.turn)
    if IsServer() then
        local skill_lv = actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL)
        local target_lv = hit_actor:GetProperty(PROP_LV)
        local practice_lv = actor:GetProperty(PROP_SPELL_PRACTICE_SKILL_LV)
        local resistance_lv = hit_actor:GetProperty(PROP_SPELL_RESISTANCE_SKILL_LV)
        local rate = formula_calc_seal_success_rate(skill_lv,target_lv,practice_lv,resistance_lv)
        local random_rate = math.random()
        cxlog_info('催眠符', rate , random_rate)
        if random_rate < rate then
            skill.success = true
            actor_add_buff(hit_actor, 30)
        else
            skill.success = false
        end
    end
    if IsClient() then
        if skill.success then
            actor_add_buff(hit_actor, 30)
        end
    end
end

function SkillOnEnd(skill, actor, target)

end