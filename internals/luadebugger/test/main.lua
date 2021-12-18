dofile('E:/Github/luadebugger/test/hello.lua')

function main()
    luadbg_listen(9527)

    local last_time = os.time()
    while true do 
        local now_time = os.time()
        if now_time - last_time >= 1 then
            hello2()
        end
        last_time = now_time
    end
end

main()