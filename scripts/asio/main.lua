script_system_dofile('../share/utils.lua')



do
    get_local_ips()
    at_exit_manager_init()
    io_service_context_init()

    if command_arg_check('Debug') then
        local dbg_port = command_arg_opt_int('dbg_port', 9403)
        luadbg_listen(dbg_port)
    end

    event_loop = ez_event_loop_create()
    cx_server = ez_tcp_server_create(event_loop, 2701, 'ARKit')

    shared_netq = net_thread_queue_create()

    cx_server:set_on_connection(function(conn)
        cxlog_info('conn-connected : ', conn:connected(), conn:tohostport())
        if conn:connected() then
            conn:SetTCPNoDelay(true)
            table.insert(connections, conn)
        else
            for i, connection in ipairs(connections) do
                if connection == conn then
                    for u_id, u_info in pairs(u3d_infos) do
                        if u_info.conn == connection then
                            u_info.conn = nil
                            u3d_infos[u_id] = nil
                            break
                        end
                    end

                    for p_id, p_info in pairs(phone_infos) do
                        if p_info.conn == connection then
                            p_info.conn = nil
                            phone_infos[p_id] = nil
                            send_phone_info_to_u3d()
                            break
                        end
                    end
                    table.remove(connections, i)
                    break
                end
            end
        end
    end)

    last_tm = 0
    cx_server:set_on_message(function(conn, buf, ts)
        while buf:readable_size() >= CX_MSG_HEADER_LEN do
            local len = buf:PeekAsInt()
            if buf:readable_size() >= len + CX_MSG_HEADER_LEN then
                -- local now =  time_now_ms()
                -- if last_tm == 0 then
                --     last_tm = now
                -- end
                -- cxlog_info('dt ' .. (now- last_tm))
                -- last_tm = now
                buf:Consume(CX_MSG_HEADER_LEN)
                pcall(handle_message, conn, buf, len)
            else
                break
            end
        end
    end)

    cx_server:Start()
    event_loop:Run()
end

