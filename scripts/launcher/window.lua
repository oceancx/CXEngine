--[[
    坐标系转换 首先解决平移旋转缩放 
    首先是 平移向量 确定平移矩阵 旋转向量确定 旋转矩阵 缩放V => 缩放M
    默认是行优先 左乘行优先
    欧拉角 转换为 四元数
]] --
local eulerAngle = {yaw = 0, pitch = 0, roll = 0}
local quaternion = {x = 0, y = 0, z = 0, w = 1}
local yawMat = {1, 0, 0, 0, 1, 0, 0, 0, 1}
local pitchMat = {1, 0, 0, 0, 1, 0, 0, 0, 1}
local rollMat = {1, 0, 0, 0, 1, 0, 0, 0, 1}
local rotMat = {1, 0, 0, 0, 1, 0, 0, 0, 1}

function EulerToQuaternion(euler)
    local q = {x = 0, y = 0, z = 0, w = 1}
    return q
end

function Mat3x3Multiply(m1, m2)
    local m11 = m1[1] * m2[1] + m1[2] * m2[3 * 1 + 1] + m1[3] * m2[3 * 2 + 1]
    local m12 = m1[1] * m2[2] + m1[2] * m2[3 * 1 + 2] + m1[3] * m2[3 * 2 + 2]
    local m13 = m1[1] * m2[3] + m1[2] * m2[3 * 1 + 3] + m1[3] * m2[3 * 2 + 3]

    local m21 = m1[3 * 1 + 1] * m2[1] + m1[3 * 1 + 2] * m2[3 * 1 + 1] +
                    m1[3 * 1 + 3] * m2[3 * 2 + 1]
    local m22 = m1[3 * 1 + 1] * m2[2] + m1[3 * 1 + 2] * m2[3 * 1 + 2] +
                    m1[3 * 1 + 3] * m2[3 * 2 + 2]
    local m23 = m1[3 * 1 + 1] * m2[3] + m1[3 * 1 + 2] * m2[3 * 1 + 3] +
                    m1[3 * 1 + 3] * m2[3 * 2 + 3]

    local m31 = m1[3] * m2[1] + m1[3 * 1 + 3] * m2[3 * 1 + 1] + m1[3 * 2 + 3] *
                    m2[3 * 2 + 1]
    local m32 = m1[3] * m2[2] + m1[3 * 1 + 3] * m2[3 * 1 + 2] + m1[3 * 2 + 3] *
                    m2[3 * 2 + 2]
    local m33 = m1[3] * m2[3] + m1[3 * 1 + 3] * m2[3 * 1 + 3] + m1[3 * 2 + 3] *
                    m2[3 * 2 + 3]
    return  {m11, m12, m13, m21, m22, m23, m31, m32, m33}
end

function CalcYawMat()
    local d = eulerAngle.yaw
    local sin_d = math.sin(d)
    local cos_d = math.cos(d)
    yawMat = {1, 0, 0, 0, cos_d, sin_d, 0, -sin_d, cos_d}
end

function CalcPitchMat()
    local d = eulerAngle.pitch
    local sin_d = math.sin(d)
    local cos_d = math.cos(d)
    pitchMat = {cos_d, 0, -sin_d, 0, 1, 0, sin_d, 0, cos_d}
end

function CalcRollMat()
    local d = eulerAngle.roll
    local sin_d = math.sin(d)
    local cos_d = math.cos(d)
    rollMat = {cos_d, sin_d, 0, -sin_d, cos_d, 0, 0, 0, 1}
end

