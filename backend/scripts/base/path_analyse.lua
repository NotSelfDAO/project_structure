#! usr/bin/env lua
-- path_analyse.lua
local path_analyse = {}

local platform = ""

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

function path_analyse.get_platform()
    if not isInit then
        set_platform()
    end
    return platform
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