local ui_is_show_props = false
function ui_toggle_show_props()
    ui_is_show_props = not ui_is_show_props
    return ui_is_show_props
end


local selected_actor_uid = 0
local edit_prop_lv = 0

local edit_health = 0
local edit_magic = 0
local edit_force = 0
local edit_stamina = 0
local edit_agility = 0

function draw_prop_bar(actor, prop, total, edit_prop)
    local remain = total - edit_health - edit_magic - edit_force - edit_stamina - edit_agility
    imgui.Text(prop[1].. ': '.. prop[3])
    imgui.SameLine()
    imgui.PushItemWidth(80)
    local res
    if remain+edit_prop == 0 then
        res, edit_prop = imgui.DragInt('##bar'..prop[1], edit_prop, 1, -1, 0)
    else
        res, edit_prop = imgui.DragInt('##bar'..prop[1], edit_prop, 1, 0, remain + edit_prop)
    end
    imgui.PopItemWidth()
    return edit_prop
end

function draw_prop_points_panel(actor, lv)
    local res 
    local total = (lv+1) * 5

    edit_health = actor:GetProperty(PROP_ASSIGN_HEALTH)
    edit_magic = actor:GetProperty(PROP_ASSIGN_MAGIC)
    edit_force = actor:GetProperty(PROP_ASSIGN_FORCE)
    edit_stamina = actor:GetProperty(PROP_ASSIGN_STAMINA)
    edit_agility = actor:GetProperty(PROP_ASSIGN_AGILITY)

    
    imgui.BeginGroup()
    imgui.Dummy(30,20)
    
    local prop_bars = {
        {'体质', PROP_ASSIGN_HEALTH, actor:GetHealthProp()},
        {'魔力', PROP_ASSIGN_MAGIC, actor:GetMagicProp()},
        {'力量', PROP_ASSIGN_FORCE, actor:GetForceProp()},
        {'耐力', PROP_ASSIGN_STAMINA, actor:GetStaminaProp()},
        {'敏捷', PROP_ASSIGN_AGILITY, actor:GetAgilityProp()},
    }
    
    edit_health =  draw_prop_bar(actor, prop_bars[1], total, edit_health)
    edit_magic =   draw_prop_bar(actor, prop_bars[2], total, edit_magic)
    edit_force =   draw_prop_bar(actor, prop_bars[3], total, edit_force)
    edit_stamina = draw_prop_bar(actor, prop_bars[4], total, edit_stamina)
    edit_agility = draw_prop_bar(actor, prop_bars[5], total, edit_agility)

    actor:SetProperty(PROP_ASSIGN_HEALTH, edit_health)
    actor:SetProperty(PROP_ASSIGN_MAGIC, edit_magic)
    actor:SetProperty(PROP_ASSIGN_FORCE, edit_force)
    actor:SetProperty(PROP_ASSIGN_STAMINA, edit_stamina)
    actor:SetProperty(PROP_ASSIGN_AGILITY, edit_agility)

    imgui.EndGroup()

    imgui.Text('剩余点：'.. actor:GetRemainPropPoints())
    imgui.SameLine()
    if imgui.Button('重新加点') then
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:ClearAssignPoints()
        ]], actor:GetID()))
    end
        
    imgui.SameLine()
    if imgui.Button('确认加点') then
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:SetProperty(PROP_ASSIGN_HEALTH, %d)
            actor:SetProperty(PROP_ASSIGN_MAGIC, %d)
            actor:SetProperty(PROP_ASSIGN_FORCE, %d)
            actor:SetProperty(PROP_ASSIGN_STAMINA, %d)
            actor:SetProperty(PROP_ASSIGN_AGILITY, %d)
        ]], actor:GetID(), edit_health, edit_magic, edit_force, edit_stamina, edit_agility))
    end
end

