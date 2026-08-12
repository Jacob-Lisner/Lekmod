-- modified by Temudjin from 1.0.3.142 code
-------------------------------------------------
-- FrontEnd
-------------------------------------------------

-- Written only by the post-ui_check FrontEnd. Joiners without ui_check never set this.
local function MarkLekmodUiCheckDone()
	pcall(function()
		local userData = Modding.OpenUserData("LekmodUiCheck", 1);
		userData.SetValue("done", 1);
	end);
end

function ShowHideHandler( bIsHide, bIsInit )

		-- Check for game invites first.  If we have a game invite, we will have flipped 
		-- the Civ5App::eHasShownLegal and not show the legal/touch screens.
		UI:CheckForCommandLineInvitation();

---------- Temudjin START
--    if not UI:HasShownLegal() then
--        UIManager:QueuePopup( Controls.LegalScreen, PopupPriority.LegalScreen );
--    end
---------- Temudjin END

	-- Presence of this FrontEnd.lua means ui_check.bat has been run.
	MarkLekmodUiCheckDone();

    if not bIsHide then
        Controls.AtlasLogo:SetTexture( "CivilzationVAtlas.dds" );
        Controls.AtlasLogo:SetTexture( "oracle_background.dds" );
    	UIManager:SetUICursor( 0 );
        UIManager:QueuePopup( Controls.MainMenu, PopupPriority.MainMenu );
    else
        Controls.AtlasLogo:UnloadTexture();
    end
end
ContextPtr:SetShowHideHandler( ShowHideHandler );
