constants = 
{
    targetHost = "127.0.0.1",
    targetPort = 30125,
    
    rogueHandshake1 = "3to8UEaoManH7wB4lKlLRgywSHHKmI0g",
    rogueHandshake2 = "Em68TrzBAFlyhBCOm4XQIjGWbdNhuplY",

    debugLog = true,
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
    if constants.debugLog then
        console:log("Send (" .. reqId .."): '" .. data .. "'")
    end
    globals.conn:send(reqId .. ";" .. data)
end


function Cmd_EstablishConnection(params)
    globals.conn:send(constants.rogueHandshake2)
end

function Cmd_HelloWorld(params)
    Cmd_Send(0, "Hello to you too!")
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
    if constants.debugLog then
        console:log("Range: '" .. addr .. " : " .. range .. "'")
        console:log("Range: '" .. result .. "'")
    end
    Cmd_Send(reqId, result)
end

commCmds = 
{
    con = Cmd_EstablishConnection,
    hello = Cmd_HelloWorld,
    writeByte = Cmd_writeByte,
    readByte = Cmd_readByte,
    writeBytes = Cmd_writeBytes,
    readBytes = Cmd_readBytes,
}

function Conn_ProcessCmd(msg)
    if constants.debugLog then
        console:log("Incoming: '" .. msg .. "'")
    end
    
    local requests = splitRequestStr(msg)

    for i, req in ipairs(requests) do
        if constants.debugLog then
            console:log("\tRequest: '" .. req .. "'")
        end
        local params = splitParamStr(req)

        for k, v in pairs(commCmds) do
            if k == params[1] then
                v(params)
                return true
            end
        end
    end

    console:error("Unknown Cmd: " .. msg)
    return false
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