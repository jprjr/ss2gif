local description = [=[
Usage: lua bin2c.lua filename 
]=]

if not arg or not arg[1] then
  io.stderr:write(description)
  return
end

function dirname(str)
	if str:match(".-/.-") then
		local name = string.gsub(str, "(.*/)(.*)", "%1")
		return name
	else
		return ''
	end
end

function basename(str)
	local name = string.gsub(str, "(.*/)(.*)", "%2")
	return name
end

local filename = arg[1]
local dir = dirname(filename)
local file = basename(filename)
local variable = file:gsub('.lua','')

local content = assert(io.open(filename,"rb")):read"*a"

local dump do
  local numtab={}; for i=0,255 do numtab[string.char(i)]=("%3d,"):format(i) end
  function dump(str)
    return (str:gsub(".", numtab):gsub(("."):rep(80), "%0\n"))
  end
end

local c_file = io.open(dir .. variable .. "-lua.c", "wb")
c_file:write("const unsigned char " .. variable .. "[" .. #content .. "]={\n")
c_file:write(dump(content))
c_file:write("};\n")
c_file:close()

local h_file = io.open(dir .. variable .. "-lua.h", "wb")
h_file:write("extern const unsigned char " .. variable .. "[" .. #content .. "];\n")
h_file:close()
