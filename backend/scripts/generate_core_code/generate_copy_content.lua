#! usr/bin/env lua
--copy.lua
local copy = {}
---拷贝单个目录
---@param fromPath string
---@param toPath string
function copy.copySingleFile(fromPath, toPath)
    local fromFile, err1 = io.open(fromPath, "r")
    if not fromFile then 
        io:write("[Lua] Error opening source file: " .. err1 .. "\n")
        io:flush()
        return
    end
    local toFile, err2 = io.open(toPath, "w")
    if not toFile then
        io:write("[Lua] Error opening destination file: " .. err2 .. "\n")
        io:flush()
        fromFile:close()
        return
    end
    local content = fromFile:read("*a")
    toFile:write(content)
    fromFile:close()
    toFile:close()
end


local pathPairs = {}

---增加多文件路径对
---@param fromPath string
---@param toPath string
local function addPathPair(fromPath, toPath)
    table.insert(pathPairs, {from = fromPath, to = toPath})
end

---清空多文件路径对
local function clearAllPaths()
    pathPairs = {}
end

---设置多文件路径对
local function setPathPairs()
    clearAllPaths()
    local isrunning = true
    while isrunning do
        io:write("[Lua] Enter source file path (or 'done' to finish): ")
        io:flush()
        local fromPath = io.read()
        while(type(fromPath) ~= "string") do
            io:write("[Lua] Invalid input. Please enter a valid file path.\n")
            io:flush()
            fromPath = io.read()
        end

        if fromPath == "done" then
            isrunning = false
        else
            io:write("[Lua] Enter destination file path: ")
            io:flush()
            local toPath = io.read()
            while(type(toPath) ~= "string") do
                io:write("[Lua] Invalid input. Please enter a valid file path.\n")
                io:flush()
                toPath = io.read()
            end

            addPathPair(fromPath, toPath)
        end
    end
end

---拷贝多个文件
function copy.copyListFile()
    setPathPairs()
    for _, pathPair in ipairs(pathPairs) do
        copy.copySingleFile(pathPair.from, pathPair.to)
    end
end

return copy