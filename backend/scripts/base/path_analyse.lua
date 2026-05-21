#! usr/bin/env lua
-- path_analyse.lua
local path_analyse = {}

local platform = ""

local current_project_path = ""

local isInit = false

---判断平台
---@return string
local function set_platform()
    if package.config:sub(1, 1) == "\\" then
        platform = "windows"
        isInit = true
        return platform
    end
        
    local handle = io.popen("uname -s")
    if(handle) then
        local system_name = handle:read("*l")
        handle:close()

        if system_name == "Darwin" then
            platform = "macos"
        elseif system_name == "Linux" then
            platform = "linux"
        end
    end

    if platform == "" then
        platform = "unknown"
    end

    isInit = true
    
    return  platform
end

-- 获取上级目录
local function dirname(path)
    if not path or path == "" then
        return nil
    end
    return path:match("^(.*)[/\\][^/\\]+$")
end

-- 规范化路径分隔符
local function normalize(path)
    if not path then
        return nil
    end
    local sep = package.config:sub(1, 1)
    return (path:gsub("[/\\]+", sep))
end

-- 基于当前模块文件位置推导项目根目录
-- path_analyse.lua 位于: backend/scripts/base/path_analyse.lua
-- 向上两级目录可得到 backend 目录
local function get_project_root_from_this_file()
    local info = debug.getinfo(1, "S")
    local src = info and info.source or nil
    if not src or src:sub(1, 1) ~= "@" then
        return nil
    end

    local file_path = normalize(src:sub(2))      -- 去掉前导 '@'
    local file_dir = dirname(file_path)          -- .../backend/scripts/base
    local scripts_dir = dirname(file_dir)        -- .../backend/scripts
    local backend_root = dirname(scripts_dir)    -- .../backend
    return backend_root
end

---获取项目根目录路径
local function set_current_project_path()
    -- 1) 优先：基于模块文件位置，稳定且不受启动目录影响
    local root = get_project_root_from_this_file()
    if root and root ~= "" then
        current_project_path = root
        return
    end

    -- 2) 回退：当前工作目录
    local cwd = os.getenv("PWD") or os.getenv("CD")
    if not cwd then
        local cmd = platform == "windows" and "cd" or "pwd"
        local handle = io.popen(cmd)
        if handle then
            cwd = handle:read("*l")
            handle:close()
        end
    end

    current_project_path = normalize(cwd) or "."
end

function path_analyse.get_platform()
    if not isInit then
        set_platform()
        set_current_project_path()
    end
    return platform
end

function path_analyse.get_current_project_path()
    if not isInit then
        set_platform()
        set_current_project_path()
    end
    return current_project_path
end

---检测当前平台的文件分隔符（Windows 是 \，Linux/Mac 是 /）
local SEP = package.config:sub(1, 1)

function path_analyse.join(...)
    local args = {...}
    local segments = {}

    for i, v in ipairs(args) do
        local segment = tostring(v)

        segment = segment:gsub("[/\\]+", SEP)

        if i > 1 then
            segment = segment:gsub("^" .. SEP .. "+", "")
        end
        if i < #args then
            segment = segment:gsub(SEP .. "+$", "")
        end

        if(segment ~= "") then
            table.insert(segments, segment)
        end
    end

    return table.concat(segments, SEP)
end

return path_analyse