/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

struct GameTurnInfo
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;
};

struct OrderData
{
	OrderData()
		: eOrderType(NO_ORDER)
		, iData1(0)
		, iData2(0)
		, bSave(false)
		, bRush(false)
	{
	}

	OrderTypes eOrderType;
	int iData1;
	int iData2;
	bool bSave;
	bool bRush;
};

//FDataStream & operator<<(FDataStream &, const OrderData &);
//FDataStream & operator>>(FDataStream &, OrderData &);

struct MissionData
{
	MissionData()
		: eMissionType(NO_MISSION)
		, iData1(0)
		, iData2(0)
		, iFlags(0)
		, iPushTurn(0)
	{
	}

	MissionTypes eMissionType;
	int iData1;
	int iData2;
	int iFlags;
	int iPushTurn;
};

//FDataStream & operator<<(FDataStream &, const MissionData &);
//FDataStream & operator>>(FDataStream &, MissionData &);

struct VoteSelectionSubData
{
	VoteSelectionSubData()
		: eVote(NO_VOTE)
		, ePlayer(NO_PLAYER)
		, iCityId(-1)
		, eOtherPlayer(NO_PLAYER)
		, strText("")
	{
	}

	VoteTypes eVote;
	PlayerTypes ePlayer;
	int iCityId;
	PlayerTypes eOtherPlayer;
	CvString strText;
};

struct VoteSelectionData
{
	VoteSelectionData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, aVoteOptions()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	std::vector<VoteSelectionSubData> aVoteOptions;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteSelectionData&);
FDataStream& operator>>(FDataStream&, VoteSelectionData&);

struct VoteTriggeredData
{
	VoteTriggeredData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
		, kVoteOption()
	{
	}

	int iId;
	VoteSourceTypes eVoteSource;
	VoteSelectionSubData kVoteOption;

	int  GetID() const;
	void SetID(int iID);
	void read(FDataStream& kStream);
	void write(FDataStream& kStream) const;
};

FDataStream& operator<<(FDataStream&, const VoteTriggeredData&);
FDataStream& operator>>(FDataStream&, VoteTriggeredData&);

struct PlotExtraYield
{
	PlotExtraYield()
		: m_iX(-1)
		, m_iY(-1)
		, m_aeExtraYield()
	{
	}

	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;
};
FDataStream& operator<<(FDataStream&, const PlotExtraYield&);
FDataStream& operator>>(FDataStream&, PlotExtraYield&);

struct PlotExtraCost
{
	PlotExtraCost()
		: m_iX(-1)
		, m_iY(-1)
		, m_iCost(0)
	{
	}

	int m_iX;
	int m_iY;
	int m_iCost;
};
FDataStream& operator<<(FDataStream&, const PlotExtraCost&);
FDataStream& operator>>(FDataStream&, PlotExtraCost&);

struct BuildingYieldChange
{
	BuildingYieldChange()
		: eBuildingClass(NO_BUILDINGCLASS)
		, eYield(NO_YIELD)
		, iChange(0)
	{
	}

	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;
};

FDataStream& operator<<(FDataStream&, const BuildingYieldChange&);
FDataStream& operator>>(FDataStream&, BuildingYieldChange&);

struct BuildingGreatWork
{
	BuildingGreatWork()
		: eBuildingClass(NO_BUILDINGCLASS)
#ifdef AUI_WARNING_FIXES
		, iSlot(MAX_UNSIGNED_INT)
#else
		, iSlot(-1)
#endif
		, iGreatWorkIndex(-1)
	{
	}

	BuildingClassTypes eBuildingClass;
#ifdef AUI_WARNING_FIXES
	uint iSlot;
#else
	int iSlot;
#endif
	int iGreatWorkIndex;
};

FDataStream& operator<<(FDataStream&, const BuildingGreatWork&);
FDataStream& operator>>(FDataStream&, BuildingGreatWork&);

