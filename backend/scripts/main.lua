#! /usr/bin/env lua
-- main.lua

local gateway = require("gateway")

local pagetype = {
    welcome = 1,
    set_params = 2,
    loading = 3,
    building = 4,
    compiling = 5,
    debugging = 6,
    choosing = 7,
    exiting = 8
}

local function clear_terminal()
    -- \27[2J: 清空整个屏幕 (Clear Screen)
    -- \27[H:  将光标移动到屏幕左上角主位置 (Home Position)
    io.write("\27[2J\27[H")
    io.flush() -- 🚀 瞬间冲刷内存屏障，让清屏指令立即生效
end

local page_welcome = [[
#========================================================
Welcome to use the backend structure generater
This tool will help you quickly generate a standardized backend project structure, including core code, CMakeLists, and third-party dependencies.

START

#========================================================
]]

local function page_welcome_controller()
    clear_terminal()
    io:write(page_welcome)
    io:flush()
    ---use . or :

    local tmp = ""
    tmp = io.read()
    return pagetype.set_params
end

local page_set_params = [[
#========================================================
Please provide the following information to initialize your project:
]]

local page_set_params_project_name = [[
project name: 
]]

local page_set_params_project_version = [[
project version: 
]]

local page_set_params_root_path = [[
root path: 
]]

local function page_set_params_controller()
    clear_terminal()
    local params = {
        project_name = "",
        project_version = "",
        root_path = "",
        repo_root_path = ""
    }
    io:write(page_set_params)
    io:flush()
    local is_project_name_right = false
    while not is_project_name_right do
        io:write(page_set_params_project_name)
        io:flush()
        local tmp_project_name = io.read()
        if tmp_project_name ~= "" then
            params.project_name = tmp_project_name
            is_project_name_right = true
        else
            io:write("Project name cannot be empty. Please enter a valid project name.\n")
            io:flush()
        end
    end

    local is_project_version_right = false
    while not is_project_version_right do
        io:write(page_set_params_project_version)
        io:flush()
        local tmp_project_version = io.read()
        if tmp_project_version ~= "" then
            params.project_version = tmp_project_version
            is_project_version_right = true
        else
            io:write("Project version cannot be empty. Please enter a valid project version.\n")
            io:flush()
        end
    end

    local is_root_path_right = false
    while not is_root_path_right do
        io:write(page_set_params_root_path)
        io:flush()
        local tmp_root_path = io.read()
        if tmp_root_path ~= "" then
            params.root_path = tmp_root_path
            is_root_path_right = true
        else
            io:write("Root path cannot be empty. Please enter a valid root path.\n")
            io:flush()
        end
    end
    gateway.initAll(params.project_name, params.project_version, params.root_path, params.repo_root_path)
    return pagetype.choosing
end

local page_building = [[
#========================================================
this function has not been implemented yet, please wait for the next version
#========================================================
]]

local function page_building_controller()
    clear_terminal()
    io:write(page_building)
    io:flush()
    return pagetype.choosing
end

local page_compiling = [[
#========================================================
this function has not been implemented yet, please wait for the next version
#========================================================
]]

local function page_compiling_controller()
    clear_terminal()
    io:write(page_compiling)
    io:flush()
    return pagetype.choosing
end

local page_debugging = [[
#========================================================
this function has not been implemented yet, please wait for the next version
#========================================================
]]


local function page_debugging_controller()
    clear_terminal()
    io:write(page_debugging)
    io:flush()
    return pagetype.choosing
end

local page_choosing = [[
#========================================================
Choosing Section
pleasing type the number to choose the function you want to execute:
1. Build project
2. Compile project
3. Debug project

pleasing type the number to choose the function you want to execute:
1. Build project
2. Compile project
3. Debug project
4. Exit

]]

local function page_choosing_controller()
    clear_terminal()
    io:write(page_choosing)
    io:flush()
    while true do
        io:write("Please enter the number corresponding to your choice: ")
        io:flush()

        local choice = io.read()
        if choice == "1" then
            return pagetype.building
        elseif choice == "2" then
            return pagetype.compiling
        elseif choice == "3" then
            return pagetype.debugging
        elseif choice == "4" then
            return pagetype.exiting
        else
            io:write("Invalid choice. Please enter a valid number.\n")
            io:flush()
        end
    end
end

    -- welcome = 1,
    -- set_params = 2,
    -- loading = 3,
    -- building = 4,
    -- compiling = 5,
    -- debugging = 6,
    -- choosing = 7,
    -- exiting = 8

local function controller()
    local page = pagetype.welcome
    while(page) do
        if(page == pagetype.welcome) then
            page = page_welcome_controller()
        elseif(page == pagetype.set_params) then
            page = page_set_params_controller()
        -- elseif(page == pagetype.loading) then
            -- page = page_loading_controller()
        elseif(page == pagetype.building) then
            page = page_building_controller()
        elseif(page == pagetype.compiling) then
            page = page_compiling_controller()
        elseif(page == pagetype.debugging) then
            page = page_debugging_controller()
        elseif(page == pagetype.choosing) then
            page = page_choosing_controller()
        elseif(page == pagetype.exiting) then
            clear_terminal()
            io:write("Thank you for using the backend structure generator. Goodbye!\n")
            io:flush()
            break
        end
    end
end

local main = {}
function main.init()
    controller()
end

return main.init()