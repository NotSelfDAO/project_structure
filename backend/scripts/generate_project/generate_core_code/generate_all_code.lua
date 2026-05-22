#! usr/bin/env lua
-- generate_all_code.lua

local generate_all_code = {}

local generate_copy_content = require("backend.scripts.base.generate_copy_content")

local path_analyse = require("backend.scripts.base.path_analyse")

local generate_core_code = require("backend.scripts.generate_project.generate_core_code.generate_core_code")
local generate_domain_code = require("backend.scripts.generate_project.generate_core_code.generate_domain_code")

function generate_all_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")

    io.write("[Lua] Starting full code generation with repo root path: " .. repo_root_path_arg .. ", project name: " .. project_name_arg .. ", and root path: " .. root_path_arg .. "\n")
    io.flush()

    generate_copy_content.copySingleFile(path_analyse.join(repo_root_path_arg, "main.cpp"), path_analyse.join(root_path_arg, "main.cpp"))

    generate_core_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
    generate_domain_code.init(repo_root_path_arg, project_name_arg, root_path_arg)

    io.write("[Lua] Full code generation completed successfully.\n")
    io.flush()
end

return generate_all_code