// PITBOSS related structures
//struct PBGameSetupData
//{
//	PBGameSetupData();
//
//	int iSize;
//	int iClimate;
//	int iSeaLevel;
//	int iSpeed;
//	int iEra;
//
//	int iMaxTurns;
//	int iCityElimination;
//	int iAdvancedStartPoints;
//	int iTurnTime;
//
//	int iNumCustomMapOptions;
//	int * aiCustomMapOptions;
//	int getCustomMapOption(int iOption) {return aiCustomMapOptions[iOption];}
//
//	int iNumVictories;
//	bool * abVictories;
//	bool getVictory(int iVictory) {return abVictories[iVictory];}
//
//	std::string szMapName;
//	std::string getMapName() {return szMapName;}
//
//	std::vector<bool> abOptions;
//	bool getOptionAt(int iOption) {return abOptions[iOption];}
//
//	std::vector<bool> abMPOptions;
//	bool getMPOptionAt(int iOption) {return abMPOptions[iOption];}
//};
//
//struct PBPlayerSetupData
//{
//	PBPlayerSetupData() :
//	iWho(0)
//	, iCiv(0)
//	, iLeader(0)
//	, iTeam(0)
//	, iDifficulty(0)
//	{
//	}
//
//	int iWho;
//	int iCiv;
//	int iLeader;
//	int iTeam;
//	int iDifficulty;
//
//	std::string szStatusText;
//	std::string getStatusText() {return szStatusText;}
//};
//
//struct PBPlayerAdminData
//{
//	PBPlayerAdminData() :
//	szName("")
//	, szPing("")
//	, szScore("")
//	, bHuman(false)
//	, bClaimed(false)
//	, bTurnActive(false)
//	{
//	}
//
//	std::string szName;
//	std::string getName() {return szName;}
//	std::string szPing;
//	std::string getPing() {return szPing;}
//	std::string szScore;
//	std::string getScore() {return szScore;}
//	bool bHuman;
//	bool bClaimed;
//	bool bTurnActive;
//};

class CvUnit;
class CvPlot;
class CvCity;

