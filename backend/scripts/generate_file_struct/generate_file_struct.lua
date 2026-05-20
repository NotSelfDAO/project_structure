#! usr/bin/env lua
-- generate_file_struct.lua

local generate_file_struct = {}

---初始化文件结构
function generate_file_struct.init(project_name_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(root_path_arg) == "string", "Project name and root path must be strings.")
    local project_path = root_path_arg .. "/" .. project_name_arg
    io:write("[Lua]Initializing file structure for project: " .. project_name_arg .. " at path: " .. project_path .. "\n")
    os.execute("mkdir -p " .. project_path)
    os.execute("mkdir -p " .. project_path .. "/cmake")
    os.execute("mkdir -p " .. project_path .. "/core")
    os.execute("mkdir -p " .. project_path .. "/core/register")
    os.execute("mkdir -p " .. project_path .. "/core/repository")
    os.execute("mkdir -p " .. project_path .. "/core/route")
    os.execute("mkdir -p " .. project_path .. "/docs/task_manager")
    os.execute("mkdir -p " .. project_path .. "/docs/transport")
    os.execute("mkdir -p " .. project_path .. "/domain")
    os.execute("mkdir -p " .. project_path .. "/third_party")
    io:write("[Lua]File structure initialized successfully.\n")
end

return generate_file_struct