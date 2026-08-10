/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

//! \author		Multiple
//! \brief		Implementation of basic Civ5 structures

#include "CvGameCoreDLLPCH.h"
#include "CvUnit.h"
//#include "CvStructs.h"
#include "CvGameCoreEnumSerialization.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"
#include "CvTypes.h"

// include this after all other headers!
#include "LintFree.h"

int VoteSelectionData::GetID() const
{
	return iId;
}

void VoteSelectionData::SetID(int iID)
{
	iId = iID;
}

void VoteSelectionData::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> iId;
	kStream >> eVoteSource;
	size_t iSize;
	kStream >> iSize;
	size_t i = 0;
	for(i = 0; i < iSize; ++i)
	{
		VoteSelectionSubData kData;
		kStream >> kData.eVote;
		kStream >> kData.ePlayer;
		kStream >> kData.iCityId;
		kStream >> kData.eOtherPlayer;
		kStream >> kData.strText;
		aVoteOptions.push_back(kData);
	}
}
FDataStream& operator>>(FDataStream& loadFrom, VoteSelectionData& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

void VoteSelectionData::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << iId;
	kStream << eVoteSource;
	kStream << aVoteOptions.size();
	for(std::vector<VoteSelectionSubData>::const_iterator it = aVoteOptions.begin(); it != aVoteOptions.end(); ++it)
	{
		kStream << (*it).eVote;
		kStream << (*it).ePlayer;
		kStream << (*it).iCityId;
		kStream << (*it).eOtherPlayer;
		kStream << (*it).strText;
	}
}
FDataStream& operator<<(FDataStream& saveTo, const VoteSelectionData& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

int VoteTriggeredData::GetID() const
{
	return iId;
}

void VoteTriggeredData::SetID(int iID)
{
	iId = iID;
}

void VoteTriggeredData::read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> iId;
	kStream >> eVoteSource;
	kStream >> kVoteOption.eVote;
	kStream >> kVoteOption.ePlayer;
	kStream >> kVoteOption.iCityId;
	kStream >> kVoteOption.eOtherPlayer;
	kStream >> kVoteOption.strText;
}
FDataStream& operator>>(FDataStream& loadFrom, VoteTriggeredData& writeTo)
{
	writeTo.read(loadFrom);
	return loadFrom;
}

void VoteTriggeredData::write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << iId;
	kStream << eVoteSource;
	kStream << kVoteOption.eVote;
	kStream << kVoteOption.ePlayer;
	kStream << kVoteOption.iCityId;
	kStream << kVoteOption.eOtherPlayer;
	kStream << kVoteOption.strText;
}
FDataStream& operator<<(FDataStream& saveTo, const VoteTriggeredData& readFrom)
{
	readFrom.write(saveTo);
	return saveTo;
}

FDataStream& operator>>(FDataStream& kStream, BuildingYieldChange& writeTo)
{
	kStream >> writeTo.eBuildingClass;
	kStream >> writeTo.eYield;
	kStream >> writeTo.iChange;
	return kStream;
}

FDataStream& operator<<(FDataStream& kStream, const BuildingYieldChange& readFrom)
{
	kStream << readFrom.eBuildingClass;
	kStream << readFrom.eYield;
	kStream << readFrom.iChange;
	return kStream;
}

FDataStream& operator>>(FDataStream& kStream, BuildingGreatWork& writeTo)
{
	kStream >> writeTo.eBuildingClass;
	kStream >> writeTo.iSlot;
	kStream >> writeTo.iGreatWorkIndex;
	return kStream;
}

FDataStream& operator<<(FDataStream& kStream, const BuildingGreatWork& readFrom)
{
	kStream << readFrom.eBuildingClass;
	kStream << readFrom.iSlot;
	kStream << readFrom.iGreatWorkIndex;
	return kStream;
}