void checkBattleUnitType(BattleUnitTypes unitType);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvCombatInfo
//!  \brief		Information necessary for combat.
//!             If you add any data members to this class, make sure to update
//!             the copy constructor.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCombatInfo
{
public:
	CvCombatInfo();
	CvCombatInfo& operator=(const CvCombatInfo&);

	CvUnit* getUnit(BattleUnitTypes unitType) const { return m_pUnits[unitType]; };
	void setUnit(BattleUnitTypes unitType, CvUnit* unit) { m_pUnits[unitType] = unit; };

	CvCity* getCity(BattleUnitTypes unitType) const { return m_pCities[unitType]; };
	void setCity(BattleUnitTypes unitType, CvCity* pkCity) { m_pCities[unitType] = pkCity; };

	const CvCombatMemberEntry* getCombatMember(BattleUnitTypes unitType) const;

	CvPlot* getPlot() const { return m_pTargetPlot; };
	void setPlot(CvPlot* plot) { m_pTargetPlot = plot; };

	// Optional override for the attacker's origin plot (river-crossing/elevation/amphib checks). Falls back to the attacker's live plot() if NULL, unless getUseLiveOriginPlot() is false.
	CvPlot* getFromPlot() const { return m_pFromPlot; };
	void setFromPlot(CvPlot* plot) { m_pFromPlot = plot; };

	// When getFromPlot() is NULL, controls whether the attacker's live plot() is used as a fallback (true, the
	// default, matches original behavior for real combat/AI callers that never touch fromPlot at all) or whether
	// origin-plot-dependent checks (river crossing, amphibious, elevation) are skipped entirely (false -- for
	// preview callers that explicitly tried to resolve a hypothetical origin and couldn't, e.g. a target that
	// can't be reached this turn, where checking against the attacker's unrelated current position is meaningless).
	bool getUseLiveOriginPlot() const { return m_bUseLiveOriginPlot; };
	void setUseLiveOriginPlot(bool bUse) { m_bUseLiveOriginPlot = bUse; };

	bool getAttackerAdvances() const { return m_bAttackerAdvances; };
	void setAttackerAdvances(bool bAdvance) { m_bAttackerAdvances = bAdvance; };

	bool getDefenderRetaliates() const { return m_bDefenderRetaliates; };
	void setDefenderRetaliates(bool bRetaliate) { m_bDefenderRetaliates = bRetaliate; };

	bool getAttackIsRanged() const { return m_bAttackIsRanged; };
	void setAttackIsRanged(bool bRanged) { m_bAttackIsRanged = bRanged; };

	bool getAttackIsBombingMission() const { return m_bAttackIsBombingMission; };
	void setAttackIsBombingMission(bool bBombingMission) { m_bAttackIsBombingMission = bBombingMission; };

	bool getAttackIsAirSweep() const { return m_bAttackIsAirSweep; };
	void setAttackIsAirSweep(bool bAirSweep) { m_bAttackIsAirSweep = bAirSweep; };

	bool getDefenderCaptured() const { return m_bDefenderCaptured; };
	void setDefenderCaptured(bool bDefenderCaptured) { m_bDefenderCaptured = bDefenderCaptured; };

	bool getAttackIsNuclear() const { return m_iNuclearDamageLevel > 0; };
	int getAttackNuclearLevel() const { return m_iNuclearDamageLevel; };
	void setAttackNuclearLevel(int iNuclearDamageLevel) { m_iNuclearDamageLevel = iNuclearDamageLevel; };

	int getDamageInflicted(BattleUnitTypes unitType) const { return m_iDamageInflicted[unitType]; };
	void setDamageInflicted(BattleUnitTypes unitType, int iDamage) { m_iDamageInflicted[unitType] = iDamage; };

	int getFinalDamage(BattleUnitTypes unitType) const { return m_iFinalDamage[unitType]; };
	void setFinalDamage(BattleUnitTypes unitType, int iFinalDamage) { m_iFinalDamage[unitType] = iFinalDamage; };

	int getFearDamageInflicted(BattleUnitTypes unitType) const { return m_iFearDamageInflicted[unitType]; };
	void setFearDamageInflicted(BattleUnitTypes unitType, int iDamage) { m_iFearDamageInflicted[unitType] = iDamage; };

	int getExperience(BattleUnitTypes unitType) const { return m_iExperienceChange[unitType]; };
	void setExperience(BattleUnitTypes unitType, int iExperience) { m_iExperienceChange[unitType] = iExperience; };

	int getMaxExperienceAllowed(BattleUnitTypes unitType) const { return m_iMaxExperienceAllowed[unitType]; };
	void setMaxExperienceAllowed(BattleUnitTypes unitType, int iMaxExperience) { m_iMaxExperienceAllowed[unitType] = iMaxExperience; };

	bool getInBorders(BattleUnitTypes unitType) const { return m_bInBorders[unitType]; };
	void setInBorders(BattleUnitTypes unitType, bool bInBorders) { m_bInBorders[unitType] = bInBorders; };

	bool getUpdateGlobal(BattleUnitTypes unitType) const { return m_bUpdateGlobal[unitType]; };
	void setUpdateGlobal(BattleUnitTypes unitType, bool bUpdateGlobal) { m_bUpdateGlobal[unitType] = bUpdateGlobal; };

	bool getVisualizeCombat() const { return m_bVisualize; };
	void setVisualizeCombat(bool bVisualize) { m_bVisualize = bVisualize; };

	bool getAttackerAdvancedVisualization() const { return m_bAttackedAdvancedVis; };
	void setAttackerAdvancedVisualization(bool bAdvance) { m_bAttackedAdvancedVis = bAdvance; };

	bool IsCombatPrediction() const { return m_bCombatPrediction; };
	void setCombatPrediction(bool bCombatPrediction) { m_bCombatPrediction = bCombatPrediction; };

	int getExtraDamageTaken(BattleUnitTypes unitType) const { return m_iExtraDamageTaken[unitType]; };
	void setExtraDamageTaken(BattleUnitTypes unitType, int iDamage) { m_iExtraDamageTaken[unitType] = iDamage; };

	int getCombatSeed(BattleUnitTypes unitType) const { return m_iCombatSeed[unitType]; };
	void setCombatSeed(BattleUnitTypes unitType, int iSeed) { m_iCombatSeed[unitType] = iSeed; };
	// Helpers for combat
	// Perform the Random Roll for this combat
	void doRandomness(BattleUnitTypes unitType, int iWoundedRatio);
	// Perform the Strength Ratio calculation for this combat
	double doStrengthRatio(int strength, int opponentStrength);
	// Perform the Experience calculation for this combat
	void doExperience();

	// Helpers
	bool IsCombatRandom() const;
	bool IsAttackerDead() const;
	bool IsDefenderDead() const;


	CvCombatMemberEntry* getDamageMembers() { return &m_kDamageMembers[0]; }
	const CvCombatMemberEntry* getDamageMembers() const { return &m_kDamageMembers[0]; }
	int getDamageMemberCount() const { return m_iDamageMemberCount; }
	int getMaxDamageMemberCount() const { return MAX_DAMAGE_MEMBER_COUNT; }
	void setDamageMemberCount(int iDamageMemberCount) { m_iDamageMemberCount = std::min(iDamageMemberCount, (int)MAX_DAMAGE_MEMBER_COUNT); }

protected:
	CvUnit* 	m_pUnits[BATTLE_UNIT_COUNT];					//!< The units involved
	CvCity* 	m_pCities[BATTLE_UNIT_COUNT];					//!< The cities involved

	CvPlot* 	m_pTargetPlot;									//!< The plot that the attacker is attacking
	CvPlot* 	m_pFromPlot;									//!< Optional override for the attacker's origin plot; NULL means use the attacker's live plot() unless m_bUseLiveOriginPlot is false
	bool		m_bUseLiveOriginPlot;							//!< When m_pFromPlot is NULL, whether to fall back to the attacker's live plot() (true) or skip origin-plot checks entirely (false)
	bool		m_bAttackerAdvances;							//!< Should the attacker advance?
	bool		m_bAttackIsRanged;								//!< Attack is ranged
	bool		m_bAttackIsBombingMission;						//!< Attack is a bombing mission by an airplane
	bool		m_bAttackIsAirSweep;							//!< Attack is a Air Sweep by an Fighter
	bool		m_bDefenderRetaliates;							//!< Defender does not retaliate (usually set with ranged attacks)
	bool		m_bDefenderCaptured;							//!< The defender is captured, not killed.

	int			m_iNuclearDamageLevel;							//!< If > 0, the attack is a nuclear attack of the specified damage level

	int			m_iFinalDamage[BATTLE_UNIT_COUNT];				//!< The units final damage value
	int			m_iDamageInflicted[BATTLE_UNIT_COUNT];			//!< How much damage this unit inflicts on the opponent
	int			m_iFearDamageInflicted[BATTLE_UNIT_COUNT];		//!< How much fear damage this unit inflicts on the opponent

	int			m_iExperienceChange[BATTLE_UNIT_COUNT];			//!< How much experience does this unit get from battle
	int			m_iMaxExperienceAllowed[BATTLE_UNIT_COUNT];		//!< Maximum experience this unit is allowed
	bool		m_bInBorders[BATTLE_UNIT_COUNT];				//!< Was this unit within its civilization borders
	bool		m_bUpdateGlobal[BATTLE_UNIT_COUNT];				//!< Should we update the global information

	bool		m_bVisualize;									//!< The combat should be visualized

	bool		m_bAttackedAdvancedVis;							//!< If true, the attacker has already advanced its visualization of the unit (happened during combat sim).
	bool		m_bCombatPrediction;							//!< If true, this combat info is being used for combat prediction and not actual combat.
	int 		m_iExtraDamageTaken[BATTLE_UNIT_COUNT];			//!< Used for Predicting RangedSupportFire, as thats a totally separate combat instance.
	int			m_iCombatSeed[BATTLE_UNIT_COUNT];				//!< The random seed for this combat

	CvCombatMemberEntry	m_kCombatMembers[BATTLE_UNIT_COUNT];
	// Units/cities damaged in the attack.  0 for most attacks that have just the normal defenders.  Primarily used with area attacks such as the various nuclear attacks.
	// If this is non-zero, the damage members will include the primary defenders so don't apply the damage twice!
	int			m_iDamageMemberCount;
	CvCombatMemberEntry	m_kDamageMembers[MAX_DAMAGE_MEMBER_COUNT];
};
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
struct CvCombatDamageRange
{
	int iMin;
	int iMax;
	int iAverage;
	CvCombatDamageRange() : iMin(0), iMax(0), iAverage(0) {}
};
struct CvCombatModifierEntry
{
	CvCombatModifierEntry()
		: m_iModifier(0)
		, m_bPercent(true)
		, m_bMiscellaneous(false)
	{
	}

