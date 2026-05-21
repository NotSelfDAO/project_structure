#! usr/bin/env lua
-- generate_core_code.lua

local generate_core_code = {}

local generate_copy_content = require("backend.scripts.generate_project.generate_core_code.generate_copy_content")

local path_analyse = require("backend.scripts.base.path_analyse")

local file_name_set = {
    registrar = {
        "AutoRegistrar.cpp",
        "AutoRegistrar.h",
        "HierarchicalRegistry.cpp",
        "HierarchicalRegistry.h",
    },
    repository = {

    },
    route = {
        "RouteKey.cpp",
        "RouteKey.h",
        "RouteSymbol.h",
        "SymbolTable.cpp",
        "SymbolTable.h",
    },
    task_manager = {
        "BackendTask.h",
        "BackendTaskPool.cpp",
        "BackendTaskPool.h",
        "TaskDispatcher.cpp",
        "TaskDispatcher.h",
        "TaskModel.h",
    },
    transport = {
        "HttpTaskServer.cpp",
        "HttpTaskServer.h",
    }
}

---生成项目结构目录树
---@param repo_root_path_arg string
---@param project_name_arg string
---@param root_path_arg string
function generate_core_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")

    io.write("[Lua] Initializing core code generation with repo root path: " .. repo_root_path_arg .. ", project name: " .. project_name_arg .. ", and root path: " .. root_path_arg .. "\n")
    io.flush()
    local project_root = path_analyse.join(root_path_arg)

    for module_name, file_list in pairs(file_name_set) do
        for _, file_name in ipairs(file_list) do
            local from_path = path_analyse.join(repo_root_path_arg, "core", module_name, file_name)
            local to_path = path_analyse.join(project_root, "core", module_name, file_name)
            generate_copy_content.copySingleFile(from_path, to_path)
        end
    end

    io.write("[Lua] Core code generation completed successfully.\n")
    io.flush()
end



return generate_core_code