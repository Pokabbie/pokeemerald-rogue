--API Docs: https://mgba.io/docs/scripting.html#class-CallbackManager
--SocketServer example: https://github.com/mgba-emu/mgba/blob/b60e0b9282c03261cd5b025ca7fb255f545b9c23/res/scripts/socketserver.lua
--SocketTest example: https://github.com/mgba-emu/mgba/blob/6822e04c08003efe59a46d30e5e0cd2259c1aa3e/res/scripts/sockettest.lua

constants = 
{
    listenPort = 30150,
    testCommHeaderAddr = 0x08a0c898,
}

activeServer = 
{
    listenServer = nil,
    sockets = {},
    idCounter = 1,
}
--constants = protect(constants)

-- Initial hookup
--

--function connectCallbacks()
    --console:log("Hooking up callbacks")
    --callbacks:add("crashed")
    --callbacks:add("frame", onEndFrame)
--end

function msgFormat(id, msg, isError)
    local prefix = "Socket " .. id
    if isError then
        prefix = prefix .. " Error: "
    else
        prefix = prefix .. " Received: "
    end
        return prefix .. msg
end


function Socket_Stop(id)
    local sock = activeServer.sockets[id]
    activeServer.sockets[id] = nil
    sock:close()
end


function Socket_Error(id)
    console:error(msgFormat(id, err, true))
    Socket_Stop(id)
end


function Socket_Received(id)
    local sock = activeServer.sockets[id]
    if not sock then
        return
    end

    while true do
        local p, err = sock:receive(4096)
        if p then
            console:log("a")
        else
            if err ~= socket.ERRORS.AGAIN then
                console:error(msgFormat(id, err, true))
                Socket_Stop(id)
            end
            return
        end
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
    console:log(msgFormat(id, "Connected"))
end

function setupServer()
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

    --callbacks:add("crashed")
    --callbacks:add("frame", onEndFrame)
end

function detectGame()
    -- TODO
    console:log("Skipping game detect..")

    local commBufferSize = emu:read32(constants.testCommHeaderAddr);
    local commBufferAddr = emu:read32(constants.testCommHeaderAddr + 4);

    console:log("Comm Buffer Size:" .. tostring(commBufferSize))
    console:log("Comm Buffer Addr:" .. tostring(commBufferAddr))
    
    console:log("frame")
    return true
end

--if emu then
--    if detectGame() == true then
--        setupServer()
--    end
--end

setupServer()