#ifdef CVASSERT_ENABLE
void checkBattleUnitType(BattleUnitTypes unitType)
{
	DEBUG_VARIABLE(unitType);
	CvAssertMsg((unitType >= 0) && (unitType < BATTLE_UNIT_COUNT), "Invalid battle unit type.");
#else
void checkBattleUnitType(BattleUnitTypes /*unitType*/)
{
#endif
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvCombatInfo::CvCombatInfo
//! \brief      Constructor
//------------------------------------------------------------------------------------------------
CvCombatInfo::CvCombatInfo() :
	m_pTargetPlot(NULL),
	m_pFromPlot(NULL),
	m_bAttackerAdvances(false),
	m_bAttackIsRanged(false),
	m_bAttackIsBombingMission(false),
	m_bAttackIsAirSweep(false),
	m_bDefenderRetaliates(true),
	m_bDefenderCaptured(false),
	m_iNuclearDamageLevel(0),
	m_bVisualize(false),
	m_bAttackedAdvancedVis(false),
	m_iDamageMemberCount(0),
	m_bCombatPrediction(false)
{
	for(int i = 0; i < BATTLE_UNIT_COUNT; i++)
	{
		m_pUnits[i] = NULL;
		m_pCities[i] = NULL;
		m_iFinalDamage[i] = 0;
		m_iDamageInflicted[i] = 0;
		m_iFearDamageInflicted[i] = 0;
		m_iExperienceChange[i] = 0;
		m_iMaxExperienceAllowed[i] = 0;
		m_bInBorders[i] = false;
		m_bUpdateGlobal[i] = false;
		m_iExtraDamageTaken[i] = 0;
		m_iCombatSeed[i] = -1;
	}
}

CvCombatInfo& CvCombatInfo::operator=(const CvCombatInfo& rhs)
{
	m_pTargetPlot = rhs.m_pTargetPlot;
	m_pFromPlot = rhs.m_pFromPlot;
	m_bAttackerAdvances = rhs.m_bAttackerAdvances;
	m_bAttackIsRanged = rhs.m_bAttackIsRanged;
	m_bAttackIsBombingMission = rhs.m_bAttackIsBombingMission;
	m_bAttackIsAirSweep = rhs.m_bAttackIsAirSweep;
	m_bDefenderCaptured = rhs.m_bDefenderCaptured;
	m_bDefenderRetaliates = rhs.m_bDefenderRetaliates;
	m_iNuclearDamageLevel = rhs.m_iNuclearDamageLevel;
	m_bVisualize = rhs.m_bVisualize;
	m_bAttackedAdvancedVis = rhs.m_bAttackedAdvancedVis;
	m_bCombatPrediction = rhs.m_bCombatPrediction;

	for(int i = 0; i < BATTLE_UNIT_COUNT; i++)
	{
		m_pUnits[i] = rhs.m_pUnits[i];
		m_pCities[i] = rhs.m_pCities[i];

		m_iFinalDamage[i] = rhs.m_iFinalDamage[i];
		m_iDamageInflicted[i] = rhs.m_iDamageInflicted[i];
		m_iFearDamageInflicted[i] = rhs.m_iFearDamageInflicted[i];

		m_iExperienceChange[i] = rhs.m_iExperienceChange[i];
		m_iMaxExperienceAllowed[i] = rhs.m_iMaxExperienceAllowed[i];
		m_bInBorders[i] = rhs.m_bInBorders[i];
		m_bUpdateGlobal[i] = rhs.m_bUpdateGlobal[i];
		m_iExtraDamageTaken[i] = rhs.m_iExtraDamageTaken[i];
		m_iCombatSeed[i] = rhs.m_iCombatSeed[i];
		m_kCombatMembers[i] = rhs.m_kCombatMembers[i];
	}

	m_iDamageMemberCount = rhs.m_iDamageMemberCount;
	for(int i=0; i<m_iDamageMemberCount; ++i)
	{
		m_kDamageMembers[i] = rhs.m_kDamageMembers[i];
	}
	return (*this);
}

const CvCombatMemberEntry* CvCombatInfo::getCombatMember(BattleUnitTypes unitType) const
{
	if((int)unitType < (int)BATTLE_UNIT_COUNT)
		return &m_kCombatMembers[unitType];
	return NULL;
}
// Perform the Random Roll for this combat
void CvCombatInfo::doRandomness(BattleUnitTypes eUnitType, int iWoundedRatio)
{
	if (getCombatSeed(eUnitType) != -1)
		return; // if we haven't set the combat seed yet, don't do randomness yet.  This is for set seeds.
	const char* szLog = "Combat Damage Roll";
	// this is bad, but theres like 4 global values that all equal this for combat so fuck it.
	int iExtraDamage = 1200;

	switch (eUnitType)
	{
	case BATTLE_UNIT_ATTACKER:
		szLog = "Attacker Damage Roll";
		break;

	case BATTLE_UNIT_DEFENDER:
		szLog = "Defender Damage Roll";
		break;

	case BATTLE_UNIT_INTERCEPTOR:
		szLog = "Interceptor Damage Roll";
		break;

	default:
		CvAssertMsg(false, "Invalid BattleUnitTypes value");
		return;
	}
	int iRoll = 0;
	if (IsCombatRandom())
	{
		iRoll = GC.getGame().getJonRandNum(iExtraDamage, szLog);
		iRoll *= iWoundedRatio;
		iRoll /= 100;
	}
	else
	{
		iRoll = iExtraDamage;
		iRoll -= 1;
		iRoll *= iWoundedRatio;
		iRoll /= 100;
		iRoll /= 2;
	}
	setCombatSeed(eUnitType, iRoll);

}
// Perform the Strength Ratio calculation for this combat
double CvCombatInfo::doStrengthRatio(int strength, int opponentStrength)
{
	const double max = 10.0f;
	double ratio = (opponentStrength > 0) ? static_cast<double>(strength) / static_cast<double>(opponentStrength) : static_cast<double>(strength);
	if (opponentStrength > strength)
	{
		ratio = (strength > 0) ? static_cast<double>(opponentStrength) / static_cast<double>(strength) : static_cast<double>(opponentStrength);
	}
	if (ratio > max)
	{
		ratio = max;
	}
	ratio = (ratio + 3.0) / 4.0;
	ratio = pow(ratio, 4.0);
	ratio = (ratio + 1.0) / 2.0;
	if (opponentStrength > strength)
	{
		ratio = 1.0 / ratio;
	}
	return ratio;
}
// Perform the Experience calculation for this combat
void CvCombatInfo::doExperience()
{
	for (int i = 0; i < BATTLE_UNIT_COUNT; ++i)
	{
		BattleUnitTypes unit = static_cast<BattleUnitTypes>(i);
		setExperience(unit, 0);
		setMaxExperienceAllowed(unit, 0);
		setInBorders(unit, false);
		setUpdateGlobal(unit, false);
	}

	CvPlot* pPlot = getPlot();
	if (pPlot == NULL || getAttackIsNuclear())
		return;

	CvUnit* pAttackerUnit = getUnit(BATTLE_UNIT_ATTACKER);
	CvCity* pAttackerCity = getCity(BATTLE_UNIT_ATTACKER);
	CvUnit* pDefenderUnit = getUnit(BATTLE_UNIT_DEFENDER);
	CvCity* pDefenderCity = getCity(BATTLE_UNIT_DEFENDER);
	CvUnit* pInterceptor = getUnit(BATTLE_UNIT_INTERCEPTOR);

	const bool bIsMelee = !getAttackIsAirSweep() && !getAttackIsBombingMission() && !getAttackIsRanged();

	int iAttackerExperience = 0;
	int iDefenderExperience = 0;
	int iInterceptorExperience = 0;

	// Air sweep: interceptor is stored as the defender.
	if (getAttackIsAirSweep())
	{
		if (pAttackerUnit != NULL &&
			pDefenderUnit != NULL)
		{
			if (pDefenderUnit->getDomainType() != DOMAIN_AIR)
			{
				iAttackerExperience = 0; // Air sweeps against ground units do not give XP to the attacker.
				iDefenderExperience = GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND();
			}
			else
			{
				iAttackerExperience = GC.getEXPERIENCE_ATTACKING_AIR_SWEEP();
				iDefenderExperience = GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_AIR();
			}
		}
	}
	// Bombing mission.
	else if (getAttackIsBombingMission())
	{
		if (pAttackerUnit != NULL)
		{
			if (pDefenderUnit != NULL)
			{
				iAttackerExperience = GC.getEXPERIENCE_ATTACKING_UNIT_AIR();
				iDefenderExperience = GC.getEXPERIENCE_DEFENDING_UNIT_AIR();
			}
			else if (pDefenderCity != NULL)
			{
				iAttackerExperience = GC.getEXPERIENCE_ATTACKING_CITY_AIR();
			}
		}

		if (pInterceptor != NULL)
		{
			iInterceptorExperience = GC.getEXPERIENCE_DEFENDING_AIR_SWEEP_GROUND();
		}
	}
	// Normal ranged strike.
	else if (getAttackIsRanged())
	{
		if (pAttackerUnit != NULL)
		{
			if (pDefenderUnit != NULL)
			{
				iAttackerExperience = GC.getEXPERIENCE_ATTACKING_UNIT_RANGED();
			}
			else if (pDefenderCity != NULL)
			{
				iAttackerExperience = GC.getEXPERIENCE_ATTACKING_CITY_RANGED();
			}
		}

		if (pDefenderUnit != NULL)
		{
			iDefenderExperience = GC.getEXPERIENCE_DEFENDING_UNIT_RANGED();
		}
	}
	else if (pAttackerUnit != NULL) // Melee.
	{
		if (pDefenderUnit != NULL)
		{
			iAttackerExperience = GC.getEXPERIENCE_ATTACKING_UNIT_MELEE();
			iDefenderExperience = GC.getEXPERIENCE_DEFENDING_UNIT_MELEE();
		}
		else if (pDefenderCity != NULL)
		{
			iAttackerExperience = GC.getEXPERIENCE_ATTACKING_CITY_MELEE();
		}
	}

	setExperience(BATTLE_UNIT_ATTACKER, iAttackerExperience);
	setExperience(BATTLE_UNIT_DEFENDER, iDefenderExperience);
	setExperience(BATTLE_UNIT_INTERCEPTOR, iInterceptorExperience);

	// Attacker metadata.
	if (pAttackerUnit != NULL)
	{
		if (pDefenderUnit != NULL)
		{
			setInBorders(BATTLE_UNIT_ATTACKER, pPlot->getOwner() == (bIsMelee ? pAttackerUnit->getOwner() : pDefenderUnit->getOwner()));
			setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, pDefenderUnit->maxXPValue());
			setUpdateGlobal(BATTLE_UNIT_ATTACKER, pDefenderUnit->canEarnGlobalXP());
		}
		else if (pDefenderCity != NULL)
		{
			setInBorders(BATTLE_UNIT_ATTACKER, pPlot->getOwner() == pDefenderCity->getOwner());
			setMaxExperienceAllowed(BATTLE_UNIT_ATTACKER, pDefenderCity->getMaxXPValue());
			setUpdateGlobal(BATTLE_UNIT_ATTACKER, pDefenderCity->canEarnGlobalXP());
		}

		// No General Progress for air sweeps against ground units.
		if (getAttackIsAirSweep() && pDefenderUnit != NULL && pDefenderUnit->getDomainType() != DOMAIN_AIR )
		{
			setUpdateGlobal(BATTLE_UNIT_ATTACKER, false);
		}
	}

	// Defender metadata.
	if (pDefenderUnit != NULL)
	{
		if (pAttackerUnit != NULL)
		{
			setInBorders(BATTLE_UNIT_DEFENDER, pPlot->getOwner() == (bIsMelee ? pDefenderUnit->getOwner() : pAttackerUnit->getOwner()));
			setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, pAttackerUnit->maxXPValue());
			setUpdateGlobal(BATTLE_UNIT_DEFENDER, pAttackerUnit->canEarnGlobalXP());
		}
		else if (pAttackerCity != NULL)
		{
			setInBorders(BATTLE_UNIT_DEFENDER, pPlot->getOwner() == pAttackerCity->getOwner());
			setMaxExperienceAllowed(BATTLE_UNIT_DEFENDER, pAttackerCity->getMaxXPValue());
			setUpdateGlobal(BATTLE_UNIT_DEFENDER, pAttackerCity->canEarnGlobalXP());
		}
	}

	// Bombing interceptor metadata.
	if (pInterceptor != NULL && pAttackerUnit != NULL )
	{
		setMaxExperienceAllowed(BATTLE_UNIT_INTERCEPTOR, pAttackerUnit->maxXPValue());
		setInBorders(BATTLE_UNIT_INTERCEPTOR, pPlot->getOwner() == pInterceptor->getOwner());
		setUpdateGlobal(BATTLE_UNIT_INTERCEPTOR, pAttackerUnit->canEarnGlobalXP());
	}
}

