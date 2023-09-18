local function dump_page(self, doc, filename)

	local fh = io.open(filename, "w")
	if(fh) then 
		fh:write(doc)
		fh:close()
	end
end

local function load_page(self, filename)

	local doc = ""
	local fh = io.open(filename, "r")
	if(fh) then 
		doc = fh:read("*a")
		fh:close()
	end    
	return doc
end

function loadJSscript( filename )
	local text = sys.load_resource(filename)
	return "<script>"..tostring(text).."</script>"
end

function loadCSSscript( filename )
	local text = sys.load_resource(filename)
	return "<style>"..tostring(text).."</style>" 
end

return {
	dump_page = dump_page,
	load_page = load_page,
	loadJSscript = loadJSscript,
	loadCSSscript = loadCSSscript,
}