static const char* script_system_lua_code = R"_(--line alignment
function script_system_dofile(relative_path, env)
    local path = vfs_get_luapath(relative_path)
    local subpaths = {} 
    path = path..'/'
    for subpath in path:gmatch('(.-)[\\/]') do
        if #subpaths == 0 then
            subpath = subpath:lower()
        end
        if subpath == '..' then
            table.remove(subpaths)
        else
            table.insert(subpaths,subpath)
        end
    end
	path = table.concat(subpaths,'\\')
	
    local fun,err = loadfile(path,'bt',env or _ENV)
	if fun then
		print('script_system_dofile',path)
        fun()
    else
        print(err)
    end
end
)_";