bool CvCombatInfo::IsCombatRandom() const
{
	return !GC.getGame().isOption(GAMEOPTION_NO_COMBAT_RANDOMNESS) && !IsCombatPrediction();
}
// Did the Attacker bite the dust?
bool CvCombatInfo::IsAttackerDead() const
{
	if (getUnit(BATTLE_UNIT_ATTACKER) != NULL)
	{
		return (getFinalDamage(BATTLE_UNIT_ATTACKER) >= getUnit(BATTLE_UNIT_ATTACKER)->GetMaxHitPoints());
	}
	if (getCity(BATTLE_UNIT_ATTACKER) != NULL)
	{
		return (getFinalDamage(BATTLE_UNIT_ATTACKER) >= getCity(BATTLE_UNIT_ATTACKER)->GetMaxHitPoints());
	}
	return false;
}
// Did the Defender bite the dust?
bool CvCombatInfo::IsDefenderDead() const
{
	if (getUnit(BATTLE_UNIT_DEFENDER) != NULL)
	{
		return (getFinalDamage(BATTLE_UNIT_DEFENDER) >= getUnit(BATTLE_UNIT_DEFENDER)->GetMaxHitPoints());
	}
	if (getCity(BATTLE_UNIT_DEFENDER) != NULL)
	{
		return (getFinalDamage(BATTLE_UNIT_DEFENDER) >= getCity(BATTLE_UNIT_DEFENDER)->GetMaxHitPoints());
	}
	return false;
}
#if defined(LEKMOD_COMBAT_PREDICTOR_IMPROVEMENTS)
void CvCombatModifierList::AddEntry(const CvString& strText, int iModifier, bool bPercent)
{
	if (iModifier == 0)
		return;

	int& iThisMiscModifier = bPercent ? iMiscModifier : iMiscModifierFlat;
	int& iThisMiscCount = bPercent ? iMiscCount : iMiscCountFlat;
	int& iThisMiscIndex = bPercent ? iMiscEntryIndex : iMiscFlatEntryIndex;

	if (iThisMiscIndex >= 0)
	{
		iThisMiscModifier += iModifier;
		++iThisMiscCount;

		RebuildMiscellaneous(bPercent);
		return;
	}

	// Unlimited, or another named row fits.
	if (iMaxLines <= 0 || static_cast<int>(m_kEntries.size()) < iMaxLines)
	{
		m_kEntries.push_back(CvCombatModifierEntry(strText, iModifier, bPercent));
		return;
	}

	if (m_kEntries.empty())
		return;

	int iVictim = -1;
	for (int i = static_cast<int>(m_kEntries.size()) - 1; i >= 0; --i)
	{
		if (m_kEntries[i].m_bPercent == bPercent && !m_kEntries[i].m_bMiscellaneous)
		{
			iVictim = i;
			break;
		}
	}

	bool bVictimSameType = (iVictim >= 0);
	if (iVictim < 0)
		iVictim = static_cast<int>(m_kEntries.size()) - 1;

	iThisMiscModifier = bVictimSameType ? (m_kEntries[iVictim].m_iModifier + iModifier) : iModifier;
	iThisMiscCount = bVictimSameType ? 2 : 1;
	iThisMiscIndex = iVictim;

	RebuildMiscellaneous(bPercent);
}

