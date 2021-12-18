-- This is a simple cache system to avoid generating vertex data each time
-- we run an example. Since generating vertex data is a time-consuming task
-- (expecially if done in Lua), when we generate the data for a model we
-- serialize it and write it to a file in the /cache directory, possibly
-- encoding any parameter in the filename. If, subsequently, we need the data
-- for the same model with the same parameters - eg. if we are running the same
-- example again and again because we are fiddling with it -  all we have to do
-- is to deserialize the data cached in the file.
--
-- Note that the cache.write() and cache.read() functions are pretty generic:
-- they just serialize/deserialize a bunch of strings and do not impose any
-- naming scheme or location for the cached files.

local function Write(filename, ...)
-- Writes to filename the passed strings.
   local f = assert(io.open(filename, "wb"))
   print("Writing data to cache file '"..filename.."'")
   local args = {...}
   for _, v in ipairs({...}) do
      -- write the string to the file, together with its length
      -- encoded as a 4 byte integer
      f:write(string.pack("s4", v))
   end
   f:close()
end

local function Read(filename)
-- Reads and returns the strings contained in filename
   local f = io.open(filename, "rb")
   if not f then return nil end
   print("Reading data from cache file '"..filename.."'")
   local data = f:read("a")
   f:close()
   local ret, pos = {}, 1
   while pos < #data do
      ret[#ret+1], pos = string.unpack("s4", data, pos)
   end
   return table.unpack(ret)
end

return {
   write = Write,
   read = Read,
}

