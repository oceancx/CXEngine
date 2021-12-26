local ui_is_show_quest = false
function ui_toggle_show_quest()
    ui_is_show_quest = not ui_is_show_quest
    return ui_is_show_quest
end


function ui_show_quest()
    if not ui_is_show_quest then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('帮助')
    imgui.Text([[简单说明：
ALT+W 人物属性编辑器，可以编辑队伍,召唤兽
ALT+E 染色 换武器 换造型
ALT+R 战斗的时候会出来，可以控制每个战斗对象的NPC战斗指令
    ]])
    imgui.End()
end
