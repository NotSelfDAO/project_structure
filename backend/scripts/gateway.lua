#! /usr/bin/env lua
-- gateway.lua
local gateway = {}

local params = {
    project_name = "",
    project_version = "",
    root_path = "",
    repo_root_path = ""
}

---设置项目配置参数
local function setParams(project_name_arg, project_version_arg, root_path_arg, repo_root_path_arg)
    params.project_name = project_name_arg

    params.project_version = project_version_arg

    params.root_path = root_path_arg


    params.repo_root_path = repo_root_path_arg
end

---项目文件初始化
function gateway.initAll(project_name_arg, project_version_arg, root_path_arg, repo_root_path_arg)
    setParams(project_name_arg, project_version_arg, root_path_arg, repo_root_path_arg)
    -- 生成cmakelist
    -- build项目
    -- 编译
    -- debug
    -- running
    gateway.initFileStruct(params.project_name, params.root_path)
    gateway.initCoreCode(params.repo_root_path, params.project_name, params.root_path)
    gateway.initCmakelists(params.project_name, params.project_version, params.root_path)
end

local generate_file_struct = require("generate_file_struct.generate_file_struct")

function gateway.initFileStruct(project_name_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(root_path_arg) == "string", "Project name and root path must be strings.")
    generate_file_struct.init(project_name_arg, root_path_arg)
end

local generate_core_code = require("generate_core_code.generate_core_code")
function gateway.initCoreCode(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")
    generate_core_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
end

local generate_all_cmakelists = require("generate_cmakelists.generate_all_cmakelists")

function gateway.initCmakelists(project_name_arg, project_version_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(project_version_arg) == "string" and type(root_path_arg) == "string", "Project name, version, and root path must be strings.")
    generate_all_cmakelists.init(project_name_arg, project_version_arg, root_path_arg)
end

function gateway.buildProject()
end

function gateway.compileProject()
end

function gateway.debugProject()
end

function gateway.runProject()
end



return gateway