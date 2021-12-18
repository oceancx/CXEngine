--[[
CXEngine.exe --cwd=[工作目录] --script_path=[lua文件夹相对工作目录]/  --luapath=[相对script_path下的lua文件路径]
示例：
./bin/Debug/CXEngine.exe --cwd=[CXEngine目录的路径]/  --script_path=scripts/examples/ --luapath=example0.lua

example7 drop files

]]
local animPathSB = imgui.CreateStrbuf('', 512)
local fbxPathSB = imgui.CreateStrbuf('', 512)
local savePathSB = imgui.CreateStrbuf('', 512)

function update()
    imgui.InputText("AnimPath", animPathSB)
    if iw_is_dropped() and imgui.IsItemHovered() then
        local files = iw_get_drop_files()
        animPathSB:reset(files[1])
        iw_set_dropped(false)
    end
    imgui.InputText("FBXPath", fbxPathSB)
    if iw_is_dropped() and imgui.IsItemHovered() then
        local files = iw_get_drop_files()
        fbxPathSB:reset(files[1])
        iw_set_dropped(false)
    end
    imgui.InputText("SavePath", savePathSB)
    if iw_is_dropped() and imgui.IsItemHovered() then
        local files = iw_get_drop_files()
        savePathSB:reset(files[1])
        iw_set_dropped(false)
    end
end

do
    iw_init(800, 600)
    iw_render(update)
    iw_deinit()
end
