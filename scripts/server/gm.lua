function handle_gm_message(conn, msg)
    cxlog_info('handle_gm_message',  msg)
    if msg.type == 'exit_server' then
        os.exit(0)
    elseif msg.type == 'account_info' then
        local infos = account_manager_fetch_infos()

        cxlog_info('response','account_info', cjson.encode(infos))
        
        local buf = ezio_buffer_create()
        buf:WriteInt(PTO_S2C_GM)
        buf:WriteString(cjson.encode(infos))
        local cnt = buf:readable_size()
        buf:PrependInt(cnt)
        conn:Send(buf)
        ezio_buffer_destroy(buf)	
    else
        net_send_message_to_all_players(PTO_S2C_GM, cjson.encode(msg))
    end
end