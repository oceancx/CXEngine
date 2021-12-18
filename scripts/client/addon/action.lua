local DIR_NAMES = {
    [6] = 'DIR_N',
    [4] = 'DIR_S',
    [5] = 'DIR_W',
    [7] = 'DIR_E',
    [3] = 'DIR_NE',
    [2] = 'DIR_NW',
    [0] = 'DIR_SE',
    [1] = 'DIR_SW'
}

local ui_is_show_action = false
function ui_toggle_show_action()
    ui_is_show_action = not ui_is_show_action
    return ui_is_show_action
end


function ui_show_action()
    if not ui_is_show_action then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    imgui.Begin('Action')
    if imgui.CollapsingHeader('PlayAction') then
        imgui_std_horizontal_button_layout({},function(t, st)
            st = st==nil and 0 or (st+1) 
            if st == action_system_get_action_size() then return nil end
            return st, st, action_system_get_action_name(st)
        end, 
        function(k,v)
            player:PushAction(v)
            player:MoveActionToBack()
        end)
    end

    if imgui.CollapsingHeader('PlayerDir') then
        imgui_std_horizontal_button_layout(DIR_NAMES,function(t, st)
            st = st==nil and 0 or (st+1) 
            if st == 8 then return nil end
            return st, st, t[st]
        end, 
        function(i,v)
            player:SetDir(v)
        end)
    end
    imgui.End()
end
