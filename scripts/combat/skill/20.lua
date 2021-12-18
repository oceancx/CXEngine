
function SkillOnStart(skill, actor, target)

end

function SkillOnHit(skill, master, target, target_i, combo_i)
    if IsServer() then
        if target:IsDead() then
            actor_add_buff(master, 11)
        elseif combo_i == 3 then
            actor_add_buff(master, 11)
        end
    else
        if skill.effects[target_i].combo == combo_i then
            actor_add_buff(master, 11)
        end
    end
end

function SkillOnEnd(skill, actor, target)

end