	CvCombatModifierEntry(const CvString& strText, int iModifier, bool bPercent = true, bool bMiscellaneous = false)
		: m_strText(strText)
		, m_iModifier(iModifier)
		, m_bPercent(bPercent)
		, m_bMiscellaneous(bMiscellaneous)
	{
	}

	CvString m_strText;
	int m_iModifier;
	bool m_bPercent;
	bool m_bMiscellaneous;
};

struct CvCombatModifierList
{
	CvCombatModifierList(int iMaxLines = 0)
		: iMaxLines(iMaxLines)
		, iMiscModifier(0)
		, iMiscCount(0)
		, bAttackerSide(false)
	{
	}

	void AddEntry(const CvString& strText, int iModifier, bool bPercent = true);
	void RebuildMiscellaneous();

	std::vector<CvCombatModifierEntry> m_kEntries;

	int iMaxLines;
	int iMiscModifier;
	int iMiscCount;
	bool bAttackerSide;
};
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMissionDefinition
//!  \brief		Base mission definition struct
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionDefinition
{
public:
	CvMissionDefinition();

	MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	float getMissionTime() const;
	void setMissionTime(float time);

	CvUnit* getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit* unit);

	const CvPlot* getPlot() const;
	void setPlot(const CvPlot* plot);

	const CvPlot* getSecondaryPlot() const;
	void setSecondaryPlot(const CvPlot* plot);