--OpenGL 默认是右手坐标系
function ImDrawCoordinateSystemWindow()
    local changed = false
    imgui.Text('欧拉角=>四元数')
    imgui.PushItemWidth(200)
    changed, eulerAngle.yaw, eulerAngle.pitch, eulerAngle.roll =
        imgui.DragFloat3('欧拉角##euler', eulerAngle.yaw, eulerAngle.pitch,
                         eulerAngle.roll, 0.1, -180, 180)
    imgui.PopItemWidth()
    if changed then
        quaternion = EulerToQuaternion(eulerAngle)
        CalcYawMat()
        CalcPitchMat()
        CalcRollMat()

        local mYP = Mat3x3Multiply(yawMat ,pitchMat)  
        rotMat =  Mat3x3Multiply(mYP ,rollMat)  
    end

    imgui.SameLine()
    imgui.Dummy(20, 1)
    imgui.SameLine()
    imgui.PushItemWidth(200)
    changed, quaternion.x, quaternion.y, quaternion.z, quaternion.w =
        imgui.DragFloat4('四元数##(xyzw)', quaternion.x, quaternion.y,
                         quaternion.z, quaternion.w, 0.01, -1, 1)
    imgui.PopItemWidth(200)

    imgui.TextFormat('Yaw旋转矩阵\n' .. '%6.3f\t%6.3f\t%6.3f\n' ..
                         '%6.3f\t%6.3f\t%6.3f\n' .. '%6.3f\t%6.3f\t%6.3f\n',
                     yawMat[1], yawMat[2], yawMat[3], yawMat[4], yawMat[5],
                     yawMat[6], yawMat[7], yawMat[8], yawMat[9])

    imgui.TextFormat('Pitch旋转矩阵\n' .. '%6.3f\t%6.3f\t%6.3f\n' ..
                         '%6.3f\t%6.3f\t%6.3f\n' .. '%6.3f\t%6.3f\t%6.3f\n',
                     pitchMat[1], pitchMat[2], pitchMat[3], pitchMat[4],
                     pitchMat[5], pitchMat[6], pitchMat[7], pitchMat[8],
                     pitchMat[9])

    imgui.TextFormat('Roll旋转矩阵\n' .. '%6.3f\t%6.3f\t%6.3f\n' ..
                         '%6.3f\t%6.3f\t%6.3f\n' .. '%6.3f\t%6.3f\t%6.3f\n',
                     rollMat[1], rollMat[2], rollMat[3], rollMat[4], rollMat[5],
                     rollMat[6], rollMat[7], rollMat[8], rollMat[9])

    imgui.TextFormat('Rot旋转矩阵\n' .. '%6.3f\t%6.3f\t%6.3f\n' ..
                         '%6.3f\t%6.3f\t%6.3f\n' .. '%6.3f\t%6.3f\t%6.3f\n',
                     rotMat[1], rotMat[2], rotMat[3], rotMat[4], rotMat[5],
                     rotMat[6], rotMat[7], rotMat[8], rotMat[9])
end


if not command_arg_check('Debug') then
    script_system_dofile('../generator/sound_tsv_parser.lua')
    script_system_dofile('summon_handle.lua')
end

local AccountSB = imgui.CreateStrbuf('simulator', 256)
local PasswordSB = imgui.CreateStrbuf('123456', 256)
local SourceSB = imgui.CreateStrbuf('print("hello")', 2560)
local IPSB = imgui.CreateStrbuf('127.0.0.1', 256)
local PortSB = imgui.CreateStrbuf('45000', 256)
local DbgPortSB = imgui.CreateStrbuf('9600', 256)
local DataPathSB = imgui.CreateStrbuf(vfs_get_workdir() .. 'data/', 256)
local PlayerNameSB = imgui.CreateStrbuf('simulator', 256)
local PosX = imgui.CreateStrbuf('200', 128)
local PosY = imgui.CreateStrbuf('2790', 128)

local account_infos = {}

stub = stub or {}
stub[PTO_S2C_GM] = function(req)
    cxlog_info('PTO_S2C_GM', cjson.encode(req))
    if req then account_infos = req end
end

stub[PTO_C2C_PLAYER_ENTER] = function() end

function net_manager_stub() return stub end
function net_manager_player_dostring(pid, code)
    local req = {pid = pid, code = code}
    net_send_message(PTO_C2S_PLAYER_DOSTRING, cjson.encode(req))
end

function launcher_init()
    if command_arg_check('gen-imgui') then
        script_system_dofile('../generator/imgui_binding_generator.lua')
        return
    end
end

