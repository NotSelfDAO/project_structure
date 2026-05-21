#! usr/bin/env lua
-- generate_all_cmakelists.lua

local generate_all_cmakelists = {}

local params = {
    project_name = "",
    project_version = "",
    root_path = ""
}

local generate_root_cmakelist = require("generate_cmakelists.generate_root_cmakelists")

local generate_core_cmakelists = require("generate_cmakelists.generate_core_cmakelists")

local generate_domain_cmakelists = require("generate_cmakelists.generate_domain_cmakelists")

local generate_cmake = require("generate_cmakelists.generate_cmake")

---生成cmakelist
local function generateAll()
    io.write("[Lua] Generating all CMakeLists.txt files...\n")
    io.flush()
    generate_root_cmakelist.generateRoot(params.project_name, params.project_version, params.root_path)
    generate_core_cmakelists.generateCore(params.project_name, params.project_version, params.root_path)
    generate_domain_cmakelists.generateDomain(params.project_name, params.project_version, params.root_path)
    generate_cmake.generate(params.root_path)
    io.write("[Lua] All CMakeLists.txt files generated successfully.\n")
    io.flush()
end

---检查构建cmakelists的参数是否符合规范
---@param project_name_arg string
---@param project_version_arg string
---@param root_path_arg string
local function checkParams(project_name_arg, project_version_arg, root_path_arg)
    assert(project_name_arg ~= "", "Project name cannot be empty.")
    assert(type(project_name_arg) == "string", "Project name must be a string.")
    assert(project_version_arg ~= "", "Project version cannot be empty.")
    assert(type(project_version_arg) == "string", "Project version must be a string.")
    assert(root_path_arg ~= "", "Root path cannot be empty.")
    assert(type(root_path_arg) == "string", "Root path must be a string.")
end

---设置构建cmakelists所需
---@param project_name_arg string
---@param project_version_arg string
---@param root_path_arg string
local function setParams(project_name_arg, project_version_arg, root_path_arg)
    checkParams(project_name_arg, project_version_arg, root_path_arg)
    params.project_name = project_name_arg
    params.project_version = project_version_arg
    params.root_path = root_path_arg
end
---用于(初始化)生成项目cmakelists
---@param project_name_arg string
---@param project_version_arg string
---@param root_path_arg string
function generate_all_cmakelists.init(project_name_arg, project_version_arg, root_path_arg)
    setParams(project_name_arg, project_version_arg, root_path_arg)
    generateAll()
end

---用途暂时不清楚
---@param project_name_arg string
---@param project_version_arg string
---@param root_path_arg string
function generate_all_cmakelists.reGenerate(project_name_arg, project_version_arg, root_path_arg)
    checkParams(project_name_arg, project_version_arg, root_path_arg)
    generateAll()
end

return generate_all_cmakelists