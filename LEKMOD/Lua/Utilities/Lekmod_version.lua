------------------------------------------------------------------------------
-- Lekmod version helpers (menu status + multiplayer handshake)
------------------------------------------------------------------------------

LekmodVersion = LekmodVersion or {}

LekmodVersion.HANDSHAKE_PREFIX = "#LEKVER#"
LekmodVersion.VERSIONS_PAGE_URL = "https://github.com/EnormousApplePie/Lekmod/blob/main/LekmodInstaller/github_setup/versions.json"
LekmodVersion.HANDSHAKE_TIMEOUT = 12 -- seconds after connect before host kicks unverified players
LekmodVersion.CACHE_RELATIVE = "My Games\\Sid Meier's Civilization 5\\Lekmod\\version_cache.txt"

function LekmodVersion.Normalize(versionText)
	if versionText == nil then
		return nil
	end
	local version = string.match(tostring(versionText), "[vV]?(%d+%.%d+[%d%.]*)")
	if version == nil then
		return nil
	end
	return "v" .. version
end

function LekmodVersion.GetLocal()
	local raw = Locale.Lookup("TXT_KEY_LEKMOD_VERSION")
	return LekmodVersion.Normalize(raw) or raw
end

function LekmodVersion.GetHandshakeMessage()
	return LekmodVersion.HANDSHAKE_PREFIX .. (LekmodVersion.GetLocal() or "unknown")
end

function LekmodVersion.ParseHandshake(text)
	if text == nil then
		return nil
	end
	local prefix = LekmodVersion.HANDSHAKE_PREFIX
	if string.sub(text, 1, #prefix) ~= prefix then
		return nil
	end
	return LekmodVersion.Normalize(string.sub(text, #prefix + 1))
end

function LekmodVersion.Compare(a, b)
	local function parts(v)
		local out = {}
		v = LekmodVersion.Normalize(v) or ""
		for n in string.gmatch(v, "%d+") do
			table.insert(out, tonumber(n) or 0)
		end
		return out
	end
	local pa, pb = parts(a), parts(b)
	local n = math.max(#pa, #pb)
	for i = 1, n do
		local va = pa[i] or 0
		local vb = pb[i] or 0
		if va > vb then return 1 end
		if va < vb then return -1 end
	end
	return 0
end

-- FrontEnd has no Game Lua bindings; read installer/DLL-written cache instead.
local function GetUserProfileRoot()
	local roots = {}
	local ok, home = pcall(function()
		if os and os.getenv then
			return os.getenv("USERPROFILE") or os.getenv("HOME")
		end
		return nil
	end)
	if ok and home ~= nil and home ~= "" then
		table.insert(roots, home .. "\\Documents")
	end
	return roots
end

function LekmodVersion.ReadCacheFile()
	-- Prefer ModUserData: works on FrontEnd without Game DLL bindings / io.
	local okUd, userData = pcall(function()
		return Modding.OpenUserData("LekmodVersionCache", 1)
	end)
	if okUd and userData ~= nil then
		local latest = LekmodVersion.Normalize(userData.GetValue("Latest"))
		if latest ~= nil then
			return {
				Latest = latest,
				FileId = userData.GetValue("FileId"),
				Source = "userdata",
			}, nil
		end
	end

	if io == nil or io.open == nil then
		return nil, "no-io"
	end

	local tried = {}
	for _, root in ipairs(GetUserProfileRoot()) do
		local path = root .. "\\" .. LekmodVersion.CACHE_RELATIVE
		table.insert(tried, path)
		local ok, f = pcall(function()
			return io.open(path, "r")
		end)
		if ok and f ~= nil then
			local line1 = f:read("*l")
			local line2 = f:read("*l")
			f:close()
			local latest = LekmodVersion.Normalize(line1)
			if latest ~= nil then
				return {
					Latest = latest,
					FileId = (line2 ~= nil and string.match(line2, "%S+")) or nil,
					Source = "cache",
				}, nil
			end
		end
	end

	return nil, "cache-miss"
end

function LekmodVersion.WriteCacheUserData(latest, fileId)
	local ok, userData = pcall(function()
		return Modding.OpenUserData("LekmodVersionCache", 1)
	end)
	if not ok or userData == nil then
		return
	end
	pcall(function()
		userData.SetValue("Latest", latest or "")
		userData.SetValue("FileId", fileId or "")
	end)
end

local function ResultFromLatest(localVersion, latest, fileId, source)
	local result = {
		Local = localVersion,
		Success = true,
		IsNewer = LekmodVersion.Compare(latest, localVersion) > 0,
		Latest = latest,
		FileId = fileId,
		DownloadUrl = nil,
		Error = nil,
		Source = source,
	}
	if fileId ~= nil and fileId ~= "" then
		result.DownloadUrl = "https://drive.google.com/uc?export=download&id=" .. tostring(fileId)
	end
	return result
end

function LekmodVersion.CheckForUpdate()
	local localVersion = LekmodVersion.GetLocal()
	local result = {
		Local = localVersion,
		Success = false,
		IsNewer = false,
		Latest = nil,
		DownloadUrl = nil,
		Error = nil,
		Source = nil,
	}

	-- Prefer DLL when available (in-game / contexts where Game is registered).
	local checker = nil
	if type(LekmodCheckUpdate) == "function" then
		checker = LekmodCheckUpdate
	elseif Game ~= nil and Game.CheckLekmodUpdate ~= nil then
		checker = function(v) return Game.CheckLekmodUpdate(v) end
	end

	if checker ~= nil then
		local ok, info = pcall(function()
			return checker(localVersion)
		end)
		if ok and info ~= nil and (info.Success == true or info.Success == 1) then
			result.Success = true
			result.Latest = LekmodVersion.Normalize(info.Latest) or info.Latest
			result.FileId = info.FileId
			result.IsNewer = (info.IsNewer == true or info.IsNewer == 1)
			result.DownloadUrl = info.DownloadUrl
			result.Source = info.Source or "dll"
			-- Mirror into FrontEnd-readable userdata for next main-menu visit.
			LekmodVersion.WriteCacheUserData(result.Latest, result.FileId)
			return result
		end
		if ok and info ~= nil then
			result.Error = tostring(info.Error or "fetch-failed")
			result.Source = info.Source
		elseif not ok then
			result.Error = "call-failed"
		else
			result.Error = "nil-result"
		end
		-- Fall through to cache if DLL HTTP failed.
	else
		result.Error = "no-api"
	end

	local cached, cacheErr = LekmodVersion.ReadCacheFile()
	if cached ~= nil then
		return ResultFromLatest(localVersion, cached.Latest, cached.FileId, cached.Source)
	end

	if result.Error == nil or result.Error == "no-api" then
		result.Error = cacheErr or result.Error or "no-api"
	else
		result.Error = tostring(result.Error) .. "+" .. tostring(cacheErr or "cache-miss")
	end
	return result
end

function LekmodVersion.OpenDownload(url)
	local target = url
	if target == nil or target == "" then
		target = LekmodVersion.VERSIONS_PAGE_URL
	end
	-- Steam overlay is flaky with https://; always prefer http:// for ActivateGameOverlayToWebPage.
	if string.sub(target, 1, 8) == "https://" then
		target = "http://" .. string.sub(target, 9)
	elseif string.sub(target, 1, 7) ~= "http://" then
		target = "http://" .. target
	end
	pcall(function()
		Steam.ActivateGameOverlayToWebPage(target)
	end)
end
