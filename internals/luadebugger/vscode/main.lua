THREAD_NAME = ...

local WORK_CWD
local PORT
local event_loop
local runtime_netq = net_thread_queue_create()
local runtime_message_cache = {}
local vscode_handler 
local runtime_handler 
local runtime_client


function luadbg_is_stdio_mode()
    return PORT==0  
end

log_trace = function(...)
    if luadbg_is_stdio_mode() then
        local path = get_default_cwd()..'dbg.log'
        local file = io.open(path,'a+')
        local args = {...}
        for i,v in ipairs(args) do
            file:write(v..'\t')
        end
        file:write('\n')
        file:close()
    else
        print(...)
    end
end

function create_on_message_parser()
    local parsed_len = -1
    local readstate = 1
    local LINE_ENDING = nil
    return function(conn,buf,netq)
        while buf:readable_size() > 0 do
            local preview = buf:Preview(buf:readable_size())
            if readstate == 1 then
                local s,e = preview:find("\n")
                if s then
                    if not LINE_ENDING then
                        local s,e = preview:find("\r\n")
                        LINE_ENDING = s and "\r\n" or "\n"
                        set_line_ending_in_c(LINE_ENDING)
                    end
                    local line = buf:ReadAsString(e)
                    local match = line:gmatch("Content%-Length: (%d*)")()
                    if tonumber(match) then
                        parsed_len = tonumber(match)
                        readstate = readstate + 1
                    else
                        break
                    end
                else
                    break
                end
            elseif readstate == 2 then
                local s,e = preview:find("\n")
                if s then
                    local line = buf:ReadAsString(e)
                    readstate = readstate+1
                else
                    break
                end
            elseif readstate == 3 then
                if buf:readable_size() >= parsed_len then
                    netq:push_back(0,buf,parsed_len)
                    buf:Consume(parsed_len)
                    readstate = 1
                else
                    break
                end
            else
                break
            end
        end
    end
end

local runtime_parser = create_on_message_parser()
function run_runtime_client(ip, port)
    runtime_client = ez_tcp_client_create(event_loop, ip, port, 'Runtime')
    runtime_client:set_on_connection(function(conn)
        log_trace('[runtime]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
        if conn:connected() then
            runtime_handler = conn
            for i,msg in ipairs(runtime_message_cache) do
                runtime_handler:send(msg)
            end
            runtime_message_cache = {}
        else
            runtime_handler = nil
            if luadbg_is_stdio_mode() then
                os.exit()
            end
        end
    end)
    runtime_client:set_on_message(function(conn, buf, ts)
        runtime_parser(conn,buf,runtime_netq)
    end)
    runtime_client:Connect()
end

local message_seq = 1
function vscode_final_send(js)
    log_trace(string.format("\nDA => VS:\n%s\n", js))
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    if luadbg_is_stdio_mode() then
        print(sent)
    else
        if not vscode_handler then return end
        vscode_handler:send(sent)
    end
end

function vscode_send_response(req)
    local resp = {}
    resp.seq = message_seq
    message_seq = message_seq + 1        

    resp.type = 'response'
    resp.request_seq = req.seq
    resp.command = req.command
    resp.success = true
    if req.body then
        resp.body = req.body
    end

    vscode_final_send(cjson.encode(resp))
end

function vscode_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1
    vscode_final_send(cjson.encode(event))
end

function runtime_final_send(js)
    log_trace(string.format("\nDA => RT:\n%s\n", js))
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    if not runtime_handler then 
        table.insert(runtime_message_cache,sent)
    else
        runtime_handler:send(sent)
    end
end

function dispatch_runtime_message(js)
    log_trace(string.format("\nRT => DA => VS:\n%s\n", js))
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    if luadbg_is_stdio_mode() then
        print(sent)
    else
        if not vscode_handler then return end
        vscode_handler:send(sent)
    end
end


function runtime_send_response(req)
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = true
    resp.body = req.body
    resp.seq = message_seq
    message_seq = message_seq + 1        

    runtime_final_send(cjson.encode(resp))
end

function runtime_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1

    runtime_final_send(cjson.encode(event))
end


function dispatch_vscode_message(js)
    log_trace(string.format("\nDA <= VS:\n%s\n", js))

    local req = cjson.decode(js)
     if req.type == 'request' then
        local cmd = req.command
        if cmd == "initialize" then
            req.body = {
                supportsConfigurationDoneRequest = true,
                supportsEvaluateForHovers = true,
                supportsStepBack = true,
                supportsDataBreakpoints = false,
                supportsCompletionsRequest = false,
                supportsCancelRequest = false,
                supportsBreakpointLocationsRequest = false
            }
            vscode_send_response(req)
            vscode_send_event('initialized')
        elseif cmd == "launch" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            local cwd = arguments.cwd
            local program = arguments.program
            local args = arguments.args
            local cmd = 'start '..cwd..program..' --cwd='..cwd..' '..table.concat(args,' ')
            log_trace(cmd)

            os.execute(cmd)
            run_runtime_client(ip,port)
            runtime_final_send(cjson.encode(req))
        elseif cmd == "attach" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            local cwd = arguments.cwd
            run_runtime_client(ip,port)    
            runtime_final_send(cjson.encode(req))
        elseif cmd == "disconnect" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "restart" then
            vscode_send_response(req)
        elseif cmd == "setBreakpoints" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setFunctionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "setExceptionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "configurationDone" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "continue" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "next" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepIn" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOut" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOver" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepBack" then
            
        elseif cmd == "reverseContinue" then
            
        elseif cmd == "restartFrame" then
            
        elseif cmd == "goto" then
            
        elseif cmd == "pause" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stackTrace" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "scopes" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "variables" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setVariable" then

        elseif cmd == "source" then
            
        elseif cmd == "threads" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "evaluate" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepInTargets" then
        elseif cmd == "gotoTargets" then
        elseif cmd == "completions" then
        elseif cmd == "exceptionInfo" then
        elseif cmd == "loadedSources" then
            vscode_send_response(req)
        end
    end
end


if THREAD_NAME == 'vscode' then
    function vscode_on_connection(conn)
        print('[vscode]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
    end

    local vscode_on_message_parser = create_on_message_parser()
    function vscode_on_message(conn,buf,netq)
        vscode_on_message_parser(conn,buf,netq)
    end
else
    WORK_CWD = command_arg_opt_str('cwd',get_default_cwd())
    PORT =  command_arg_opt_int('port',0)
    log_trace(WORK_CWD, PORT)

    event_loop = ez_event_loop_create()
    if luadbg_is_stdio_mode() then
        start_stdio_read_thread()
    else
        cx_server = ez_tcp_server_create(event_loop, PORT,'VscodeServer')
        cx_server:set_on_connection(function(conn)
            log_trace('[vscode]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
            if conn:connected() then
                vscode_handler = conn
            else
                vscode_handler = nil
            end
        end)        
        local vscode_on_message_parser = create_on_message_parser()
        cx_server:set_on_message(function(conn, buf, ts)
            vscode_on_message_parser(conn,buf,vscode_netq)
        end)       
        cx_server:Start()
    end
    
    event_loop:RunTaskEvery(function()    
        while not vscode_netq:empty(0) do
            local msg = vscode_netq:front_as_string(0)
            vscode_netq:pop_front(0)
            dispatch_vscode_message(msg)
        end

        while not runtime_netq:empty(0) do
            local msg = runtime_netq:front_as_string(0)
            runtime_netq:pop_front(0)
            dispatch_runtime_message(msg)
        end
    end,10)

    event_loop:Run()
end
