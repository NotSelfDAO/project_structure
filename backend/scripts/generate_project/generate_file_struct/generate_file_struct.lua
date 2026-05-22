#! usr/bin/env lua
-- generate_file_struct.lua

local generate_file_struct = {}

local path_analyse = require("backend.scripts.base.path_analyse")

local platform = path_analyse.get_platform()

local function mkdir_path(path)
    if platform == "windows" then
        os.execute('mkdir "' .. path .. '" >NUL 2>NUL')
    else
        os.execute('mkdir -p "' .. path .. '"')
    end
end

---初始化文件结构
function generate_file_struct.init(project_name_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(root_path_arg) == "string", "Project name and root path must be strings.")
    local project_path = path_analyse.join(root_path_arg, project_name_arg)
    io.write("[Lua]Initializing file structure for project: " .. project_name_arg .. " at path: " .. project_path .. "\n")
    mkdir_path(project_path)
    mkdir_path(path_analyse.join(project_path, "cmake"))
    mkdir_path(path_analyse.join(project_path, "core"))
    mkdir_path(path_analyse.join(project_path, "core", "api"))
    mkdir_path(path_analyse.join(project_path, "core", "app"))
    mkdir_path(path_analyse.join(project_path, "core", "registrar"))
    mkdir_path(path_analyse.join(project_path, "core", "repository"))
    mkdir_path(path_analyse.join(project_path, "core", "route"))
    mkdir_path(path_analyse.join(project_path, "core", "task_manager"))
    mkdir_path(path_analyse.join(project_path, "core", "transport"))
    mkdir_path(path_analyse.join(project_path, "domain"))
    mkdir_path(path_analyse.join(project_path, "domain", "base"))
    mkdir_path(path_analyse.join(project_path, "domain", "handlers"))
    mkdir_path(path_analyse.join(project_path, "docs"))
    mkdir_path(path_analyse.join(project_path, "third_party"))

    io.write("[Lua]File structure initialized successfully.\n")
end

return generate_file_struct