local prop_school_skill_lv_hp = 0
local prop_school_skill_lv_mp = 0
local prop_school_skill_lv_targethit = 0
local prop_school_skill_lv_damage = 0
local prop_school_skill_lv_defend = 0
local prop_school_skill_lv_spiritual = 0
local prop_school_skill_lv_speed = 0
local prop_school_skill_lv_dodge = 0


function draw_player_skill_bar(actor, bar)
    local actor_lv = actor:GetProperty(PROP_LV)
    imgui.PushItemWidth(50)
    local res, lv = imgui.DragInt(bar[1].. '技能等级',  actor:GetProperty(bar[2]) , 1.0, 0, actor_lv+10)
    imgui.PopItemWidth()
    if res then
        actor:SetProperty(bar[2], lv)
        net_manager_player_dostring(string.format([[ 
            local actor = actor_manager_fetch_player_by_id(%d)
            actor:SetProperty(%d, %d)
        ]], actor:GetID(), bar[2], lv))
    end
end

local pannel = {
    {'HP', PROP_SCHOOL_SKILL_LV_HP },
    {'MP', PROP_SCHOOL_SKILL_LV_MP },
    {'命中', PROP_SCHOOL_SKILL_LV_TARGETHIT },
    {'伤害', PROP_SCHOOL_SKILL_LV_DAMAGE },
    {'防御', PROP_SCHOOL_SKILL_LV_DEFEND },
    {'灵力', PROP_SCHOOL_SKILL_LV_SPIRITUAL },
    {'速度', PROP_SCHOOL_SKILL_LV_SPEED },
    {'闪避', PROP_SCHOOL_SKILL_LV_DODGE },
}

function draw_player_skill_pannel(actor)
    imgui.BeginGroup()
    draw_player_skill_bar(actor, pannel[1])
    draw_player_skill_bar(actor, pannel[2])
    draw_player_skill_bar(actor, pannel[3])
    draw_player_skill_bar(actor, pannel[4])
    draw_player_skill_bar(actor, pannel[5])
    draw_player_skill_bar(actor, pannel[6])
    draw_player_skill_bar(actor, pannel[7])
    draw_player_skill_bar(actor, pannel[8])
    imgui.EndGroup()
end

function draw_player_equip_pannel(actor)
    -- prop_equip_hp	float	0	1
    -- prop_equip_mp	float	0	1
    -- prop_equip_target	float	0	1
    -- prop_equip_damage	float	0	1
    -- prop_equip_defend	float	0	1
    -- prop_equip_spiritual	float	0	1
    -- prop_equip_agile	float	0	1
    imgui.Text('头盔')
    imgui.Text('项链')
    imgui.Text('武器')
    imgui.Text('衣服')
    imgui.Text('腰带')
    imgui.Text('鞋子')
end

function draw_player_practice_lv(actor)
    function draw_practice_lv_bar(label, enum)
        local actor_lv = actor:GetProperty(PROP_LV)
        imgui.PushItemWidth(50)
        local res, lv = imgui.DragInt(label,  actor:GetProperty(enum) , 1.0, 0, math.min(25,actor_lv//5-4))
        imgui.PopItemWidth()
        if res then
            actor:SetProperty(enum, lv)
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(%d, %d)
            ]], actor:GetID(), enum, lv))
        end
    end

    draw_practice_lv_bar('攻击修炼等级' , PROP_ATK_PRACTICE_SKILL_LV) 
    draw_practice_lv_bar('防御修炼等级' , PROP_ATK_RESISTANCE_SKILL_LV) 
    draw_practice_lv_bar('法术修炼等级' , PROP_SPELL_PRACTICE_SKILL_LV) 
    draw_practice_lv_bar('法抗修炼等级' , PROP_SPELL_RESISTANCE_SKILL_LV) 
end

function actor_type_tostring(actor_type)
    if actor_type == ACTOR_TYPE_PLAYER then
        return '玩家'
    elseif actor_type == ACTOR_TYPE_NPC then
        return 'NPC'
    elseif actor_type == ACTOR_TYPE_SUMMON then
        return '召唤兽'
    else
        return '其他类型'
    end
