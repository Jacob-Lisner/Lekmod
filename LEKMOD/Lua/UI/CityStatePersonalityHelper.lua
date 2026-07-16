------------------------------------------------------------------------------
-- CityStatePersonalityHelper.lua
-- Data-driven city-state personality display helpers (LEKMOD_MINOR_CIV_PERSONALITIES)
-- Shared by vanilla UI and EUI; lives in Lua/UI (not ui_check.bat tmp copies).
------------------------------------------------------------------------------

local g_PersonalityColorPositiveTypes = {
	MINOR_CIV_PERSONALITY_FRIENDLY = true,
	MINOR_CIV_PERSONALITY_WEALTHY = true,
	MINOR_CIV_PERSONALITY_PACIFISTIC = true,
};

local g_PersonalityColorNegativeTypes = {
	MINOR_CIV_PERSONALITY_HOSTILE = true,
	MINOR_CIV_PERSONALITY_IMPOVERISHED = true,
};

local g_PersonalityHelloKeys = {
	MINOR_CIV_PERSONALITY_FRIENDLY = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_FRIENDLY",
	MINOR_CIV_PERSONALITY_NEUTRAL = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_NEUTRAL",
	MINOR_CIV_PERSONALITY_HOSTILE = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_HOSTILE",
	MINOR_CIV_PERSONALITY_IRRATIONAL = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_IRRATIONAL",
	MINOR_CIV_PERSONALITY_WEALTHY = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_WEALTHY",
	MINOR_CIV_PERSONALITY_IMPOVERISHED = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_IMPOVERISHED",
	MINOR_CIV_PERSONALITY_PIRATE_REPUBLIC = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_PIRATE_REPUBLIC",
	MINOR_CIV_PERSONALITY_THEOCRATIC = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_THEOCRATIC",
	MINOR_CIV_PERSONALITY_PACIFISTIC = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_PACIFISTIC",
	MINOR_CIV_PERSONALITY_ISOLATIONIST = "TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_ISOLATIONIST",
};

MinorCivPersonalityGiveLabelKey = "TXT_KEY_POP_CSTATE_GIFT_GOLD";
MinorCivPersonalityGiveTooltipKey = "TXT_KEY_POP_CSTATE_GIFT_GOLD_TT";
MinorCivPersonalityTakeLabelKey = "TXT_KEY_POP_CSTATE_BULLY";
MinorCivPersonalityTakeTooltipKey = "TXT_KEY_POP_CSTATE_BULLY_TT";

local function IsPersonalityFlagEnabled(row, fieldName)
	if row == nil or fieldName == nil then
		return false;
	end

	local value = row[fieldName];
	return value == true or value == 1;
end

local function StripPersonalityHelpBulletList(strTooltip)
	if strTooltip == nil or strTooltip == "" then
		return strTooltip;
	end

	local bulletStart = string.find(strTooltip, "\n\n%-", 1, true);
	if bulletStart ~= nil then
		return string.sub(strTooltip, 1, bulletStart - 1);
	end

	return strTooltip;
end

local function ColorPersonalityNameInTooltip(strTooltip, strPersonalityName)
	if strTooltip == nil or strTooltip == "" or strPersonalityName == nil or strPersonalityName == "" then
		return strTooltip;
	end

	if string.find(strTooltip, "%[COLOR_POSITIVE_TEXT%]", 1, true) ~= nil then
		return strTooltip;
	end

	local coloredName = "[COLOR_POSITIVE_TEXT]" .. strPersonalityName .. "[ENDCOLOR]";
	local coloredTooltip, replaceCount = string.gsub(strTooltip, strPersonalityName, coloredName, 1);
	if replaceCount > 0 then
		return coloredTooltip;
	end

	return strTooltip;
end

function GetMinorCivPersonalityRow(pMinorPlayer)
	if pMinorPlayer == nil or not pMinorPlayer:IsMinorCiv() then
		return nil;
	end

	local personalityType = pMinorPlayer:GetMinorCivPersonalityType();
	if personalityType ~= nil and personalityType ~= "" and GameInfo.Minor_Civ_Personalities[personalityType] ~= nil then
		return GameInfo.Minor_Civ_Personalities[personalityType];
	end

	local iPersonality = pMinorPlayer:GetPersonality();
	if MinorCivPersonalityTypes ~= nil then
		if iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_FRIENDLY then
			return GameInfo.Minor_Civ_Personalities["MINOR_CIV_PERSONALITY_FRIENDLY"];
		elseif iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_NEUTRAL then
			return GameInfo.Minor_Civ_Personalities["MINOR_CIV_PERSONALITY_NEUTRAL"];
		elseif iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_HOSTILE then
			return GameInfo.Minor_Civ_Personalities["MINOR_CIV_PERSONALITY_HOSTILE"];
		elseif iPersonality == MinorCivPersonalityTypes.MINOR_CIV_PERSONALITY_IRRATIONAL then
			return GameInfo.Minor_Civ_Personalities["MINOR_CIV_PERSONALITY_IRRATIONAL"];
		end
	end

	return nil;
end

