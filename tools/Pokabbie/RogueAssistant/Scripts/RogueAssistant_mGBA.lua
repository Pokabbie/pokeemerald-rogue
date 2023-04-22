constants = 
{
    targetHost = "127.0.0.1",
    targetPort = 30150,
    
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

function Cmd_EstablishConnection(params)
    globals.conn:send(constants.rogueHandshake2)
end

function Cmd_HelloWorld(params)
    globals.conn:send("Hello to you too!")
end

-- Write memory a byte a time (This is to avoid alignment issues for larger writes e.g. u32 has to be on the 4 byte boundary otherwise, it causes awkward to notice bugs)

function Cmd_writeByte(params)
    local addr = tonumber(params[2])
    emu:write8(addr, tonumber(params[3]))
    globals.conn:send(1)
end

function Cmd_readByte(params)
    local addr = tonumber(params[2])
    local result = emu:read8(addr)
    globals.conn:send(result)
end

function Cmd_writeBytes(params)
    local addr = tonumber(params[2])
    for i, value in ipairs(params) do
        if i >= 3 then
            emu:write8(addr + tonumber(i) - 3, tonumber(value))
        end
    end

    globals.conn:send(1)
end

function Cmd_readBytes(params)
    local addr = tonumber(params[2])
    local range = tonumber(params[3])
    local result = emu:readRange(addr, range)
    globals.conn:send(result)
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
        console:log("In: " .. msg)
    end
    
    local requests = splitRequestStr(msg)

    for i, req in ipairs(requests) do
        if constants.debugLog then
            console:log("\tReq: " .. req)
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