void CvCombatModifierList::RebuildMiscellaneous(bool bPercent)
{
	int iIndex = bPercent ? iMiscEntryIndex : iMiscFlatEntryIndex;
	if (iIndex < 0 || iIndex >= static_cast<int>(m_kEntries.size()))
		return;

	int iCount = bPercent ? iMiscCount : iMiscCountFlat;
	int iModifier = bPercent ? iMiscModifier : iMiscModifierFlat;

	Localization::String localizedText = Localization::Lookup("TXT_KEY_COMBATMOD_MISCELLANEOUS");
	localizedText << iCount;

	CvCombatModifierEntry& kMiscEntry = m_kEntries[iIndex];
	kMiscEntry.m_strText = localizedText.toUTF8();
	kMiscEntry.m_iModifier = iModifier;
	kMiscEntry.m_bPercent = bPercent;
	kMiscEntry.m_bMiscellaneous = true;
}
#endif
//------------------------------------------------------------------------------------------------
// FUNCTION:    CvMissionDefinition::CvMissionDefinition
//! \brief      Default constructor.
//------------------------------------------------------------------------------------------------
CvMissionDefinition::CvMissionDefinition() :
	m_fMissionTime(0.0f),
	m_eMissionType(NO_MISSION),
	m_pPlot(NULL),
	m_pSecondaryPlot(NULL)
{
	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
		m_aUnits[i] = NULL;
}

