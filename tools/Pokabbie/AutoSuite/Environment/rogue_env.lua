


constants = 
{
    listenPort = 30150,
    logCommands = false, --This can cause memory leaks due to the shear amount of messages
}

activeServer = 
{
    listenServer = nil,
    sockets = {},
    idCounter = 1,
}

-- Utils
--

function splitStr(inputstr)
    words = {}
    for word in string.gmatch(inputstr, '([^;]+)') do
        table.insert(words, word) 
    end

    return words
end

function msgFormat(id, msg, isError)
    local prefix = "Socket " .. id
    if isError then
        prefix = prefix .. " Error: "
    else
        prefix = prefix .. " : "
    end
        return prefix .. msg
end

-- Emu Specific
--
--function AutoEmu_OnFrame()
--end

-- Automation Cmds
--

function AutoCmd_HelloWorld(sock, params)
    sock:send("Hello to you too!")
end

function AutoCmd_emu_read8(sock, params)
    local addr = tonumber(params[2])
    local result = emu:read8(addr)
    sock:send(tostring(result))
end

function AutoCmd_emu_read16(sock, params)
    local addr = tonumber(params[2])
    local result = emu:read16(addr)
    sock:send(tostring(result))
end

function AutoCmd_emu_read32(sock, params)
    local addr = tonumber(params[2])
    local result = emu:read32(addr)
    sock:send(tostring(result))
end

function AutoCmd_emu_readRange(sock, params)
    local addr = tonumber(params[2])
    local range = tonumber(params[3])
    local result = emu:readRange(addr, range)
    sock:send(tostring(result))
end

function AutoCmd_emu_write8(sock, params)
    local addr = tonumber(params[2])
    local value = tonumber(params[32])
    emu:write8(addr, value)
    sock:send(tostring(value))
end

function AutoCmd_emu_write16(sock, params)
    local addr = tonumber(params[2])
    local value = tonumber(params[3])
    emu:write16(addr, value)
    sock:send(tostring(value))
end

function AutoCmd_emu_write32(sock, params)
    local addr = tonumber(params[2])
    local value = tonumber(params[3])
    emu:write32(addr, value)
    sock:send(tostring(value))
end

function AutoCmd_emu_setKeys(sock, params)
    local keys = tonumber(params[2])
    emu:setKeys(keys)
    sock:send(tostring(keys))
end

function AutoCmd_emu_reset(sock, params)
    emu:reset()
    sock:send("1")
end

function AutoCmd_emu_saveStateSlot(sock, params)
    local slot = tonumber(params[2])
    emu:saveStateSlot(slot, 31)
    sock:send("1")
end

function AutoCmd_emu_loadStateSlot(sock, params)
    local slot = tonumber(params[2])
    emu:loadStateSlot(slot, 29)
    sock:send("1")
end

autoCmds = 
{
    hello = AutoCmd_HelloWorld,
    emu_read8 = AutoCmd_emu_read8,
    emu_read16 = AutoCmd_emu_read16,
    emu_read32 = AutoCmd_emu_read32,
    emu_readRange = AutoCmd_emu_readRange,
    emu_write8 = AutoCmd_emu_write8,
    emu_write16 = AutoCmd_emu_write16,
    emu_write32 = AutoCmd_emu_write32,
    emu_setkeys = AutoCmd_emu_setKeys,
    emu_reset = AutoCmd_emu_reset,
    emu_saveStateSlot = AutoCmd_emu_saveStateSlot,
    emu_loadStateSlot = AutoCmd_emu_loadStateSlot,
}

function Auto_ProcessCmd(sock, msg)
    local params = splitStr(msg)

    for k, v in pairs(autoCmds) do
        if k == params[1] then

            if constants.logCommands then
                console:log("Executing Cmd: " .. msg)
            end

            v(sock, params)
            return true
        end
    end

    console:error("Unknown Cmd: " .. msg)
    return false
end

-- Server
--
function Socket_Stop(id)
    console:log(msgFormat(id .. ":stop", "Closing connection"))
    local sock = activeServer.sockets[id]
    activeServer.sockets[id] = nil
    sock:close()
end


function Socket_Error(id)
    console:error(msgFormat(id .. ":error", err, true))
    Socket_Stop(id)
end


function Socket_Received(id)
    local sock = activeServer.sockets[id]
    if not sock then
        return
    end

    -- loop here?
    local p, err = sock:receive(4096)
    if p then
        if Auto_ProcessCmd(sock, p) == false then
            Socket_Stop(id)
            return
        end
    else
        if err ~= socket.ERRORS.AGAIN then
            console:error(msgFormat(id .. ":receive", err, true))
            Socket_Stop(id)
        end
        return
    end
end


function Server_Accept()
    local sock, err = activeServer.listenServer:accept()
    if err then
        console:error(msgFormat("Accept", err, true))
        return
    end

    local id = activeServer.idCounter
    activeServer.idCounter = id + 1

    activeServer.sockets[id] = sock

    sock:add("received", function() Socket_Received(id) end)
    sock:add("error", function() Socket_Error(id) end)
    console:log(msgFormat(id .. ":connect", "Connected"))
end

-- Run Server
if emu then
    --callbacks:add("frame", AutoEmu_OnFrame)

    console:log("Hooking up Server")

    while not activeServer.listenServer do
        activeServer.listenServer, err = socket.bind(nil, constants.listenPort)
        if err then
            console:error(msgFormat("Bind", err, true))
            break
        else
            local ok
            ok, err = activeServer.listenServer:listen()
            if err then
                activeServer.listenServer:close()
                console:error(msgFormat("Listen", err, true))
            else
                console:log("Server Listening on port: " .. constants.listenPort)
                activeServer.listenServer:add("received", Server_Accept)
            end
        end
    end
end