function launcher_update()
    local shared_netq = net_get_shared_queue()
    if not shared_netq:empty(0) then
        local pt = shared_netq:front(0)
        local type = pt:ReadAsInt()
        local js = pt:ReadAllAsString()
        local req = cjson.decode(js)
        cxlog_info('read msg ', js)
        if stub[type] then stub[type](req) end
        shared_netq:pop_front(0)
    end

    while not shared_netq:empty(1) do
        local req = shared_netq:front(1)
        net_send_buffer_in_c(req)
        shared_netq:pop_front(1)
    end

    local viewport, x, y, w, h = imgui.GetMainViewport();
    imgui.SetNextWindowPos(x, y);
    imgui.SetNextWindowSize(w, h);
    imgui.SetNextWindowViewport(viewport);

    imgui.Begin('Launcher', nil, ImGuiWindowFlags_NoDecoration)
    if imgui.Button('Reload') then script_system_dofile('window.lua') end

    if imgui.CollapsingHeader('lua代码教程') then
        local examples = vfs_list_files(vfs_makepath('scripts/examples'))
        imgui.HorizontalLayout(examples, next, function(k, v)
            if v:match('.lua') then
                local fname = v:match('([^/]+%.lua)')
                if imgui.Button(fname .. "##" .. k) then
                    local exepath
                    if command_arg_check('Debug') then
                        exepath = vfs_get_workdir() .. 'bin/Debug/CXEngine.exe'
                    else
                        exepath = vfs_get_workdir() .. 'bin/CXEngine.exe'
                    end
                    local tcmd = {
                        'start ' .. exepath, '--cwd=' .. vfs_get_workdir(),
                        '--dbg_port=' .. 9610,
                        '--script_path=scripts/examples/', '--luapath=' .. fname
                    }
                    local cmd = table.concat(tcmd, ' ')
                    cxlog_info(cmd)
                    os.execute(cmd)
                end
            end
        end)
    end

    imgui.Button('启动游戏说明')
    if imgui.IsItemHovered() then
        imgui.BeginTooltip()
        imgui.Text([[先点"启动服务器"，然后点"注册帐号"，
注册完之后，点"更新帐号信息"，
下面会根据帐号信息刷新出来新的按钮，
点击这些按钮就是启动对应帐号的客户端。
如果刷新不出来，帐号信息刷新按钮可以多点几次
帐号信息保存在res/storage/account.data

梦幻西游经典版目录下的所有资源放到data/目录下

有问题请加群：983630090]])
        imgui.EndTooltip()
    end

    res, show_demo = imgui.Checkbox('Demo', show_demo)
    if show_demo then imgui.ShowDemoWindow() end

    if command_arg_check('Debug') then

        imgui.Text('Server:')
        imgui.InputTextMultiline('##source', SourceSB, 300, 50,
                                 ImGuiInputTextFlags_AllowTabInput)

        if imgui.Button('服务端执行') then
            local code = SourceSB:str()
            net_manager_player_dostring(code)
        end
        imgui.SameLine()

    end

    if imgui.Button('启动服务器') then
        local exepath
        if command_arg_check('Debug') then
            exepath = vfs_get_workdir() .. 'bin/Debug/CXServer.exe'
        else
            exepath = vfs_get_workdir() .. 'bin/CXServer.exe'
        end

        local cmd = string.format('start %s --cwd=%s', exepath,
                                  vfs_get_workdir())
        cxlog_info(cmd)
        os.execute(cmd)
    end
    imgui.SameLine()
    if imgui.Button('重连服务器') then net_manager_reconnect() end

    imgui.SameLine()
    if imgui.Button('关闭服务器') then end
    imgui.SameLine()
    if imgui.Button('关闭所有端') then
        net_send_message(PTO_C2S_GM, cjson.encode({type = 'exit'}))
        net_send_message(PTO_C2S_GM, cjson.encode({type = 'exit_server'}))
    end
    imgui.SameLine()
    if imgui.Button('关闭客户端') then
        net_send_message(PTO_C2S_GM, cjson.encode({type = 'exit'}))
    end

    if imgui.CollapsingHeader('坐标系转换') then
        ImDrawCoordinateSystemWindow()
    end

    if command_arg_check('Debug') then
        if imgui.Button('启动DA 4711') then
            local path = vfs_makepath(
                             'internals/luadebugger/vscode/Debug/vsdbgadapter.exe')
            local cwd = vfs_makepath('internals/luadebugger/vscode/')
            local cmd =
                string.format('start %s --cwd=%s --port=4711', path, cwd)
            os.execute(cmd)
        end
        if imgui.Button('启动DA 4712') then
            local path = vfs_makepath(
                             'internals/luadebugger/vscode/Debug/vsdbgadapter.exe')
            local cwd = vfs_makepath('internals/luadebugger/vscode/')
            local cmd =
                string.format('start %s --cwd=%s --port=4712', path, cwd)
            os.execute(cmd)
        end
        if imgui.Button('生成imguiBinding') then
            script_system_dofile('../generator/imgui_binding_generator.lua')
        end

        if imgui.Button('生成luadbg.inl') then
            local debuggee_path = vfs_makepath(
                                      'internals/luadebugger/luadbg/debuggee.lua')
            local debugger_path = vfs_makepath(
                                      'internals/luadebugger/luadbg/debugger.lua')
            local luadbg_inl_path = vfs_makepath(
                                        'internals/luadebugger/luadbg/src/luadbg.inl')
            local luadbg_inl_file = io.open(luadbg_inl_path, 'w+')
            luadbg_inl_file:write(
                [[const char* debuggee_code = "--__debuggee_code__"]] .. '\n')
            local lines = {}
            for line in io.lines(debuggee_path) do
                table.insert(lines, string.format('%q', line) .. '"\\n"')
            end
            luadbg_inl_file:write(table.concat(lines, '\n') .. ';\n')
            luadbg_inl_file:write('\n')

            luadbg_inl_file:write(
                [[const char* debugger_code = "--__debugger_code__"]] .. '\n')
            lines = {}
            for line in io.lines(debugger_path) do
                table.insert(lines, string.format('%q', line) .. '"\\n"')
            end
            luadbg_inl_file:write(table.concat(lines, '\n') .. ';\n')
            luadbg_inl_file:write('\n')
            luadbg_inl_file:close()
        end

        if imgui.Button('重新生成ActorProp') then
            script_system_dofile('../generator/actor_template.lua')
        end

        if imgui.Button('table_template') then
            -- script_system_dofile('generator/table_template.lua')
            -- generate_avatar_role_tsv()
        end
        if imgui.Button('安装VSCode插件') then
            local dir = vfs_makepath('')
            local cmd = string.format(
                            'sh  %sinternals/luadebugger/install_extension.sh %s',
                            dir, dir)
            print(cmd)
            os.execute(cmd)
        end
        -- if imgui.Button('生成Protocol') then
        -- 	script_system_dofile('../generator/protocol.lua')
        -- end
        if imgui.Button('Test') then
            local file = io.open(path, 'rb')
            local flag = file:read(4)
        end

        if imgui.Button('HandleAvatarNpc') then handle_avatar_npc() end

        if imgui.Button('TestHandle') then output_names() end
    end

    if imgui.Button('更新帐号信息') then
        net_send_message(PTO_C2S_GM, cjson.encode({type = 'account_info'}))
    end

    if imgui.Button('注册帐号') then imgui.OpenPopup('RigsterAccount') end
    if imgui.BeginPopup('RigsterAccount') then
        imgui.Text('IP  :')
        imgui.SameLine()
        imgui.InputText('##IP', IPSB)

        imgui.Text('Port  :')
        imgui.SameLine()
        imgui.InputText('##Port', PortSB)

        imgui.Text('DbgPort  :')
        imgui.SameLine()
        imgui.InputText('##DbgPort', DbgPortSB)

        imgui.Text("Account   :");
        imgui.SameLine();
        imgui.InputText("##account", AccountSB);

        imgui.Text("Password   :");
        imgui.SameLine();
        imgui.InputText("##password", PasswordSB);

        if imgui.Button('注册') then
            local msg = {}
            msg.account = AccountSB:str()
            msg.password = PasswordSB:str()
            net_send_message(PTO_C2C_SIGNUP, cjson.encode(msg))
            imgui.CloseCurrentPopup()
        end

        imgui.SameLine()

        imgui.EndPopup('RigsterAccount')
    end

    if next(account_infos) then
        imgui.HorizontalLayout(account_infos, next, function(k, v)
            if imgui.Button(string.format('%s:%d', v.account, v.pid) .. "##" ..
                                k) then
                local exepath
                if command_arg_check('Debug') then
                    exepath = vfs_get_workdir() .. 'bin/Debug/CXEngine.exe'
                else
                    exepath = vfs_get_workdir() .. 'bin/CXEngine.exe'
                end
                local tcmd = {
                    'start ' .. exepath, '--cwd=' .. vfs_get_workdir(),
                    '--host=' .. IPSB:str(), '--port=' .. PortSB:str(),
                    '--dbg_port=' .. DbgPortSB:str(), '--user=' .. v.account,
                    '--pass=' .. v.password
                }
                local cmd = table.concat(tcmd, ' ')
                cxlog_info(cmd)
                os.execute(cmd)
            end
        end)
    end
    imgui.End()
end

function cx_client_on_connected()
    net_send_message(PTO_C2S_GM, cjson.encode({type = 'account_info'}))
end

function cx_client_on_disconnected() end

