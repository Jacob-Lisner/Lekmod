------------------------------------------------------------------------------
-- Lekmod version helpers (menu status + multiplayer handshake)
------------------------------------------------------------------------------

LekmodVersion = LekmodVersion or {}

LekmodVersion.HANDSHAKE_PREFIX = "#LEKVER#"
LekmodVersion.VERSIONS_PAGE_URL = "https://github.com/EnormousApplePie/Lekmod/blob/main/LekmodInstaller/github_setup/versions.json"
-- FrontEnd fetches these via undocumented vanilla Network.HttpRequest (Civ5-Patch pattern).
LekmodVersion.VERSIONS_RAW_URLS = {
	"https://raw.githubusercontent.com/EnormousApplePie/Lekmod/main/LekmodInstaller/github_setup/versions.json",
	"https://cdn.jsdelivr.net/gh/EnormousApplePie/Lekmod@main/LekmodInstaller/github_setup/versions.json",
}
LekmodVersion.HTTP_TIMEOUT = 10 -- seconds
LekmodVersion.HANDSHAKE_TIMEOUT = 12 -- seconds after connect before host kicks unverified players

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

function LekmodVersion.UnreachableResult(errorCode)
	return {
		Local = LekmodVersion.GetLocal(),
		Success = false,
		IsNewer = false,
		Latest = nil,
		DownloadUrl = nil,
		Error = errorCode or "unreachable",
		Source = "http",
	}
end

-- Parse installer versions.json body; returns latest version + file_id.
function LekmodVersion.ParseVersionsJson(body)
	if body == nil or body == "" then
		return nil, nil
	end
	local best, bestFileId = nil, nil
	for ver, block in string.gmatch(tostring(body), '"([vV]?%d+%.%d+[%d%.]*)"%s*:%s*(%b{})') do
		local norm = LekmodVersion.Normalize(ver)
		if norm ~= nil then
			local fileId = string.match(block, '"file_id"%s*:%s*"([^"]+)"')
			if best == nil or LekmodVersion.Compare(norm, best) > 0 then
				best = norm
				bestFileId = fileId
			end
		end
	end
	return best, bestFileId
end

function LekmodVersion.ResultFromVersionsBody(body, source)
	local localVersion = LekmodVersion.GetLocal()
	local latest, fileId = LekmodVersion.ParseVersionsJson(body)
	if latest == nil then
		return LekmodVersion.UnreachableResult("parse-failed")
	end
	return ResultFromLatest(localVersion, latest, fileId, source or "http")
end

-- Undocumented vanilla API (FireWire FHttpRequest in CivilizationV*.exe).
function LekmodVersion.HasHttpRequest()
	return Network ~= nil and type(Network.HttpRequest) == "function"
end

function LekmodVersion.StartHttpVersionsRequest(urlIndex)
	if not LekmodVersion.HasHttpRequest() then
		return nil, nil
	end
	local urls = LekmodVersion.VERSIONS_RAW_URLS
	local index = urlIndex or 1
	if index < 1 or index > #urls then
		return nil, nil
	end
	local url = urls[index]
	local ok, req = pcall(function()
		return Network.HttpRequest(url)
	end)
	if ok and req ~= nil then
		return req, index
	end
	return nil, nil
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
