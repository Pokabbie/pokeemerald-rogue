constants = 
{
    targetHost = "127.0.0.1",
    targetPort = 30125,
    rogueHandshake1 = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g",
    rogueHandshake2 = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY",
    debugLog = false,
}

globals = 
{
    conn = nil,
    state = nil
}

-- Utils
--

function splitRequestStr(inputstr)
    words = {}
    for word in string.gmatch(inputstr, '([^:]+)') do
        table.insert(words, word) 
    end

    return words
end

function splitParamStr(inputstr)
    words = {}
    for word in string.gmatch(inputstr, '([^;]+)') do
        table.insert(words, word) 
    end

    return words
end

-- Commands
--

function Cmd_Send(reqId, data)
    local dataSize = string.len(data)
    if constants.debugLog then
        console:log("\tSend (" .. reqId .."): " .. dataSize .. ":'" .. data .. "'")
    end
    if reqId == nil then
        console:error("ReqId was nil ")
    else
        globals.conn:send(reqId .. ";" .. dataSize .. ";" .. data)
    end
end


function Cmd_EstablishConnection(params)
    globals.conn:send(constants.rogueHandshake2)
end

-- Write memory a byte a time (This is to avoid alignment issues for larger writes e.g. u32 has to be on the 4 byte boundary otherwise, it causes awkward to notice bugs)

function Cmd_writeByte(params)
    local reqId = tonumber(params[2])
    local addr = tonumber(params[3])
    emu:write8(addr, tonumber(params[4]))
    Cmd_Send(reqId, 1)
end

function Cmd_readByte(params)
    local reqId = tonumber(params[2])
    local addr = tonumber(params[3])
    local result = emu:read8(addr)
    Cmd_Send(reqId, result)
end

function Cmd_writeBytes(params)
    local reqId = tonumber(params[2])
    local addr = tonumber(params[3])
    for i, value in ipairs(params) do
        if i >= 4 then
            emu:write8(addr + tonumber(i) - 4, tonumber(value))
        end
    end

    Cmd_Send(reqId, 1)
end

function Cmd_readBytes(params)
    local reqId = tonumber(params[2])
    local addr = tonumber(params[3])
    local range = tonumber(params[4])
    local result = emu:readRange(addr, range)
    Cmd_Send(reqId, result)
end

commCmds = 
{
    con = Cmd_EstablishConnection,
    ws = Cmd_writeByte,
    rs = Cmd_readByte,
    w = Cmd_writeBytes,
    r = Cmd_readBytes,
}

function Conn_ProcessCmd(msg)
    if constants.debugLog then
        console:log("Incoming: '" .. msg .. "'")
    end
    
    local requests = splitRequestStr(msg)
    local allSuccess = true

    for i, req in ipairs(requests) do
        if constants.debugLog then
            console:log("\tRequest: '" .. req .. "'")
        end
        local params = splitParamStr(req)
        local success = false

        for k, v in pairs(commCmds) do
            if k == params[1] then
                v(params)
                success = true
                break
            end
        end

        if not success then
            console:error("Unknown Cmd (Ignoring...)")
            if constants.debugLog then
                console:error("Cmd: " .. req)
            end
            allSuccess = false
        end
    end
    
    -- we don't really want to disconnect whenever an unknown cmd makes it's way here
    -- return allSuccess
    return true
end


-- Socket callbacks
--
function Conn_Stop()
    if not globals.conn then
        return
    end

    console:log("Connection closed.")
    globals.conn:close()
    globals.conn = nil
end


function Conn_Error(err)
	console:error("Connection Error: " .. err)
    Conn_Stop()
end


function Conn_Received()
    local p, err = globals.conn:receive(4096)
    if p then
        if Conn_ProcessCmd(p) == false then
            Conn_Stop()
            return
        end
    else
        if err ~= socket.ERRORS.AGAIN then
            Conn_Error(err)
        end
        return
    end
end


-- Launch connection to Assistant
--
function LaunchAssistant()
    console:log("== Openning connection to RogueAssistant ==")

    globals.conn = socket.tcp()
    globals.conn:add("received", Conn_Received)
    globals.conn:add("error", Conn_Error)

    if globals.conn:connect(constants.targetHost, constants.targetPort) then
        console:log("Connected to Assistant.")

        globals.conn:send(constants.rogueHandshake1)
    else
        console:error("Unable to connect")
    end
end

function AssistantFrame()
    if globals.state == nil then
        globals.state = {}
        LaunchAssistant()
    end
	    globals.conn:poll()
end

callbacks:add("frame", AssistantFrame)