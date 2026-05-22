#! usr/bin/env lua
-- generate_domain_code.lua

local generate_domain_code = {}

local generate_copy_content = require("backend.scripts.base.generate_copy_content")

local path_analyse = require("backend.scripts.base.path_analyse")

local file_name_set = {
    base = {
        "DomainError.h",
        "EnvelopeBuilders.h",
        "TaskRequest.h",
        "TaskResponse.h",
    },
    handlers = {
        "UserProfileGetHandler.cpp",
        "UserProfileGetHandler.h",
    }
}

---生成domain层代码
---@param repo_root_path_arg string
---@param project_name_arg string
---@param root_path_arg string
function generate_domain_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")

    io.write("[Lua] Initializing domain code generation with repo root path: " .. repo_root_path_arg .. ", project name: " .. project_name_arg .. ", and root path: " .. root_path_arg .. "\n")
    io.flush()
    local project_root = path_analyse.join(root_path_arg, "domain")

    for subdir, files in pairs(file_name_set) do
        for _, file_name in ipairs(files) do
            local source_file = path_analyse.join(repo_root_path_arg, "domain", subdir, file_name)
            local target_file = path_analyse.join(project_root, subdir, file_name)
            generate_copy_content.copySingleFile(source_file, target_file)
        end
    end
    
end