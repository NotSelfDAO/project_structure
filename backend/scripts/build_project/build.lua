#! /usr/bin/env lua
-- build.lua

local build = {}

local path_analyse = require("backend.scripts.base.path_analyse")

function build.buildProject(project_name, root_path_arg)
    if(os.execute("ninja --version") ~= 0) then
        error("Ninja build system is not installed or not found in PATH. Please install Ninja to proceed.")
    end

    if(path_analyse.get_platform() == "windows") then
        os.execute("cmake -S " .. path_analyse.join(root_path_arg, project_name) .. " -B " .. path_analyse.join(root_path_arg, project_name, "build") .. " -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=20")
    elseif(path_analyse.get_platform() == "linux") then
        os.execute("cmake -S " .. path_analyse.join(root_path_arg, project_name) .. " -B " .. path_analyse.join(root_path_arg, project_name, "build") .. " -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=20")
    elseif(path_analyse.get_platform() == "macos") then
        os.execute("cmake -S " .. path_analyse.join(root_path_arg, project_name) .. " -B " .. path_analyse.join(root_path_arg, project_name, "build") .. " -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_STANDARD=20")
    else
        error("Unsupported platform: " .. path_analyse.get_platform())
    end

end

return build