MissionTypes CvMissionDefinition::getMissionType() const
{
	return m_eMissionType;
}

void CvMissionDefinition::setMissionType(MissionTypes missionType)
{
	m_eMissionType = missionType;
}

float CvMissionDefinition::getMissionTime() const
{
	return m_fMissionTime;
}

void CvMissionDefinition::setMissionTime(float time)
{
	m_fMissionTime = time;
}

CvUnit* CvMissionDefinition::getUnit(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aUnits[unitType];
}

void CvMissionDefinition::setUnit(BattleUnitTypes unitType, CvUnit* unit)
{
	checkBattleUnitType(unitType);
	m_aUnits[unitType] = unit;
}

const CvPlot* CvMissionDefinition::getPlot() const
{
	return m_pPlot;
}

void CvMissionDefinition::setPlot(const CvPlot* plot)
{
	m_pPlot = plot;
}

const CvPlot* CvMissionDefinition::getSecondaryPlot() const
{
	return m_pSecondaryPlot;
}

void CvMissionDefinition::setSecondaryPlot(const CvPlot* plot)
{
	m_pSecondaryPlot = plot;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Constructor
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition() :
	CvMissionDefinition()
#ifdef AUI_WARNING_FIXES
	, m_aDamage()		//!< The ending damage of the units
#endif
{
	m_fMissionTime = 0.0f;
	m_eMissionType = CvTypes::getMISSION_AIRPATROL();
}

//------------------------------------------------------------------------------------------------
// FUNCTION:    CvAirMissionDefinition::CvAirMissionDefinition
//! \brief      Copy constructor
//! \param      kCopy The object to copy
//------------------------------------------------------------------------------------------------
CvAirMissionDefinition::CvAirMissionDefinition(const CvAirMissionDefinition& kCopy)
{
	m_fMissionTime = kCopy.m_fMissionTime;
	m_eMissionType = kCopy.m_eMissionType;
	m_pPlot = kCopy.m_pPlot;

	for(int i=0; i<BATTLE_UNIT_COUNT; i++)
	{
		m_aDamage[i] = kCopy.m_aDamage[i];
		m_aUnits[i] = kCopy.m_aUnits[i];
	}
}

int CvAirMissionDefinition::getDamage(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	return m_aDamage[unitType];
}

void CvAirMissionDefinition::setDamage(BattleUnitTypes unitType, int damage)
{
	checkBattleUnitType(unitType);
	m_aDamage[unitType] = damage;
}

bool CvAirMissionDefinition::isDead(BattleUnitTypes unitType) const
{
	checkBattleUnitType(unitType);
	CvAssertMsg(getUnit(unitType) != NULL, "[Jason] Invalid battle unit type.");
	if(getDamage(unitType) >= getUnit(unitType)->GetMaxHitPoints())
		return true;
	else
		return false;
}

//PBGameSetupData::PBGameSetupData() :
//iSize(0)
//, iClimate(0)
//, iSeaLevel(0)
//, iSpeed(0)
//, iEra(0)
//, iMaxTurns(0)
//, iCityElimination(0)
//, iAdvancedStartPoints(0)
//, iTurnTime(0)
//, iNumCustomMapOptions(0)
//, aiCustomMapOptions(0)
//, iNumVictories(0)
//, abVictories(0)
//, szMapName("")
//, abOptions()
//, abMPOptions()
//{
//	for (int i = 0; i < NUM_GAMEOPTION_TYPES; i++)
//	{
//		abOptions.push_back(false);
//	}
//	for (int i = 0; i < NUM_MPOPTION_TYPES; i++)
//	{
//		abMPOptions.push_back(false);
//	}
//}

FDataStream& operator<<(FDataStream& saveTo, const OrderData& readFrom)
{
	saveTo << readFrom.eOrderType;
	saveTo << readFrom.iData1;
	saveTo << readFrom.iData2;
	saveTo << readFrom.bSave;
	saveTo << readFrom.bRush;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, OrderData& writeTo)
{
	loadFrom >> writeTo.eOrderType;
	loadFrom >> writeTo.iData1;
	loadFrom >> writeTo.iData2;
	loadFrom >> writeTo.bSave;
	loadFrom >> writeTo.bRush;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const PlotExtraYield& readFrom)
{
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_aeExtraYield;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, PlotExtraYield& writeTo)
{
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_aeExtraYield;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const PlotExtraCost& readFrom)
{
	saveTo << readFrom.m_iX;
	saveTo << readFrom.m_iY;
	saveTo << readFrom.m_iCost;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, PlotExtraCost& writeTo)
{
	loadFrom >> writeTo.m_iX;
	loadFrom >> writeTo.m_iY;
	loadFrom >> writeTo.m_iCost;
	return loadFrom;
}


FDataStream& operator<<(FDataStream& saveTo, const IDInfo& readFrom)
{
	saveTo << readFrom.eOwner;
	saveTo << readFrom.iID;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, IDInfo& writeTo)
{
	loadFrom >> writeTo.eOwner;
	loadFrom >> writeTo.iID;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const MissionData& readFrom)
{
	saveTo << readFrom.eMissionType;
	saveTo << readFrom.iData1;
	saveTo << readFrom.iData2;
	saveTo << readFrom.iFlags;
	saveTo << readFrom.iPushTurn;
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, MissionData& writeTo)
{
	loadFrom >> writeTo.eMissionType;
	loadFrom >> writeTo.iData1;
	loadFrom >> writeTo.iData2;
	loadFrom >> writeTo.iFlags;
	loadFrom >> writeTo.iPushTurn;
	return loadFrom;
}

#if defined(LEKMOD_GOLDEN_AGE_YIELD_THRESHOLD)
FDataStream& operator>>(FDataStream& kStream, GoldenAgeYieldThreshold& writeTo)
{
	kStream >> writeTo.m_eThresholdYield;
	kStream >> writeTo.m_iThresholdAmount;
	kStream >> writeTo.m_eRwdYield;
	kStream >> writeTo.m_iRwdAmount;
	return kStream;
}

FDataStream& operator<<(FDataStream& kStream, const GoldenAgeYieldThreshold& readFrom)
{
	kStream << readFrom.m_eThresholdYield;
	kStream << readFrom.m_iThresholdAmount;
	kStream << readFrom.m_eRwdYield;
	kStream << readFrom.m_iRwdAmount;
	return kStream;
}
#endif