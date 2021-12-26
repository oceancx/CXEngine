local ui_is_show_options = false
function ui_toggle_show_options()
    ui_is_show_options = not ui_is_show_options
    return ui_is_show_options
end

local select_scene_name = 0
local str_templ_name = imgui.CreateStrbuf('', 512)
local copy_actor_type = ACTOR_TYPE_PLAYER
 
local cbx_draw_mask = true
local cbx_draw_strider = true
local cbx_draw_cell = false
local cbx_draw_map  = true
local cbx_draw_announcement = true
scene_manager_sync_draw_cbx(cbx_draw_map,cbx_draw_cell,cbx_draw_strider,cbx_draw_mask,cbx_draw_announcement,cbx_auto_run)


local checkbox_names = {
    {   name = 'DrawMap', cb  = function(v) v.res,cbx_draw_map = imgui.Checkbox(v.name, cbx_draw_map) end },
    {   name = 'DrawCell', cb = function(v) v.res,cbx_draw_cell = imgui.Checkbox(v.name, cbx_draw_cell) end },
    {   name = 'DrawStrider', cb = function(v) v.res,cbx_draw_strider = imgui.Checkbox(v.name, cbx_draw_strider) end },
    {   name = 'DrawMask', cb = function(v) v.res,cbx_draw_mask = imgui.Checkbox(v.name, cbx_draw_mask) end },
    {   name = 'DrawAnnouncement', cb = function(v) v.res,cbx_draw_announcement = imgui.Checkbox(v.name, cbx_draw_announcement) end},
}

local SourceSB = imgui.CreateStrbuf('print("hello")',2560)

local IPSB = imgui.CreateStrbuf('127.0.0.1',256)
local PortSB = imgui.CreateStrbuf('45000',256)



local im_pa_select_avatar_name = '名称'
local im_pa_select_avatar_type = '类型'
local im_pa_select_avatar_action = '动作'
local AnimResSB =  imgui.CreateStrbuf('',256)
local select_avatar_types = {
    'avatar_role',
    'avatar_weapon',
    'avatar_npc',
    'avatar_scene_npc',
    'avatar_summon'
}
local select_avatar_names = {}
local select_avatar_actions = {}

local select_animation
function reset_play_anim(v)
    if  select_animation~=nil then
        animation_destroy(select_animation)
        select_animation = nil
    end
    
    local pack, was = res_decode(res_parse_resid(v))
    select_animation = animation_create(pack,was)
    select_animation:SetPos(600,500)
    select_animation:SetFrameInterval(0.128)
    select_animation:Reset()
    select_animation:SetLoop(0,2)
    select_animation:Play()
end

action_affix_keys = {'idle','walk','sit','angry','sayhi','dance','salute','clps','cry','batidle','attack','cast','behit','runto','runback','defend','unknown'}
function imgui_show_play_anim()


    if imgui.Button(im_pa_select_avatar_type..'##im_pa_type') then
        imgui.OpenPopup('OPP_im_pa_select_avatar_type')
    end
    if imgui.BeginPopup('OPP_im_pa_select_avatar_type') then
        imgui.HorizontalLayout(select_avatar_types,next,function(k,v)
            if imgui.Button(v..'##'..v) then
                im_pa_select_avatar_type = v
                select_avatar_names = {}
                local tbl = content_system_get_table(im_pa_select_avatar_type)
                for k,v in pairs(tbl) do 
                    table.insert(select_avatar_names,k)
                end
                table.sort(select_avatar_names)
                imgui.CloseCurrentPopup()
            end
        end,400)
        imgui.EndPopup()
    end

    if imgui.Button(im_pa_select_avatar_name..'##im_pa_name') then
        imgui.OpenPopup('OPP_im_pa_select_avatar_name')
    end
    if imgui.BeginPopup('OPP_im_pa_select_avatar_name') then
        imgui.HorizontalLayout(select_avatar_names,next,function(k,v)
            if imgui.Button(v..'##'..v) then
                im_pa_select_avatar_name = v
                select_avatar_actions= {}
                local tbl = content_system_get_table(im_pa_select_avatar_type)
                local row = tbl[im_pa_select_avatar_name]
                for i,k in ipairs(action_affix_keys) do
                    if row[k] and row[k] ~= '' then
                        if row[k]:find(',') then
                            local v1,v2 = row[k]:match('(.+),(.+)')
                            table.insert(select_avatar_actions, v1)
                            table.insert(select_avatar_actions, v2)
                        else
                            table.insert(select_avatar_actions, row[k])
                        end
                    end
                end
                
                imgui.CloseCurrentPopup()
            end
        end,400)
        imgui.EndPopup()
    end

    imgui.HorizontalLayout(select_avatar_actions,next,function(k,v)
        if imgui.Button(v..'##'..v) then
            reset_play_anim( v)
        end
    end)

   
    
end



