--this file is only used debuggee thread
cjson = require 'cjson'

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
                        luadbg_set_line_ending_in_c(LINE_ENDING)
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

function debuggee_on_connection(conn,netq)
    --print('[debuggee]' ..conn:tohostport().. ' ' ..(conn:connected() and 'connected' or 'disconnected'))
    local buf = ezio_buffer_create()
    buf:WriteString(cjson.encode({ type='debuggee', event='connection_state', connected = conn:connected(), addr = conn:tohostport()  }))
    netq:push_back(0,buf,buf:readable_size())
    ezio_buffer_destroy(buf)
end

local debuggee_on_message_parser = create_on_message_parser()
function debuggee_on_message(conn,buf,netq)
    debuggee_on_message_parser(conn,buf,netq)
end