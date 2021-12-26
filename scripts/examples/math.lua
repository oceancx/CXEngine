
function update() 
    
    imgui.Begin("四元数")
    if imgui.Button('') then

    end
    imgui.End()
end

do
    iw_init(800, 600)
    iw_set_font(vfs_get_workdir()..'/assets/font/simsun.ttc',14)

    iw_render(update)
    iw_deinit()
end
