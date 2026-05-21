#! usr/bin/env lua
-- compile.lua

local compile = {}

local path_analyse = require("backend.scripts.base.path_analyse")

function compile.compileProject(project_name, root_path_arg)
    os.execute("cmake --build " .. path_analyse.join(root_path_arg, project_name, "build") .. " -j 16")
end

return compile