protected:
	MissionTypes		m_eMissionType;					//!< The type of event
	CvUnit*				m_aUnits[BATTLE_UNIT_COUNT];	//!< The units involved
	float				m_fMissionTime;					//!< The amount of time that the event will take
	const CvPlot*		m_pPlot;						//!< The plot associated with the event
	const CvPlot*		m_pSecondaryPlot;				//!< The other plot (rarely) associated with the event
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAirMissionDefinition
//!  \brief		A definition passed to CvAirMissionManager to start an air mission
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAirMissionDefinition : public CvMissionDefinition
{
public:
	CvAirMissionDefinition();
	CvAirMissionDefinition(const CvAirMissionDefinition& kCopy);

	int getDamage(BattleUnitTypes unitType) const;
	void setDamage(BattleUnitTypes unitType, int damage);
	bool isDead(BattleUnitTypes unitType) const;

private:
	int	m_aDamage[BATTLE_UNIT_COUNT];		//!< The ending damage of the units
};
#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
struct GoldenAgeYieldThreshold
{
	GoldenAgeYieldThreshold()
		: m_eThresholdYield(NO_YIELD)
		, m_iThresholdAmount(0)
		, m_eRwdYield(NO_YIELD)
		, m_iRwdAmount(0)
	{
	}
	YieldTypes m_eThresholdYield;
	int m_iThresholdAmount;
	YieldTypes m_eRwdYield;
	int m_iRwdAmount;
};

FDataStream& operator<<(FDataStream&, const GoldenAgeYieldThreshold&);
FDataStream& operator>>(FDataStream&, GoldenAgeYieldThreshold&);
#endif
#endif	// CVSTRUCTS_H
