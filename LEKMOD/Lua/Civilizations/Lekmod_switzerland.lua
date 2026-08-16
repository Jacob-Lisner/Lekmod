-- Author: EnormousApplePie & 404NotFound & Loup fixing Loup's fuckups ~~

include("Lekmod_utilities.lua")
include("PlotIterators.lua")

local this_civ = GameInfoTypes["CIVILIZATION_SWISS"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)
-----------------------------------------------------------------------------------------------------------------------
-- Switzerland UU: Reislaufer. Check if the unit that just moved is a reislaufer and if it is near a mountain. If it is,
-- give it the active promotion. If it is not, remove the active promotion.
------------------------------------------------------------------------------------------------------------------------
local mountaineer = GameInfoTypes["PROMOTION_SWISS_MOUNTAINEER"];
local mountaineer_active = GameInfoTypes["PROMOTION_SWISS_MOUNTAINEER_ACTIVE"];

function lekmod_switzerland_uu_mountain_bonus(player_id, unit_id)

	local player = Players[player_id]
	local unit = player:GetUnitByID(unit_id)
	if not unit:IsHasPromotion(mountaineer) then return end
   -- Note: IsNearTerrainType (method) is Lekmod specific! Not available in the base game
   if unit:IsNearTerrainType(GameInfoTypes["TERRAIN_MOUNTAIN"], 1, false) then
      unit:SetHasPromotion(mountaineer_active, true);
   else
      unit:SetHasPromotion(mountaineer_active, false);
   end

end
------------------------------------------------------------------------------------------------------------------------
-- Unique Units can be active even if the civilization is not active
GameEvents.UnitSetXY.Add(lekmod_switzerland_uu_mountain_bonus)
-- Note: UnitCreated is a Lekmod Event! Not available in the base game
GameEvents.UnitCreated.Add(lekmod_switzerland_uu_mountain_bonus)