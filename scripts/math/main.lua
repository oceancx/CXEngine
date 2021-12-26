

function update() 
    
    if imgui.Button('asd asd ') then

    end    

    
end

do
    iw_init(1024, 768)
    iw_set_font(vfs_get_workdir()..'/res/font/simsun.ttc' , 21)
    
    iw_render(update)
    iw_deinit()
end
