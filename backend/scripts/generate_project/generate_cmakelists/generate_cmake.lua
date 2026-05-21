#! usr/bin/env lua
-- generate_cmake.lua

local generate_cmake = {}

local path_analyse = require("backend.scripts.base.path_analyse")   

local ThirdPartyHandler_cmake = [[
function(handle_third_party target_name target_repo target_tag)
    FetchContent_Declare(
        ${target_name}
        GIT_REPOSITORY ${target_repo}
        GIT_TAG ${target_tag}
    )
    FetchContent_MakeAvailable(${target_name})
endfunction()
]]

function generate_cmake.generate(root_path_arg)
    io.write("[Lua] Generating core cmake files...\n")
    io.flush()
    local file,err = io.open(path_analyse.join(root_path_arg, "cmake", "ThirdPartyHandler.cmake"), "w")
    if not file then    
        io.write("[Lua] Error opening file: " .. err .. "\n")
        io.flush()
        return
    end
    file:write(ThirdPartyHandler_cmake)
    file:close()
    io.write("[Lua] core cmake files generated successfully.\n")
    io.flush()
end
return generate_cmake