end


function fetch_weapon_keys(tbl, avatar_key)
    local weapon_keys = {}
    local strs= utils_string_split(avatar_key,'-')
    local role_key , weapon_key  = strs[1], strs[2]
    for k, v in pairs(tbl) do
        if role_key == v.role and weapon_key == v.type then
            table.insert(weapon_keys,k)
        end
    end
    table.sort(weapon_keys)
    return weapon_keys
end


local team_info_select_actor 
local ActorNameSB = imgui.CreateStrbuf('test',256)
function ui_show_props()
    if not ui_is_show_props then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('ActorEditor')
    imgui.BeginChild('LEFT_PANNEL#uishoprops',100)
        local actors = actor_manager_fetch_all_actors()
        local drawbutton = function(actor)
            if imgui.Button(actor:GetName()..'##'..actor:GetID()) then
                selected_actor_uid = actor:GetID()
                prop_school_skill_lv_hp = actor:GetProperty(PROP_SCHOOL_SKILL_LV_HP) 
                prop_school_skill_lv_mp = actor:GetProperty(PROP_SCHOOL_SKILL_LV_MP) 
                prop_school_skill_lv_targethit = actor:GetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT) 
                prop_school_skill_lv_damage = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE) 
                prop_school_skill_lv_defend = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DEFEND) 
                prop_school_skill_lv_spiritual = actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL) 
                prop_school_skill_lv_speed = actor:GetProperty(PROP_SCHOOL_SKILL_LV_SPEED) 
                prop_school_skill_lv_dodge = actor:GetProperty(PROP_SCHOOL_SKILL_LV_DODGE) 

                prop_atk_practice_skill_lv	= actor:GetProperty(PROP_ATK_PRACTICE_SKILL_LV)
                prop_atk_resistance_skill_lv	= actor:GetProperty(PROP_ATK_RESISTANCE_SKILL_LV)
                prop_spell_practice_skill_lv	= actor:GetProperty(PROP_SPELL_PRACTICE_SKILL_LV)
                prop_spell_resistance_skill_lv	= actor:GetProperty(PROP_SPELL_RESISTANCE_SKILL_LV)

                ActorNameSB:reset(actor:GetName())
            end
        end
        imgui.Text('玩家')
        for i,actor in pairs(actors) do
            if actor:IsPlayer() then
                drawbutton(actor)
            end
        end

        imgui.Text('召唤兽')
        for i,actor in pairs(actors) do
            if actor:IsSummon() then
                drawbutton(actor)
            end
        end
    imgui.EndChild()
    local actor = actor_manager_fetch_player_by_id(selected_actor_uid)
    if actor then
        imgui.SameLine()
        imgui.BeginChild('RightPanel##uiprops')
        if imgui.Button('创建') then
            local req = {}
            req.pid = player:GetID()
            req.props = actor:GetProperties()
            net_send_message(PTO_C2S_CREATE_ACTOR, cjson.encode(req))
        end
        imgui.SameLine()
        if imgui.Button('删除') then
            net_send_message(PTO_C2S_DELETE_ACTOR, cjson.encode({ 
                    pid = player:GetID(),
                    delete_pid = actor:GetID()
                } 
            ))
            selected_actor_uid = nil
            imgui.EndChild()
            imgui.End()
            return
        end
        imgui.SameLine()
        if imgui.Button('保存') then
            net_send_message(PTO_C2C_SAVE_ACTORS, cjson.encode({}))
        end
        imgui.SameLine()
        if imgui.Button('传送') then
            net_manager_player_dostring(string.format([[
                player:SetPos(%.f, %.f)
            ]], actor:GetPos()))  
            player:SetPos(actor:GetPos())
        end
        imgui.SameLine()
        if imgui.Button('摆怪') then
            local x,y = player:GetPos()
            net_manager_actor_dostring(actor:GetID(),[[
                actor:SetPos(%.f, %.f)
            ]],x,y) 
            actor:SetPos(player:GetPos())
        end
        imgui.SameLine()
        if imgui.Button('SetLocal') then
            actor_manager_set_local_player(actor:GetID())
        end

        imgui.PushItemWidth(100)
        imgui.InputText("名字", ActorNameSB)
        imgui.PopItemWidth()
        imgui.SameLine()
        if imgui.Button('改名##change_name') then
            net_manager_actor_dostring(actor:GetID(),[[ 
                actor:SetProperty(PROP_NAME, '%s')
            ]], ActorNameSB:str() )
        end

        imgui.Button('Actor类型')
        imgui.SameLine()
        local actor_type = actor:GetProperty(PROP_ACTOR_TYPE) 
        if imgui.RadioButton('玩家##TYPE_PLAYER', actor_type == ACTOR_TYPE_PLAYER) then
            net_manager_actor_dostring(actor:GetID(),[[ 
                actor:SetProperty(PROP_ACTOR_TYPE, %d)
            ]], ACTOR_TYPE_PLAYER)
        end
        imgui.SameLine()
        if imgui.RadioButton('召唤兽##TYPE_SUMMON', actor_type == ACTOR_TYPE_SUMMON) then
            net_manager_actor_dostring(actor:GetID(),[[ 
                actor:SetProperty(PROP_ACTOR_TYPE, %d)
            ]], ACTOR_TYPE_SUMMON)
        end
        imgui.SameLine()   
        if imgui.RadioButton('其他类型##TYPE_OTHER', 
            actor_type~= ACTOR_TYPE_PLAYER
            and actor_type ~= ACTOR_TYPE_SUMMON
            and actor_type ~= ACTOR_TYPE_NPC
        ) then
            net_manager_actor_dostring(actor:GetID(),[[ 
                actor:SetProperty(PROP_ACTOR_TYPE, %d)
            ]], ACTOR_TYPE_DEFAULT)
        end

        if actor_type == ACTOR_TYPE_SUMMON then
            local owner = actor:GetSummonOwner()
            if owner then
                imgui.Text('Owner:'..owner:GetName())
                imgui.SameLine()
                if imgui.Button('ClearOwner') then
                    net_manager_actor_dostring(actor:GetID(),[[ 
                        actor:RemoveSummonOwner()
                    ]])
                end
            end
        end

        local auto_cmd = actor:GetProperty(PROP_IS_AUTO_COMMAND)
        if imgui.Checkbox('自动战斗', auto_cmd)  then
            auto_cmd = not auto_cmd
            net_manager_actor_dostring(actor:GetID(),[[
                actor:SetProperty(PROP_IS_AUTO_COMMAND, %s)
            ]], auto_cmd)
        end
        
        if imgui.Button(actor:GetProperty(PROP_AVATAR_ID)) then
            imgui.OpenPopup('PopupAvatar')
        end
        if imgui.BeginPopup('PopupAvatar') then
            local avatar_tbl 
            if actor_type == ACTOR_TYPE_PLAYER then
                avatar_tbl = content_system_get_table('avatar_role')    
            elseif actor_type == ACTOR_TYPE_SUMMON then
                avatar_tbl = content_system_get_table('avatar_npc')  
                local tmp = {}
                for id, row in pairs(avatar_tbl)   do
                    if row.can_take == 1 then
                        tmp[row.ID] = row
                    end
                end
                avatar_tbl = tmp
            end
            local role_keys =  utils_fetch_sort_keys(avatar_tbl)
            imgui.HorizontalLayout(role_keys,next,function(k,v) 
                if imgui.Button(v ..'##rolekey') then
                    net_manager_actor_dostring(actor:GetID(),[[ 
                        actor:SetProperty(PROP_ACTOR_TYPE, %d)
                        actor:SetProperty(PROP_AVATAR_ID, '%s') 
                        actor:SetProperty(PROP_WEAPON_AVATAR_ID,'')
                    ]], actor_type, v)
                    imgui.CloseCurrentPopup()
                end
            end,300)
            imgui.EndPopup()
        end
        if actor:IsPlayer() then
            imgui.SameLine()
            if imgui.Button(actor:GetProperty(PROP_WEAPON_AVATAR_ID)) then
                imgui.OpenPopup('PopupWeaponAvatar')
            end

            if imgui.BeginPopup('PopupWeaponAvatar') then
                local avatar_weapon_tbl =  content_system_get_table('avatar_weapon')    
                local avatar_key = actor:GetProperty(PROP_AVATAR_ID)
                local keys = fetch_weapon_keys(avatar_weapon_tbl,avatar_key)
              
                imgui.HorizontalLayout(keys,next,function(k,v) 
                    if imgui.Button(v ..'##weaponkey') then
                        net_manager_actor_dostring(actor:GetID(),[[ 
                            actor:SetProperty(PROP_WEAPON_AVATAR_ID,'%s')
                        ]], v)
                        imgui.CloseCurrentPopup()
                    end
                end,300)
                imgui.EndPopup()
            end 
        end

        if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_PLAYER then
            if imgui.Button('添加召唤兽') then
                imgui.OpenPopup('PopupAddSummon')                
            end
            if imgui.BeginPopup('PopupAddSummon') then
                local actors = actor_manager_fetch_all_actors()
                local all_summons = {}
                for i,_actor_ in ipairs(actors) do
                    if _actor_:IsSummon() then
                        if not _actor_:GetSummonOwner() then
                            table.insert(all_summons, _actor_) 
                        end
                    end
                end
                table.sort(all_summons, function(a,b) return a:GetID() < b:GetID() end)
                imgui.HorizontalLayout(all_summons,next,function(k,v) 
                    if imgui.Button(v:GetName()..'##summon'..v:GetID()) then
                        net_manager_actor_dostring(actor:GetID(),[[ 
                            local summon = actor_manager_fetch_player_by_id(%d)
                            if not summon then return end
                            summon:SetSummonOwner(actor)
                            local uids_str = actor:GetProperty(PROP_SUMMON_UIDS) 
                            local uids = cjson.decode(uids_str) 
                            table.insert(uids, summon:GetID())
                            actor:SetProperty(PROP_SUMMON_UIDS, cjson.encode(uids))
                        ]], v:GetID())
                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup()
            end
            imgui.SameLine()
            if imgui.Button('移除召唤兽') then
                imgui.OpenPopup('PopupRemoveSummon')                
            end
            if imgui.BeginPopup('PopupRemoveSummon') then
                local summons = actor:GetSummons()
                imgui.HorizontalLayout(summons,next,function(k,v) 
                    if imgui.Button(v:GetName()..'##'..v:GetID()) then
                        net_manager_actor_dostring(actor:GetID(),[[ 
                            local summon = actor_manager_fetch_player_by_id(%d)
                            if not summon then return end
                            local uids_str = actor:GetProperty(PROP_SUMMON_UIDS) 
                            local uids = cjson.decode(uids_str) 
                            if #uids == 0 then return end
                            for i,uid in pairs(uids) do
                                if uid == summon:GetID() then
                                    table.remove(uids, i)
                                    break
                                end
                            end
                            summon:RemoveSummonOwner()
                            actor:SetProperty(PROP_SUMMON_UIDS, cjson.encode(uids))
                        ]], v:GetID())
                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup()
            end
            imgui.SameLine()
            if imgui.Button('清空召唤兽') then
                net_manager_actor_dostring(actor:GetID(),[[ 
                    local summons = actor:GetSummons()
                    for i,summon in ipairs(summons) do
                        summon:RemoveSummonOwner()
                    end    
                    actor:SetProperty(PROP_SUMMON_UIDS, '[]') 
                ]])
            end
            imgui.Text('召唤兽信息')
            local summons = actor:GetSummons() 
            imgui.HorizontalLayout(summons,next,function(k,v)
                if imgui.Button(v:GetName()..'##summon info') then
                    
                end
            end)

            imgui.Button('队伍信息')
            local team = actor:GetTeam()
            if team then
                imgui.Checkbox('队长', actor:IsTeamLeader())
                imgui.SameLine()
                for i, member in ipairs(team:GetMembers()) do
                    if imgui.Button(member:GetName()) then
                        team_info_select_actor = member
                    end
                    imgui.SameLine()
                end
                
                if imgui.Button('离队') then
                    if team_info_select_actor then
                        team_info_select_actor:LeaveTeam()
                    end
                end
                imgui.SameLine()

                if imgui.Button('加人') then
                    imgui.OpenPopup('PopupTeamInfoAddMember')
                end
                if imgui.BeginPopup('PopupTeamInfoAddMember') then
                    local players = actor_manager_fetch_all_players()
                    imgui.HorizontalLayout(players,next,function(k,v)
                        local v_team = v:GetTeam()
                        if v_team then return end
                        if imgui.Button(v:GetName()..'##'..v:GetID()) then
                            -- if actor:IsTeamLeader() then
                            actor:AddTeamMember(v)
                            -- end
                        end
                    end)
                    imgui.EndPopup()
                end
            else
                if imgui.Button('创建队伍') then
                    actor:CreateTeam()
                end
            end            
        end

        edit_prop_lv = actor:GetProperty(PROP_LV)
        imgui.PushItemWidth(80)
        local lv_changed , new_prop_lv =  imgui.DragInt('等级', edit_prop_lv)
        imgui.PopItemWidth()
        if lv_changed then
            actor:SetProperty(PROP_LV, new_prop_lv)
            net_manager_actor_dostring(actor:GetID(),[[
                actor:SetProperty(PROP_LV, %d)
                actor:ClearAssignPoints()

                if actor:GetProperty(PROP_ACTOR_TYPE)==ACTOR_TYPE_SUMMON then
                    actor:ApplySummonQual('芙蓉仙子')
                else
                    local lv = actor:GetProperty(PROP_LV)
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_TARGETHIT, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DAMAGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DEFEND, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPEED, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_DODGE, lv) 
                    actor:SetProperty(PROP_SCHOOL_SKILL_LV_SPIRITUAL, lv) 
                    actor:SetGlobalStandardEquip(lv)
                end
            
                actor:SetProperty(PROP_HP, actor:GetMaxHP())
            ]],new_prop_lv) 
        end

        imgui.SameLine()


        if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
            imgui.Text('种族:'..actor:GetRaceName())
            imgui.SameLine()
            if imgui.Button('门派') then
                imgui.OpenPopup('SchoolSelector')
            end
            imgui.SameLine()
            imgui.Text(actor:GetSchoolName())    
        end
        if imgui.BeginPopup('SchoolSelector') then
            local school = content_system_get_table('school')
            imgui.HorizontalLayout(school,next,function(k,v) 
                if imgui.Button(v.name..'##'..v.ID) then
                    net_manager_player_dostring(string.format([[ 
                        local actor = actor_manager_fetch_player_by_id(%d)
                        actor:SetProperty(PROP_SCHOOL, %d) 
                        ]],actor:GetID(), v.ID))
                    imgui.CloseCurrentPopup()
                end
            end)
            imgui.EndPopup('SchoolSelector')
        end

        if imgui.Button('满状态') then
            net_manager_player_dostring(string.format([[ 
                local actor = actor_manager_fetch_player_by_id(%d)
                actor:SetProperty(PROP_HP, actor:GetMaxHP()) 
                actor:SetProperty(PROP_MP, actor:GetMaxMP()) 
                actor:SetProperty(PROP_SP, 150) 
            ]], actor:GetID() ))
        end
        imgui.SameLine()
        imgui.Text(string.format('HP:%.f/%.f',actor:GetProperty(PROP_HP), actor:GetMaxHP()))
        imgui.SameLine()
        imgui.Text(string.format('MP:%.0f/%.0f',actor:GetProperty(PROP_MP),actor:GetMaxMP()))
        
        if actor:GetProperty(PROP_ACTOR_TYPE) ==  ACTOR_TYPE_PLAYER then
            imgui.SameLine()
            imgui.Text(string.format('SP:%.0f/%.0f',actor:GetProperty(PROP_SP), 150))
        end
        imgui.Separator()

        imgui.BeginGroup()
        imgui.Dummy(30,20)
        if actor:GetProperty(PROP_ACTOR_TYPE)  == ACTOR_TYPE_PLAYER then
            imgui.Text(string.format('命中 %.1f', actor:CalcTargetHit()) )
        end
        imgui.Text(string.format('攻击 %.1f', actor:CalcAttack()) )
        imgui.Text(string.format('防御 %.1f', actor:CalcDefend()) )
        imgui.Text(string.format('灵力 %.1f', actor:CalcSpiritual()) )
        imgui.Text(string.format('速度 %.1f', actor:CalcSpeed()) )
        imgui.Text(string.format('躲闪 %.1f', actor:CalcDodge()) )
        imgui.EndGroup()

        imgui.SameLine()
        imgui.Dummy(30,10)
        imgui.SameLine()
        draw_prop_points_panel(actor, edit_prop_lv)
        imgui.Dummy(30,10)

        if actor:GetProperty(PROP_ACTOR_TYPE)  == ACTOR_TYPE_PLAYER then
            if imgui.CollapsingHeader('师门技能等级') then
                draw_player_skill_pannel(actor)
            end

            if imgui.CollapsingHeader('装备') then
                draw_player_equip_pannel(actor)
            end

            if imgui.CollapsingHeader('修炼等级') then
                draw_player_practice_lv(actor)
            end

        end
        if actor:GetProperty(PROP_ACTOR_TYPE) == ACTOR_TYPE_SUMMON then
            if imgui.CollapsingHeader('修炼等级') then
                draw_player_practice_lv(actor)
            end

            imgui.BeginGroup()
            imgui.Text('攻击资质 '..actor:GetProperty(PROP_SUMMON_ATK_QUAL))
            imgui.Text('防御资质 '..actor:GetProperty(PROP_SUMMON_DEF_QUAL))
            imgui.Text('体力资质 '..actor:GetProperty(PROP_SUMMON_HEALTH_QUAL))
            imgui.Text('法力资质 '..actor:GetProperty(PROP_SUMMON_MAGIC_QUAL))
            imgui.Text('速度资质 '..actor:GetProperty(PROP_SUMMON_SPEED_QUAL))
            imgui.Text('躲闪资质 '..actor:GetProperty(PROP_SUMMON_DODGE_QUAL))
            imgui.Text(string.format('成长 %.4f',  actor:GetProperty(PROP_SUMMON_GROW_COEF)))
            imgui.EndGroup()

            if imgui.Button('BB资质模板') then
                imgui.OpenPopup('BBQualSelector')
            end
            if imgui.BeginPopup('BBQualSelector') then
                local tbl = content_system_get_table('summon_quality')
                imgui.HorizontalLayout(tbl,next,function(k,v) 
                    if imgui.Button(k..'##bb_templ') then
                        net_manager_player_dostring(string.format([[ 
                            local actor = actor_manager_fetch_player_by_id(%d)
                            actor:SetProperty(PROP_SUMMON_ATK_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_DEF_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_HEALTH_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_MAGIC_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_SPEED_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_DODGE_QUAL, %d)
                            actor:SetProperty(PROP_SUMMON_GROW_COEF, %f)
                        ]], actor:GetID(), v.atk_qual, v.def_qual, v.health_qual,v.magic_qual,v.speed_qual,v.dodge_qual,v.grow_coef))

                        imgui.CloseCurrentPopup()
                    end
                end)
                imgui.EndPopup('BBQualSelector')
            end
        end
        imgui.EndChild()
    end
    imgui.End()
end