function GetMinorCivPersonalityDisplayText(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	if row == nil then
		return "", "";
	end

	local strTitle = Locale.ConvertTextKey(row.Description);
	local strTooltip = Locale.ConvertTextKey(row.Help);
	strTooltip = StripPersonalityHelpBulletList(strTooltip);
	strTooltip = ColorPersonalityNameInTooltip(strTooltip, strTitle);

	if g_PersonalityColorPositiveTypes[row.Type] then
		strTitle = "[COLOR_POSITIVE_TEXT]" .. strTitle .. "[ENDCOLOR]";
	elseif g_PersonalityColorNegativeTypes[row.Type] then
		strTitle = "[COLOR_NEGATIVE_TEXT]" .. strTitle .. "[ENDCOLOR]";
	end

	return strTitle, strTooltip;
end

function GetMinorCivPersonalityHelloText(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	if row ~= nil and g_PersonalityHelloKeys[row.Type] ~= nil then
		return Locale.ConvertTextKey(g_PersonalityHelloKeys[row.Type]);
	end

	return Locale.ConvertTextKey("TXT_KEY_CITY_STATE_DIPLO_HELLO_PEACE_NEUTRAL");
end

function DoesMinorCivPersonalityBlockGifts(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	return IsPersonalityFlagEnabled(row, "NoGifts") or IsPersonalityFlagEnabled(row, "BlocksGoldGifts");
end

function DoesMinorCivPersonalityBlockPledgeToProtect(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	return IsPersonalityFlagEnabled(row, "BlocksPledgeToProtect");
end

function DoesMinorCivPersonalityBlockTribute(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	return IsPersonalityFlagEnabled(row, "BlocksTribute");
end

function DoesMinorCivPersonalityBlockWarDeclarationPenalty(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	return IsPersonalityFlagEnabled(row, "BlocksWarDeclarationPenalty");
end

function IsMinorCivMajorBlockedByAlliedWar(pMinorPlayer, iMajorPlayer)
	if pMinorPlayer == nil or not pMinorPlayer:IsMinorCiv() or iMajorPlayer == nil then
		return false;
	end

	if pMinorPlayer.IsMajorBlockedByAlliedWar ~= nil then
		return pMinorPlayer:IsMajorBlockedByAlliedWar(iMajorPlayer);
	end

	return false;
end

function GetMinorCivPersonalityColoredName(pMinorPlayer)
	local row = GetMinorCivPersonalityRow(pMinorPlayer);
	if row == nil then
		return "";
	end

	local strPersonalityName = Locale.ConvertTextKey(row.Description);
	return "[COLOR_POSITIVE_TEXT]" .. strPersonalityName .. "[ENDCOLOR]";
end

function GetMinorCivPersonalityBlockGiftsTooltip(pMinorPlayer)
	if not DoesMinorCivPersonalityBlockGifts(pMinorPlayer) then
		return nil;
	end

	return Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_PERSONALITY_BLOCK_GIFTS_TT", GetMinorCivPersonalityColoredName(pMinorPlayer));
end

function GetMinorCivPersonalityBlockPledgeTooltip(pMinorPlayer)
	if not DoesMinorCivPersonalityBlockPledgeToProtect(pMinorPlayer) then
		return nil;
	end

	return Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_PERSONALITY_BLOCK_PLEDGE_TT", GetMinorCivPersonalityColoredName(pMinorPlayer));
end

function GetMinorCivPersonalityBlockTributeTooltip(pMinorPlayer)
	if not DoesMinorCivPersonalityBlockTribute(pMinorPlayer) then
		return nil;
	end

	return Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_PERSONALITY_BLOCK_TRIBUTE_TT", GetMinorCivPersonalityColoredName(pMinorPlayer));
end

function GetMinorCivPersonalityNoWarPenaltyTooltip(pMinorPlayer)
	if not DoesMinorCivPersonalityBlockWarDeclarationPenalty(pMinorPlayer) then
		return nil;
	end

	return Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_PERSONALITY_NO_WAR_PENALTY_TT", GetMinorCivPersonalityColoredName(pMinorPlayer));
end

function GetMinorCivPersonalityAlliedWarBlockGiftsTooltip(pMinorPlayer, iMajorPlayer)
	if not IsMinorCivMajorBlockedByAlliedWar(pMinorPlayer, iMajorPlayer) then
		return nil;
	end

	local iAllyPlayer = pMinorPlayer:GetAlly();
	if iAllyPlayer == nil or iAllyPlayer < 0 then
		return nil;
	end

	local pAllyPlayer = Players[iAllyPlayer];
	if pAllyPlayer == nil then
		return nil;
	end

	local strAllyName = Locale.ConvertTextKey(pAllyPlayer:GetNameKey());
	return Locale.ConvertTextKey("TXT_KEY_MINOR_CIV_PERSONALITY_ALLIED_WAR_BLOCK_GIFTS_TT", GetMinorCivPersonalityColoredName(pMinorPlayer), strAllyName);
end

function ApplyMinorCivPersonalityBlockedButton(strLabelKey, strBlockTooltip, labelControl, buttonControl, animControl)
	local strLabel = Locale.ConvertTextKey(strLabelKey);
	labelControl:SetText("[COLOR_WARNING_TEXT]" .. strLabel .. "[ENDCOLOR]");
	animControl:SetHide(true);
	buttonControl:SetDisabled(true);
	buttonControl:ClearCallback(Mouse.eLClick);
	if strBlockTooltip ~= nil and strBlockTooltip ~= "" then
		buttonControl:SetToolTipString(strBlockTooltip);
	end
end

function ClearMinorCivPersonalityBlockedButton(strLabelKey, strDefaultTooltipKey, labelControl, buttonControl, animControl, clickHandler)
	if labelControl.LocalizeAndSetText ~= nil then
		labelControl:LocalizeAndSetText(strLabelKey);
	else
		labelControl:SetText(Locale.ConvertTextKey(strLabelKey));
	end

	buttonControl:SetDisabled(false);
	animControl:SetHide(false);

	if strDefaultTooltipKey ~= nil and strDefaultTooltipKey ~= "" then
		buttonControl:SetToolTipString(Locale.ConvertTextKey(strDefaultTooltipKey));
	else
		buttonControl:SetToolTipString(nil);
	end

	if clickHandler ~= nil then
		buttonControl:RegisterCallback(Mouse.eLClick, clickHandler);
	end
end