function ui_show_options_on_draw()
    
    if select_animation then
        select_animation:Update()
        select_animation:Draw()
    end    
end
function ui_show_options()
    if not ui_is_show_options then return end
    local player = actor_manager_fetch_local_player()
    if not player then return end
    
    imgui.Begin('Options')

        
    imgui.InputTextEx('AnimResSB',AnimResSB,120)
    imgui.SameLine()
    if imgui.Button('查看') then
        reset_play_anim(AnimResSB:str())
    end

    if imgui.CollapsingHeader('PosInfo') then
        local mx, my = imgui.GetMousePos()
        local cur_x ,cur_y = imgui.GetCursorScreenPos()
        if imgui.IsMousePosValid(mx,my) then
            mx = mx - cur_x
            my = my - cur_y
        end
        local mapx, mapy = util_screen_pos_to_map_pos(mx,my)
        local px,py = player:GetPos()
        imgui.Text(string.format("[mouse] : x=%.0f,y=%.0f world.x=%.0f,world.y=%.0f",mx,my, mapx,mapy))
        imgui.Text(string.format("[player] : x=%f,y=%f,dir=%f", px,py, player:GetDir()))
    end

    if imgui.CollapsingHeader('MapDrawOption') then
        imgui.HorizontalLayout(checkbox_names,next,function(k,v) v:cb() end)
    end

    scene_manager_sync_draw_cbx(cbx_draw_map,cbx_draw_cell,cbx_draw_strider,cbx_draw_mask,cbx_draw_announcement,cbx_auto_run)

    if imgui.CollapsingHeader('Maps') then
        local scenes  = content_system_get_table('scene')
        imgui.HorizontalLayout(scenes,next,function(k,v) 
            if imgui.Button(v.name) then
                -- net_manager_player_dostring(string.format([[ 
                --     player:SetProperty(PROP_SCENE_ID, %d)
                -- ]], scene_manager_get_current_scene_id() ))

                player:SetProperty(PROP_SCENE_ID, v.ID)
                scene_manager_switch_scene_by_id(v.ID)    
            end
        end)
    end  

    if imgui.CollapsingHeader('NPCTemplate') then
        local npc_tbl = content_system_get_table('npc')
        imgui.HorizontalLayout(npc_tbl,next,function(k,v) 
            if imgui.Button(v.ID) then
                local actor = actor_manager_create_actor(os.time())
                actor:SetProperty(PROP_ACTOR_TYPE,ACTOR_TYPE_SUMMON)
                actor:SetProperty(PROP_AVATAR_ID, v.ID)
                actor:SetProperty(PROP_WEAPON_AVATAR_ID,'')
                actor:SetPos(player:GetPos())
                actor:SetProperty(PROP_SCENE_ID, player:GetProperty(PROP_SCENE_ID))
                actor:SetDir(player:GetDir())
                -- actor:ModifyHP(100)
                actor:SetProperty(PROP_HP,100) 
                -- actor:(PROP_SCENE_ID, player:GetProperty(PROP_SCENE_ID))
            end
        end)
    end  

    if imgui.CollapsingHeader('PlaySound') then
        local files = vfs_list_files(vfs_makepath([[res\data\sound.wdf.ResFiles]]) )
        local map3 = vfs_list_files(vfs_makepath([[res\data\music.wdf.ResFiles]]))
        
        for i,v in ipairs(map3) do
            table.insert(files,v)
        end
        table.insert(files,1,[[e:\CloudMusic\BIGBANG - La La La.mp3]])
        
        imgui.HorizontalLayout(files,next,function(k,v) 
            -- cxlog_info(k,v)
            if imgui.Button(v) then
                audio_manager_play(v)
            end
        end)
    end

    if imgui.CollapsingHeader('PlayAnim') then
        imgui_show_play_anim()
    end  

    if imgui.CollapsingHeader('Login') then 
        
        imgui.Text('IP  :')
        imgui.SameLine()
        imgui.InputText('##IP', IPSB)

        imgui.Text('Port  :')
        imgui.SameLine()
        imgui.InputText('##Port', PortSB)

        if imgui.Button('连接服务器') then
            local ip = IPSB:str()
            local port = math.tointeger(PortSB:str())
            net_manager_deinit()
            net_manager_init(ip, port)
        end

        imgui.Text('Server:')
        imgui.InputTextMultiline('##source', SourceSB, 400, 200,ImGuiInputTextFlags_AllowTabInput)
        if imgui.Button('服务端执行') then
            local code = SourceSB:str()
            net_manager_player_dostring(code)
        end

        imgui.SameLine()
        if imgui.Button('客户端执行') then
            local code = SourceSB:str()
            local func, err = load(code,'@client_dostring','bt',_ENV)
            if func then
                local ok,ret = pcall(func)
                if not ok then
                    cxlog_info(ret)
                end
            else
                cxlog_info('PTO_C2S_PLAYER_DOSTRING', err)
            end
        end
    end

    imgui.End()
end
