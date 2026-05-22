#! usr/bin/env lua
-- generate_docs.lua

local generate_docs = {}

local generate_copy_content = require("backend.scripts.base.generate_copy_content")

local path_analyse = require("backend.scripts.base.path_analyse")

local docs_set = {
    "backend_business_handler_rules.md",
    "backend_framework_overview.md",
}

---生成文档
---@param repo_root_path_arg string
---@param project_name_arg string
---@param root_path_arg string
function generate_docs.init(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")

    io.write("[Lua] Initializing documentation generation with repo root path: " .. repo_root_path_arg .. ", project name: " .. project_name_arg .. ", and root path: " .. root_path_arg .. "\n")
    io.flush()
    local project_root = path_analyse.join(root_path_arg)

    for _, doc_name in ipairs(docs_set) do
        local from_path = path_analyse.join(repo_root_path_arg, "docs", doc_name)
        local to_path = path_analyse.join(project_root, "docs", doc_name)
        generate_copy_content.copySingleFile(from_path, to_path)
    end

    io.write("[Lua] Documentation generation completed successfully.\n")
    io.flush()
end

return generate_docs