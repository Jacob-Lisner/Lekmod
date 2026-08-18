"""
UI Manager - Handles UI detection and file configuration
"""
import os
import shutil
import zipfile
from pathlib import Path

try:
    import winreg
except ImportError:
    winreg = None  # Not on Windows

class UIManager:
    def __init__(self):
        self.civ5_path = None
        
    def find_civ5_installation(self):
        """Find Civ 5 installation path"""
        # Check common locations
        common_paths = [
            r"C:\Program Files (x86)\Steam\steamapps\common\Sid Meier's Civilization V",
            r"C:\Program Files\Steam\steamapps\common\Sid Meier's Civilization V",
            r"D:\Steam\steamapps\common\Sid Meier's Civilization V",
            r"E:\Steam\steamapps\common\Sid Meier's Civilization V",
        ]
        
        for path in common_paths:
            if os.path.exists(path):
                self.civ5_path = path
                return path
        
        # Check Steam registry (Windows only)
        if winreg:
            try:
                key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, 
                                    r"SOFTWARE\WOW6432Node\Valve\Steam")
                steam_path, _ = winreg.QueryValueEx(key, "InstallPath")
                winreg.CloseKey(key)
                
                civ5_path = os.path.join(steam_path, 
                                        "steamapps", "common", 
                                        "Sid Meier's Civilization V")
                if os.path.exists(civ5_path):
                    self.civ5_path = civ5_path
                    return civ5_path
            except:
                pass
        
        # Check user documents (for non-Steam versions)
        try:
            docs_path = Path.home() / "Documents" / "My Games" / "Sid Meier's Civilization 5"
            if docs_path.exists():
                # Try to find installation from here
                pass
        except:
            pass
            
        return None
        
    def check_eui_installed(self, civ5_path):
        """Check if EUI is installed in Assets/DLC (for informational purposes)"""
        if not civ5_path:
            return False
            
        # DLC folder is in the same directory as Civ5.exe
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        
        if not os.path.exists(dlc_path):
            return False
        
        # Check for EUI folders in Assets/DLC
        eui_folders = ["UI_bc1", "UI_bc1_xits"]
        
        for folder in eui_folders:
            folder_path = os.path.join(dlc_path, folder)
            if os.path.exists(folder_path):
                return True
                
        return False
        
    def get_current_lekmod_version(self, civ5_path):
        """Get currently installed Lekmod version (from Assets/DLC)"""
        if not civ5_path:
            return None
            
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        
        if not os.path.exists(dlc_path):
            return None
        
        # Look for LEKMOD_* folders
        lekmod_folders = []
        try:
            for item in os.listdir(dlc_path):
                if item.startswith("LEKMOD_"):
                    lekmod_folders.append(item)
        except:
            return None
        
        # If multiple versions, get the newest
        if not lekmod_folders:
            return None
        
        # Return the version from the folder name (e.g., LEKMOD_v34.10 -> v34.10)
        latest_folder = sorted(lekmod_folders)[-1]  # Simple alphabetical sort
        version_from_folder = latest_folder.replace("LEKMOD_", "")
        
        lekmod_path = os.path.join(dlc_path, latest_folder)
        
        # Check various version indicators
        version_files = [
            os.path.join(lekmod_path, "version.txt"),
            os.path.join(lekmod_path, "VERSION"),
            os.path.join(lekmod_path, "Lekmod (v 1).modinfo"),
            os.path.join(lekmod_path, "LEKMOD.modinfo"),
        ]
        
        for version_file in version_files:
            if os.path.exists(version_file):
                try:
                    with open(version_file, 'r', encoding='utf-8') as f:
                        content = f.read()
                        # Try to extract version number
                        import re
                        match = re.search(r'v?\d+\.\d+', content)
                        if match:
                            return match.group(0)
                except:
                    pass
        
        # If no version file found, return version from folder name
        if version_from_folder:
            return version_from_folder
            
        return None
        
    def extract_mod(self, zip_path, log_callback, extract_name="lekmod_temp"):
        """Extract downloaded archive"""
        extract_dir = os.path.join(os.path.dirname(zip_path), extract_name)
        
        if os.path.exists(extract_dir):
            shutil.rmtree(extract_dir)
        os.makedirs(extract_dir)
            
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            # Get total file count for progress
            total_files = len(zip_ref.namelist())
            extracted = 0
            
            for file in zip_ref.namelist():
                zip_ref.extract(file, extract_dir)
                extracted += 1
                if extracted % 100 == 0 or extracted == total_files:
                    log_callback(f"Extracting... {extracted}/{total_files} files")
            
        return extract_dir

    def _find_lekmod_folder(self, root_path):
        """Find the LEKMOD content folder, skipping LEKMOD_DLL and similar."""
        if os.path.isdir(os.path.join(root_path, "Lua", "tmp")):
            return root_path

        exact = None
        versioned = []
        for dirpath, dirnames, _files in os.walk(root_path):
            for dir_name in list(dirnames):
                upper = dir_name.upper()
                if upper == "LEKMOD_DLL":
                    continue
                if not upper.startswith("LEKMOD"):
                    continue
                full = os.path.join(dirpath, dir_name)
                if upper == "LEKMOD":
                    exact = full
                else:
                    versioned.append(full)
            if exact:
                break

        if exact:
            return exact
        if versioned:
            return versioned[0]
        return None

    def _find_eui_folder(self, civ5_path):
        """Return the installed EUI DLC folder, matching ui_check.bat preference."""
        if not civ5_path:
            return None
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        for folder in ("UI_bc1", "UI_bc1_xits"):
            path = os.path.join(dlc_path, folder)
            if os.path.isdir(path):
                return path
        return None

    def _resolve_tmp_source(self, lekmod_path, tmp_rel):
        """Resolve Lua/tmp/<rel> with .ignore / .IGNORE / raw filename."""
        base = os.path.join(lekmod_path, "Lua", "tmp", tmp_rel.replace("/", os.sep))
        for candidate in (base + ".ignore", base + ".IGNORE", base):
            if os.path.isfile(candidate):
                return candidate
        return None

    def _copy_ui_file(self, lekmod_path, tmp_rel, ui_dest, dest_name=None):
        source = self._resolve_tmp_source(lekmod_path, tmp_rel)
        if not source:
            return False
        dest_name = dest_name or os.path.basename(tmp_rel)
        shutil.copy2(source, os.path.join(ui_dest, dest_name))
        return True

    def _eui_has_file(self, eui_folder, rel_path):
        return bool(eui_folder) and os.path.isfile(
            os.path.join(eui_folder, rel_path.replace("/", os.sep))
        )

    def _eui_file_contains(self, eui_folder, rel_path, text):
        if not eui_folder:
            return False
        path = os.path.join(eui_folder, rel_path.replace("/", os.sep))
        if not os.path.isfile(path):
            return False
        try:
            with open(path, "r", encoding="utf-8", errors="ignore") as handle:
                return text in handle.read()
        except Exception:
            return False

    def _use_eui_overlay(self, want_eui, eui_folder, eui_rel=None):
        """Match ui_check.bat IF EXIST / IF NOT EXIST against the installed EUI pack."""
        if not want_eui:
            return False
        if eui_folder is None:
            return True
        if eui_rel is None:
            return True
        return self._eui_has_file(eui_folder, eui_rel)

    def configure_ui_files(self, mod_path, ui_type, log_callback, civ5_path=None):
        """Configure UI files based on UI type (replicates ui_check.bat)."""
        lekmod_path = self._find_lekmod_folder(mod_path)
        if not lekmod_path or not os.path.exists(lekmod_path):
            raise Exception("LEKMOD folder not found in extracted files")

        ui_dest = os.path.join(lekmod_path, "Lua", "UI")
        want_eui = "Enhanced UI" in ui_type
        eui_folder = self._find_eui_folder(civ5_path) if want_eui else None
        if want_eui and eui_folder:
            log_callback(f"✓ Configuring for EUI ({os.path.basename(eui_folder)})")
        elif want_eui:
            log_callback("✓ Configuring for EUI (pack not found next to Civ5; using EUI overlays)")
        else:
            log_callback("✓ Configuring for Standard UI")

        # Keep files that live only in Lua/UI (not Lua/tmp), e.g. CityStatePersonalityHelper.
        preserved = {}
        if os.path.isdir(ui_dest):
            for name in os.listdir(ui_dest):
                src = os.path.join(ui_dest, name)
                if os.path.isfile(src):
                    with open(src, "rb") as handle:
                        preserved[name] = handle.read()
            shutil.rmtree(ui_dest)
        os.makedirs(ui_dest)

        copied = []
        missing = []

        def copy_tmp(tmp_rel, dest_name=None):
            dest_name = dest_name or os.path.basename(tmp_rel)
            if self._copy_ui_file(lekmod_path, tmp_rel, ui_dest, dest_name):
                copied.append(dest_name)
            else:
                missing.append(tmp_rel)

        def copy_eui_or_vanilla(eui_rel, vanilla_rel, dest_name, eui_check_rel=None):
            if self._use_eui_overlay(want_eui, eui_folder, eui_check_rel):
                copy_tmp(eui_rel, dest_name)
            elif vanilla_rel:
                copy_tmp(vanilla_rel, dest_name)

        log_callback("Copying UI files...")

        copy_tmp("ui/AncientRuins/GoodyHutPopup.lua")

        # City banners: EUI has two variants depending on CityBannerProductionBox.
        banner_text = "CityBannerProductionBox = function( city )"
        if self._eui_has_file(eui_folder, "CityBanners/CityBannerManager.lua"):
            if self._eui_file_contains(eui_folder, "CityBanners/CityBannerManager.lua", banner_text):
                copy_tmp("eui/CityBanners/CityBannerManager_1.lua", "CityBannerManager.lua")
                copy_tmp("eui/CityBanners/CityBannerManager_1.xml", "CityBannerManager.xml")
            else:
                copy_tmp("eui/CityBanners/CityBannerManager_2.lua", "CityBannerManager.lua")
                copy_tmp("eui/CityBanners/CityBannerManager_2.xml", "CityBannerManager.xml")
        elif want_eui and eui_folder is None:
            copy_tmp("eui/CityBanners/CityBannerManager_2.lua", "CityBannerManager.lua")
            copy_tmp("eui/CityBanners/CityBannerManager_2.xml", "CityBannerManager.xml")
        else:
            copy_tmp("ui/CityBanners/CityBannerManager.lua")
            copy_tmp("ui/CityBanners/CityBannerManager.xml")

        copy_eui_or_vanilla(
            "eui/CityStatePopup/CityStateDiploPopup.lua",
            "ui/CityStatePopup/CityStateDiploPopup.lua",
            "CityStateDiploPopup.lua",
            "CityStatePopup/CityStateDiploPopup.lua",
        )

        cityview_text = "-- coded by bc1 from 1.0.3.276 brave new world code"
        use_eui_cityview = want_eui and (
            eui_folder is None
            or self._eui_file_contains(eui_folder, "CityView/CityView.lua", cityview_text)
        )
        if use_eui_cityview:
            copy_tmp("eui/CityView/CityView.lua", "CityView.lua")
            copy_tmp("eui/CityView/CityView.xml", "CityView.xml")
            copy_tmp("eui/CityView/CityView_small.xml", "CityView_small.xml")
        else:
            copy_tmp("ui/CityView/CityView.lua", "CityView.lua")
            copy_tmp("ui/CityView/CityView.xml", "CityView.xml")
            copy_tmp("ui/CityView/CityView_small.xml", "CityView_small.xml")

        if not self._use_eui_overlay(want_eui, eui_folder, "CityView/ProductionPopup.lua"):
            copy_tmp("ui/CityView/ProductionPopup.lua")
            copy_tmp("ui/CityView/ProductionPopup.xml")

        if not self._use_eui_overlay(want_eui, eui_folder, "Core/CityStateStatusHelper.lua"):
            copy_tmp("ui/Core/CityStateStatusHelper.lua")
        if self._use_eui_overlay(want_eui, eui_folder, "Core/EUI_tooltip_library.lua"):
            copy_tmp("eui/Core/EUI_tooltip_library.lua")
        if self._use_eui_overlay(want_eui, eui_folder, "Core/EUI_unit_include.lua"):
            copy_tmp("eui/Core/EUI_unit_include.lua")
        copy_tmp("ui/Core/IconSupport.lua")

        copy_tmp("ui/CultureOverview/CultureOverview.lua")
        copy_tmp("ui/CultureOverview/CultureOverview.xml")
        copy_eui_or_vanilla(
            "eui/EconomicGeneralInfo.lua",
            "ui/CultureOverview/EconomicGeneralInfo.lua",
            "EconomicGeneralInfo.lua",
            "EconomicGeneralInfo.lua",
        )
        copy_eui_or_vanilla(
            "eui/EconomicGeneralInfo.xml",
            "ui/CultureOverview/EconomicGeneralInfo.xml",
            "EconomicGeneralInfo.xml",
            "EconomicGeneralInfo.lua",
        )

        copy_tmp("ui/FrontEnd/EULA.lua")
        copy_tmp("ui/FrontEnd/FrontEnd.lua")
        copy_tmp("ui/FrontEnd/MainMenu.lua")
        copy_tmp("ui/FrontEnd/MainMenu.xml")

        copy_eui_or_vanilla(
            "eui/GameSetup/SelectCivilization.lua",
            "ui/GameSetup/SelectCivilization.lua",
            "SelectCivilization.lua",
            "GameSetup/SelectCivilization.lua",
        )

        copy_tmp("ui/GPList/GPList.lua")
        copy_tmp("ui/GPList/GPList.xml")

        copy_eui_or_vanilla(
            "eui/Improvements/WorldView.lua",
            "ui/Improvements/WorldView.lua",
            "WorldView.lua",
            "Improvements/WorldView.lua",
        )
        copy_eui_or_vanilla(
            "eui/LeaderHead/TradeLogic.lua",
            "ui/LeaderHead/TradeLogic.lua",
            "TradeLogic.lua",
            "LeaderHead/TradeLogic.lua",
        )
        copy_tmp("ui/LeaderHead/SimpleDiploTrade.lua")
        copy_tmp("ui/LeaderHead/SimpleDiploTrade.xml")

        for lobby_file in (
            "AdvancedSetup.lua",
            "JoiningRoom.lua",
            "JoiningRoom.xml",
            "LoadMenu.lua",
            "Lobby.lua",
            "Lobby.xml",
            "MPGameOptions.lua",
            "MPGameSetupScreen.lua",
            "MPGameSetupScreen.xml",
            "StagingRoom.lua",
            "StagingRoom.xml",
        ):
            copy_tmp(f"ui/Lobby/{lobby_file}")
        copy_tmp("ui/PlotMouseoverInclude.lua")

        copy_tmp("ui/MiniMapPanel/MiniMapPanel.lua")
        copy_tmp("ui/MiniMapPanel/MiniMapPanel.xml")
        copy_tmp("ui/MPTurnPanel/MPTurnPanel.lua")
        copy_tmp("ui/MPTurnPanel/MPTurnPanel.xml")

        copy_eui_or_vanilla(
            "eui/NotificationPanel/DiploCorner.lua",
            "ui/NotificationPanel/DiploCorner.lua",
            "DiploCorner.lua",
            "NotificationPanel/DiploCorner.xml",
        )
        copy_eui_or_vanilla(
            "eui/NotificationPanel/DiploCorner.xml",
            "ui/NotificationPanel/DiploCorner.xml",
            "DiploCorner.xml",
            "NotificationPanel/DiploCorner.xml",
        )
        copy_eui_or_vanilla(
            "eui/NotificationPanel/NotificationPanel.lua",
            "ui/NotificationPanel/NotificationPanel.lua",
            "NotificationPanel.lua",
            "NotificationPanel/NotificationPanel.lua",
        )
        copy_eui_or_vanilla(
            "eui/NotificationPanel/NotificationPanel.xml",
            "ui/NotificationPanel/NotificationPanel.xml",
            "NotificationPanel.xml",
            "NotificationPanel/NotificationPanel.lua",
        )
        copy_eui_or_vanilla(
            "eui/Options/OptionsMenu.lua",
            "ui/Options/OptionsMenu.lua",
            "OptionsMenu.lua",
            "Options/OptionsMenu.lua",
        )
        copy_eui_or_vanilla(
            "eui/Options/OptionsMenu.xml",
            "ui/Options/OptionsMenu.xml",
            "OptionsMenu.xml",
            "Options/OptionsMenu.lua",
        )

        copy_tmp("ui/Replays/Demographics.lua")
        copy_tmp("ui/Replays/GameMenu.lua")
        copy_tmp("ui/Replays/ReplayViewer.lua")
        copy_tmp("ui/Replays/ReplayViewer.xml")

        if self._use_eui_overlay(want_eui, eui_folder, "TechTree/TechTree.lua"):
            copy_tmp("eui/TechTree/TechPopup.lua")
            copy_tmp("eui/TechTree/TechPopup.xml")
            copy_tmp("eui/TechTree/TechTree.lua")
            copy_tmp("eui/TechTree/TechTree.xml")
        else:
            copy_tmp("ui/TechTree/TechPopup.lua")
            copy_tmp("ui/TechTree/TechTree.lua")
            copy_tmp("ui/TechTree/TechTree.xml")

        copy_eui_or_vanilla(
            "eui/ToolTips/TechButtonInclude.lua",
            "ui/ToolTips/TechButtonInclude.lua",
            "TechButtonInclude.lua",
            "ToolTips/TechButtonInclude.lua",
        )
        copy_eui_or_vanilla(
            "eui/ToolTips/InfoTooltipInclude.lua",
            "ui/ToolTips/InfoTooltipInclude.lua",
            "InfoTooltipInclude.lua",
            "ToolTips/InfoTooltipInclude.lua",
        )
        copy_eui_or_vanilla(
            "eui/UnitFlagManager/UnitFlagManager.lua",
            "ui/UnitFlagManager/UnitFlagManager.lua",
            "UnitFlagManager.lua",
            "UnitFlagManager/UnitFlagManager.lua",
        )
        copy_eui_or_vanilla(
            "eui/UnitFlagManager/UnitFlagManager.xml",
            "ui/UnitFlagManager/UnitFlagManager.xml",
            "UnitFlagManager.xml",
            "UnitFlagManager/UnitFlagManager.lua",
        )

        copy_tmp("ui/UnitList/UnitList.lua")
        copy_tmp("ui/UnitList/UnitList.xml")
        copy_tmp("ui/UnitPanel/EnemyUnitPanel.lua")
        copy_tmp("ui/UnitPanel/EnemyUnitPanel.xml")
        copy_tmp("ui/UnitPanel/EnemyUnitPanel_small.xml")

        unitpanel_text = "-- modified by bc1 from Civ V 1.0.3.276 code"
        use_eui_unitpanel = want_eui and (
            eui_folder is None
            or self._eui_file_contains(eui_folder, "UnitPanel/UnitPanel.lua", unitpanel_text)
        )
        if use_eui_unitpanel:
            copy_tmp("eui/UnitPanel/UnitPanel.lua")
        else:
            copy_tmp("ui/UnitPanel/UnitPanel.lua")

        for voting_file in (
            "CCVotePopup.lua",
            "CCVotePopup.xml",
            "NetworkKickedPopup.lua",
            "ProposalChartPopup.lua",
            "ProposalChartPopup.xml",
            "VictoryProgress.lua",
            "VictoryProgress.xml",
        ):
            copy_tmp(f"ui/VotingSystem/{voting_file}")

        copy_tmp("ui/Bombardment.lua")
        copy_tmp("ui/DiscussionDialog.lua")
        copy_tmp("ui/EndGameMenu.lua")
        copy_tmp("ui/Highlights.xml")
        copy_tmp("ui/InGame.lua")
        copy_tmp("ui/ActionInfoPanel.lua")
        copy_tmp("ui/ActionInfoPanel.xml")
        copy_tmp("ui/prophetreplace.lua")

        copy_eui_or_vanilla(
            "eui/Improvements/YieldIconManager.lua",
            "ui/YieldIconManager.lua",
            "YieldIconManager.lua",
            "Improvements/YieldIconManager.lua",
        )
        if self._use_eui_overlay(want_eui, eui_folder, "TopPanel/TopPanel.lua"):
            copy_tmp("eui/TopPanel.lua")
            copy_tmp("eui/TopPanel.xml")
        else:
            copy_tmp("ui/TopPanel.lua")

        if self._use_eui_overlay(want_eui, eui_folder, "Improvements/SocialPolicyPopup.lua"):
            copy_tmp("eui/Improvements/SocialPolicyPopup.lua")
        else:
            # ui_check.bat still points at ToolTips/; the file lives in Popups/.
            if not self._copy_ui_file(lekmod_path, "ui/Popups/SocialPolicyPopup.lua", ui_dest):
                copy_tmp("ui/ToolTips/SocialPolicyPopup.lua")
            else:
                copied.append("SocialPolicyPopup.lua")

        copy_tmp("ui/Popups/ChooseInternationalTradeRoutePopup.lua")
        copy_tmp("ui/Popups/TradeRouteOverview.lua")
        copy_tmp("ui/ToolTips/TradeRouteHelpers.lua")

        restored = []
        for name, data in preserved.items():
            dest = os.path.join(ui_dest, name)
            if not os.path.exists(dest):
                with open(dest, "wb") as handle:
                    handle.write(data)
                restored.append(name)

        stamp_path = os.path.join(ui_dest, "LekmodUiConfigured.lua")
        with open(stamp_path, "w", encoding="utf-8") as handle:
            handle.write("LekmodUiConfigured = true\n")
        copied.append("LekmodUiConfigured.lua")

        log_callback(f"✓ Configured {len(copied)} UI files in Lua/UI")
        if restored:
            log_callback(f"✓ Preserved {', '.join(restored)}")
        if missing:
            log_callback(f"⚠ Missing UI sources: {', '.join(missing)}")
        
    def find_existing_lekmod_folders(self, civ5_path):
        """Find any existing LEKMOD installations"""
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        
        if not os.path.exists(dlc_path):
            return []
        
        existing_folders = []
        try:
            for item in os.listdir(dlc_path):
                item_path = os.path.join(dlc_path, item)
                if os.path.isdir(item_path) and item.startswith("LEKMOD"):
                    existing_folders.append(item)
        except:
            pass
        
        return existing_folders
    
    def remove_lekmod_folders(self, civ5_path, folders, log_callback):
        """Remove specified LEKMOD folders"""
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        
        for folder in folders:
            folder_path = os.path.join(dlc_path, folder)
            if os.path.exists(folder_path):
                log_callback(f"Removing {folder}...")
                shutil.rmtree(folder_path)
                log_callback(f"✓ Removed {folder}")
    
    def install_mod(self, mod_source, civ5_path, version, log_callback):
        """Install mod to Civ 5 Assets/DLC (for multiplayer compatibility)"""
        # Install to Assets/DLC instead of MODS
        dlc_path = os.path.join(civ5_path, "Assets", "DLC")
        
        # Ensure DLC folder exists
        if not os.path.exists(dlc_path):
            os.makedirs(dlc_path)
            log_callback(f"✓ Created DLC folder: {dlc_path}")
        
        # Destination is Assets/DLC/LEKMOD_v34.10 (uses version number)
        folder_name = f"LEKMOD_{version}"
        dlc_dest = os.path.join(dlc_path, folder_name)
            
        lekmod_source = self._find_lekmod_folder(mod_source)
        if lekmod_source:
            log_callback(f"Found mod folder: {os.path.basename(lekmod_source)}")

        if not lekmod_source or not os.path.exists(lekmod_source):
            raise Exception("LEKMOD source folder not found in downloaded archive!")
        
        # Copy to DLC folder
        log_callback(f"Copying files to {dlc_dest}...")
        shutil.copytree(lekmod_source, dlc_dest)
        log_callback(f"✓ Files copied successfully!")
        
        # Delete ui_check.bat as it's only needed for manual installation
        ui_check_path = os.path.join(dlc_dest, "ui_check.bat")
        if os.path.exists(ui_check_path):
            try:
                os.remove(ui_check_path)
                log_callback(f"✓ Removed ui_check.bat (not needed for installer-based installation)")
            except Exception as e:
                log_callback(f"⚠ Could not remove ui_check.bat: {e}")

    def find_civ5_maps_folder(self):
        """Civ5 custom map scripts live in Documents/My Games/.../Maps."""
        home = Path.home()
        candidates = [
            home / "Documents" / "My Games" / "Sid Meier's Civilization 5" / "Maps",
            home / "OneDrive" / "Documents" / "My Games" / "Sid Meier's Civilization 5" / "Maps",
        ]
        for path in candidates:
            civ5_user = path.parent
            if civ5_user.is_dir() or path.is_dir():
                return str(path)
        return str(candidates[0])

    def get_current_lekmap_version(self):
        """Read installer-written version stamp, or detect existing Lekmap scripts."""
        maps_dir = self.find_civ5_maps_folder()
        version_file = os.path.join(maps_dir, "Lekmap_VERSION.txt")
        if os.path.isfile(version_file):
            try:
                with open(version_file, "r", encoding="utf-8") as handle:
                    line = handle.readline().strip()
                    if line:
                        return line
            except Exception:
                pass

        if os.path.isdir(maps_dir):
            try:
                for name in os.listdir(maps_dir):
                    lower = name.lower()
                    if lower.startswith("lekmap") and lower.endswith(".lua"):
                        return "installed"
            except Exception:
                pass
        return None

    def install_lekmap(self, extract_path, version, log_callback):
        """Copy map scripts from an extracted zip into the Civ5 Maps folder."""
        maps_dir = self.find_civ5_maps_folder()
        os.makedirs(maps_dir, exist_ok=True)
        log_callback(f"Installing map files to {maps_dir}...")

        skip_dirs = {"__macosx", ".git", ".svn", "lekmod_dll"}
        copied = 0
        seen = {}
        for root, dirs, files in os.walk(extract_path):
            dirs[:] = [d for d in dirs if d.lower() not in skip_dirs and not d.startswith(".")]
            for name in files:
                ext = os.path.splitext(name)[1].lower()
                if ext not in (".lua", ".xml"):
                    continue
                if name.lower() in ("lekmap_version.txt",):
                    continue
                source = os.path.join(root, name)
                dest = os.path.join(maps_dir, name)
                if name in seen:
                    log_callback(f"⚠ Duplicate {name}; using later copy")
                shutil.copy2(source, dest)
                seen[name] = source
                copied += 1

        if copied == 0:
            raise Exception("No .lua / .xml map files found in the Lekmap archive.")

        version_path = os.path.join(maps_dir, "Lekmap_VERSION.txt")
        with open(version_path, "w", encoding="utf-8") as handle:
            handle.write(str(version).strip() + "\n")

        log_callback(f"✓ Copied {copied} map file(s)")
        return maps_dir

