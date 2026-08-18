-- modified by Temudjin from 1.0.3.142 code
-------------------------------------------------
-- FrontEnd
-------------------------------------------------

local function MarkLekmodUiCheckDone()
	pcall(function()
		local userData = Modding.OpenUserData("LekmodUiCheck", 1);
		userData.SetValue("done", 1);
	end);
end

local function IsUiCheckConfigured()
	LekmodUiConfigured = nil;
	pcall(function()
		include("LekmodUiConfigured");
	end);
	return LekmodUiConfigured == true;
end

function ShowHideHandler( bIsHide, bIsInit )

		-- Check for game invites first.  If we have a game invite, we will have flipped 
		-- the Civ5App::eHasShownLegal and not show the legal/touch screens.
		UI:CheckForCommandLineInvitation();

    if not bIsHide then
        Controls.AtlasLogo:SetTexture( "CivilzationVAtlas.dds" );
        Controls.AtlasLogo:SetTexture( "oracle_background.dds" );
    	UIManager:SetUICursor( 0 );
		if not IsUiCheckConfigured() then
			-- Manual install without ui_check.bat: block the menus.
			UIManager:QueuePopup( Controls.LegalScreen, PopupPriority.LegalScreen );
		else
			MarkLekmodUiCheckDone();
			UIManager:QueuePopup( Controls.MainMenu, PopupPriority.MainMenu );
		end
    else
        Controls.AtlasLogo:UnloadTexture();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
