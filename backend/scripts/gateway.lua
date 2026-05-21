#! /usr/bin/env lua
-- gateway.lua
local gateway = {}
local path_analyse = require("base.path_analyse")

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
    local project_root = path_analyse.join(params.root_path, params.project_name)
    -- 生成cmakelist
    -- build项目
    -- 编译
    -- debug
    -- running
    gateway.initFileStruct(params.project_name, params.root_path)
    gateway.initCoreCode(params.repo_root_path, params.project_name, project_root)
    gateway.initCmakelists(params.project_name, params.project_version, project_root)
end

local generate_file_struct = require("backend.scripts.generate_project.generate_file_struct.generate_file_struct")

function gateway.initFileStruct(project_name_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(root_path_arg) == "string", "Project name and root path must be strings.")
    generate_file_struct.init(project_name_arg, root_path_arg)
end

local generate_core_code = require("backend.scripts.generate_project.generate_core_code.generate_core_code")
function gateway.initCoreCode(repo_root_path_arg, project_name_arg, root_path_arg)
    assert(type(repo_root_path_arg) == "string" and type(project_name_arg) == "string" and type(root_path_arg) == "string", "Repo root path, project name, and root path must be strings.")
    generate_core_code.init(repo_root_path_arg, project_name_arg, root_path_arg)
end

local generate_all_cmakelists = require("backend.scripts.generate_project.generate_cmakelists.generate_all_cmakelists")

function gateway.initCmakelists(project_name_arg, project_version_arg, root_path_arg)
    assert(type(project_name_arg) == "string" and type(project_version_arg) == "string" and type(root_path_arg) == "string", "Project name, version, and root path must be strings.")
    generate_all_cmakelists.init(project_name_arg, project_version_arg, root_path_arg)
end

local build = require("backend.scripts.build_project.build")

function gateway.buildProject(project_name, root_path)
    build.buildProject(project_name, root_path)
end

local compile = require("backend.scripts.compile_project.compile")

function gateway.compileProject(project_name, root_path)
    compile.compileProject(project_name, root_path)
end

function gateway.debugProject()
end

function gateway.runProject()
end



return gateway