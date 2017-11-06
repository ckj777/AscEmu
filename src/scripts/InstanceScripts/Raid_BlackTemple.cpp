/*
 * ArcScripts for ArcEmu MMORPG Server
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2008-2015 Sun++ Team <http://www.sunplusplus.info/>
 * Copyright (C) 2005-2007 Ascent Team
 * Copyright (C) 2007-2015 Moon++ Team <http://www.moonplusplus.info/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// \todo move most defines to enum, text to db (use SendScriptTextChatMessage(ID))
#include "Setup.h"
#include "Raid_BlackTemple.h"
#include "Objects/Faction.h"
#include "Spell/SpellMgr.h"


class BlackTempleScript : public InstanceScript
{
    public:

        BlackTempleScript(MapMgr* pMapMgr) : InstanceScript(pMapMgr)
        {
            if (getData(CN_SUPREMUS) == Finished)
                setGameObjectStateForEntry(185882, GO_STATE_OPEN);    // Gate to Black Temple behind Supremus
        }

        static InstanceScript* Create(MapMgr* pMapMgr) { return new BlackTempleScript(pMapMgr); }

        void OnCreatureDeath(Creature* pVictim, Unit* pKiller)
        {
            // You don't have to use additional scripts to open any gates / doors
            switch (pVictim->GetEntry())
            {
                case CN_SUPREMUS:
                    setGameObjectStateForEntry(185882, GO_STATE_OPEN);    // Gate to Black Temple behind Supremus
                    break;
                default:
                    break;
            }
        }
};

//////////////////////////////////////////////////////////////////////////
/////// Trash AIs
/*
class TemplateAI : public MoonScriptCreatureAI
{
    MOONSCRIPT_FACTORY_FUNCTION(TemplateAI, MoonScriptCreatureAI);
    TemplateAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
    {
    }
};
*/
//////////////////////////////////////////////////////////////////////////
/////// Beast AIs

class DragonTurtleAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(DragonTurtleAI, MoonScriptCreatureAI);
        DragonTurtleAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DRAGON_TRUTLE_SHELL_SHIELD, Target_Self, 10, 0, 25);
            AddSpell(DRAGON_TURTLE_WATER_SPIT, Target_Current, 8, 2.5, 20, 0, 45);        // Random?
        }
};


class LeviathanAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(LeviathanAI, MoonScriptCreatureAI);
        LeviathanAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(LEVIATHAN_DEBILITATING_SPRAY, Target_RandomPlayer, 8, 2.5, 20);
            AddSpell(LEVIATHAN_POISON_SPIT, Target_RandomPlayer, 7, 2.5, 25);
            AddSpell(LEVIATHAN_TAIL_SWEEP, Target_Self, 7, 0, 30);
        }
};


class MutantWarHoundAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(MutantWarHoundAI, MoonScriptCreatureAI);
        MutantWarHoundAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature) {}

        void OnDied(Unit* pKiller)
        {
            ParentClass::OnDied(pKiller);

            Aura* pAura = sSpellFactoryMgr.NewAura(sSpellCustomizations.GetSpellInfo(MUTANT_WAR_HOUND_CLOUD_OF_DISEASE), (uint32)20000, _unit, _unit);
            _unit->AddAura(pAura);
        }
};


// I don't have this creature in DB, so hasn't checked it yet
void SpellFunc_Charge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class ShadowmoonRidingHoundAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonRidingHoundAI, MoonScriptCreatureAI);
        ShadowmoonRidingHoundAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_RIDING_HOUND_CARNIVOROUS_BITE, Target_Current, 10, 0, 20, 0, 10);
            mCharge = AddSpell(SHADOWMOON_RIDING_HOUND_CHARGE, Target_Current, 0, 0, 0);
            AddSpellFunc(&SpellFunc_Charge, Target_RandomPlayer, 8, 0, 35);
            AddSpell(SHADOWMOON_RIDING_HOUND_ENRAGE, Target_Self, 6, 0, 40);
        }

        SpellDesc* mCharge;
};

void SpellFunc_Charge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ShadowmoonRidingHoundAI* pHoundAI = (pCreatureAI != NULL) ? static_cast< ShadowmoonRidingHoundAI* >(pCreatureAI) : NULL;
    if (pHoundAI != NULL)
    {
        Unit* pCurrentTarget = pHoundAI->GetUnit()->GetAIInterface()->getNextTarget();
        if (pCurrentTarget != NULL && pCurrentTarget != pTarget)
        {
            pHoundAI->GetUnit()->GetAIInterface()->AttackReaction(pTarget, 500);
            pHoundAI->GetUnit()->GetAIInterface()->setNextTarget(pTarget);
            pHoundAI->GetUnit()->GetAIInterface()->RemoveThreatByPtr(pCurrentTarget);
        }

        pHoundAI->CastSpell(pHoundAI->mCharge);
    }
}

//////////////////////////////////////////////////////////////////////////
/////// Demon AIs


class SisterOfPleasureAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(SisterOfPleasureAI, MoonScriptCreatureAI);
        SisterOfPleasureAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SISTER_OF_PLEASURE_GREATER_HEAL, Target_WoundedFriendly, 10, 2, 45);
            AddSpell(SISTER_OF_PLEASURE_HOLY_NOVA, Target_Self, 10, 0, 20);
            AddSpell(SISTER_OF_PLEASURE_SHARED_BONDS, Target_Current, 5, 0, 25);
            AddSpell(SISTER_OF_PLEASURE_SHELL_OF_LIFE, Target_Self, 7, 1, 45);
        }
};


class SisterOfPainAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(SisterOfPainAI, MoonScriptCreatureAI);
        SisterOfPainAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SISTER_OF_PAIN_LASH_OF_PAIN, Target_Current, 10, 0, 25, 0, 10);
            AddSpell(SISTER_OF_PAIN_PAINFUL_RAGE, Target_Self, 5, 0, 60);
            AddSpell(SISTER_OF_PAIN_SHADOW_WORD_PAIN, Target_RandomPlayer, 10, 0, 25);
            AddSpell(SISTER_OF_PAIN_SHARED_BONDS, Target_Current, 5, 0, 25);
            AddSpell(SISTER_OF_PAIN_SHELL_OF_PAIN, Target_Self, 7, 1, 45);
        }
};


class PriestessOfDementiaAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(PriestessOfDementiaAI, MoonScriptCreatureAI);
        PriestessOfDementiaAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(PRIESTESS_OF_DEMENTIA_CONFUSION, Target_Self, 8, 0, 30);
            AddSpell(PRIESTESS_OF_DEMENTIA_DEMENTIA, Target_Self, 5, 0, 60);
        }
};


class PriestessOfDelightAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(PriestessOfDelightAI, MoonScriptCreatureAI);
        PriestessOfDelightAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(PRIESTESS_OF_DELIGHT_CURSE_OF_VITALITY, Target_RandomPlayer, 10, 0, 30);
        }
};


class IllidariNightlordAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariNightlordAI, MoonScriptCreatureAI);
        IllidariNightlordAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_NIGHTLORD_SUMMON_SHADOWFIENDS, Target_Self, 7, 0, 45);            // does it work?
            AddSpell(ILLIDARI_NIGHTLORD_SHADOW_INFERNO, Target_Self, 10, 0, 25);
            AddSpell(ILLIDARI_NIGHTLORD_FEAR, Target_Self, 7, 1, 30);
            AddSpell(ILLIDARI_NIGHTLORD_CURSE_OF_MENDING, Target_RandomPlayer, 8, 2, 35);
        }
};


void SpellFunc_RapidShot(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class IllidariHeartseekerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariHeartseekerAI, MoonScriptCreatureAI);
        IllidariHeartseekerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_HEARTSEEKER_CURSE_OF_THE_BLEAKHEART, Target_RandomPlayer, 8, 0, 40, 0, 30);
            mRapidShot = AddSpell(ILLIDARI_HEARTSEEKER_RAPID_SHOT, Target_Self, 0, 8, 0);                        // I need to check mechanics (maybe it should speed up usage of Shoot spell (2x lesser freq))
            AddSpell(ILLIDARI_HEARTSEEKER_SHOOT, Target_Current, 80, 0, 1, 0, 30);
            AddSpell(ILLIDARI_HEARTSEEKER_SKELETON_SHOT, Target_RandomPlayer, 7, 0, 25, 0, 30);
            AddSpellFunc(&SpellFunc_RapidShot, Target_Current, 7, 8, 40, 0, 30);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 30.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 30.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }

        SpellDesc* mRapidShot;
};

void SpellFunc_RapidShot(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    IllidariHeartseekerAI* pHeartseeker = (pCreatureAI != NULL) ? static_cast< IllidariHeartseekerAI* >(pCreatureAI) : NULL;
    if (pHeartseeker != NULL)
    {
        pHeartseeker->CastSpell(pHeartseeker->mRapidShot);
    }
}


class IllidariFearbringerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariFearbringerAI, MoonScriptCreatureAI);
        IllidariFearbringerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_FEARBRINGER_ILLIDARI_FLAMES, Target_Current, 8, 2, 25, 0, 10);
            AddSpell(ILLIDARI_FEARBRINGER_RAIN_OF_CHAOS, Target_RandomPlayerDestination, 7, 0, 35);
            AddSpell(ILLIDARI_FEARBRINGER_WAR_STOMP, Target_Self, 8, 0, 35);
        }
};


class IllidariDefilerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariDefilerAI, MoonScriptCreatureAI);
        IllidariDefilerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_DEFILER_BANISH, Target_RandomPlayer, 8, 1, 30);
            AddSpell(ILLIDARI_DEFILER_CURSE_OF_AGONY, Target_RandomPlayer, 7, 0, 45);
            AddSpell(ILLIDARI_DEFILER_FEL_IMMOLATE, Target_RandomPlayer, 8, 2, 25);
            AddSpell(ILLIDARI_DEFILER_RAIN_OF_CHAOS, Target_RandomPlayerDestination, 7, 6, 35);
        }
};


class IllidariCenturionAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariCenturionAI, MoonScriptCreatureAI);
        IllidariCenturionAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_CENTURION_CLEAVE, Target_Current, 10, 0, 20, 0, 10);
            AddSpell(ILLIDARI_CENTURION_SONIC_STRIKE, Target_Self, 8, 0, 35);
        }
};


void SpellFunc_Gouge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class IllidariBoneslicerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariBoneslicerAI, MoonScriptCreatureAI);
        IllidariBoneslicerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_BONESLICER_CLOAK_OF_SHADOWS, Target_Self, 8, 0, 25);
            mGouge = AddSpell(ILLIDARI_BONESLICER_GOUGE, Target_Current, 0, 0, 0);
            AddSpellFunc(&SpellFunc_Gouge, Target_Current, 7, 0, 25, 0, 10);
            AddSpell(ILLIDARI_BONESLICER_SHADOWSTEP, Target_Current, 7, 0, 30);
        }

        SpellDesc* mGouge;
};

void SpellFunc_Gouge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    IllidariBoneslicerAI* pBoneslicer = (pCreatureAI != NULL) ? static_cast< IllidariBoneslicerAI* >(pCreatureAI) : NULL;
    if (pBoneslicer != NULL)
    {
        pBoneslicer->CastSpell(pBoneslicer->mGouge);
        Unit* pSecondHated = pBoneslicer->GetUnit()->GetAIInterface()->GetSecondHated();
        if (pSecondHated != NULL)
        {
            pBoneslicer->GetUnit()->GetAIInterface()->AttackReaction(pSecondHated, 2000);
            pBoneslicer->GetUnit()->GetAIInterface()->setNextTarget(pSecondHated);
            pBoneslicer->GetUnit()->GetAIInterface()->RemoveThreatByPtr(pTarget);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
/////// Humanoid AIs


class AshtongueBattlelordAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueBattlelordAI, MoonScriptCreatureAI);
        AshtongueBattlelordAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_BATTLELORD_CLEAVE, Target_Current, 10, 0, 15, 0, 10);
            AddSpell(ASHTONGUE_BATTLELORD_CONCUSSION_BLOW, Target_Current, 8, 0, 30, 0, 10);
            AddSpell(ASHTONGUE_BATTLELORD_CONCUSSIVE_THROW, Target_RandomPlayerNotCurrent, 8, 0, 25, 10, 40);
            AddSpell(ASHTONGUE_BATTLELORD_ENRAGE, Target_Self, 4, 0, 50);
        }
};


class AshtongueDefenderAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueDefenderAI, MoonScriptCreatureAI);
        AshtongueDefenderAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_DEFENDER_DEBILITATING_STRIKE, Target_Current, 10, 0, 20, 0, 10);
            AddSpell(ASHTONGUE_DEFENDER_SHIELD_BASH, Target_Current, 7, 0, 25, 0, 10);
        }
};


class AshtongueElementalistAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueElementalistAI, MoonScriptCreatureAI);
        AshtongueElementalistAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_ELEMENTALIST_LIGHTNING_BOLT, Target_Current, 8, 1, 20, 0, 50);    // Random?
            AddSpell(ASHTONGUE_ELEMENTALIST_RAID_OF_FIRE, Target_RandomPlayerDestination, 6, 0, 25);
        }
};


///\ todo Add Totem AIs
class AshtongueMysticAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueMysticAI, MoonScriptCreatureAI);
        AshtongueMysticAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_MYSTIC_BLOODLUST, Target_Self, 5, 0, 45);
            AddSpell(ASHTONGUE_MYSTIC_CHAIN_HEAL, Target_WoundedFriendly, 6, 2.5, 35);
            AddSpell(ASHTONGUE_MYSTIC_CYCLONE_TOTEM, Target_Self, 7, 0, 35);
            AddSpell(ASHTONGUE_MYSTIC_FLAME_SHOCK, Target_Current, 8, 0, 25, 0, 20);
            AddSpell(ASHTONGUE_MYSTIC_FROST_SHOCK, Target_Current, 8, 0, 25, 0, 20);
            AddSpell(ASHTONGUE_MYSTIC_SEARING_TOTEM, Target_Self, 7, 0, 35);
            AddSpell(ASHTONGUE_MYSTIC_SUMMON_WINDFURY_TOTEM, Target_Self, 7, 0, 35);
        }
};


class AshtonguePrimalistAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtonguePrimalistAI, MoonScriptCreatureAI);
        AshtonguePrimalistAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_PRIMALIST_MULTISHOT, Target_Current, 8, 0, 40, 0, 30);            // Target_Self - self dmg too (kill == crash)
            AddSpell(ASHTONGUE_PRIMALIST_SHOOT, Target_Current, 80, 0, 1, 0, 30);
            AddSpell(ASHTONGUE_PRIMALIST_SWEEPING_WING_CLIP, Target_Current, 8, 0, 15, 0, 10, true);
            AddSpell(ASHTONGUE_PRIMALIST_WYVERN_STRING, Target_RandomPlayer, 7, 0, 25, 0, 30);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 30.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 30.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }
};


class AshtongueRogueAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueRogueAI, MoonScriptCreatureAI);
        AshtongueRogueAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_ROGUE_DEBILITATING_POISON, Target_Current, 7, 0, 30, 0, 10);
            AddSpell(ASHTONGUE_ROGUE_EVISCERATE, Target_Current, 10, 0, 15, 0, 10);
        }
};


class AshtongueSpiritbinderAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueSpiritbinderAI, MoonScriptCreatureAI);
        AshtongueSpiritbinderAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_SPIRITBINDER_CHAIN_HEAL, Target_WoundedFriendly, 7, 1, 25);
            if (!_isHeroic())    // Guessed
                AddSpell(ASHTONGUE_SPIRITBINDER_SPIRIT_HEAL, Target_Self, 7, 0, 25);                // Weaker version
            else
                AddSpell(ASHTONGUE_SPIRITBINDER_SPIRIT_HEAL2, Target_Self, 7, 0, 25);                // Stronger version
            AddSpell(ASHTONGUE_SPIRITBINDER_SPIRIT_MEND, Target_WoundedFriendly, 8, 0, 25);
        }
};


///\brief Completely guessed mechanics
class AshtongueStalkerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueStalkerAI, MoonScriptCreatureAI);
        AshtongueStalkerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_STALKER_BLIND, Target_RandomPlayer, 7, 1, 25);                // Should be Blind - > Attack Next Target?
            AddSpell(ASHTONGUE_STALKER_INSTANT_POISON, Target_Current, 10, 0, 20, 0, 10);
            AddSpell(ASHTONGUE_STALKER_MINDNUMBING_POISON, Target_RandomPlayer, 8, 0, 20);
            _applyAura(ASHTONGUE_STALKER_STEATH);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            SpellDesc* pSpell = FindSpellById(ASHTONGUE_STALKER_BLIND);
            if (pSpell != NULL)
            {
                CastSpellNowNoScheduling(pSpell);
            }
        }

        void OnCombatStop(Unit* pTarget)
        {
            ParentClass::OnCombatStop(pTarget);
            if (isAlive())
            {
                _applyAura(ASHTONGUE_STALKER_STEATH);
            }
        }
};


class AshtongueStormcallerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueStormcallerAI, MoonScriptCreatureAI);
        AshtongueStormcallerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ASHTONGUE_STORMCALLER_CHAIN_LIGHTNING, Target_Current, 7, 3, 35, 0, 30);        // Random?
            AddSpell(ASHTONGUE_STORMCALLER_LIGHTNING_BOLT, Target_Current, 10, 2, 20);
            AddSpell(ASHTONGUE_STORMCALLER_LIGHTNING_SHIELD, Target_Self, 10, 0, 50);
        }
};


class BonechewerBehemothAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerBehemothAI, MoonScriptCreatureAI);
        BonechewerBehemothAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_BEHEMOTH_BEHEMOTH_CHARGE, Target_Current, 0, 0, 20);    // Mechanics are like charge on random target?
            AddSpell(BONECHEWER_BEHEMOTH_ENRAGE, Target_Self, 5, 0, 45);
            AddSpell(BONECHEWER_BEHEMOTH_FEL_STOMP, Target_Self, 7, 0, 30);
            AddSpell(BONECHEWER_BEHEMOTH_FIERY_COMET, Target_RandomPlayerDestination, 6, 1, 30);
            AddSpell(BONECHEWER_BEHEMOTH_METEOR, Target_RandomPlayerDestination, 5, 2, 40);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            SpellDesc* pCharge = FindSpellById(BONECHEWER_BEHEMOTH_BEHEMOTH_CHARGE);
            if (pCharge != NULL)
            {
                CastSpellNowNoScheduling(pCharge);
            }
        }
};


class BonechewerBladeFuryAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerBladeFuryAI, MoonScriptCreatureAI);
        BonechewerBladeFuryAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_BLADE_FURY_WHIRLWIND, Target_Self, 7, 8, 40);    // Self / Current ?
        }
};


class BonechewerBloodProphetAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerBloodProphetAI, MoonScriptCreatureAI);
        BonechewerBloodProphetAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_BLOOD_PROPHET_BLOOD_DRAIN, Target_Current, 6, 5, 30, 0, 20);    // Random?
            AddSpell(BONECHEWER_BLOOD_PROPHET_BLOODBOLT, Target_RandomPlayer, 7, 1.5, 25);
            AddSpell(BONECHEWER_BLOOD_PROPHET_ENRAGE, Target_Self, 5, 0, 50);
            if (_isHeroic())                        // Guessed
                AddSpell(BONECHEWER_BLOOD_PROPHET_PROPHECY_OF_BLOOD, Target_RandomPlayer, 10, 0, 30);
            else
                AddSpell(BONECHEWER_BLOOD_PROPHET_PROPHECY_OF_BLOOD2, Target_RandomPlayer, 10, 0, 30);
        }
};


class BonechewerBrawlerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerBrawlerAI, MoonScriptCreatureAI);
        BonechewerBrawlerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_BRAWLER_FRENZY, Target_Self, 7, 0, 30);
        }
};


class BonechewerCombatantAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerCombatantAI, MoonScriptCreatureAI);
        BonechewerCombatantAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_COMBATANT_FRENZY, Target_Self, 7, 0, 45);
        }
};


class BonechewerShieldDiscipleAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerShieldDiscipleAI, MoonScriptCreatureAI);
        BonechewerShieldDiscipleAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_SHIELD_DISCIPLE_INTERVENE, Target_Current, 0, 0, 20);
            AddSpell(BONECHEWER_SHIELD_DISCIPLE_SHIELD_BASH, Target_Current, 8, 0, 25, 0, 10);
            AddSpell(BONECHEWER_SHIELD_DISCIPLE_SHIELD_WALL, Target_Self, 8, 0, 35);
            AddSpell(BONECHEWER_SHIELD_DISCIPLE_THROW_SHIELD, Target_RandomPlayer, 7, 0, 30);    // Current?
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            SpellDesc* pIntervene = FindSpellById(BONECHEWER_SHIELD_DISCIPLE_INTERVENE);
            if (pIntervene != NULL)
            {
                CastSpellNowNoScheduling(pIntervene);
            }
        }
};


class BonechewerSpectatorAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerSpectatorAI, MoonScriptCreatureAI);
        BonechewerSpectatorAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_SPECTATOR_CHARGE, Target_Current, 0, 0, 20);
            AddSpell(BONECHEWER_SPECTATOR_CLEAVE, Target_Current, 10, 0, 25, 0, 10);
            AddSpell(BONECHEWER_SPECTATOR_MORTAL_WOUND, Target_Current, 7, 0, 15, 0, 10);
            AddSpell(BONECHEWER_SPECTATOR_STRIKE, Target_Current, 10, 0, 10, 0, 10);
            AddSpell(BONECHEWER_SPECTATOR_SUNDER_ARMOR, Target_Current, 7, 0, 20, 0, 10);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            SpellDesc* pCharge = FindSpellById(BONECHEWER_SPECTATOR_CHARGE);
            if (pCharge != NULL)
            {
                CastSpellNowNoScheduling(pCharge);
            }
        }
};


class BonechewerTaskmasterAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerTaskmasterAI, MoonScriptCreatureAI);
        BonechewerTaskmasterAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_TASKMASTER_DISGRUNTLED, Target_Self, 0, 0, 45);    //5, 0, 45);    // Disabled for now (buffs players instead of Taskmasters)
            AddSpell(BONECHEWER_TASKMASTER_FURY, Target_Self, 8, 0, 15);
        }
};


class BonechewerWorkerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(BonechewerWorkerAI, MoonScriptCreatureAI);
        BonechewerWorkerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(BONECHEWER_WORKER_THROW_PICK, Target_RandomPlayer, 8, 0.5, 15);    // Current? Also shouldn't be cast once only?
        }
};


class CharmingCourtesanAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CharmingCourtesanAI, MoonScriptCreatureAI);
        CharmingCourtesanAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(CHARMING_COURTESAN_INFATUATION, Target_RandomPlayer, 7, 20, 40);
            AddSpell(CHARMING_COURTESAN_POISONOUS_THROW, Target_RandomPlayer, 8, 0, 20);
        }
};


class CoilskarGeneralAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CoilskarGeneralAI, MoonScriptCreatureAI);
        CoilskarGeneralAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(COILSKAR_GENERAL_BOOMING_VOICE, Target_Self, 7, 0, 35);
            AddSpell(COILSKAR_GENERAL_FREE_FRIEND, Target_RandomFriendly, 7, 0, 20);    // SpellFunc to check incapacitating effects?
        }
};


///\todo Is it Ranged unit throwing spears all the time?
class CoilskarHarpoonerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CoilskarHarpoonerAI, MoonScriptCreatureAI);
        CoilskarHarpoonerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(COILSKAR_HARPOONER_HARPOONERS_MARK, Target_RandomPlayer, 7, 0, 35);        // I must link Dragon Turtle with Harpooner before scripting it
            AddSpell(COILSKAR_HARPOONER_HOOKED_NET, Target_RandomPlayer, 7, 0.5, 25);
            AddSpell(COILSKAR_HARPOONER_SPEAR_THROW, Target_Current, 8, 0, 15, 0, 40);            // Random? Also isn't it typical ranged unit? (using Spear Throw instead of Shoot/Shot spell?)
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            SpellDesc* pMark = FindSpellById(COILSKAR_HARPOONER_HARPOONERS_MARK);
            if (pMark != NULL)
            {
                CastSpellNowNoScheduling(pMark);
            }
        }
};


class CoilskarSeacallerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CoilskarSeacallerAI, MoonScriptCreatureAI);
        CoilskarSeacallerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(COILSKAR_SEACALLER_FORKED_LIGHTNING, Target_Self, 8, 2, 20);
            AddSpell(COILSKAR_SEACALLER_HURRICANE, Target_RandomPlayerDestination, 8, 20, 35);
            AddSpell(COILSKAR_SEACALLER_SUMMON_GEYSER, Target_RandomPlayer, 7, 2, 25);            // Npc to add to DB + AI
        }
};


class CoilskarSoothsayerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CoilskarSoothsayerAI, MoonScriptCreatureAI);
        CoilskarSoothsayerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(COILSKAR_SOOTHSAYER_HOLY_NOVA, Target_Self, 10, 0, 20);
            AddSpell(COILSKAR_SOOTHSAYER_RESTORATION, Target_WoundedFriendly, 8, 1.5, 35);
        }
};


class CoilskarWranglerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CoilskarWranglerAI, MoonScriptCreatureAI);
        CoilskarWranglerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(COILSKAR_WRANGLER_CLEAVE, Target_Current, 10, 0, 20, 0, 10);
            AddSpell(COILSKAR_WRANGLER_ELECTRIC_SPUR, Target_Self, 8, 3, 45);                    // Should fraize Leviathan, but it does not (core? or script? :<)
            AddSpell(COILSKAR_WRANGLER_LIGHTNING_PROD, Target_RandomPlayer, 8, 3, 25);
        }
};


class DragonmawSkyStalkerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(DragonmawSkyStalkerAI, MoonScriptCreatureAI);
        DragonmawSkyStalkerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DRAGONMAW_SKY_STALKER_IMMOLATION_ARROW, Target_Current, 8, 2, 15, 0, 40);
            AddSpell(DRAGONMAW_SKY_STALKER_SHOOT, Target_Current, 75, 0, 1, 0, 40);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 40.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 40.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }
};


///\todo Should it run away from tank when he's close?
class DragonmawWindReaverAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(DragonmawWindReaverAI, MoonScriptCreatureAI);
        DragonmawWindReaverAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DRAGONMAW_WIND_REAVER_DOOM_BOLT, Target_Current, 10, 1.5, 15, 0, 40);
            AddSpell(DRAGONMAW_WIND_REAVER_FIREBALL, Target_Current, 75, 2, 0, 0, 40);
            AddSpell(DRAGONMAW_WIND_REAVER_FREEZE, Target_RandomPlayer, 10, 2, 15);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 40.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 40.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }
};


class DragonmawWyrmcallerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(DragonmawWyrmcallerAI, MoonScriptCreatureAI);
        DragonmawWyrmcallerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DRAGONMAW_WYRMCALLER_CLEAVE, Target_Current, 10, 0, 15, 0, 10);
            AddSpell(DRAGONMAW_WYRMCALLER_FIXATE, Target_RandomUnit, 7, 0, 20);
            AddSpell(DRAGONMAW_WYRMCALLER_JAB, Target_Current, 8, 0, 25, 0, 10);
        }
};


class EnslavedServantAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EnslavedServantAI, MoonScriptCreatureAI);
        EnslavedServantAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ENSLAVED_SERVANT_KIDNEY_SHOT, Target_Current, 7, 0, 25, 0, 10);
            AddSpell(ENSLAVED_SERVANT_UPPERCUT, Target_RandomUnit, 8, 0, 20);

            mHealthResetTimer = -1;
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            mHealthResetTimer = _addTimer(45000);    // to check
        }

        void AIUpdate()
        {
            if (_isTimerFinished(mHealthResetTimer))
            {
                _unit->SetHealth(_unit->getUInt32Value(UNIT_FIELD_MAXHEALTH));    // Found such note about this mob
                _resetTimer(mHealthResetTimer, 45000);
            }

            ParentClass::AIUpdate();
        }

        int32    mHealthResetTimer;
};


class HandOfGorefiendAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(HandOfGorefiendAI, MoonScriptCreatureAI);
        HandOfGorefiendAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(HAND_OF_GOREFIEND_FRENZY, Target_Self, 6, 0, 45);
        }
};


///\todo Mechanics are guessed. I'm also not sure if it's not typical caster unit
class IllidariArchonAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariArchonAI, MoonScriptCreatureAI);
        IllidariArchonAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mIsShadowPriest = false;
            switch (RandomUInt(1))
            {
                case 0:
                    AddSpell(ILLIDARI_ARCHON_HEAL, Target_WoundedFriendly, 5, 2, 30);
                    AddSpell(ILLIDARI_ARCHON_HOLY_SMITE, Target_Current, 8, 2.5, 25, 0, 40);
                    AddSpell(ILLIDARI_ARCHON_POWER_WORD_SHIELD, Target_Self, 7, 0, 35);
                    break;
                case 1:
                    AddSpell(ILLIDARI_ARCHON_MIND_BLAST, Target_Current, 8, 1.5, 25, 0, 30);
                    AddSpell(ILLIDARI_ARCHON_SHADOW_WORD_DEATH, Target_RandomPlayer, 7, 0, 35);
                    _applyAura(ILLIDARI_ARCHON_SHADOWFORM);
                    mIsShadowPriest = true;
                    break;
            }
        }

        void OnCombatStop(Unit* pTarget)
        {
            ParentClass::OnCombatStop(pTarget);
            if (mIsShadowPriest && isAlive())
            {
                _applyAura(ILLIDARI_ARCHON_SHADOWFORM);
            }
        }

        void AIUpdate()
        {
            SpellDesc* pDeath = FindSpellById(ILLIDARI_ARCHON_SHADOW_WORD_DEATH);
            if (mIsShadowPriest && _unit->getUInt32Value(UNIT_FIELD_HEALTH) <= 2500 && pDeath->mEnabled)
            {
                Spell* pCurrentSpell = _unit->GetCurrentSpell();
                if (pCurrentSpell != NULL && pCurrentSpell->pSpellId == pDeath->mInfo->getId())
                {
                    pCurrentSpell->cancel();
                }
                if (!mQueuedSpells.empty())
                    for (SpellDescList::iterator itr = mQueuedSpells.begin(); itr != mQueuedSpells.end(); ++itr)
                    {
                        if ((*itr)->mInfo->getId() == pDeath->mInfo->getId())
                            itr = mQueuedSpells.erase(itr);
                    }
                if (!mScheduledSpells.empty())
                    for (SpellDescList::iterator itr = mScheduledSpells.begin(); itr != mScheduledSpells.end(); ++itr)
                    {
                        if ((*itr)->mInfo->getId() == pDeath->mInfo->getId())
                            itr = mScheduledSpells.erase(itr);
                    }

                pDeath->mEnabled = false;
            }

            ParentClass::AIUpdate();
        }

        bool    mIsShadowPriest;
};


///\todo Couldn't find mechanics nowhere around the net, so kept it simple
class IllidariAssassinAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariAssassinAI, MoonScriptCreatureAI);
        IllidariAssassinAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_ASSASSIN_PARALYZING_POISON, Target_Current, 8, 0, 25, 0, 10);
            AddSpell(ILLIDARI_ASSASSIN_VANISH, Target_Self, 7, 1, 30);
        }

        void AIUpdate()
        {
            if (_unit->HasAura(ILLIDARI_ASSASSIN_VANISH))
            {
                _delayNextAttack(1500);
            }

            ParentClass::AIUpdate();
        }
};


///\todo I've parted it on frost and fire mage - correct me if it's wrong (also slap me if it's typical caster)
class IllidariBattlemageAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariBattlemageAI, MoonScriptCreatureAI);
        IllidariBattlemageAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            switch (RandomUInt(1))
            {
                case 0:
                    AddSpell(ILLIDARI_BATTLEMAGE_BLIZZARD, Target_RandomPlayerDestination, 8, 8, 35);
                    AddSpell(ILLIDARI_BATTLEMAGE_FROSTBOLT, Target_Current, 15, 0, 10, 0, 40);
                    break;
                case 1:
                    AddSpell(ILLIDARI_BATTLEMAGE_FIREBALL, Target_Current, 15, 0, 10, 0, 40);
                    AddSpell(ILLIDARI_BATTLEMAGE_FLAMESTRIKE, Target_RandomPlayerDestination, 8, 0, 40);
                    break;
            }
        }
};


class IllidariBloodLordAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidariBloodLordAI, MoonScriptCreatureAI);
        IllidariBloodLordAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ILLIDARI_BLOOD_LORD_DIVINE_SHIELD, Target_Self, 8, 0, 30);
            AddSpell(ILLIDARI_BLOOD_LORD_HAMMER_OF_JUSTICE, Target_Current, 9, 0, 20, 0, 10);
            AddSpell(ILLIDARI_BLOOD_LORD_JUDGEMENT_OF_COMMAND, Target_Current, 8, 0, 25, 0, 10);
        }
};


///\todo Should be summoned by Priestess of Dementia
void SpellFunc_Whirlwind(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class ImageOfDementiaAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ImageOfDementiaAI, MoonScriptCreatureAI);
        ImageOfDementiaAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mWhirlwind = AddSpell(IMAGE_OF_DEMENTIA_WHRILWIND, Target_Self, 0, 15, 0);
            AddSpellFunc(&SpellFunc_Whirlwind, Target_Self, 15, 15, 35, 0, 10);
        }

        SpellDesc* mWhirlwind;
};

void SpellFunc_Whirlwind(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ImageOfDementiaAI* Dementia = (pCreatureAI != NULL) ? static_cast< ImageOfDementiaAI* >(pCreatureAI) : NULL;
    if (Dementia != NULL)
    {
        Dementia->CastSpell(Dementia->mWhirlwind);
        Dementia->despawn(25000);
    }
}


class ShadowmoonBloodMageAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonBloodMageAI, MoonScriptCreatureAI);
        ShadowmoonBloodMageAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_BLOOD_MAGE_BLOOD_SIPHON, Target_Self, 8, 0, 35);
            AddSpell(SHADOWMOON_BLOOD_MAGE_BLOOD_BOLT, Target_RandomPlayer, 9, 3, 25);
        }
};


class ShadowmoonChampionAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonChampionAI, MoonScriptCreatureAI);
        ShadowmoonChampionAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_CHAMPION_CHAOTIC_LIGHT, Target_Current, 8, 0, 25, 0, 10);
            AddSpell(SHADOWMOON_CHAMPION_WHIRLING_BLADE, Target_RandomPlayer, 8, 1, 30);    // I must check its mechanics
        }
};


void SpellFunc_RaiseDead(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class ShadowmoonDeathshaperAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonDeathshaperAI, MoonScriptCreatureAI);
        ShadowmoonDeathshaperAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_DEATHSHAPER_DEATH_COIL, Target_RandomPlayer, 7, 0, 30);
            AddSpell(SHADOWMOON_DEATHSHAPER_DEMON_ARMOR, Target_Self, 8, 0, 60);
            mRaiseDead = AddSpell(SHADOWMOON_DEATHSHAPER_RAISE_DEAD, Target_Self, 0, 1.5, 0);
            AddSpellFunc(&SpellFunc_RaiseDead, Target_RandomCorpse, 12, 1.5, 30);
            AddSpell(SHADOWMOON_DEATHSHAPER_SHADOW_BOLT, Target_Current, 75, 3, 0, 0, 40);    // Typical caster? Slap me if not...

            _unit->setUInt32Value(UNIT_FIELD_POWER1, 100000);    // temporary way to set up mana of this unit
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 40.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 40.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }

        SpellDesc* mRaiseDead;
};

void SpellFunc_RaiseDead(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ShadowmoonDeathshaperAI* pDeathshaper = (pCreatureAI != NULL) ? static_cast< ShadowmoonDeathshaperAI* >(pCreatureAI) : NULL;
    if (pDeathshaper != NULL)
    {
        pDeathshaper->CastSpellNowNoScheduling(pDeathshaper->mRaiseDead);
        MoonScriptCreatureAI* pAI = pDeathshaper->SpawnCreature(23371, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation());
        if (pAI != NULL)
        {
            pAI->GetUnit()->GetAIInterface()->StopMovement(2500);
            pAI->_delayNextAttack(2500);
        }

        static_cast<Creature*>(pTarget)->Despawn(3000, 0);
    }
}


///\todo No entry in my DB, but AI may stay :)?
class ShadowmoonFallenAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonFallenAI, MoonScriptCreatureAI);
        ShadowmoonFallenAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AggroNearestPlayer(200);
        }
};


class ShadowmoonHoundmasterAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonHoundmasterAI, MoonScriptCreatureAI);
        ShadowmoonHoundmasterAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_HOUNDMASTER_FLARE, Target_RandomPlayerDestination, 5, 0, 30);
            AddSpell(SHADOWMOON_HOUNDMASTER_FREEZING_TRAP, Target_Self, 5, 0, 30);
            AddSpell(SHADOWMOON_HOUNDMASTER_SHOOT, Target_Current, 70, 0, 1, 0, 30);
            AddSpell(SHADOWMOON_HOUNDMASTER_SILENCING_SHOT, Target_RandomPlayer, 6, 0, 35);
            AddSpell(SHADOWMOON_HOUNDMASTER_SUMMON_RIDING_WARHOUND, Target_Self, 5, 0, 45);
            AddSpell(SHADOWMOON_HOUNDMASTER_VOLLEY, Target_RandomPlayerDestination, 5, 1, 25);
            AddSpell(SHADOWMOON_HOUNDMASTER_WING_CLIP, Target_Current, 5, 0, 20, 0, 10, true);
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            if (getRangeToObject(pTarget) <= 30.0f)
            {
                setAIAgent(AGENT_SPELL);
                setRooted(true);
            }
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();
            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL)
            {
                if (getRangeToObject(pTarget) <= 30.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    setRooted(true);
                }
            }
        }
};


class ShadowmoonReaverAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonReaverAI, MoonScriptCreatureAI);
        ShadowmoonReaverAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_REAVER_SPELL_ABSORPTION, Target_Self, 8, 0, 35);
        }
};


class ShadowmoonSoldierAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonSoldierAI, MoonScriptCreatureAI);
        ShadowmoonSoldierAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWMOON_SOLDIER_STRIKE, Target_Current, 10, 0, 15, 0, 10);
        }
};


///\todo Haven't found informations about Shield Wall ability
void SpellFunc_DefensiveStance(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);
void SpellFunc_BerserkerStance(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class ShadowmoonWeaponMasterAI : public MoonScriptBossAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowmoonWeaponMasterAI, MoonScriptBossAI);
        ShadowmoonWeaponMasterAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
            AddPhaseSpell(1, AddSpell(SHADOWMOON_WEAPON_MASTER_KNOCK_AWAY, Target_Current, 9, 0, 25, 0, 10));
            AddPhaseSpell(1, AddSpell(SHADOWMOON_WEAPON_MASTER_MUTILATE, Target_Current, 8, 0, 30, 0, 10));
            AddPhaseSpell(2, AddSpell(SHADOWMOON_WEAPON_MASTER_KNOCK_AWAY, Target_Current, 9, 0, 25, 0, 10));
            AddPhaseSpell(2, AddSpell(SHADOWMOON_WEAPON_MASTER_SHIELD_WALL, Target_Self, 10, 0, 35));
            AddPhaseSpell(3, AddSpell(SHADOWMOON_WEAPON_MASTER_KNOCK_AWAY, Target_Current, 9, 0, 25, 0, 10));
            AddPhaseSpell(3, AddSpell(SHADOWMOON_WEAPON_MASTER_WHIRLWIND, Target_Self, 10, 15, 35));
            AddPhaseSpell(3, AddSpell(SHADOWMOON_WEAPON_MASTER_MUTILATE, Target_Current, 8, 0, 30, 0, 10));
            mDefensiveStance = AddSpellFunc(&SpellFunc_DefensiveStance, Target_Self, 0, 0, 0);
            mBerserkerStance = AddSpellFunc(&SpellFunc_BerserkerStance, Target_Self, 0, 0, 0);
            //SetDisplayWeaponIds(0, 0)    // Sword
            _applyAura(SHADOWMOON_WEAPON_MASTER_BATTLE_STANCE);
            _applyAura(SHADOWMOON_WEAPON_MASTER_BATTLE_AURA);
        }

        void OnCombatStop(Unit* pTarget)
        {
            ParentClass::OnCombatStop(pTarget);

            if (isAlive())
            {
                _removeAura(SHADOWMOON_WEAPON_MASTER_DEFENSIVE_AURA);
                _removeAura(SHADOWMOON_WEAPON_MASTER_BERSEKER_AURA);
                //SetDisplayWeaponIds(0, 0)    // Sword
                _applyAura(SHADOWMOON_WEAPON_MASTER_BATTLE_STANCE);
                _applyAura(SHADOWMOON_WEAPON_MASTER_BATTLE_AURA);
            }
        }

        void AIUpdate()
        {
            if (GetPhase() == 1 && _getHealthPercent() <= 85)
            {
                SetPhase(2, mDefensiveStance);
                return;
            }
            if (GetPhase() == 2 && _getHealthPercent() <= 35)
            {
                SetPhase(3, mBerserkerStance);
                return;
            }

            ParentClass::AIUpdate();
        }

        SpellDesc* mDefensiveStance;
        SpellDesc* mBerserkerStance;
};

void SpellFunc_DefensiveStance(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ShadowmoonWeaponMasterAI* pWeaponMaster = (pCreatureAI != NULL) ? static_cast< ShadowmoonWeaponMasterAI* >(pCreatureAI) : NULL;
    if (pWeaponMaster != NULL)
    {
        pWeaponMaster->_removeAura(SHADOWMOON_WEAPON_MASTER_BATTLE_AURA);
        //SetDisplayWeaponIds(0, 0)    // Axe + Shield
        pWeaponMaster->_applyAura(SHADOWMOON_WEAPON_MASTER_DEFENSIVE_STANCE);
        pWeaponMaster->_applyAura(SHADOWMOON_WEAPON_MASTER_DEFENSIVE_AURA);
    }
}

void SpellFunc_BerserkerStance(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ShadowmoonWeaponMasterAI* pWeaponMaster = (pCreatureAI != NULL) ? static_cast< ShadowmoonWeaponMasterAI* >(pCreatureAI) : NULL;
    if (pWeaponMaster != NULL)
    {
        pWeaponMaster->_removeAura(SHADOWMOON_WEAPON_MASTER_DEFENSIVE_AURA);
        //SetDisplayWeaponIds(0, 0)    // Sword
        pWeaponMaster->_applyAura(SHADOWMOON_WEAPON_MASTER_BERSERKER_STANCE);
        pWeaponMaster->_applyAura(SHADOWMOON_WEAPON_MASTER_BERSEKER_AURA);
        pWeaponMaster->sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "Berserker stance! Attack them recklessly!");
    }
}


class SpellboundAttendantAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(SpellboundAttendantAI, MoonScriptCreatureAI);
        SpellboundAttendantAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SPELLBOUND_ATTENDANT_KICK, Target_Current, 9, 0, 25, 0, 10);
            AddSpell(SPELLBOUND_ATTENDANT_SLEEP, Target_RandomPlayerNotCurrent, 8, 1, 25);
        }
};


class TempleConcubineAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(TempleConcubineAI, MoonScriptCreatureAI);
        TempleConcubineAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(TEMPLE_CONCUBINE_LOVE_TAP, Target_Current, 9, 0, 25, 0, 10);
            AddSpell(TEMPLE_CONCUBINE_POLYMORPH, Target_RandomPlayerNotCurrent, 7, 1, 25);
        }
};

//////////////////////////////////////////////////////////////////////////
/////// Elemental AIs


void SpellFunc_StormBlink(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class StormFuryAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(StormFuryAI, MoonScriptCreatureAI);
        StormFuryAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mStormBlink = AddSpell(STORM_FURY_STORM_BLINK, Target_Self, 0, 1, 0);    // Mechanics was guessed
            AddSpellFunc(&SpellFunc_StormBlink, Target_RandomPlayerNotCurrent, 8, 0, 35);
        }

        void AIUpdate()
        {
            if (_unit->HasAura(STORM_FURY_STORM_BLINK))
            {
                _delayNextAttack(2000);
            }

            ParentClass::AIUpdate();
        }

        SpellDesc* mStormBlink;
};

void SpellFunc_StormBlink(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    StormFuryAI* pStormFuryAI = (pCreatureAI != NULL) ? static_cast< StormFuryAI* >(pCreatureAI) : NULL;
    if (pStormFuryAI != NULL)
    {
        Unit* pCurrentTarget = pStormFuryAI->GetUnit()->GetAIInterface()->getNextTarget();
        if (pCurrentTarget == NULL)
            return;

        if (pTarget == pCurrentTarget)
            return;

        pStormFuryAI->_clearHateList();
        pStormFuryAI->GetUnit()->GetAIInterface()->AttackReaction(pTarget, 500);
        pStormFuryAI->GetUnit()->GetAIInterface()->setNextTarget(pTarget);
        pStormFuryAI->GetUnit()->GetAIInterface()->RemoveThreatByPtr(pCurrentTarget);
        pStormFuryAI->CastSpell(pStormFuryAI->mStormBlink);
    }
}


class AqueousSurgerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AqueousSurgerAI, MoonScriptCreatureAI);
        AqueousSurgerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(AQUEOUS_SURGER_POISON_BOLT_VOLLEY, Target_Self, 7, 0, 20);
        }
};


class AqueousSpawnAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AqueousSpawnAI, MoonScriptCreatureAI);
        AqueousSpawnAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(AQUEOUS_SPAWN_MERGE, Target_Self, 7, 11, 45);
            AddSpell(AQUEOUS_SPAWN_SLUDGE_NOVA, Target_RandomPlayer, 8, 2.5, 20);
        }
};


void SpellFunc_SpawnAqueousSpawn(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class AqueousLordAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AqueousLordAI, MoonScriptCreatureAI);
        AqueousLordAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(AQUEOUS_LORD_CRASHING_WAVE, Target_Self, 8, 0, 35);
            AddSpell(AQUEOUS_LORD_VILE_SLIME, Target_RandomPlayer, 10, 0, 25);
            AddSpellFunc(&SpellFunc_SpawnAqueousSpawn, Target_Self, 10, 0, 30);
        }
};

void SpellFunc_SpawnAqueousSpawn(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    AqueousLordAI* pAqueousLordAI = (pCreatureAI != NULL) ? static_cast< AqueousLordAI* >(pCreatureAI) : NULL;
    if (pAqueousLordAI != NULL)
    {
        MoonScriptCreatureAI* pSpawnAI = pAqueousLordAI->SpawnCreature(CN_AQUEOUS_SPAWN, pCreatureAI->GetUnit()->GetPositionX(), pCreatureAI->GetUnit()->GetPositionY(), pCreatureAI->GetUnit()->GetPositionZ(), pCreatureAI->GetUnit()->GetOrientation(), false);
        if (pSpawnAI != NULL)
        {
            pSpawnAI->AggroRandomUnit(500);
            pSpawnAI->_setDespawnWhenInactive(true);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
/////// Mechanical AIs


class PromenadeSentinelAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(PromenadeSentinelAI, MoonScriptCreatureAI);
        PromenadeSentinelAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(PROMENADE_SENTINEL_L5_ARCANE_CHARGE, Target_RandomPlayer, 8, 2.7f, 35);
        }
};

//////////////////////////////////////////////////////////////////////////
/////// Uncategorized AIs


class AngeredSoulFragmentAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AngeredSoulFragmentAI, MoonScriptCreatureAI);
        AngeredSoulFragmentAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ANGERED_SOUL_FRAGMENT_ANGER, Target_Self, 10, 3, 25);
        }
};


class AshtongueFeralSpiritAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AshtongueFeralSpiritAI, MoonScriptCreatureAI);
        AshtongueFeralSpiritAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
        }
};

//////////////////////////////////////////////////////////////////////////
/////// Undead AIs


class EnslavedSoulAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EnslavedSoulAI, MoonScriptCreatureAI);
        EnslavedSoulAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature) {}

        void OnDied(Unit* mKiller)
        {
            _applyAura(ENSLAVED_SOUL_SOUL_RELEASE);            // beg core to support OnDied casts
            despawn(120000, 0);
            ParentClass::OnDied(mKiller);
        }
};


class HungeringSoulFragmentAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(HungeringSoulFragmentAI, MoonScriptCreatureAI);
        HungeringSoulFragmentAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            _applyAura(HUNGERING_SOUL_FRAGMENT_CONSUMING_STRIKES);
        }

        void OnCombatStop(Unit* pTarget)
        {
            ParentClass::OnCombatStop(pTarget);
            if (isAlive())
            {
                _applyAura(HUNGERING_SOUL_FRAGMENT_CONSUMING_STRIKES);
            }
        }
};


class ShadowyConstructAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowyConstructAI, MoonScriptCreatureAI);
        ShadowyConstructAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SHADOWY_CONSTRUCT_ATROPHY, Target_Current, 10, 0, 45, 0, 10);
        }
};


class SufferingSoulFragmentAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(SufferingSoulFragmentAI, MoonScriptCreatureAI);
        SufferingSoulFragmentAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(SUFFERING_SOUL_FRAGMENT_SOUL_BLAST, Target_Self, 8, 2, 30);
        }
};


///\todo Teron Gorefiend transforms random player into it with those abilities, but AI might be handy too (too overpowered?) - cannot check (no npc in DB)
class VangefulSpiritAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(VangefulSpiritAI, MoonScriptCreatureAI);
        VangefulSpiritAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(VANGEFUL_SPIRIT_SPIRIT_CHAINS, Target_Self, 8, 0, 25);
            AddSpell(VANGEFUL_SPIRIT_SPIRIT_LANCE, Target_RandomPlayer, 6, 0, 35);    // Current?
            AddSpell(VANGEFUL_SPIRIT_SPIRIT_SHIELD, Target_Self, 6, 0, 50);
            AddSpell(VANGEFUL_SPIRIT_SPIRIT_STRIKE, Target_Current, 9, 0, 15, 0, 10);
            AddSpell(VANGEFUL_SPIRIT_SPIRIT_VOLLEY, Target_Self, 4, 0, 40);
        }
};


class WrathboneFlayerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(WrathboneFlayerAI, MoonScriptCreatureAI);
        WrathboneFlayerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(WRATHBONE_FLAYER_CLEAVE, Target_Current, 10, 0, 15, 0, 10);
            AddSpell(WRATHBONE_FLAYER_IGNORED, Target_Current, 7, 0, 25, 0, 10);        // Current or random?
        }
};

// Bosses
//----------------------High Warlord Naj'entus--------------------------//

const uint32 CN_NAJENTUS = 22887;

const uint32 CRASHING_WAVE = 40100;  // Crashing Wave (2313-2687 Nature damage)
const uint32 NEEDLE_SPINE = 39835;  // Needle Spine (3188-4312 Damage, AoE of 2375-2625 Aura): Fires a needle spine at an enemy target.
const uint32 NEEDLE_SPINE_EXPLOSION = 39968;
const uint32 TIDAL_SHIELD = 39872;    // Tidal Shield : Impervious to normal attack and spells. Regenerating health. Will deal 8500 frost damage to the raid when broken.
const uint32 IMPALING_SPINE = 39837;
const uint32 NAJENTUS_SPINE_GO = 3264;    // Campfire so it wont crash Real go is 185584 //Did not find it in any database (including drake's go's), tested with a chair, and it works
// There are also other sounds, but Idk where they should go (mostly specials and enrage - which erange spell is that O_O)
class NajentusAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(NajentusAI);
        SP_AI_Spell spells[4];
        bool m_spellcheck[4];

        NajentusAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            nrspells = 4;

            for (uint8 i = 0; i < nrspells; i++)
            {
                m_spellcheck[i] = false;
            }

            spells[0].info = sSpellCustomizations.GetSpellInfo(CRASHING_WAVE);
            spells[0].targettype = TARGET_VARIOUS;
            spells[0].instant = false;
            spells[0].perctrigger = 10.0f;
            spells[0].attackstoptimer = 1000;
            spells[0].cooldown = 15;

            spells[1].info = sSpellCustomizations.GetSpellInfo(NEEDLE_SPINE);
            spells[1].targettype = TARGET_RANDOM_SINGLE;
            spells[1].instant = false;
            spells[1].perctrigger = 10.0f;
            spells[1].attackstoptimer = 2000;
            spells[1].cooldown = 10;
            spells[1].mindist2cast = 0.0f;
            spells[1].maxdist2cast = 60.0f;

            spells[2].info = sSpellCustomizations.GetSpellInfo(TIDAL_SHIELD);
            spells[2].targettype = TARGET_SELF;
            spells[2].instant = false;
            spells[2].perctrigger = 0.0f;
            spells[2].attackstoptimer = 2000;
            spells[2].cooldown = 105;

            spells[3].info = sSpellCustomizations.GetSpellInfo(IMPALING_SPINE);
            spells[3].targettype = TARGET_RANDOM_SINGLE;
            spells[3].instant = false;
            spells[3].perctrigger = 0.0f;
            spells[3].attackstoptimer = 2000;
            spells[3].cooldown = 20;
            spells[3].mindist2cast = 0.0f;
            spells[3].maxdist2cast = 60.0f;
        }

        void OnCombatStart(Unit* mTarget)
        {
            for (uint8 i = 0; i < nrspells; i++)
                spells[i].casttime = 0;

            uint32 t = (uint32)time(NULL);
            spells[2].casttime = t + 60;
            spells[3].casttime = t + 20;

            sendDBChatMessage(4720);     // You will die in the name of Lady Vashj!

            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);

            RemoveAIUpdateEvent();
        }

        void OnDied(Unit* mKiller)
        {
            sendDBChatMessage(4710);     // Lord Illidan will... crush you!

            RemoveAIUpdateEvent();
        }

        void OnTargetDied(Unit* mTarget)
        {
            switch (RandomUInt(2))
            {
                case 1:
                    sendDBChatMessage(4705);     // Time for you to go.
                    break;
                default:
                    {
                        sendDBChatMessage(4704); // Your success was short-lived!
                    }
            }
        }

        void AIUpdate()
        {
            // Disabled till I find way to make it dispellable like on blizz
            uint32 t = (uint32)time(NULL);

            if (t > spells[3].casttime)
            {
                switch (RandomUInt(2))
                {
                    case 1:
                        sendDBChatMessage(4702);     // Stick around!
                        break;
                    default:
                        {
                            sendDBChatMessage(4703); // I'll deal with you later.
                        }
                }
                _unit->setAttackTimer(spells[3].attackstoptimer, false);

                CastSpellOnRandomTarget(3, spells[3].mindist2cast, spells[3].maxdist2cast, 0, 100);

                spells[3].casttime = t + spells[3].cooldown;
                return;
            }

            float val = RandomFloat(100.0f);
            SpellCast(val);
        }

        void SpellCast(float val)
        {
            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                float comulativeperc = 0;
                Unit* target = NULL;
                for (uint8 i = 0; i < nrspells; i++)
                {
                    if (!spells[i].perctrigger) continue;

                    if (m_spellcheck[i])
                    {
                        target = _unit->GetAIInterface()->getNextTarget();
                        switch (spells[i].targettype)
                        {
                            case TARGET_SELF:
                            case TARGET_VARIOUS:
                                _unit->CastSpell(_unit, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_ATTACKING:
                                _unit->CastSpell(target, spells[i].info, spells[i].instant);
                                break;
                                //_unit->GetMapMgr()->GetInterface()->SpawnGameObject(NAJENTUS_SPINE_GO , target->GetPosition(), 0.207343, true, 0, 0); break;
                            case TARGET_DESTINATION:
                                _unit->CastSpellAoF(target->GetPosition(), spells[i].info, spells[i].instant);
                                break;
                            case TARGET_RANDOM_FRIEND:
                            case TARGET_RANDOM_SINGLE:
                            case TARGET_RANDOM_DESTINATION:
                                CastSpellOnRandomTarget(i, spells[i].mindist2cast, spells[i].maxdist2cast, spells[i].minhp2cast, spells[i].maxhp2cast);
                                break;
                        }
                        m_spellcheck[i] = false;
                        return;
                    }

                    uint32 t = (uint32)time(NULL);
                    if (val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger) && t > spells[i].casttime)
                    {
                        _unit->setAttackTimer(spells[i].attackstoptimer, false);
                        spells[i].casttime = t + spells[i].cooldown;
                        m_spellcheck[i] = true;
                    }
                    comulativeperc += spells[i].perctrigger;
                }
            }
        }

        void CastSpellOnRandomTarget(uint32 i, float mindist2cast, float maxdist2cast, int minhp2cast, int maxhp2cast)
        {
            if (!maxdist2cast) maxdist2cast = 100.0f;
            if (!maxhp2cast) maxhp2cast = 100;

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                std::vector<Unit*> TargetTable;
                for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                {
                    if (((spells[i].targettype == TARGET_RANDOM_FRIEND && isFriendly(_unit, (*itr))) || (spells[i].targettype != TARGET_RANDOM_FRIEND && isHostile(_unit, (*itr)) && (*itr) != _unit)) && (*itr)->IsUnit())  // isAttackable(_unit, (*itr)) &&
                    {
                        Unit* RandomTarget = NULL;
                        RandomTarget = static_cast<Unit*>(*itr);

                        if (RandomTarget == _unit->GetAIInterface()->GetMostHated() && i == 3)
                            continue;

                        if (RandomTarget->isAlive() && _unit->GetDistance2dSq(RandomTarget) >= mindist2cast * mindist2cast && _unit->GetDistance2dSq(RandomTarget) <= maxdist2cast * maxdist2cast && ((RandomTarget->GetHealthPct() >= minhp2cast && RandomTarget->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND) || (_unit->GetAIInterface()->getThreatByPtr(RandomTarget) > 0 && isHostile(_unit, RandomTarget))))
                        {
                            TargetTable.push_back(RandomTarget);
                        }
                    }
                }

                if (_unit->GetHealthPct() >= minhp2cast && _unit->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND)
                    TargetTable.push_back(_unit);

                if (!TargetTable.size())
                    return;

                auto random_index = RandomUInt(0, uint32(TargetTable.size() - 1));
                auto random_target = TargetTable[random_index];

                if (random_target == nullptr)
                    return;

                switch (spells[i].targettype)
                {
                    case TARGET_RANDOM_FRIEND:
                    case TARGET_RANDOM_SINGLE:
                        _unit->CastSpell(random_target, spells[i].info, spells[i].instant);
                        break;
                    case TARGET_RANDOM_DESTINATION:
                        _unit->CastSpellAoF(random_target->GetPosition(), spells[i].info, spells[i].instant);
                        break;
                }

                TargetTable.clear();
            }
        }

    protected:

        uint8 nrspells;
};

//------------------------------------
//          -= Supremus =-
//------------------------------------

//Abilities - Phase 1
const uint32 MOLTEN_FLAME = 40253; //Molten Flame: Blue-flamed fire sent along the ground in a straight line toward random players that deals 3325-3675 fire damage every 1 second while stood on. Ability used every 20 seconds.
const uint32 HURTFUL_STRIKE = 40126; //33813 //Hurtful Strike: A massive melee ability applied to the second highest on his aggro list. If there are no other melee targets in range, it will be performed on the main tank instead.

//Abilities - Phase 2
//Gaze: Supremus will target a random player and follow that target around for 10 seconds at 90% of normal movement speed. Switches targets after 10 seconds.
const uint32 MOLTEN_PUNCH = 40126; //Molten Punch: Once a targeted player is within a 40 yard range, Supremus will do a knockback ability for 5250 damage.
const uint32 VOLCANIC_GAZER = 42055; //Volcanic Geyser: Emoting "the ground begins to crack open", Supremus will summon small volcanoes that spit out blue flames for 4163-4837 fire damage in a 15 yard radius.

class SupremusAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(SupremusAI);
        SupremusAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            m_MoltenFlame = m_HurtfulStrike = m_MoltenPunch = m_VolcanicGazer = true;

            infoMoltenFlame = sSpellCustomizations.GetSpellInfo(MOLTEN_FLAME);
            infoHurtfulStrike = sSpellCustomizations.GetSpellInfo(HURTFUL_STRIKE);
            infoMoltenPunch =  sSpellCustomizations.GetSpellInfo(MOLTEN_PUNCH);
            infoVolcanicGazer =  sSpellCustomizations.GetSpellInfo(VOLCANIC_GAZER);

            timer = 0;
            m_phase = 0;
        }

        void OnCombatStart(Unit* mTarget)
        {
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
            sendDBChatMessage(5034);    // Bear witness to the agent of your demise! used when he kills Warden Mellichar
            timer = 0;
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
            RemoveAIUpdateEvent();
        }

        void OnDied(Unit* mKiller)
        {
            sendDBChatMessage(5042);     // I am merely one of... infinite multitudes.
            RemoveAIUpdateEvent();
        }

        void OnTargetDied(Unit* mTarget)
        {
            if (_unit->GetHealthPct() > 0)
            {
                switch (RandomUInt(1))
                {
                    case 0:
                        sendDBChatMessage(5035);     // Your fate is written.
                        break;
                    case 1:
                        sendDBChatMessage(5036);     // The chaos I have sown here is but a taste....
                        break;
                }
            }
        }

        void AIUpdate()
        {
            switch (m_phase)
            {
                case 1:
                    {
                        PhaseOne();
                    }
                    break;
                case 2:
                    {
                        PhaseTwo();
                    }
                    break;
                default:
                    {
                        m_phase = 1;
                    }
            }
        }

        void PhaseOne()
        {
            timer++;

            uint32 val = RandomUInt(1000);

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())//_unit->getAttackTarget())
            {
                if (m_MoltenFlame)
                {
                    _unit->CastSpell(_unit, infoMoltenFlame, false);
                    m_MoltenFlame = false;
                    return;
                }

                else if (m_HurtfulStrike)
                {
                    _unit->CastSpell(_unit, infoHurtfulStrike, false);
                    m_HurtfulStrike = false;
                    return;
                }

                if (val <= 500)
                {
                    _unit->setAttackTimer(6000, false);//6000
                    m_MoltenFlame = true;
                }

                else
                {
                    _unit->setAttackTimer(4000, false);//2000
                    m_HurtfulStrike = true;
                }
            }

            if (timer >= 45)
            {
                sendDBChatMessage(5041);     // We span the universe, as countless as the stars!
                timer = 0;
                m_phase = 2;
            }
        }

        void PhaseTwo()
        {
            timer++;

            uint32 val = RandomUInt(1000);

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())//_unit->getAttackTarget())
            {
                if (m_MoltenPunch)
                {
                    _unit->CastSpell(_unit, infoMoltenPunch, false);
                    m_MoltenPunch = false;
                    return;
                }

                else if (m_VolcanicGazer)
                {
                    sendDBChatMessage(4690);     // The ground begins to crack open"
                    _unit->CastSpell(_unit, infoVolcanicGazer, false);
                    m_VolcanicGazer = false;
                    return;

                }

                if (val <= 500)
                {
                    _unit->setAttackTimer(6000, false);//6000
                    m_MoltenPunch = true;
                }

                else
                {
                    _unit->setAttackTimer(4000, false);//2000
                    m_VolcanicGazer = true;
                }
            }

            if (timer >= 45)
            {
                sendDBChatMessage(5041);     // We span the universe, as countless as the stars!
                timer = 0;
                m_phase = 1;

            }
        }

    protected:
        uint32 timer;
        uint32 m_phase;
        bool m_MoltenFlame, m_HurtfulStrike, m_MoltenPunch, m_VolcanicGazer;
        SpellInfo* infoMoltenFlame, *infoHurtfulStrike, *infoMoltenPunch, *infoVolcanicGazer;
};

//------------------------------------
//    -= Shade Of Akama =-
//------------------------------------
const uint32 CN_SHADE_OF_AKAMA = 22841;
const uint32 CN_ASHTONGUE_CHANNELER = 23421;
const uint32 CN_ASHTONGUE_SORCERER = 23215;

const uint32 DEBILITATING_STRIKE = 41178;

const uint32 RAIN_OF_FIRE = 42023;

const uint32 DEBILITATING_POISON = 41978;

const uint32 SPIRIT_MEND = 42025;
const uint32 CHAIN_HEAL = 42027;

//-------------------------Gurtogg Bloodboil----------------------------//

const uint32 CN_GURTOGG_BLOODBOIL = 22948;

// Phase 1
const uint32 BLOODBOIL = 42005;
const uint32 ACIDIC_WOUND = 40481;
const uint32 DISORIENT = 32752;
const uint32 KNOCKBACK = 38576;

// Phase 2
const uint32 FEL_RAGE1 = 40604;
const uint32 FEL_RAGE2 = 40594;   // 40616 - doesn't work at all
const uint32 ACID_GEYSER = 40629;

// Spells common for both phases
const uint32 ARCING_SMASH = 40599;
const uint32 FEL_ACID_BREATH = 40595;

class GurtoggAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(GurtoggAI);
        SP_AI_Spell spells[9];
        bool m_spellcheck[9];

        GurtoggAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            nrspells = 5;

            for (uint8 i = 0; i < nrspells; i++)
            {
                m_spellcheck[i] = false;
            }

            spells[0].info = sSpellCustomizations.GetSpellInfo(ARCING_SMASH);
            spells[0].targettype = TARGET_ATTACKING;
            spells[0].instant = true;
            spells[0].perctrigger = 0.0f;
            spells[0].attackstoptimer = 1000;
            spells[0].cooldown = 10;

            spells[1].info = sSpellCustomizations.GetSpellInfo(FEL_ACID_BREATH);
            spells[1].targettype = TARGET_ATTACKING;
            spells[1].instant = true;
            spells[1].perctrigger = 10.0f;
            spells[1].attackstoptimer = 2000;
            spells[1].cooldown = 15;

            // Phase 1
            spells[2].info = sSpellCustomizations.GetSpellInfo(BLOODBOIL);
            spells[2].targettype = TARGET_VARIOUS;
            spells[2].instant = true;
            spells[2].perctrigger = 0.0f;
            spells[2].attackstoptimer = 2000;
            spells[2].cooldown = 10;

            // Not sure about this one system of casting
            spells[3].info = sSpellCustomizations.GetSpellInfo(ACIDIC_WOUND);
            spells[3].targettype = TARGET_ATTACKING;
            spells[3].instant = true;
            spells[3].perctrigger = 10.0f;
            spells[3].attackstoptimer = 2000;
            spells[3].cooldown = 15;

            spells[4].info = sSpellCustomizations.GetSpellInfo(DISORIENT);
            spells[4].targettype = TARGET_ATTACKING;
            spells[4].instant = true;
            spells[4].perctrigger = 7.0f;
            spells[4].attackstoptimer = 2000;
            spells[4].cooldown = 10;

            spells[5].info = sSpellCustomizations.GetSpellInfo(KNOCKBACK);
            spells[5].targettype = TARGET_ATTACKING;
            spells[5].instant = true;
            spells[5].perctrigger = 0.0f;
            spells[5].attackstoptimer = 2000;
            spells[5].cooldown = 15;

            // Phase 2
            spells[6].info = sSpellCustomizations.GetSpellInfo(ACID_GEYSER);
            spells[6].targettype = TARGET_DESTINATION;
            spells[6].instant = true;
            spells[6].perctrigger = 0.0f;
            spells[6].attackstoptimer = 2000;
            spells[6].cooldown = 10;

            spells[7].info = sSpellCustomizations.GetSpellInfo(FEL_RAGE1);
            spells[7].targettype = TARGET_RANDOM_SINGLE;
            spells[7].instant = true;
            spells[7].perctrigger = 0.0f;
            spells[7].attackstoptimer = 2000;
            spells[7].cooldown = 90;
            spells[7].mindist2cast = 0.0f;
            spells[7].maxdist2cast = 60.0f;

            spells[8].info = sSpellCustomizations.GetSpellInfo(FEL_RAGE2);
            spells[8].targettype = TARGET_RANDOM_SINGLE;
            spells[8].instant = true;
            spells[8].perctrigger = 0.0f;
            spells[8].attackstoptimer = 2000;
            spells[8].cooldown = 90;
            spells[8].mindist2cast = 0.0f;
            spells[8].maxdist2cast = 60.0f;

            LastThreat = 0;
            PhaseTimer = 0;
            Phase = 1;
        }

        void OnCombatStart(Unit* mTarget)
        {
            for (uint8 i = 0; i < 9; i++)
            {
                spells[i].casttime = 0;
            }

            spells[2].casttime = (uint32)time(NULL) + 10;

            sendDBChatMessage(4642);     // Horde will crush you!"
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));

            PhaseTimer = (uint32)time(NULL) + 60;
            LastThreat = 0;
            Phase = 1;
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);

            if (isAlive())
                sendDBChatMessage(4648);     //I'll rip the meat from your bones!

            RemoveAIUpdateEvent();

            LastThreat = 0;
            PhaseTimer = 0;
            Phase = 1;
        }

        void OnDied(Unit* mKiller)
        {
            sendDBChatMessage(4649);     // Aaaahrg...

            RemoveAIUpdateEvent();
        }

        void OnTargetDied(Unit* mTarget)
        {
            switch (RandomUInt(2))
            {
                case 1:
                    sendDBChatMessage(4644);     // "More! I want more!"
                    break;
                case 2:
                    sendDBChatMessage(4643);     // Time to feast!"
                    break;
            }
        }

        void AIUpdate()
        {
            uint32 t = (uint32)time(NULL);
            if (Phase == 1 && t > PhaseTimer)
            {
                spells[7].casttime = 0;

                PhaseTimer = 0;
                Phase = 2;
            }

            // Phase 1
            if (Phase == 1 && _unit->GetAIInterface()->getNextTarget() && _unit->GetCurrentSpell() == NULL)
            {
                // Bloodboil
                if (t > spells[2].casttime)
                {
                    _unit->setAttackTimer(spells[2].attackstoptimer, false);

                    _unit->CastSpell(_unit, spells[2].info, spells[2].instant);

                    spells[2].casttime = t + spells[2].cooldown;
                    return;
                }

                // Arcing Smash
                if (t > spells[0].casttime)
                {
                    _unit->setAttackTimer(spells[0].attackstoptimer, false);

                    Unit* target = _unit->GetAIInterface()->getNextTarget();
                    _unit->CastSpell(target, spells[0].info, spells[0].instant);

                    spells[0].casttime = t + spells[0].cooldown;
                    return;
                }

                // Knockback
                if (t > spells[5].casttime)
                {
                    _unit->setAttackTimer(spells[5].attackstoptimer, false);

                    Unit* target = _unit->GetAIInterface()->getNextTarget();
                    _unit->CastSpell(target, spells[5].info, spells[5].instant);

                    if (_unit->GetAIInterface()->GetSecondHated())
                        _unit->GetAIInterface()->setNextTarget(_unit->GetAIInterface()->GetSecondHated());

                    spells[5].casttime = t + spells[5].cooldown;
                    return;
                }
            }

            if (Phase == 2 && _unit->GetAIInterface()->getNextTarget() && _unit->GetCurrentSpell() == NULL)
            {
                // Fel Rage check
                if (t > spells[7].casttime - 62 && LastThreat != 0)
                {
                    Unit* RageTarget = NULL;
                    RageTarget = _unit->GetAIInterface()->getNextTarget();
                    if (RageTarget->getAuraWithId(FEL_RAGE1) && RageTarget->getAuraWithId(FEL_RAGE2))
                    {
                        _unit->GetAIInterface()->RemoveThreatByPtr(RageTarget);
                        _unit->GetAIInterface()->AttackReaction(RageTarget, LastThreat, 0);

                        LastThreat = 0;
                    }
                }

                // Acid Geyser
                if (LastThreat != 0 && RandomUInt(5) == 1 && t > spells[6].casttime)
                {
                    Unit* RageTarget = NULL;
                    RageTarget = _unit->GetAIInterface()->getNextTarget();
                    if (RageTarget->getAuraWithId(FEL_RAGE1) || RageTarget->getAuraWithId(FEL_RAGE2))
                    {
                        _unit->setAttackTimer(spells[6].attackstoptimer, false);

                        _unit->CastSpellAoF(RageTarget->GetPosition(), spells[6].info, spells[6].instant);

                        spells[6].casttime = t + spells[6].cooldown;
                        return;
                    }
                }

                // Arcing Smash
                if (t > spells[0].casttime)
                {
                    _unit->setAttackTimer(spells[0].attackstoptimer, false);

                    Unit* target = _unit->GetAIInterface()->getNextTarget();
                    _unit->CastSpell(target, spells[0].info, spells[0].instant);

                    spells[0].casttime = t + spells[0].cooldown;
                    return;
                }

                // Fel Rage - not sure about system
                if (t > spells[7].casttime)
                {
                    _unit->setAttackTimer(spells[7].attackstoptimer, false);

                    std::vector<Unit*> TargetTable;
                    for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                    {
                        if (isHostile(_unit, (*itr)) && (*itr) != _unit && (*itr)->IsUnit())
                        {
                            Unit* RandomTarget = NULL;
                            RandomTarget = static_cast<Unit*>(*itr);

                            if (RandomTarget->isAlive() && _unit->GetDistance2dSq(RandomTarget) >= 0.0f && _unit->GetDistance2dSq(RandomTarget) <= 6400.0f && _unit->GetAIInterface()->getThreatByPtr(RandomTarget) > 0 && isHostile(_unit, RandomTarget))
                            {
                                TargetTable.push_back(RandomTarget);
                            }
                        }
                    }

                    if (!TargetTable.size())
                        return;

                    auto random_index = RandomUInt(0, uint32(TargetTable.size() - 1));
                    auto random_target = TargetTable[random_index];

                    if (random_target == nullptr)
                        return;

                    _unit->CastSpell(random_target, spells[7].info, spells[7].instant);
                    _unit->CastSpell(_unit, spells[8].info, spells[8].instant);

                    LastThreat = _unit->GetAIInterface()->getThreatByPtr(random_target);

                    Unit* Target = _unit->GetAIInterface()->GetMostHated();
                    uint32 MostHatedThreat = 0;
                    if (Target)    // unneeded, but still safe check :P
                    {
                        MostHatedThreat = _unit->GetAIInterface()->getThreatByPtr(Target);
                    }

                    _unit->GetAIInterface()->AttackReaction(random_target, MostHatedThreat + 100000, 0);

                    TargetTable.clear();

                    switch (RandomUInt(2))
                    {
                        case 1:
                            sendDBChatMessage(4648);     // I'll rip the meat from your bones!"
                            break;
                        case 2:
                            _unit->PlaySoundToSet(11437);               // <babbling>
                            break;
                    }

                    spells[7].casttime = t + spells[7].cooldown;
                    spells[6].casttime = t;
                    return;
                }
            }

            float val = RandomFloat(100.0f);
            SpellCast(val);
        }

        void SpellCast(float val)
        {
            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                float comulativeperc = 0;
                Unit* target = NULL;
                for (uint8 i = 0; i < nrspells; i++)
                {
                    if ((i == 3 || i == 4) && Phase != 1)
                        continue;

                    if (!spells[i].perctrigger) continue;

                    if (m_spellcheck[i])
                    {
                        target = _unit->GetAIInterface()->getNextTarget();
                        switch (spells[i].targettype)
                        {
                            case TARGET_SELF:
                            case TARGET_VARIOUS:
                                _unit->CastSpell(_unit, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_ATTACKING:
                                _unit->CastSpell(target, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_DESTINATION:
                                _unit->CastSpellAoF(target->GetPosition(), spells[i].info, spells[i].instant);
                                break;
                            case TARGET_RANDOM_FRIEND:
                            case TARGET_RANDOM_SINGLE:
                            case TARGET_RANDOM_DESTINATION:
                                CastSpellOnRandomTarget(i, spells[i].mindist2cast, spells[i].maxdist2cast, spells[i].minhp2cast, spells[i].maxhp2cast);
                                break;
                        }
                        m_spellcheck[i] = false;
                        return;
                    }

                    uint32 t = (uint32)time(NULL);
                    if (val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger) && t > spells[i].casttime)
                    {
                        _unit->setAttackTimer(spells[i].attackstoptimer, false);
                        spells[i].casttime = t + spells[i].cooldown;
                        m_spellcheck[i] = true;
                    }
                    comulativeperc += spells[i].perctrigger;
                }
            }
        }

        void CastSpellOnRandomTarget(uint32 i, float mindist2cast, float maxdist2cast, int minhp2cast, int maxhp2cast)
        {
            if (!maxdist2cast) maxdist2cast = 100.0f;
            if (!maxhp2cast) maxhp2cast = 100;

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                std::vector<Unit*> TargetTable;
                for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                {
                    if (((spells[i].targettype == TARGET_RANDOM_FRIEND && isFriendly(_unit, (*itr))) || (spells[i].targettype != TARGET_RANDOM_FRIEND && isHostile(_unit, (*itr)) && (*itr) != _unit)) && (*itr)->IsUnit())  // isAttackable(_unit, (*itr)) &&
                    {
                        Unit* RandomTarget = NULL;
                        RandomTarget = static_cast<Unit*>(*itr);

                        if (RandomTarget->isAlive() && _unit->GetDistance2dSq(RandomTarget) >= mindist2cast * mindist2cast && _unit->GetDistance2dSq(RandomTarget) <= maxdist2cast * maxdist2cast && ((RandomTarget->GetHealthPct() >= minhp2cast && RandomTarget->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND) || (_unit->GetAIInterface()->getThreatByPtr(RandomTarget) > 0 && isHostile(_unit, RandomTarget))))
                        {
                            TargetTable.push_back(RandomTarget);
                        }
                    }
                }

                if (_unit->GetHealthPct() >= minhp2cast && _unit->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND)
                    TargetTable.push_back(_unit);

                if (!TargetTable.size())
                    return;

                auto random_index = RandomUInt(0, uint32(TargetTable.size() - 1));
                auto random_target = TargetTable[random_index];

                if (random_target == nullptr)
                    return;

                switch (spells[i].targettype)
                {
                    case TARGET_RANDOM_FRIEND:
                    case TARGET_RANDOM_SINGLE:
                        _unit->CastSpell(random_target, spells[i].info, spells[i].instant);
                        break;
                    case TARGET_RANDOM_DESTINATION:
                        _unit->CastSpellAoF(random_target->GetPosition(), spells[i].info, spells[i].instant);
                        break;
                }

                TargetTable.clear();

                if (i == 7 || i == 8)
                {
                    //Aura* aura = new Aura*(spells[7].info, 30000, _unit, RTarget);
                    //if (aura)
                    //    RTarget->AddAura(aura);
                    random_target->CastSpell(random_target, spells[7].info, spells[7].instant);

                    LastThreat = _unit->GetAIInterface()->getThreatByPtr(random_target);
                    Unit* Target = _unit->GetAIInterface()->getNextTarget();
                    random_target->GetAIInterface()->AttackReaction(random_target, _unit->GetAIInterface()->getThreatByPtr(Target) + 100000, 0);
                }
            }
        }

    protected:

        uint32 LastThreat;
        uint32 PhaseTimer;
        uint32 Phase;
        uint8 nrspells;
};

//----------------------------------------------------------------------//
//-------------------------Reliquary of Souls---------------------------//
//--------------------------Scripted by Higi----------------------------//
//----------------------------------------------------------------------//

//////////////////////////////////////////////////////////////////////////
// Essence of Suffering

const uint32 CN_ESSENCEOFSUFFERING = 23418;

const uint32 EOS_FIXATE = 40893; //40414
const uint32 EOS_FRENZY = 41305;
const uint32 EOS_SOUL_DRAIN = 41303;
const uint32 EOS_AURA_OF_SUFFERING = 41292;

class EssenceOfSufferingAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EssenceOfSufferingAI, MoonScriptCreatureAI);
        EssenceOfSufferingAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {

            AddSpell(EOS_FIXATE, Target_ClosestPlayer, 10, 0, 5);
            AddSpell(EOS_FRENZY, Target_Self, 100, 3, 60);
            AddSpell(EOS_SOUL_DRAIN, Target_RandomPlayerNotCurrent, 7, 1, 15);
            mAuraOfSuffering = AddSpell(EOS_AURA_OF_SUFFERING, Target_RandomPlayer, 0, 0, 0);

            AddEmote(Event_OnTargetDied, "Look at what you made me do!", CHAT_MSG_MONSTER_YELL, 11417);
            AddEmote(Event_OnTargetDied, "I didn't ask for this!", CHAT_MSG_MONSTER_YELL, 11418);
            AddEmote(Event_OnTargetDied, "The pain is only beginning!", CHAT_MSG_MONSTER_YELL, 11419);

            // Freed
            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11415, "Pain and suffering are all that await you.");
        }

        void OnCombatStart(Unit* mTarget)
        {
            ParentClass::OnCombatStart(mTarget);
            CastSpellNowNoScheduling(mAuraOfSuffering);
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();
            if (_getHealthPercent() <= 1)
            {
                _unit->SetHealthPct(1);
                setCanEnterCombat(false);
                _setMeleeDisabled(false);
                _setCastDisabled(true);
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9);
                _removeAllAuras();
                _removeAuraOnPlayers(EOS_AURA_OF_SUFFERING);
                MoonScriptCreatureAI* mRoS = GetNearestCreature(22856);
                if (mRoS != NULL && mRoS->isAlive())
                    moveToUnit(mRoS->GetUnit());
            }
        }

        SpellDesc* mAuraOfSuffering;
};

//////////////////////////////////////////////////////////////////////////
// Essence Of Desire

const uint32 CN_ESSENCEOFDESIRE = 23419;

const uint32 EOD_RUNE_SHIELD = 41431;
const uint32 EOD_DEADEN = 41410;
const uint32 EOD_SPIRIT_SHOCK = 41426;
const uint32 EOD_AURA_OF_DESIRE = 41350;

class EssenceOfDesireAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EssenceOfDesireAI, MoonScriptCreatureAI);
        EssenceOfDesireAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(EOD_RUNE_SHIELD, Target_Self, 6, 0, 15);
            AddSpell(EOD_DEADEN, Target_Current, 6, 1, 15);
            AddSpell(EOD_SPIRIT_SHOCK, Target_Current, 100, 1, 15);
            mAuraOfDesire = AddSpell(EOD_AURA_OF_DESIRE, Target_RandomPlayer, 0, 0, 0);

            AddEmote(Event_OnTargetDied, "Fulfilment is at hand.", CHAT_MSG_MONSTER_YELL, 11409);
            AddEmote(Event_OnTargetDied, "Yes, you'll stay with us now.", CHAT_MSG_MONSTER_YELL, 11410);
            AddEmote(Event_OnTargetDied, "Your reach exceeds your grasp.", CHAT_MSG_MONSTER_YELL, 11413);

            // Freed
            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11408, "You can have anything you desire... for a price.");
        }

        void OnCombatStart(Unit* mTarget)
        {
            ParentClass::OnCombatStart(mTarget);
            CastSpellNowNoScheduling(mAuraOfDesire);
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();
            if (_getHealthPercent() <= 1)
            {
                _unit->SetHealthPct(1);

                setCanEnterCombat(false);
                _setMeleeDisabled(false);
                _setCastDisabled(true);
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9);
                _removeAllAuras();
                _removeAuraOnPlayers(EOD_AURA_OF_DESIRE);
                MoonScriptCreatureAI* mRoS = GetNearestCreature(22856);
                if (mRoS != NULL && mRoS->isAlive())
                    moveToUnit(mRoS->GetUnit());
            }
        }

        SpellDesc* mAuraOfDesire;
};

//////////////////////////////////////////////////////////////////////////
// EssenceOfAnger

const uint32 CN_ESSENCEOFANGER = 23420;

const uint32 EOA_SEETHE = 41520;
const uint32 EOA_SOUL_SCREAM = 41545;
const uint32 EOA_SPITE = 41377;
const uint32 EOA_AURA_OF_ANGER = 41337;

class EssenceOfAngerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EssenceOfAngerAI, MoonScriptCreatureAI);
        EssenceOfAngerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mTaunt = AddSpell(EOA_SEETHE, Target_Current, 0, 0, 0); // on Taunt
            mSoulScream = AddSpell(EOA_SOUL_SCREAM, Target_Current, 6, 1, 15);
            mSoulScream->AddEmote("So foolish!", CHAT_MSG_MONSTER_YELL, 11400);
            mSpite = AddSpell(EOA_SPITE, Target_Current, 6, 0, 15);
            mSpite->AddEmote("On your knees!", CHAT_MSG_MONSTER_YELL, 11403);
            mAuraOfAnger = AddSpell(EOA_AURA_OF_ANGER, Target_RandomPlayer, 0, 0, 0);
            AddEmote(Event_OnTargetDied, "", CHAT_MSG_MONSTER_YELL, 11401);
            AddEmote(Event_OnTargetDied, "Enough, no more!", CHAT_MSG_MONSTER_YELL, 11402);
            AddEmote(Event_OnDied, "Beware, cowards!", CHAT_MSG_MONSTER_YELL, 11405);
            AddEmote(Event_OnDied, "I won't be ingored.", CHAT_MSG_MONSTER_YELL, 11404);

            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11399, "Beware - I live!");
        }

        void OnCombatStart(Unit* mTarget)
        {
            ParentClass::OnCombatStart(mTarget);
            CastSpellNowNoScheduling(mAuraOfAnger);
        }
        void OnDied(Unit* mKiller)
        {
            ParentClass::OnDied(mKiller);
            _removeAuraOnPlayers(EOD_AURA_OF_DESIRE);
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();

            if (_unit->GetAIInterface()->GetIsTaunted())
            {
                CastSpellNowNoScheduling(mTaunt);
            }
        }

        SpellDesc* mSoulScream;
        SpellDesc* mAuraOfAnger;
        SpellDesc* mTaunt;
        SpellDesc* mSpite;
};

const uint32 CN_RELIQUARY_OF_SOULS = 22856;
const uint32 ROS_SUMMON_SUFFERING = 41488;
const uint32 ROS_SUMMON_DESIRE = 41493;
const uint32 ROS_SUMMON_ANGER = 41496;

class ReliquaryOfSoulsAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ReliquaryOfSoulsAI, MoonScriptCreatureAI);
        ReliquaryOfSoulsAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mSummonSuffering = AddSpell(ROS_SUMMON_SUFFERING, Target_Self, 0, 0, 0);
            mSummonDesire = AddSpell(ROS_SUMMON_DESIRE, Target_Self, 0, 0, 0);
            mSummonAnger = AddSpell(ROS_SUMMON_ANGER, Target_Self, 0, 0, 0);
            setCanEnterCombat(true);
            _setMeleeDisabled(true);
            _setRangedDisabled(false);
            setRooted(true);
            Phase = 0;
            mEnslavedSoulTimer = -1;
            SpawnedEnsalvedSoul = false;
            DeadSoulCount = 0;
            mEoS = NULL;
            mEoD = NULL;
            mEoA = NULL;

        }

        void OnCombatStart(Unit* mTarget)
        {
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9);
            _setMeleeDisabled(false);
            _setRangedDisabled(true);

            if (Phase == 0)
            {
                _unit->Emote(EMOTE_ONESHOT_SUBMERGE);
                CastSpellNowNoScheduling(mSummonSuffering);
                Phase = 1;
            }
            RegisterAIUpdateEvent(1000);
            ParentClass:: OnCombatStart(mTarget);
        }

        void AIUpdate()
        {
            ParentClass::AIUpdate();
            switch (Phase)
            {
                case 1:
                    {

                        _unit->Emote(EMOTE_STATE_SUBMERGED_NEW);
                        mEoS = GetNearestCreature(CN_ESSENCEOFSUFFERING);
                        if (mEoS && mEoS->GetUnit() && mEoS->isAlive())
                        {
                            Creature* pEoS = mEoS->GetUnit();
                            if (pEoS->GetHealthPct() <= 1 && pEoS->CalcDistance(_unit) <= 3)
                            {
                                _unit->Emote(EMOTE_STATE_STAND);
                                mEoS->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11414, "Now what do I do?!");
                                pEoS->Emote(EMOTE_ONESHOT_SUBMERGE);
                                pEoS->Despawn(100, 0);
                                Phase = 2;
                                mEnslavedSoulTimer = _addTimer(5000);
                            }
                        }
                    }
                    break;
                case 3:
                    {
                        mEoD = GetNearestCreature(CN_ESSENCEOFDESIRE);
                        if (!mEoD || !mEoD->GetUnit())
                        {
                            _unit->Emote(EMOTE_ONESHOT_SUBMERGE);
                            CastSpellNowNoScheduling(mSummonDesire);
                            Phase = 4;
                        }
                    }
                    break;
                case 4:
                    {
                        mEoD = GetNearestCreature(CN_ESSENCEOFDESIRE);
                        if (mEoD && mEoD->GetUnit() && mEoD->isAlive())
                        {
                            Creature* pEoD = mEoD->GetUnit();
                            if (pEoD->GetHealthPct() <= 1 && pEoD->CalcDistance(_unit) <= 3)
                            {
                                _unit->Emote(EMOTE_STATE_STAND);
                                mEoD->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11413, "I'll be waiting.");
                                pEoD->Emote(EMOTE_ONESHOT_SUBMERGE);
                                pEoD->Despawn(100, 0);
                                Phase = 5;
                                mEnslavedSoulTimer = _addTimer(5000);
                            }
                            else
                            {
                                _unit->Emote(EMOTE_STATE_SUBMERGED_NEW);
                            }
                        }
                    }
                    break;
                case 6:
                    {
                        mEoA = GetNearestCreature(CN_ESSENCEOFANGER);
                        if (!mEoA || !mEoA->GetUnit())
                        {
                            CastSpellNowNoScheduling(mSummonAnger);
                            _unit->Emote(EMOTE_ONESHOT_SUBMERGE);
                            Phase = 7;
                        }
                    }
                    break;
                case 7:
                    {
                        _unit->Emote(EMOTE_STATE_SUBMERGED_NEW);
                        mEoA = GetNearestCreature(CN_ESSENCEOFANGER);
                        if (mEoA && mEoA->GetUnit() && !mEoA->GetUnit()->isAlive())
                        {
                            despawn(100, 0);
                            Phase = 8;
                        }
                    }
                    break;
                case 2:
                case 5:
                    {
                        _unit->Emote(EMOTE_STATE_STAND);
                        if (_isTimerFinished(mEnslavedSoulTimer) && !SpawnedEnsalvedSoul)
                        {
                            _removeTimer(mEnslavedSoulTimer);
                            SpawnedEnsalvedSoul = true;
                            MoonScriptCreatureAI* pSpawnedEnsalvedSoul;
                            for (uint8 i = 0; i < 10; i++)
                            {
                                pSpawnedEnsalvedSoul = SpawnCreature(CN_ENSLAVED_SOUL, _unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), _unit->GetOrientation(), false);
                                if (pSpawnedEnsalvedSoul)
                                {
                                    pSpawnedEnsalvedSoul->AggroNearestPlayer();
                                    pSpawnedEnsalvedSoul = NULL;
                                }
                            }
                            _removeTimer(mEnslavedSoulTimer);
                        }
                        if (SpawnedEnsalvedSoul)
                        {

                            Creature* creature = NULL;
                            for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                            {
                                if ((*itr)->IsCreature())
                                {
                                    creature = static_cast<Creature*>((*itr));
                                    if (creature->GetCreatureProperties()->Id == CN_ENSLAVED_SOUL && !creature->isAlive())
                                        DeadSoulCount++;
                                }
                            }
                            if (DeadSoulCount == 10)
                            {
                                Phase++;
                                SpawnedEnsalvedSoul = false;
                            }
                        }
                    }
                    break;
            }
        }

        bool SpawnedEnsalvedSoul;
        int Phase;                  // do we have negative phase?
        int DeadSoulCount;          // negative count?
        uint32 mEnslavedSoulTimer;   // negative timer?
        MoonScriptCreatureAI* mEoS;
        MoonScriptCreatureAI* mEoD;
        MoonScriptCreatureAI* mEoA;
        SpellDesc* mSummonAnger;
        SpellDesc* mSummonDesire;
        SpellDesc* mSummonSuffering;
};

//--------------------------Mother Shahraz------------------------------//

const uint32 CN_MOTHER_SHAHRAZ = 22947;

const uint32 SINFUL_BEAM = 40827;
const uint32 SINISTER_BEAM = 40859;
const uint32 VILE_BEAM = 40860;
const uint32 WICKED_BEAM = 40861;

const uint32 SABER_LASH = 40810;   // Should be 40816 but 40816 is not supported by core
const uint32 FATAL_ATTRACTION = 41001;   // 40869 - needs more scripting
const uint32 MS_ENRAGE = 40743;   // 40683
// Dunno where "spell" sounds/texts should go
class ShahrazAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(ShahrazAI);
        SP_AI_Spell spells[6];
        bool m_spellcheck[6];

        ShahrazAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            nrspells = 2;

            for (uint8 i = 0; i < nrspells; i++)
            {
                m_spellcheck[i] = false;
            }

            spells[0].info = sSpellCustomizations.GetSpellInfo(SABER_LASH);
            spells[0].targettype = TARGET_ATTACKING;
            spells[0].instant = true;
            spells[0].perctrigger = 8.0f;
            spells[0].attackstoptimer = 1000;
            spells[0].cooldown = 15;

            spells[1].info = sSpellCustomizations.GetSpellInfo(FATAL_ATTRACTION);
            spells[1].targettype = TARGET_RANDOM_SINGLE;
            spells[1].instant = true;
            spells[1].perctrigger = 10.0f;
            spells[1].attackstoptimer = 2000;
            spells[1].cooldown = 15;
            spells[1].mindist2cast = 10.0f;
            spells[1].maxdist2cast = 60.0f;

            spells[2].info = sSpellCustomizations.GetSpellInfo(SINFUL_BEAM);
            spells[2].targettype = TARGET_RANDOM_SINGLE;
            spells[2].instant = true;
            spells[2].perctrigger = 0.0f;
            spells[2].attackstoptimer = 2000;
            spells[2].cooldown = 9;
            spells[2].mindist2cast = 0.0f;
            spells[2].maxdist2cast = 80.0f;

            spells[3].info = sSpellCustomizations.GetSpellInfo(WICKED_BEAM);
            spells[3].targettype = TARGET_RANDOM_SINGLE;
            spells[3].instant = true;
            spells[3].perctrigger = 0.0f;
            spells[3].attackstoptimer = 2000;
            spells[3].cooldown = 9;
            spells[3].mindist2cast = 0.0f;
            spells[3].maxdist2cast = 80.0f;

            spells[4].info = sSpellCustomizations.GetSpellInfo(VILE_BEAM);
            spells[4].targettype = TARGET_RANDOM_SINGLE;
            spells[4].instant = true;
            spells[4].perctrigger = 0.0f;
            spells[4].attackstoptimer = 2000;
            spells[4].cooldown = 9;
            spells[4].mindist2cast = 0.0f;
            spells[4].maxdist2cast = 80.0f;

            spells[5].info = sSpellCustomizations.GetSpellInfo(SINISTER_BEAM);
            spells[5].targettype = TARGET_RANDOM_SINGLE;
            spells[5].instant = true;
            spells[5].perctrigger = 0.0f;
            spells[5].attackstoptimer = 2000;
            spells[5].cooldown = 9;
            spells[5].mindist2cast = 0.0f;
            spells[5].maxdist2cast = 80.0f;

            Enraged = false;
            AuraChange = 0;
            SoundTimer = 0;
        }

        void OnCombatStart(Unit* mTarget)
        {
            sendDBChatMessage(4653);     //So, business... or pleasure?"

            for (uint8 i = 0; i < 6; i++)
                spells[i].casttime = 0;

            AuraChange = (uint32)time(NULL) + 15;
            Enraged = false;
            SoundTimer = 5;

            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);

            RemoveAIUpdateEvent();
        }

        void OnDied(Unit* mKiller)
        {
            sendDBChatMessage(4660);     // I wasn't finished.
            RemoveAIUpdateEvent();
        }

        void OnTargetDied(Unit* mTarget)
        {
            switch (RandomUInt(2))
            {
                case 1:
                    sendDBChatMessage(4658);     // So much for a happy ending.
                    break;
                case 2:
                    sendDBChatMessage(4657);     // Easy come, easy go.
                    break;
            }
        }

        void AIUpdate()
        {
            SoundTimer++;
            if (_unit->GetAIInterface()->GetIsTaunted() && SoundTimer > 10)
            {
                switch (RandomUInt(3))
                {
                    case 1:
                        sendDBChatMessage(4651);     // I'm not impressed.
                        break;
                    case 2:
                        sendDBChatMessage(4652);     // Enjoying yourselves?
                        break;
                    default:
                        sendDBChatMessage(4650);     // You play, you pay.
                        break;
                }

                SoundTimer = 0;
            }

            if (!Enraged && _unit->GetHealthPct() <= 20)
            {
                sendDBChatMessage(4659);     // Stop toying with my emotions!
                _unit->CastSpell(_unit, MS_ENRAGE, true);

                Enraged = true;
            }

            uint32 t = (uint32)time(NULL);

            // In normal way it is applied to players all around enemy caster =/
            if (t > AuraChange)
            {
                uint32 SpellId = 0;
                switch (RandomUInt(6))
                {
                    case 1:
                        SpellId = 40891;    // Arcane
                        break;
                    case 2:
                        SpellId = 40882;    // Fire
                        break;
                    case 3:
                        SpellId = 40896;    // Frost
                        break;
                    case 4:
                        SpellId = 40897;    // Holy
                        break;
                    case 5:
                        SpellId = 40883;    // Nature
                        break;
                    case 6:
                        SpellId = 40880;    // Shadow
                        break;
                    default:
                        {
                            SpellId = 40880;    // Shadow
                        }
                }

                //_unit->CastSpell(_unit, SpellId, true);
                Aura* aura = sSpellFactoryMgr.NewAura(sSpellCustomizations.GetSpellInfo(SpellId), (uint32)15000, _unit, _unit);
                _unit->AddAura(aura);

                AuraChange = t + 15;
            }

            if (t > spells[2].casttime)
            {
                uint32 SpellId = RandomUInt(4) + 1;
                if (SpellId < 2 || SpellId > 5)
                    SpellId = 5;

                _unit->setAttackTimer(spells[2].attackstoptimer, false);

                CastSpellOnRandomTarget(SpellId, spells[2].mindist2cast, spells[2].maxdist2cast, 0, 100);

                spells[2].casttime = t + spells[2].cooldown;
                return;
            }

            float val = RandomFloat(100.0f);
            SpellCast(val);
        }

        void SpellCast(float val)
        {
            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                float comulativeperc = 0;
                Unit* target = NULL;
                for (uint8 i = 0; i < nrspells; i++)
                {
                    if (!spells[i].perctrigger) continue;

                    if (m_spellcheck[i])
                    {
                        target = _unit->GetAIInterface()->getNextTarget();
                        switch (spells[i].targettype)
                        {
                            case TARGET_SELF:
                            case TARGET_VARIOUS:
                                _unit->CastSpell(_unit, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_ATTACKING:
                                _unit->CastSpell(target, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_DESTINATION:
                                _unit->CastSpellAoF(target->GetPosition(), spells[i].info, spells[i].instant);
                                break;
                            case TARGET_RANDOM_FRIEND:
                            case TARGET_RANDOM_SINGLE:
                            case TARGET_RANDOM_DESTINATION:
                                CastSpellOnRandomTarget(i, spells[i].mindist2cast, spells[i].maxdist2cast, spells[i].minhp2cast, spells[i].maxhp2cast);
                                break;
                        }
                        m_spellcheck[i] = false;
                        return;
                    }

                    uint32 t = (uint32)time(NULL);
                    if (val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger) && t > spells[i].casttime)
                    {
                        _unit->setAttackTimer(spells[i].attackstoptimer, false);
                        spells[i].casttime = t + spells[i].cooldown;
                        m_spellcheck[i] = true;
                    }
                    comulativeperc += spells[i].perctrigger;
                }
            }
        }

        void CastSpellOnRandomTarget(uint32 i, float mindist2cast, float maxdist2cast, int minhp2cast, int maxhp2cast)
        {
            if (!maxdist2cast) maxdist2cast = 100.0f;
            if (!maxhp2cast) maxhp2cast = 100;

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                std::vector<Unit*> TargetTable;
                for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                {
                    if (((spells[i].targettype == TARGET_RANDOM_FRIEND && isFriendly(_unit, (*itr))) || (spells[i].targettype != TARGET_RANDOM_FRIEND && isHostile(_unit, (*itr)) && (*itr) != _unit)) && (*itr)->IsUnit())  // isAttackable(_unit, (*itr)) &&
                    {
                        Unit* RandomTarget = NULL;
                        RandomTarget = static_cast<Unit*>(*itr);

                        if (RandomTarget->isAlive() && _unit->GetDistance2dSq(RandomTarget) >= mindist2cast * mindist2cast && _unit->GetDistance2dSq(RandomTarget) <= maxdist2cast * maxdist2cast && ((RandomTarget->GetHealthPct() >= minhp2cast && RandomTarget->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND) || (_unit->GetAIInterface()->getThreatByPtr(RandomTarget) > 0 && isHostile(_unit, RandomTarget))))
                        {
                            TargetTable.push_back(RandomTarget);
                        }
                    }
                }

                if (_unit->GetHealthPct() >= minhp2cast && _unit->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND)
                    TargetTable.push_back(_unit);

                if (!TargetTable.size())
                    return;

                auto random_index = RandomUInt(0, uint32(TargetTable.size() - 1));
                auto random_target = TargetTable[random_index];

                if (random_target == nullptr)
                    return;

                switch (spells[i].targettype)
                {
                    case TARGET_RANDOM_FRIEND:
                    case TARGET_RANDOM_SINGLE:
                        _unit->CastSpell(random_target, spells[i].info, spells[i].instant);
                        break;
                    case TARGET_RANDOM_DESTINATION:
                        _unit->CastSpellAoF(random_target->GetPosition(), spells[i].info, spells[i].instant);
                        break;
                }

                TargetTable.clear();
            }
        }

    protected:

        uint32 SoundTimer;
        uint32 AuraChange;
        bool Enraged;
        uint8 nrspells;
};

//-----------------------The Illidari Council---------------------------//

//------------------------------------
//    -= Gathios the Shatterer =-
//------------------------------------
const uint32 CN_GATHIOS_THE_SHATTERER = 22949;

typedef std::vector<Creature*> EncounterVector;

const uint32 HAMMER_OF_JUSTICE = 41468;
const uint32 SEAL_OF_COMMAND = 41469;
const uint32 SEAL_OF_BLOOD = 41459;
const uint32 CONSECRATION = 41541;
const uint32 BLESSING_OF_SPELL_WARDING = 41451;
const uint32 BLESSING_OF_PROTECTION = 41450;
const uint32 CHROMATIC_RESISTANCE_AURA = 41453;
const uint32 DEVOTION_AURA = 41452;

class GathiosAI : public MoonScriptCreatureAI
{
    public:
        MOONSCRIPT_FACTORY_FUNCTION(GathiosAI, MoonScriptCreatureAI);
        GathiosAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(HAMMER_OF_JUSTICE, Target_RandomPlayer, 15, 0, 14);
            AddSpell(SEAL_OF_COMMAND, Target_Self, 15, 0, 30);
            AddSpell(SEAL_OF_BLOOD, Target_Self, 15, 0, 30);
            AddSpell(CONSECRATION, Target_Self, 15, 0, 30);
            AddSpell(BLESSING_OF_SPELL_WARDING, Target_RandomFriendly, 15, 0, 60);
            AddSpell(BLESSING_OF_PROTECTION, Target_RandomFriendly, 15, 0, 60);

            AddEmote(Event_OnTargetDied, "Selama amor'anore!", CHAT_MSG_MONSTER_YELL, 11423);
            AddEmote(Event_OnCombatStart, "I have better things to do!", CHAT_MSG_MONSTER_YELL, 11422);
            AddEmote(Event_OnDied, "Lord Illidan... I...", CHAT_MSG_MONSTER_YELL, 11425);
        }

        void OnCombatStart(Unit* pTarget)
        {
            _applyAura(DEVOTION_AURA);
            ParentClass::OnCombatStart(pTarget);
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            DealDamageToFriends(fAmount, _unit->GetEntry());
        }

        void AddEncounterCreature(Creature* pCreature)
        {
            mEncounterVector.push_back(pCreature);
        }

        void DealDamageToFriends(uint32 val, uint32 pCreatureEntry)
        {
            for (std::vector<Creature*>::iterator itr = mEncounterVector.begin(); itr != mEncounterVector.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive() && (*itr)->GetEntry() != pCreatureEntry)
                {
                    (*itr)->DealDamage((*itr), val, 0, 0, 0);
                }
            }

            if (isAlive() && _unit->GetEntry() != pCreatureEntry)
                _unit->DealDamage(_unit, val, 0, 0, 0);
        }

        void Destroy()
        {
            mEncounterVector.clear();
            delete this;
        }

    private:
        EncounterVector mEncounterVector;
};

//------------------------------------
//    -=Veras Darkshadow  =-
//------------------------------------
const uint32 CN_VERAS_DARKSHADOW = 22952;

const uint32 DEADLY_POISON = 41485;
const uint32 ENVENOM = 41487;
const uint32 VANISH = 41476;

class VerasAI : public MoonScriptCreatureAI
{
    public:
        MOONSCRIPT_FACTORY_FUNCTION(VerasAI, MoonScriptCreatureAI);
        VerasAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DEADLY_POISON, Target_RandomPlayer, 15.0f, 0, 20);
            AddSpell(ENVENOM, Target_RandomPlayer, 15.0f, 0, 20);

            AddEmote(Event_OnTargetDied, "Valiant Effort.", CHAT_MSG_MONSTER_YELL, 11525);
            AddEmote(Event_OnDied, "You got lucky!", CHAT_MSG_MONSTER_YELL, 11527);
            AddEmote(Event_OnCombatStart, "You wish to test me?", CHAT_MSG_MONSTER_YELL, 11524);

            pGethois = static_cast< GathiosAI* >(GetNearestCreature(CN_GATHIOS_THE_SHATTERER));
            if (pGethois != NULL)
                pGethois->AddEncounterCreature(_unit);
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            pGethois->DealDamageToFriends(fAmount, _unit->GetEntry());
        }

        GathiosAI* pGethois;
};

//------------------------------------
//    -= High Nethermancer Zerevor  =-
//------------------------------------
const uint32 CN_HIGH_NETHERMANCER_ZEREVOR = 22950;

const uint32 ARCANE_BOLT = 41483;
const uint32 BLIZZARD = 41482;
const uint32 FLAMESTRIKE = 41481;
const uint32 ARCANE_EXPLOSION = 29973;
const uint32 DAMPEN_MAGIC = 41478;

class ZerevorAI : public MoonScriptCreatureAI
{
    public:
        MOONSCRIPT_FACTORY_FUNCTION(ZerevorAI, MoonScriptCreatureAI);
        ZerevorAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(ARCANE_BOLT, Target_Current, 65.0f, 2, 6);
            AddSpell(BLIZZARD, Target_RandomDestination, 25.0f, 0, 16);
            AddSpell(FLAMESTRIKE, Target_RandomDestination, 10.0f, 1.5f, 25);
            AddSpell(DAMPEN_MAGIC, Target_Self, 10, 0, 60);
            AddSpell(ARCANE_EXPLOSION, Target_RandomPlayer, 15, 0, 6);

            AddEmote(Event_OnCombatStart, "Common... such a crude language. Bandal!", CHAT_MSG_MONSTER_YELL, 11440);
            AddEmote(Event_OnTargetDied, "Shorel'aran.", CHAT_MSG_MONSTER_YELL, 11441);

            pGethois = static_cast< GathiosAI* >(GetNearestCreature(CN_GATHIOS_THE_SHATTERER));
            if (pGethois != NULL)
                pGethois->AddEncounterCreature(_unit);
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            pGethois->DealDamageToFriends(fAmount, _unit->GetEntry());
        }

        GathiosAI* pGethois;
};

//------------------------------------
//    -= Lady Malande =-
//------------------------------------
const uint32 CN_LADY_MALANDE = 22951;

const uint32 DIVINE_WRATH = 41472;
const uint32 REFLECTIVE_SHIELD = 41475;
const uint32 EMPOWERED_SMITE = 41471;
const uint32 CIRCLE_OF_HEALING = 41455;

class MalandeAI : public MoonScriptCreatureAI
{
    public:
        MOONSCRIPT_FACTORY_FUNCTION(MalandeAI, MoonScriptCreatureAI);
        MalandeAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            AddSpell(DIVINE_WRATH, Target_RandomPlayer, 15, 0, 14);
            AddSpell(REFLECTIVE_SHIELD, Target_Self, 10, 0, 30);
            AddSpell(EMPOWERED_SMITE, Target_RandomPlayer, 30, 2.0f, 6);
            AddSpell(CIRCLE_OF_HEALING, Target_RandomFriendly, 30, 2.5f, 18);

            AddEmote(Event_OnCombatStart, "Flee or die!", CHAT_MSG_MONSTER_YELL, 11482);
            AddEmote(Event_OnDied, "Destiny... awaits!", CHAT_MSG_MONSTER_YELL, 11485);
            AddEmote(Event_OnTargetDied, "My work is done.", CHAT_MSG_MONSTER_YELL, 11483);

            pGethois = static_cast< GathiosAI* >(GetNearestCreature(CN_GATHIOS_THE_SHATTERER));
            if (pGethois != NULL)
                pGethois->AddEncounterCreature(_unit);
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            if (pGethois != NULL)
                pGethois->DealDamageToFriends(fAmount, _unit->GetEntry());
        }

        GathiosAI* pGethois;
};

//--------------------------Teron Gorefiend-----------------------------//

const uint32 CN_TERON_GOREFIEND = 22871;

const uint32 DOOM_BLOSSOM = 40188;   // needs additional creature in DB
const uint32 CRUSHING_SHADOWS = 40243;
const uint32 INCINERATE = 40239;
const uint32 SHADOW_OF_DEATH = 40251;    // need further scripting?
// Dunno where other sounds should go
class TeronGorefiendAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(TeronGorefiendAI);
        SP_AI_Spell spells[4];
        bool m_spellcheck[4];

        TeronGorefiendAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            nrspells = 2;
            for (uint8 i = 0; i < nrspells; i++)
            {
                m_spellcheck[i] = false;

            }

            spells[0].info = sSpellCustomizations.GetSpellInfo(DOOM_BLOSSOM); //summon visual effect
            spells[0].targettype = TARGET_SELF;
            spells[0].instant = true;
            spells[0].perctrigger = 8.0f;
            spells[0].attackstoptimer = 1000;
            spells[0].cooldown = 25;

            spells[1].info = sSpellCustomizations.GetSpellInfo(CRUSHING_SHADOWS);
            spells[1].targettype = TARGET_VARIOUS;
            spells[1].instant = false;
            spells[1].perctrigger = 10.0f;
            spells[1].attackstoptimer = 1000;
            spells[1].cooldown = 20;

            spells[2].info = sSpellCustomizations.GetSpellInfo(INCINERATE);
            spells[2].targettype = TARGET_RANDOM_SINGLE;
            spells[2].instant = true;
            spells[2].perctrigger = 0.0f;
            spells[2].attackstoptimer = 1000;
            spells[2].cooldown = 20;
            spells[2].mindist2cast = 0.0f;
            spells[2].maxdist2cast = 45.0f;

            spells[3].info = sSpellCustomizations.GetSpellInfo(SHADOW_OF_DEATH);
            spells[3].targettype = TARGET_RANDOM_SINGLE;
            spells[3].instant = false;
            spells[3].perctrigger = 0.0f;
            spells[3].attackstoptimer = 2000;
            spells[3].cooldown = 30;
            spells[3].mindist2cast = 0.0f;
            spells[3].maxdist2cast = 60.0f;
        }

        void OnCombatStart(Unit* mTarget)
        {
            sendDBChatMessage(4692);     // Vengeance is mine!

            for (uint8 i = 0; i < 4; i++)
                spells[i].casttime = 0;

            uint32 t = (uint32)time(NULL);
            spells[2].casttime = t + spells[2].cooldown;
            spells[3].casttime = t + 10;

            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnTargetDied(Unit* mTarget)
        {
            if (_unit->GetHealthPct() > 0)
            {
                switch (RandomUInt(2))
                {
                    case 1:
                        sendDBChatMessage(4694);     // It gets worse...
                        break;
                    default:
                        sendDBChatMessage(4693);     // I have use for you!
                        break;
                }
            }
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);

            RemoveAIUpdateEvent();
        }

        void OnDied(Unit* mKiller)
        {
            sendDBChatMessage(4700);     // The wheel...spins...again....
            RemoveAIUpdateEvent();
        }

        void AIUpdate()
        {
            uint32 t = (uint32)time(NULL);
            if (t > spells[2].casttime)
            {
                _unit->setAttackTimer(spells[2].attackstoptimer, false);

                CastSpellOnRandomTarget(2, spells[2].mindist2cast, spells[2].maxdist2cast, 0, 100);

                spells[2].casttime = t + spells[2].cooldown;
            }

            if (t > spells[3].casttime)
            {
                _unit->setAttackTimer(spells[3].attackstoptimer, false);

                CastSpellOnRandomTarget(3, spells[3].mindist2cast, spells[3].maxdist2cast, 0, 100);

                spells[3].casttime = t + spells[3].cooldown;
            }

            float val = RandomFloat(100.0f);
            SpellCast(val);
        }

        void SpellCast(float val)
        {
            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                float comulativeperc = 0;
                Unit* target = NULL;
                for (uint8 i = 0; i < nrspells; i++)
                {
                    if (!spells[i].perctrigger) continue;

                    if (m_spellcheck[i])
                    {
                        target = _unit->GetAIInterface()->getNextTarget();
                        switch (spells[i].targettype)
                        {
                            case TARGET_SELF:
                            case TARGET_VARIOUS:
                                _unit->CastSpell(_unit, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_ATTACKING:
                                _unit->CastSpell(target, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_DESTINATION:
                                _unit->CastSpellAoF(target->GetPosition(), spells[i].info, spells[i].instant);
                                break;
                            case TARGET_RANDOM_FRIEND:
                            case TARGET_RANDOM_SINGLE:
                            case TARGET_RANDOM_DESTINATION:
                                CastSpellOnRandomTarget(i, spells[i].mindist2cast, spells[i].maxdist2cast, spells[i].minhp2cast, spells[i].maxhp2cast);
                                break;
                        }
                        m_spellcheck[i] = false;
                        return;
                    }

                    uint32 t = (uint32)time(NULL);
                    if (val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger) && t > spells[i].casttime)
                    {
                        _unit->setAttackTimer(spells[i].attackstoptimer, false);
                        spells[i].casttime = t + spells[i].cooldown;
                        m_spellcheck[i] = true;
                    }
                    comulativeperc += spells[i].perctrigger;
                }
            }
        }

        void CastSpellOnRandomTarget(uint32 i, float mindist2cast, float maxdist2cast, int minhp2cast, int maxhp2cast)
        {
            if (!maxdist2cast) maxdist2cast = 100.0f;
            if (!maxhp2cast) maxhp2cast = 100;

            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                std::vector<Unit*> TargetTable;
                for (std::set<Object*>::iterator itr = _unit->GetInRangeSetBegin(); itr != _unit->GetInRangeSetEnd(); ++itr)
                {
                    if (((spells[i].targettype == TARGET_RANDOM_FRIEND && isFriendly(_unit, (*itr))) || (spells[i].targettype != TARGET_RANDOM_FRIEND && isHostile(_unit, (*itr)) && (*itr) != _unit)) && (*itr)->IsUnit())  // isAttackable(_unit, (*itr)) &&
                    {
                        Unit* RandomTarget = NULL;
                        RandomTarget = static_cast<Unit*>(*itr);

                        if (RandomTarget->isAlive() && _unit->GetDistance2dSq(RandomTarget) >= mindist2cast * mindist2cast && _unit->GetDistance2dSq(RandomTarget) <= maxdist2cast * maxdist2cast && ((RandomTarget->GetHealthPct() >= minhp2cast && RandomTarget->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND) || (_unit->GetAIInterface()->getThreatByPtr(RandomTarget) > 0 && isHostile(_unit, RandomTarget))))
                        {
                            TargetTable.push_back(RandomTarget);
                        }
                    }
                }

                if (_unit->GetHealthPct() >= minhp2cast && _unit->GetHealthPct() <= maxhp2cast && spells[i].targettype == TARGET_RANDOM_FRIEND)
                    TargetTable.push_back(_unit);

                if (!TargetTable.size())
                    return;

                auto random_index = RandomUInt(0, uint32(TargetTable.size() - 1));
                auto random_target = TargetTable[random_index];

                if (random_target == nullptr)
                    return;

                switch (spells[i].targettype)
                {
                    case TARGET_RANDOM_FRIEND:
                    case TARGET_RANDOM_SINGLE:
                        _unit->CastSpell(random_target, spells[i].info, spells[i].instant);
                        break;
                    case TARGET_RANDOM_DESTINATION:
                        _unit->CastSpellAoF(random_target->GetPosition(), spells[i].info, spells[i].instant);
                        break;
                }

                TargetTable.clear();
            }
        }

    protected:

        uint8 nrspells;
};

//Shade of AkamaAI
//\todo all these texts seems to be incorrect... is the creature entry correct?

const uint32 SINFUL_BEAM0 = 00000;

class ShadeofakamaAI : public CreatureAIScript
{
    public:
        ADD_CREATURE_FACTORY_FUNCTION(ShadeofakamaAI);
        SP_AI_Spell spells[1];
        bool m_spellcheck[1];

        ShadeofakamaAI(Creature* pCreature) : CreatureAIScript(pCreature)
        {
            nrspells = 1;
            for (uint8 i = 0; i < nrspells; i++)
            {
                m_spellcheck[i] = false;

            }
            spells[0].info = sSpellCustomizations.GetSpellInfo(SINFUL_BEAM0);
            spells[0].targettype = TARGET_VARIOUS;
            spells[0].instant = false;
            spells[0].perctrigger = 0.0f;
            spells[0].attackstoptimer = 2000;

            hm = 0;
        }

        void OnCombatStart(Unit* mTarget)
        {
            hm = 100;
            RegisterAIUpdateEvent(_unit->GetBaseAttackTime(MELEE));
        }

        void OnTargetDied(Unit* mTarget)
        {
            if (_unit->GetHealthPct() > 0)
            {
                switch (RandomUInt(2))
                {
                    case 0:
                        _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I will not last much longer...");
                        _unit->PlaySoundToSet(11385);
                        break;
                    case 1:
                        _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "It is time to free your soul from Illidan's grasp!");
                        //_unit->PlaySoundToSet(11510);
                        break;
                    default:
                        break;

                }
            }
        }

        void OnCombatStop(Unit* mTarget)
        {
            hm = 100;
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
            RemoveAIUpdateEvent();
        }

        void OnDied(Unit* mKiller)
        {
            Creature* cre = NULL;
            _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I don't want to go back!");
            _unit->PlaySoundToSet(11420);
            cre = spawnCreature(22990, 643.741f, 305.852f, 271.689f, 0.00628f);
            //cre->GetAIInterface()->setOutOfCombatRange(30000);
            RemoveAIUpdateEvent();
        }

        void AIUpdate()
        {
            if (_unit->GetHealthPct() <= 85 && hm == 100)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 2; i++)
                {
                    cre = spawnCreature(23421, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 85;
            }
            else if (_unit->GetHealthPct() <= 70 && hm == 85)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 2; i++)
                {
                    cre = spawnCreature(23215, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 70;
            }
            else if (_unit->GetHealthPct() <= 55 && hm == 70)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 2; i++)
                {
                    cre = spawnCreature(23216, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 55;
            }
            else if (_unit->GetHealthPct() <= 40 && hm == 55)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 2; i++)
                {
                    cre = spawnCreature(23523, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 40;
            }
            else if (_unit->GetHealthPct() <= 25 && hm == 40)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 5; i++)
                {
                    cre = spawnCreature(23318, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 25;
            }
            else if (_unit->GetHealthPct() <= 10 && hm == 25)
            {
                Creature* cre = NULL;
                for (uint8 i = 0; i < 5; i++)
                {
                    cre = spawnCreature(23524, _unit->GetPosition());
                    if (cre)
                        cre->GetAIInterface()->setOutOfCombatRange(30000);
                }
                _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!");
                _unit->PlaySoundToSet(10999);
                hm = 10;
            }
            else
            {
                float val = RandomFloat(100.0f);
                SpellCast(val);
            }
        }

        void SpellCast(float val)
        {
            if (_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->getNextTarget())
            {
                float comulativeperc = 0;
                Unit* target = NULL;
                for (uint8 i = 0; i < nrspells; i++)
                {
                    if (!spells[i].perctrigger) continue;

                    if (m_spellcheck[i])
                    {
                        target = _unit->GetAIInterface()->getNextTarget();
                        switch (spells[i].targettype)
                        {
                            case TARGET_SELF:
                            case TARGET_VARIOUS:
                                _unit->CastSpell(_unit, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_ATTACKING:
                                _unit->CastSpell(target, spells[i].info, spells[i].instant);
                                break;
                            case TARGET_DESTINATION:
                                _unit->CastSpellAoF(target->GetPosition(), spells[i].info, spells[i].instant);
                                break;
                        }

                        if (spells[i].speech != "")
                        {
                            _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
                            _unit->PlaySoundToSet(spells[i].soundid);
                        }

                        m_spellcheck[i] = false;
                        return;
                    }

                    if (val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
                    {
                        _unit->setAttackTimer(spells[i].attackstoptimer, false);
                        m_spellcheck[i] = true;
                    }
                    comulativeperc += spells[i].perctrigger;
                }
            }
        }

    protected:

        uint8 nrspells;
        int hm;
};

//------------------------------------------------------------------------------------------------------------------//
//----------------------------------------Illidan Stormrage Encounter-----------------------------------------------//
//------------------------------------------Percent done: 85% - 90+%-------------------------------------------------//
//--------------------------------------------Scripted by: M4ksiu---------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------//
//-----------------Script is totally private and none of outside forces were involved in its creation---------------//
//--It will remain like that until author decides what to do to not give people opportunity to take credits for it--//
//-----------------------------------For more informations ask script author----------------------------------------//
//------------------------------------------------------------------------------------------------------------------//

/*
 * TO DO List:
 * 1) Fix Maiev positioning during Demon Form encounter                                            *DONE*                +
 * 2) Fix Maiev weapon wielding during dialogs and fight                                        *PARTLY DONE*        *
 * 3) Fix Illidan weapon appearing during Shadow Demons summoning                                *DONE*                +
 * 4) Fix not casting spells by Illidan in phases 1, 3                                            *DONE*                +
 * 5) Add demon cursing after Akama runs away                                                    *DONE*                +
 * 6) Fix trigger channeling during trap execution                                                                    *
 * 7) Maiev yelling when Illidan is caged                                                        *DONE*                +
 * 8) Move Illidan yelling from Maiev class to Illidan's                                        *DONE*                +
 * 9) Add aura for Shadow Demons                                                                *DONE*                +
 * 10) Recheck texts and their purposes/palces                                                    *PARTLY DONE*        *
 * 11) Adjust Eye Beam movement timers                                                            *DONE*                *
 * 12) Adjust Database data - about speed, damage, health etc.                                    *PARTLY DONE*        +
 * 13) Prevent Maiev from taking aggro of Illidan                                                *PARTLY*            +
 * 14) Add Immunities                                                                            *HANDLED BY CORE*    +
 * 15) Adjust Enrage timer with trap timer to prevent dealing massive damage                    *DONE*                +
 * 16) Adjust Trap Cage behavior                                                                *DONE W/O ANIM PROB**
 * 17) Adjust Flame Burst spell and its mechanism                                                *DONE*                +
 * 18) Adjust Flame Burst timer                                                                    *SHOULD BE DONE*    +
 * 19) Fix Maiev facing during Demon Form encounter                                                *DONE*                *
 * 20) Find Maiev proper knife                                                                                        *
 * 21) Add Flame of Azzinoth check to charge target only by one FoA???                            *UNKNOWN SITUATION*    *
 * 22) Add 'snow' that appears in Demon phase                                                                        *
 * 23) Fix eventual animation problems                                                                                *
 * 24) Fix problem when sometimes trap catches Illidan too far and rest of it can't be exe.        *SHOULD BE FIXED*    +
 * 25) Check if Parasitic Shadowfiends should attack random target after appearing                *DONE*                +
 * 26) Inspect why Cage Trap crashes server    so often (mostly on spawning or taking Gameobject)    *FIXED?*            +
 * 27) Fix trigger aggroing - they stop in place                                                *INVALID?*            +
 * 28) Fix Akama not appearing in death scene                                                    *DONE*                +
 * 29) Change mechanics of Blaze effect (should be spawned after using that Flame Blast spell    *DONE*                +
 * 30) Check core mess with multi-npc spawning by spell
 * 31) Fix triggers falling through floor or not being spawned correctly                        *DONE FOR EYE BLAST**
 * 32) Check what happens with Akama when he runs away and when he should exactly come back
 * 33) Check timer of Shadow Demon spawning (according to WoWWiki.com)                            *COULDN'T FIND INFO**
 */

// Structures

struct Transformation
{
    uint32 mTimer;
    uint32 mEmote;
    uint32 mEmoteType;
    const char* mText;
    uint32 mSoundId;
    uint32 mAura;
    uint32 mUnAura;
    bool mEquipWeapons;
};

// Transformations
// Emote Types - 0 (Oneshot), 1 (Emotestate)

static Transformation Ascend[] =
{
    { 250 * 5, 403, 1, "", 0, 0, 0, false },
    { 250 * 8, 404, 1, "Behold the power... of the demon within!", 11475, 0, 0, false },
    { 250 * 20, 404, 1, "", 0, 40506, 0, false },
    { 250 * 10, 405, 1, "", 0, 0, 0, false },
    { 250 * 10, 405, 1, "", 0, 0, 0, false },                // additional code inside function
    { 250 * 8, 0, 1, "", 0, 0, 0, false },                    // additional code inside function
};

static Transformation Descend[] =
{
    { 250 * 7, 403, 1, "", 0, 0, 0, false },
    { 250 * 3, 404, 1, "", 0, 0, 0, false },
    { 250 * 24, 404, 1, "", 0, 0, 40506, false },            // additional code inside function
    { 250 * 6, 405, 1, "", 0, 0, 0, false },
    { 250 * 4, 405, 1, "", 0, 0, 0, true },
    { 250 * 8, 0, 1, "", 0, 0, 0, true },                    // additional code inside function
};

// Paths
// Eye Beam stuff
static Movement::Location EyeBeamPaths[] =
{
    { 642.240601f, 297.297180f, 354.423401f },
    { 641.197449f, 314.330963f, 354.300262f },
    { 657.239807f, 256.925568f, 353.996094f },
    { 657.913330f, 353.562775f, 353.996185f },
    { 706.751343f, 298.312683f, 354.653809f },
    { 706.593262f, 310.011475f, 354.693848f },
    { 706.592407f, 343.425568f, 353.996124f },
    { 707.019043f, 270.441772f, 353.996063f }
};

// Akama stuff
static Movement::Location ToIllidan[] =
{
    {  },
    { 660.248596f, 330.695679f, 271.688110f, 1.243284f },
    { 671.172241f, 353.273193f, 271.689453f, 1.022600f },
    { 694.227783f, 379.461365f, 271.792145f, 6.232135f },
    { 724.159973f, 373.263275f, 282.614349f, 5.324218f },
    { 747.034973f, 335.668274f, 307.047150f, 4.972365f },
    { 755.477234f, 304.194580f, 312.167328f, 6.276120f },
    { 771.809753f, 303.744873f, 313.563507f, 6.265894f },
    // After door event waypoints
    { 778.550232f, 304.515198f, 318.818542f, 0.002354f },
    { 789.564697f, 304.493652f, 319.759583f, 6.248631f },
    { 799.598389f, 295.776642f, 319.760040f, 4.687257f },
    { 799.054016f, 288.826660f, 320.030334f, 4.563174f },
    { 794.595459f, 262.302856f, 341.463715f, 4.500343f },
    { 794.396973f, 256.420471f, 341.463715f, 4.557680f },
    { 783.355957f, 250.460892f, 341.463776f, 3.746361f },
    { 764.988098f, 238.561462f, 353.646484f, 3.324606f },
    { 749.337463f, 236.288681f, 352.997223f, 1.633631f },
    { 751.941528f, 304.626221f, 352.996124f, 3.128243f },
    { 747.236511f, 304.922913f, 352.996155f, 6.278462f },
    { 747.834106f, 362.513977f, 352.996155f, 1.604554f }
};

// Illidan stuff
static Movement::Location ForIllidan[] =
{
    {  },
    { 709.783203f, 305.073883f, 362.988770f, 1.546307f },    // Middle pos
    { 678.703735f, 337.249664f, 362.988770f, 4.741172f },    // Eastern pos
    { 679.968384f, 284.618011f, 362.988770f, 1.603503f },    // Western pos
    { 680.798157f, 305.648743f, 353.192200f, 3.196716f }    // Land
};

static Movement::Location UnitPos[] =
{
    { 676.717346f, 322.445251f, 354.153320f, 5.732623f },    // Blade 1
    { 677.368286f, 285.374725f, 354.242157f, 5.645614f }    // Blade 2
};

uint32 DoorEventTimers[] =
{
    1000 * 2,
    1000 * 4,
    1000 * 4,
    1000 * 9,
    1000 * 1,
    1000 * 6,
    1000 * 1,
    1000 * 6,
    1000 * 5,
    1000 * 1,
    1000 * 9,
    1000 * 1,
    1000 * 3,
    1000 * 3,
    1000 * 3,
    // After breaking gate
    1000 * 2,
    1000 * 15
};

uint32 IllidanDialog[] =
{
    1000 * 1,
    1000 * 1,
    1000 * 1,
    1000 * 4,
    1000 * 5,
    1000 * 3,
    1000 * 3,
    1000 * 4,
    1000 * 3,
    1000 * 3,
    1000 * 4,
    1000 * 2,
    1000 * 2,
    1000 * 2,
    1000 * 2
};

uint32 AkamaEscapeTimers[] =
{
    1000 * 1,
    1000 * 7,
    1000 * 1,
    1000 * 1,
    1000 * 1,
    1000 * 1,
    1000 * 2,
    1000 * 2
};

uint32 BladeEvent[] =
{
    500 * 8,
    500 * 2,
    500 * 2,
    500 * 4,
    500 * 4,
    // After killing both Azzinoth elementals
    500 * 4,
    500 * 1,
    500 * 4,
    500 * 6
};

uint32 MaievTimers[] =
{
    1000 * 3,
    1000 * 3,
    1000 * 3,
    1000 * 1,
    1000 * 1,
    1000 * 1,
    1000 * 1,
    1000 * 5,
    1000 * 3,
    1000 * 3,
    1000 * 3,
    1000 * 1,
    1000 * 2//3
};

uint32 DeathSceneTimers[] =
{
    // Maiev
    1000 * 1,
    1000 * 1,
    1000 * 3,
    1000 * 20,
    1000 * 5,
    1000 * 9,
    1000 * 1,
    1000 * 1,
    // Akama
    1000 * 2,
    1000 * 4,
    1000 * 4
};

// ---     Illidan Encounter Event Options    --- //

#define USE_SHADOW_PRISON    // aggroes a lot of mobs/NPCs if they are not friendly to Illidan

// Unselectable Trigger AI
const uint32 CN_DOOR_EVENT_TRIGGER = 60001;
const uint32 CN_FACE_TRIGGER = 60002;

class UnselectableTriggerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(UnselectableTriggerAI, MoonScriptCreatureAI);
        UnselectableTriggerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
};

// Generic Trigger AI

// Demon Fire settings
const uint32 CN_DEMON_FIRE = 23069;
const uint32 DEMON_FIRE = 40029;
const uint32 DEMON_FIRE_DESPAWN = 60000;   // time in ms

// Blaze Effect settings
const uint32 CN_BLAZE_EFFECT = 23259;
const uint32 BLAZE_EFFECT = 40610;
const uint32 BLAZE_EFFECT_DESPAWN = 60000;   // time in ms

// Flame Crash Effect settings
const uint32 CN_FLAME_CRASH = 23336;
const uint32 FLAME_CRASH_EFFECT = 40836;
const uint32 FLAME_CRASH_EFFECT_DESPAWN = 120000;    // time in ms

// Flame Burst settings
const uint32 CN_FLAME_BURST = 30003;
const uint32 FLAME_BURST = 41131;
const uint32 FLAME_BURST_DESPAWN = 1000;    // time in ms

class GenericTriggerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(GenericTriggerAI, MoonScriptCreatureAI);
        GenericTriggerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            setRooted(true);
            _unit->m_noRespawn = true;

            mSpellId = mDespawnTimer = 0;
            uint32 AIUpdate = 2000;
            bool OnSpawn = false;
            switch (_unit->GetEntry())
            {
                case CN_DEMON_FIRE:
                    OnSpawn = true;
                    mSpellId = DEMON_FIRE;
                    mDespawnTimer = DEMON_FIRE_DESPAWN;
                    break;
                case CN_BLAZE_EFFECT:
                    AIUpdate = 4000;                        // to recheck
                    mSpellId = BLAZE_EFFECT;
                    mDespawnTimer = BLAZE_EFFECT_DESPAWN;
                    break;
                case CN_FLAME_CRASH:
                    mSpellId = FLAME_CRASH_EFFECT;
                    mDespawnTimer = FLAME_CRASH_EFFECT_DESPAWN;
                    break;
                case CN_FLAME_BURST:
                    mSpellId = FLAME_BURST;
                    mDespawnTimer = FLAME_BURST_DESPAWN;
                    break;
            }

            if (mSpellId == 0)
                return;

            if (OnSpawn)
            {
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
                _applyAura(mSpellId);
                _setMeleeDisabled(false);
                despawn(mDespawnTimer, 0);
            }
            else
            {
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                RegisterAIUpdateEvent(AIUpdate);
            }
        }

        void AIUpdate()
        {
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            _applyAura(mSpellId);
            _setMeleeDisabled(false);
            despawn(mDespawnTimer, 0);
            RemoveAIUpdateEvent();

            if (_unit->GetEntry() == CN_FLAME_BURST)
            {
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }

        uint32    mDespawnTimer;
        uint32    mSpellId;
};

// Eye Beam Trigger AI
const uint32 CN_EYE_BEAM_TRIGGER = 30000;

const uint32 EYE_BLAST = 40017;

class EyeBeamTriggerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(EyeBeamTriggerAI, MoonScriptCreatureAI);
        EyeBeamTriggerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            _unit->GetAIInterface()->setAiState(AI_STATE_SCRIPTMOVE);
            _unit->m_noRespawn = true;

            _setMeleeDisabled(false);
            RegisterAIUpdateEvent(1000);

            mPosition = -1;
        }

        void OnCombatStop(Unit* mTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
        }

        void AIUpdate()
        {
            if (mPosition == -1)
            {
                despawn(10000, 0);

                mPosition = -2;
            }
            else if (mPosition != -2)
            {
                moveTo(EyeBeamPaths[7 - mPosition].x, EyeBeamPaths[7 - mPosition].y, EyeBeamPaths[7 - mPosition].z, false);

                mPosition = -2;
            }

            _applyAura(EYE_BLAST);
        }

        int32    mPosition;
};

// ---    Other creature AIs    --- //

// Shadow Demon AI
const uint32 CN_SHADOW_DEMON = 23375;

const uint32 SHADOW_DEMON_PARALYZE = 41083;
const uint32 SHADOW_DEMON_CONSUME_SOUL = 41080;
const uint32 SHADOW_DEMON_PURPLE_BEAM = 39123;   // temporary spell
const uint32 SHADOW_DEMON_PASSIVE = 41079;

class ShadowDemonAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ShadowDemonAI, MoonScriptCreatureAI);
        ShadowDemonAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mParalyze = AddSpell(SHADOW_DEMON_PARALYZE, Target_Current, 0, 0, 0);
            mConsumeSoul = AddSpell(SHADOW_DEMON_CONSUME_SOUL, Target_Current, 0, 0, 0);
            _setDespawnWhenInactive(true);
            _unit->m_noRespawn = true;

            Unit* pTarget = GetBestPlayerTarget();
            if (pTarget != NULL)
            {
                _unit->GetAIInterface()->AttackReaction(pTarget, 200000);
                SetTargetToChannel(pTarget, SHADOW_DEMON_PURPLE_BEAM);
                CastSpellNowNoScheduling(mParalyze);
                _applyAura(SHADOW_DEMON_PASSIVE);
                RegisterAIUpdateEvent(1000);
            }
            else
            {
                despawn(1);
            }
        }

        void OnDied(Unit* pKiller)
        {
            if (GetTargetToChannel() != NULL)
            {
                Unit* pUnit = GetTargetToChannel();
                pUnit->RemoveAura(SHADOW_DEMON_PARALYZE);
            }

            ParentClass::OnDied(pKiller);
        }

        void OnTargetDied(Unit* pTarget)
        {
            despawn(2000, 0);
            ParentClass::OnDied(pTarget);
        }

        void AIUpdate()
        {
            // Ugly code :P
            Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
            if (pTarget != NULL && GetTargetToChannel() != NULL && pTarget == GetTargetToChannel())
            {
                if (getRangeToObject(pTarget) <= 8.0f)
                {
                    pTarget->RemoveAura(SHADOW_DEMON_PARALYZE);
                    CastSpellNowNoScheduling(mConsumeSoul);

                    RemoveAIUpdateEvent();
                    despawn(500, 0);
                    return;
                }
            }
            else
            {
                despawn(500, 0);
            }

            ParentClass::AIUpdate();
        }

        SpellDesc* mParalyze;
        SpellDesc* mConsumeSoul;
};

// Parasitic Shadowfiend AI - This AI is one huge hack :P
const uint32 CN_PARASITIC_SHADOWFIEND = 23498;

const uint32 PARASITIC_SHADOWFIEND_PASSIVE = 41913;
const uint32 PARASITIC_SHADOWFIEND_WITH_DAMAGE = 41917;

void SpellFunc_Parasitic(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class ParasiticShadowfiendAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(ParasiticShadowfiendAI, MoonScriptCreatureAI);
        ParasiticShadowfiendAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mParasiticDmg = AddSpell(PARASITIC_SHADOWFIEND_WITH_DAMAGE, Target_Current, 0, 0, 0);
            mParasitic = AddSpellFunc(&SpellFunc_Parasitic, Target_Current, 0, 0, 0);

            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            _applyAura(PARASITIC_SHADOWFIEND_PASSIVE);
            _unit->m_noRespawn = true;
            _setDespawnWhenInactive(true);
            _setScale(0.0f);

            Unit* pTarget = GetBestPlayerTarget(TargetFilter_Closest);
            if (pTarget != NULL)
            {
                _unit->GetAIInterface()->SetUnitToFollow(pTarget);
                _unit->GetAIInterface()->SetUnitToFollowAngle(2.0f);
                RegisterAIUpdateEvent(11000);
            }
            else
            {
                despawn(1);
            }
        }

        // Won't work until it's hooked in core
        void OnHit(Unit* pTarget, float pAmount)
        {
            if (!pTarget->HasAura(PARASITIC_SHADOWFIEND_PASSIVE))
            {
                CastSpellNowNoScheduling(mParasitic);
            }
        }

        void AIUpdate()
        {
            if (_unit->getUInt64Value(UNIT_FIELD_FLAGS) == UNIT_FLAG_NOT_SELECTABLE)
            {
                _delayNextAttack(3000);
                _setScale(1.0f);
                _unit->setUInt64Value(UNIT_FIELD_FLAGS, 0);
                _unit->GetAIInterface()->ResetUnitToFollow();
                _unit->GetAIInterface()->SetUnitToFollowAngle(0.0f);
                RemoveAIUpdateEvent();

                Unit* pTarget = GetBestPlayerTarget();
                if (pTarget != NULL)
                {
                    _unit->GetAIInterface()->setNextTarget(pTarget);
                    _unit->GetAIInterface()->AttackReaction(pTarget, 200);
                }
                else
                {
                    despawn(0);
                }
            }
        }

        SpellDesc* mParasiticDmg;
        SpellDesc* mParasitic;
};

void SpellFunc_Parasitic(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    ParasiticShadowfiendAI* Parasitic = (pCreatureAI != NULL) ? static_cast< ParasiticShadowfiendAI* >(pCreatureAI) : NULL;
    if (Parasitic != NULL)
    {
        Parasitic->CastSpell(Parasitic->mParasiticDmg);
        if (pTarget != NULL)                        // not sure if target is really added here
        {
            // Workaround - we will spawn 2 Parasitic Shadowfiends on that player place
            Parasitic->spawnCreature(CN_PARASITIC_SHADOWFIEND, pTarget->GetPosition());
            Parasitic->spawnCreature(CN_PARASITIC_SHADOWFIEND, pTarget->GetPosition());
        }
    }
}

class SCRIPT_DECL AkamaGossip : public Arcemu::Gossip::Script
{
    public:
        void OnHello(Object* pObject, Player* pPlayer)
        {
            Creature* pAIOwner = static_cast<Creature*>(pObject);
            if (pAIOwner->GetScript() == NULL)
                return;

            MoonScriptCreatureAI* pAI = static_cast< MoonScriptCreatureAI* >(pAIOwner->GetScript());

            if (pAI->GetCurrentWaypoint() >= 10)
            {
                Arcemu::Gossip::Menu menu(pObject->GetGUID(), 229902);
                menu.AddItem(GOSSIP_ICON_CHAT, pPlayer->GetSession()->LocalizedGossipOption(444), 2);     // We're ready to face Illidan.
                menu.Send(pPlayer);
            }
            else
            {
                Arcemu::Gossip::Menu menu(pObject->GetGUID(), 229901);
                menu.AddItem(GOSSIP_ICON_CHAT, pPlayer->GetSession()->LocalizedGossipOption(445), 1);     // I'm ready, Akama.
                menu.Send(pPlayer);
            }
        }

        void OnSelectOption(Object* pObject, Player* pPlayer, uint32 Id, const char* EnteredCode, uint32 gossipId)
        {
            Creature* pAIOwner = static_cast<Creature*>(pObject);
            if (pAIOwner->GetScript() == NULL)
                return;

            MoonScriptCreatureAI* pAI = static_cast< MoonScriptCreatureAI* >(pAIOwner->GetScript());
            switch (Id)
            {
                case 1:
                    pAIOwner->setUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                    pAI->ForceWaypointMove(1);
                    break;
                case 2:
                    pAIOwner->setUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                    pAI->ForceWaypointMove(17);
                    pAI->_setWieldWeapon(false);
                    break;
            }
            Arcemu::Gossip::Menu::Complete(pPlayer);
        }
};

/* Akama AI - Creature AI Class */

const uint32 CN_AKAMA = 22990;

const uint32 AKAMA_HEALING_POTION = 40535;
const uint32 AKAMA_BLESSING_OF_KINGS = 20217;
const uint32 AKAMA_DESPAWN = 41242;

// Door Event Spells
const uint32 AKAMA_DOOR_FAIL = 41271;
const uint32 AKAMA_DOOR_OPEN = 41268;
const uint32 DEATHSWORN_DOOR_OPEN = 41269;
const uint32 GATE_FAILURE = 10390;

const uint32 AKAMA_WAYPOINT_SIZE = 20;

class AkamaAI : public MoonScriptBossAI
{
        MOONSCRIPT_FACTORY_FUNCTION(AkamaAI, MoonScriptBossAI);
        AkamaAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
            AddPhaseSpell(2, AddSpell(AKAMA_BLESSING_OF_KINGS, Target_Self, 15, 0, 60));
            mHealingPotion = AddSpell(AKAMA_HEALING_POTION, Target_Self, 0, 0, 0);
            mHealingPotion->AddEmote("I will not last much longer!", CHAT_MSG_MONSTER_YELL, 11385);
            mHealingPotion->AddEmote("No! Not yet!", CHAT_MSG_MONSTER_YELL, 11386);
            mDespawn = AddSpell(AKAMA_DESPAWN, Target_Self, 0, 0.5f, 0);

            AddEmote(Event_OnTargetDied, "Illidan will pay!", CHAT_MSG_MONSTER_YELL, 11381);
            AddEmote(Event_OnTargetDied, "One step closer!", CHAT_MSG_MONSTER_YELL, 11382);
            AddEmote(Event_OnDied, "Fight on friends! Kill him and end the curse on my people!", CHAT_MSG_MONSTER_YELL, 11391);

            SetWaypointMoveType(Movement::WP_MOVEMENT_SCRIPT_NONE);
            setCanEnterCombat(false);
            SetPhase(1);

            for (uint8 i = 1; i < AKAMA_WAYPOINT_SIZE; ++i)
            {
                AddWaypoint(CreateWaypoint(i, 0, Movement::WP_MOVE_TYPE_RUN, ToIllidan[i]));
            }

            _unit->setUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            _unit->SetDualWield(true);

            mUdaloAI = mOlumAI = NULL;
            mIllidanAI = NULL;
            mTimeLeft = mScenePart = 0;
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            _setWieldWeapon(true);
            CancelAllCooldowns();
        }

        void OnCombatStop(Unit* pTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
            _setWieldWeapon(false);
        }

        void OnDied(Unit* pKiller)
        {
            ParentClass::OnDied(pKiller);
        }

        void AIUpdate()
        {
            switch (GetPhase())
            {
                case 1:
                    WayToIllidan();
                    break;
                case 2:
                    FightWithIllidan();
                    break;
                case 3:
                    EncounterEnd();
                    break;
            }
        }

        void ToIllidanEvent()
        {
            if (mScenePart > 6 && mScenePart < 15 && (mUdaloAI == NULL || mOlumAI == NULL))
            {
                mScenePart = -1;
            }

            GameObject* pGate = getNearestGameObject(774.7f, 304.6f, 314.85f, 185905);
            Unit* pDoorTrigger = getNearestCreature(771.5f, 304.7f, 319.0f, CN_DOOR_EVENT_TRIGGER);
            if ((mScenePart <= 15 && pGate == NULL) || mScenePart == -1)
            {
                sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "It's strange that Illidan doesn't protect himself against intruders.");
                SetTargetToChannel(NULL, 0);
                ForceWaypointMove(7);

                if (mUdaloAI != NULL)
                {
                    mUdaloAI->SetTargetToChannel(NULL, 0);
                }
                if (mOlumAI != NULL)
                {
                    mOlumAI->SetTargetToChannel(NULL, 0);
                }

                RemoveAIUpdateEvent();
                mUdaloAI = mOlumAI = NULL;
                mTimeLeft = DoorEventTimers[15];
                mScenePart = 16;
                return;
            }

            mTimeLeft -= mAIUpdateFrequency;
            if (mTimeLeft > 0)
                return;

            switch (mScenePart)
            {
                case 1:
                    _unit->SetFacing(6.248631f);
                    break;
                case 2:
                    sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "The door is all that stands between us and the Betrayer. Stand aside, friends.");
                    _unit->Emote(EMOTE_ONESHOT_TALK);
                    break;
                case 3:
                    if (pDoorTrigger != NULL)
                    {
                        _applyAura(AKAMA_DOOR_FAIL);
                        SetTargetToChannel(pDoorTrigger, AKAMA_DOOR_FAIL);
                    }
                    break;
                case 4:
                    SetTargetToChannel(NULL, 0);
                    break;
                case 5:
                    sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "I cannot do this alone...");
                    _unit->Emote(EMOTE_ONESHOT_NO);
                    break;
                case 6:        // summoning two spirits to help Akama with breaking doors
                    mUdaloAI = SpawnCreature(23410, 751.884705f, 311.270050f, 312.121185f, 0.047113f);
                    mOlumAI  = SpawnCreature(23411, 751.687744f, 297.408600f, 312.124817f, 0.054958f);
                    if (mUdaloAI == NULL || mOlumAI == NULL)
                    {
                        pGate->SetState(GO_STATE_OPEN);
                        break;
                    }
                    break;
                case 7:
                    mUdaloAI->sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "You are not alone, Akama.");
                    break;
                case 8:
                    mOlumAI->sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "Your people will always be with you.");
                    break;
                case 9:
                    if (pDoorTrigger != NULL)
                    {
                        _applyAura(AKAMA_DOOR_OPEN);
                        SetTargetToChannel(pDoorTrigger, AKAMA_DOOR_OPEN);
                    }
                    break;
                case 10:
                    if (pDoorTrigger != NULL)
                    {
                        mUdaloAI->_applyAura(DEATHSWORN_DOOR_OPEN);
                        mUdaloAI->SetTargetToChannel(pDoorTrigger, DEATHSWORN_DOOR_OPEN);
                        mOlumAI->_applyAura(DEATHSWORN_DOOR_OPEN);
                        mOlumAI->SetTargetToChannel(pDoorTrigger, DEATHSWORN_DOOR_OPEN);
                    }
                    break;
                case 11:
                    pGate->SetState(GO_STATE_OPEN);
                    if (pDoorTrigger != NULL)
                    {
                        pDoorTrigger->CastSpell(pDoorTrigger, sSpellCustomizations.GetSpellInfo(GATE_FAILURE), true);
                    }
                    break;
                case 12:
                    SetTargetToChannel(NULL, 0);
                    mUdaloAI->SetTargetToChannel(NULL, 0);
                    mOlumAI->SetTargetToChannel(NULL, 0);
                    break;
                case 13:
                    sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "I thank you for your aid, my brothers. Our people will be redeemed!");
                    _unit->Emote(EMOTE_ONESHOT_SALUTE);
                    break;
                case 14:
                    mUdaloAI->GetUnit()->Emote(EMOTE_ONESHOT_SALUTE);
                    mOlumAI->GetUnit()->Emote(EMOTE_ONESHOT_SALUTE);

                    mUdaloAI = NULL;
                    mOlumAI = NULL;
                    break;
                case 15:
                    ForceWaypointMove(7);
                    RemoveAIUpdateEvent();
                    break;
                case 16:
                    sendChatMessage(CHAT_MSG_MONSTER_SAY, 11388, "");
                    _unit->SetFacing(2.113512f);
                    break;
                case 17:
                    _unit->setUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    RemoveAIUpdateEvent();

                    mScenePart = 0;
                    return;
                    break;
            }

            mTimeLeft = DoorEventTimers[mScenePart];
            ++mScenePart;
        }

        void IllidanDialogEvent()
        {
            if (mIllidanAI == NULL)
            {
                Creature* pIllidan = getNearestCreature(704.539001f, 305.282013f, 353.919006f, 22917);
                if (pIllidan != NULL && pIllidan->GetScript() != NULL)
                {
                    mIllidanAI = static_cast< MoonScriptCreatureAI* >(pIllidan->GetScript());
                }
            }

            if (mIllidanAI == NULL || !mIllidanAI->isAlive())
            {
                sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "Not this time my friends.");
                CastSpellNowNoScheduling(mDespawn);
                despawn(0);
                return;
            }

            mTimeLeft -= mAIUpdateFrequency;
            if (mTimeLeft > 0)
                return;

            switch (mScenePart)
            {
                case 1:
                    _unit->SetFacing(3.126680f);
                    break;
                case 2:
                    mIllidanAI->_removeAura(39656);
                    break;
                case 3:
                    mIllidanAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11463, "Akama... your duplicity is hardly surprising. I should have slaughtered you and your malformed brethren long ago.");
                    break;
                case 4:
                    mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_QUESTION);
                    break;
                case 5:
                    mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_QUESTION);
                    break;
                case 6:
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11389, "We've come to end your reign, Illidan. My people and all of Outland shall be free!");
                    _unit->Emote(EMOTE_ONESHOT_POINT);
                    break;
                case 7:
                    _unit->Emote(EMOTE_ONESHOT_TALK);
                    break;
                case 8:
                    _unit->Emote(EMOTE_ONESHOT_SALUTE);
                    break;
                case 9:
                    mIllidanAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11464, "Boldly said. But I remain unconvinced.");
                    mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_QUESTION);
                    break;
                case 10:
                    mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_QUESTION);
                    break;
                case 11:
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11380, "The time has come! The moment is at hand!");
                    _unit->Emote(EMOTE_ONESHOT_SHOUT);
                    break;
                case 12:
                    _setWieldWeapon(true);
                    _unit->Emote(EMOTE_ONESHOT_ROAR);
                    break;
                case 13:
                    mIllidanAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11466, "You are not prepared!");
                    mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_CUSTOMSPELL05);

                    _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                    break;
                case 14:
                    mIllidanAI->_setWieldWeapon(true);
                    break;
                case 15:
                    GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
                    GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
                    if (pRightGate != NULL)
                    {
                        pRightGate->SetState(GO_STATE_CLOSED);
                    }
                    if (pLeftGate != NULL)
                    {
                        pLeftGate->SetState(GO_STATE_CLOSED);
                    }

                    setRooted(false);
                    setCanEnterCombat(true);
                    setAIAgent(AGENT_NULL);
                    SetWaypointMoveType(Movement::WP_MOVEMENT_SCRIPT_NONE);
                    SetWaypointToMove(0);
                    RemoveAIUpdateEvent();
                    _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);

                    _unit->GetAIInterface()->AttackReaction(mIllidanAI->GetUnit(), 1, 0);

                    mIllidanAI->setCanEnterCombat(true);
                    mIllidanAI->setRooted(false);

                    Unit* pTarget = FindClosestTargetToUnit(mIllidanAI->GetUnit());
                    if (pTarget == NULL)
                    {
                        pTarget = _unit;
                    }

                    mIllidanAI->GetUnit()->GetAIInterface()->AttackReaction(pTarget, 500, 0);

                    mScenePart = 0;
                    SetPhase(2);
                    return;
            }

            mTimeLeft = IllidanDialog[mScenePart];
            ++mScenePart;
        }

        void WayToIllidan()
        {
            if (mScenePart != 0)
            {
                if (GetCurrentWaypoint() == 6 || GetCurrentWaypoint() == 16)
                {
                    ToIllidanEvent();
                }
                else if (GetCurrentWaypoint() == 17)
                {
                    IllidanDialogEvent();
                }
            }
        }

        void FightWithIllidan()
        {
            if (_getHealthPercent() <= 15 && !_isCasting())
            {
                CastSpellNowNoScheduling(mHealingPotion);
            }
            if (mIllidanAI != NULL && !mIllidanAI->isAlive())
            {
                mIllidanAI = NULL;
            }
            if (mScenePart <= 2)
            {
                if (mIllidanAI == NULL)
                {
                    sendChatMessage(CHAT_MSG_MONSTER_SAY, 0, "Not this time my friends.");
                    if (!_isCasting())
                    {
                        CastSpellNowNoScheduling(mDespawn);
                    }

                    despawn(0);
                    return;
                }
                else if (mIllidanAI->GetUnit()->GetAIInterface()->isFlying())
                {
                    setCanEnterCombat(false);
                    _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
                    // azolex to prevent compile error
                    uint32 nullfix = 0;
                    _unit->GetAIInterface()->setNextTarget(nullfix);
                    _unit->GetAIInterface()->WipeTargetList();
                    _unit->GetAIInterface()->WipeHateList();
                    _setMeleeDisabled(false);
                    setRooted(true);
                    mIllidanAI->GetUnit()->GetAIInterface()->RemoveThreatByPtr(_unit);
                }
                else
                {
                    _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                    ParentClass::AIUpdate();
                }
            }
            if (mScenePart == 0 && mIllidanAI != NULL && mIllidanAI->_getHealthPercent() <= 85)
            {
                mTimeLeft = AkamaEscapeTimers[0];
                mScenePart = 1;
            }
            if (GetCurrentWaypoint() < 18 && mScenePart != 0)
            {
                mTimeLeft -= mAIUpdateFrequency;
                if (mTimeLeft > 0)
                    return;

                switch (mScenePart)
                {
                    case 1:
                        mIllidanAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11465, "Come, my minions. Deal with this traitor as he deserves!");
                        break;
                    case 2:
                        if (canEnterCombat())
                        {
                            setCanEnterCombat(false);
                            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
                            // ugly code, trows compile error if left just null, this should do it ~ azolex
                            uint32 nullfix = 0;
                            _unit->GetAIInterface()->setNextTarget(nullfix);
                            _unit->GetAIInterface()->WipeTargetList();
                            _unit->GetAIInterface()->WipeHateList();
                            _setMeleeDisabled(false);
                            setRooted(true);

                            mIllidanAI->GetUnit()->GetAIInterface()->RemoveThreatByPtr(_unit);
                        }

                        mIllidanAI = NULL;
                        break;
                    case 3:
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11390, "I will deal with these mongrels! Strike now, friends! Strike at the Betrayer!");
                        _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
                        break;
                    case 4:
                        _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                        break;
                    case 5:
                        _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
                        break;
                    case 6:
                        _setWieldWeapon(false);
                        _unit->Emote(EMOTE_ONESHOT_EXCLAMATION);
                        break;
                    case 7:
                        _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                        _setWieldWeapon(true);
                        break;
                    case 8:
                        setRooted(false);
                        ForceWaypointMove(18);
                        RemoveAIUpdateEvent();
                        //_unit->SetEmoteState(EMOTE_ONESHOT_NONE);

                        mScenePart = 0;
                        return;
                        break;
                }

                mTimeLeft = AkamaEscapeTimers[mScenePart];
                ++mScenePart;
            }
        }

        void EncounterEnd()
        {
            mTimeLeft -= mAIUpdateFrequency;
            if (mTimeLeft > 0)
                return;

            switch (mScenePart)
            {
                case 1:
                    {
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11387, "The Light will fill these dismal halls once again. I swear it.");

                        Unit* pIllidan = getNearestCreature(22917);
                        if (pIllidan != NULL)
                        {
                            _unit->GetAIInterface()->setNextTarget(pIllidan);
                        }

                        _unit->Emote(EMOTE_ONESHOT_TALK);
                    }
                    break;
                case 2:
                    _unit->Emote(EMOTE_ONESHOT_SALUTE);
                    break;
                case 3:
                    RemoveAIUpdateEvent();
                    CastSpellNowNoScheduling(mDespawn);
                    despawn(0);
                    return;
                    break;
            }

            mTimeLeft = DeathSceneTimers[mScenePart + 8];
            ++mScenePart;
        }

        void OnReachWP(uint32 iWaypointId, bool bForwards)
        {
            switch (iWaypointId)
            {
                case 6:
                    RegisterAIUpdateEvent(1000);
                    mTimeLeft = DoorEventTimers[0];
                    mScenePart = 1;
                    break;
                case 16:
                    RegisterAIUpdateEvent(1000);
                    break;
                case 17:
                    RegisterAIUpdateEvent(1000);
                    mTimeLeft = IllidanDialog[0];
                    mScenePart = 1;
                    break;
                case 19:
                    //_unit->m_auracount[SPELL_AURA_MOD_INVISIBILITY] = true;                        // Arc's
                    _unit->UpdateVisibility();
                    break;
                case 20:
                    RegisterAIUpdateEvent(1000);
                    mTimeLeft = DeathSceneTimers[8];
                    mScenePart = 1;
                    SetPhase(3);
                    break;
                default:
                    {
                        _unit->GetAIInterface()->setWaypointScriptType(Movement::WP_MOVEMENT_SCRIPT_WANTEDWP);
                        _unit->GetAIInterface()->setWayPointToMove(iWaypointId + 1);
                    }
            }
        }

        // A bit rewritten FindTarget function
        Unit* FindClosestTargetToUnit(Unit* pSeeker)
        {
            if (pSeeker == NULL)
                return NULL;

            Unit* pTarget = NULL;
            float distance = 70.0f;
            float z_diff;

            Unit* pUnit;
            float dist;

            for (std::set< Object* >::iterator itr = _unit->GetInRangePlayerSetBegin(); itr != _unit->GetInRangePlayerSetEnd(); ++itr)
            {
                pUnit = static_cast< Unit* >(*itr);

                if (pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH))
                    continue;

                z_diff = fabs(_unit->GetPositionZ() - pUnit->GetPositionZ());
                if (z_diff > 5.0f)
                    continue;

                if (!pUnit->isAlive())
                    continue;

                dist = pSeeker->GetDistance2dSq(pUnit);

                if (dist > distance * distance)
                    continue;

                distance = dist;
                pTarget = pUnit;
            }

            return pTarget;
        }

        // SPells
        SpellDesc* mHealingPotion;
        SpellDesc* mDespawn;

        // AIs
        MoonScriptCreatureAI* mIllidanAI;
        MoonScriptCreatureAI* mUdaloAI;
        MoonScriptCreatureAI* mOlumAI;

        // Other variables
        int32 mScenePart;
        uint32 mTimeLeft;
};

/* Maiev AI - Creature AI Class */

const uint32 CN_MAIEV = 23197;

//const uint32 TELEPORT_MAIEV  = 41221;
const uint32 MAIEV_TELEPORT = 34673;
const uint32 MAIEV_SHADOW_STRIKE = 40685;
const uint32 MAIEV_THROW_DAGGER = 41152;
const uint32 MAIEV_CAGE_TRAP_SUMMON = 40694;

class MaievAI : public MoonScriptBossAI
{
        MOONSCRIPT_FACTORY_FUNCTION(MaievAI, MoonScriptBossAI);
        MaievAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
            AddPhaseSpell(1, AddSpell(MAIEV_SHADOW_STRIKE, Target_Current, 10, 0, 30));
            AddPhaseSpell(2, AddSpell(MAIEV_THROW_DAGGER, Target_Current, 50, 1, 0, 15, 40));
            mTeleport = AddSpell(MAIEV_TELEPORT, Target_Self, 0, 0, 0);
            mTrapSummon = AddSpell(MAIEV_CAGE_TRAP_SUMMON, Target_Self, 0, 1.3f, 0);

            // HACK!
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
            _unit->setUInt32Value(UNIT_FIELD_MAXHEALTH, 1000000);
            _unit->SetHealth(1000000);
            _unit->GetAIInterface()->SetAllowedToEnterCombat(false);

            SetWaypointMoveType(Movement::WP_MOVEMENT_SCRIPT_NONE);
            SetPhase(1);

            mYellTimer = mScenePart = mTrapTimer = 0;
            mSummonTrap = false;
            mIllidanAI = NULL;
            mTimeLeft = 0;
        }

        void OnCombatStart(Unit* pTarget)
        {
            _setWieldWeapon(true);
            CancelAllCooldowns();
        }

        void OnCombatStop(Unit* pTarget)
        {
            //OnCombatStop(pTarget);        // causes crashes
            CancelAllSpells();
            _cancelAllTimers();
            _removeAllAuras();
            setAIAgent(AGENT_NULL);
            //_unit->GetAIInterface()->SetAIState(STATE_IDLE);
            _setWieldWeapon(false);
            if (mIllidanAI != NULL)
            {
                SetPhase(3);
                mScenePart = 1;
                mTimeLeft = DeathSceneTimers[0];
                RegisterAIUpdateEvent(1000);
                DeathScene();
            }
            else
            {
                CastSpellNowNoScheduling(mTeleport);
                RemoveAIUpdateEvent();
                despawn(500, 0);
            }
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            _unit->SetHealth(_unit->getUInt32Value(UNIT_FIELD_MAXHEALTH));
        }

        void AIUpdate()
        {
            switch (GetPhase())
            {
                case 1:
                    FightWithIllidan();
                    break;
                case 2:
                    FightWithDemon();
                    break;
                case 3:
                    DeathScene();
                    break;
            }
        }

        void FightWithIllidan()
        {
            if (mIllidanAI == NULL)
            {
                CastSpellNowNoScheduling(mTeleport);
                RemoveAIUpdateEvent();
                despawn(500, 0);
                return;
            }
            if (mIllidanAI->isAlive())
            {
                if (mIllidanAI->GetUnit()->HasAura(40506))
                {
                    SetPhase(2);
                    FightWithDemon();
                    return;
                }
                else if (mSummonTrap)
                {
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11495, "There shall be no prison for you this time!");
                    CastSpellNowNoScheduling(mTrapSummon);
                    _resetTimer(mYellTimer, _getTimeForTimer(mYellTimer) + (5 + RandomUInt(10)) * 1000);
                    mSummonTrap = false;
                    return;
                }
                else if (_isTimerFinished(mTrapTimer))
                {
                    Unit* pTarget = GetBestPlayerTarget();
                    if (pTarget != NULL)
                    {
                        CastSpellNowNoScheduling(mTeleport);
                        _unit->SetPosition(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation());        // does it really work?
                        _unit->GetAIInterface()->StopMovement(2500);
                    }

                    _resetTimer(mTrapTimer, ((RandomUInt(5) + 25) * 1000));
                    mSummonTrap = true;
                    return;
                }
                else if (_isTimerFinished(mYellTimer))
                {
                    switch (RandomUInt(2))
                    {
                        case 0:
                            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11494, "Bleed as I have bled!");
                            break;
                        case 1:
                            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11493, "That is for Naisha!");
                            break;
                        case 2:                                                            // Not sure if this one should be here
                            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11500, "Meet your end, demon!");
                            break;
                    }

                    _resetTimer(mYellTimer, (RandomUInt(20) + 20) * 1000);
                }

                ParentClass::AIUpdate();
            }
            else
            {
                mTimeLeft = DeathSceneTimers[0];
                mScenePart = 1;
                SetPhase(3);
                DeathScene();
            }
        }

        void FightWithDemon()
        {
            if (mIllidanAI == NULL)
            {
                CastSpellNowNoScheduling(mTeleport);
                RemoveAIUpdateEvent();
                despawn(500, 0);
                return;
            }
            if (!mIllidanAI->GetUnit()->HasAura(40506))
            {
                if (mIllidanAI->isAlive())
                {
                    setAIAgent(AGENT_NULL);
                    _setDisplayWeapon(true, false);
                    _setWieldWeapon(true);
                    _resetTimer(mTrapTimer, (RandomUInt(5) + 20) * 1000);
                    SetPhase(1);
                    FightWithIllidan();
                }
                else
                {
                    mTimeLeft = DeathSceneTimers[0];
                    mScenePart = 1;
                    SetPhase(3);
                    DeathScene();
                }

                return;
            }
            else if (getRangeToObject(mIllidanAI->GetUnit()) < 15.0f)
            {
                _unit->GetAIInterface()->_CalcDestinationAndMove(mIllidanAI->GetUnit(), 20.0f);
                setAIAgent(AGENT_SPELL);
                return;
            }
            else if (getRangeToObject(mIllidanAI->GetUnit()) > 35.0f)
            {
                _unit->GetAIInterface()->_CalcDestinationAndMove(mIllidanAI->GetUnit(), 30.0f);
                setAIAgent(AGENT_SPELL);
                return;
            }

            _setDisplayWeapon(false, false);
            if (mIllidanAI->GetUnit()->GetEmoteState() == 0)        // dunno if it's really needed
            {
                ParentClass::AIUpdate();
            }

            // Ugly -.-'
            float Facing = _unit->calcRadAngle(_unit->GetPositionX(), _unit->GetPositionY(), mIllidanAI->GetUnit()->GetPositionX(), mIllidanAI->GetUnit()->GetPositionY());
            if (_unit->GetOrientation() != Facing)
            {
                _unit->SetFacing(Facing);
            }

            setAIAgent(AGENT_SPELL);
        }

        void DeathScene()
        {
            if (mIllidanAI == NULL && mScenePart <= 6)
            {
                CastSpellNowNoScheduling(mTeleport);
                RemoveAIUpdateEvent();
                despawn(500, 0);
                return;
            }
            if ((mIllidanAI != NULL && mScenePart <= 6) || mScenePart > 6)
            {
                mTimeLeft -= mAIUpdateFrequency;
                if (mTimeLeft > 0)
                    return;

                switch (mScenePart)
                {
                    case 1:
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11496, "Ah, it is finished. You are beaten.");

                        mIllidanAI->GetUnit()->Emote(EMOTE_ONESHOT_CUSTOMSPELL06);
                        if (mIllidanAI->GetUnit()->GetCurrentSpell() != NULL)
                            mIllidanAI->GetUnit()->GetCurrentSpell()->cancel();
                        break;
                    case 2:
                        mIllidanAI->GetUnit()->SetEmoteState(EMOTE_ONESHOT_CUSTOMSPELL07);

                        _unit->GetAIInterface()->setNextTarget(mIllidanAI->GetUnit());
                        break;
                    case 3:
                        mIllidanAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11478, "You have won... Maiev. But the huntress... is nothing without the hunt. You... are nothing... without me.");
                        _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
                        _setWieldWeapon(false);
                        break;
                    case 4:
                        {
                            GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
                            GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
                            if (pRightGate != NULL)
                            {
                                pRightGate->SetState(GO_STATE_OPEN);
                            }
                            if (pLeftGate != NULL)
                            {
                                pLeftGate->SetState(GO_STATE_OPEN);
                            }

                            mIllidanAI->GetUnit()->SetEmoteState(EMOTE_ONESHOT_NONE);
                            mIllidanAI->GetUnit()->SetHealth(0);
                            mIllidanAI->GetUnit()->setDeathState(JUST_DIED);
                        }
                        break;
                    case 5:
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11497, "He's right. I feel nothing. I am nothing.");
                        break;
                    case 6:
                        {
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11498, "Farewell, champions.");
                            Creature* pAkama = getNearestCreature(ToIllidan[19].x, ToIllidan[19].y, ToIllidan[19].z, CN_AKAMA);
                            if (pAkama != NULL)
                            {
                                AkamaAI* pAkamaAI = static_cast< AkamaAI* >(pAkama->GetScript());
                                //pAkama->m_auracount[SPELL_AURA_MOD_INVISIBILITY] = true;                        // Arc's
                                pAkama->UpdateVisibility();
                                if (pAkamaAI->isRooted())
                                {
                                    pAkamaAI->setRooted(false);
                                }

                                float IllidanX = mIllidanAI->GetUnit()->GetPositionX();
                                float IllidanY = mIllidanAI->GetUnit()->GetPositionY();
                                float IllidanZ = mIllidanAI->GetUnit()->GetPositionZ();
                                float IllidanO = mIllidanAI->GetUnit()->GetOrientation();

                                float Angle = _unit->calcAngle(pAkama->GetPositionX(), pAkama->GetPositionY(), IllidanX, IllidanY) * M_PI_FLOAT / 180.0f;
                                float X = 12.0f * cosf(Angle);
                                float Y = 12.0f * sinf(Angle);

                                X -= cosf(IllidanO);
                                Y -= sinf(IllidanO);

                                Movement::Location pCoords;
                                pCoords.x = IllidanX - X;
                                pCoords.y = IllidanY - Y;
                                pCoords.z = IllidanZ;
                                pCoords.o = 0.0f;

                                pAkamaAI->AddWaypoint(CreateWaypoint(20, 0, Movement::WP_MOVE_TYPE_RUN, pCoords));
                                pAkamaAI->ForceWaypointMove(20);
                            }
                        }
                        break;
                    case 7:
                        CastSpellNowNoScheduling(mTeleport);
                        break;
                    case 8:
                        RemoveAIUpdateEvent();
                        despawn(0);
                        return;
                        break;
                }

                mTimeLeft = DeathSceneTimers[mScenePart];
                ++mScenePart;
            }

            else
            {
                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11498, "Farewell, champions.");
                despawn(0);
            }
        }

        // AI class pointers
        MoonScriptCreatureAI* mIllidanAI;

        // Spell Description pointers
        SpellDesc* mTrapSummon;
        SpellDesc* mTeleport;

        // Other variables
        bool mSummonTrap;
        int32 mTrapTimer;
        int32 mYellTimer;
        uint32 mScenePart;
        uint32 mTimeLeft;

};

///\todo rewritten Illidan Script
//------- Macros
const uint32 CN_ILLIDAN_STORMRAGE = 22917;

// Normal Form Spells
const uint32 ILLIDAN_SHEAR = 41032; // +
const uint32 ILLIDAN_DRAW_SOUL = 40904; // +
const uint32 ILLIDAN_FLAME_CRASH = 40832;  // +

// Phase 2 Spells
const uint32 ILLIDAN_FIREBALL = 40598;  // +
const uint32 ILLIDAN_DARK_BARRAGE = 40585;  // ? Haven't Tried
const uint32 ILLIDAN_SUMMON_TEAR_OF_AZZINOTH = 39855;  // +
const uint32 ILLIDAN_EYE_BLAST1 = 39908;
const uint32 ILLIDAN_EYE_BLAST2 = 40017;

// Illidan's Glaive spells
const uint32 ILLIDAN_SUMMON_GLAIVE = 41466;
const uint32 ILLIDAN_GLAIVE_RETURNS = 39873;
const uint32 ILLIDAN_THROW_GLAIVE1 = 39635;
const uint32 ILLIDAN_THROW_GLAIVE2 = 39849;

const uint32 TEAR_OF_AZZINOTH_CHANNEL = 39857;

// Phase 3 Spells
const uint32 ILLIDAN_AGONIZING_FLAMES = 40932;  // +

// Phase 4 Spells
const uint32 ILLIDAN_DEMON_FORM = 40506;
const uint32 ILLIDAN_AURA_OF_DREAD = 41142;
const uint32 ILLIDAN_SHADOW_BLAST = 41078;  // +
const uint32 ILLIDAN_SUMMON_DEMON = 41117;  // - Core Support
const uint32 ILLIDAN_FLAME_BURST1 = 41131;  // ? Frost-nova-like effect dealing damage
const uint32 ILLIDAN_FLAME_BURST2 = 41126;  // ? Cast Effect

// Demon Forms
const uint32 ILLIDAN_DEMON_FORM1 = 40511;  // +
const uint32 ILLIDAN_DEMON_FORM2 = 40398;  // +
const uint32 ILLIDAN_DEMON_FORM3 = 40510; // +

// Phase 5 Spells
const uint32 ILLIDAN_ENRAGE = 40683; // +

// Other spells
const uint32 ILLIDAN_SHADOW_PRISON = 40647; // +
const uint32 ILLIDAN_SKULL_INTRO = 39656; // + Works with RemoveAura
const uint32 ILLIDAN_SUMMON_PARASITIC_SHADOWFIENDS = 41915; // ? Haven't Tried
const uint32 ILLIDAN_PARASITIC_SHADOWFIEND_WITH_SE = 41914;
const uint32 ILLIDAN_BERSERK = 45078;
const uint32 ILLIDAN_DEATH1 = 41218;
const uint32 ILLIDAN_DEATH2 = 41220;
const uint32 ILLIDAN_RAKE = 1822;  // + Used to force animation use

// Other macros
const uint32 CN_BLADE_OF_AZZINOTH = 22996;
const uint32 ILLIDAN_WAYPOINT_SIZE = 5;

void SpellFunc_Illidan_Parasitic(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class IllidanStormrageAI : public MoonScriptBossAI
{
        MOONSCRIPT_FACTORY_FUNCTION(IllidanStormrageAI, MoonScriptBossAI);
        IllidanStormrageAI(Creature* pCreature) : MoonScriptBossAI(pCreature)
        {
            //Phase 1 spells
            AddPhaseSpell(1, AddSpell(ILLIDAN_SHEAR, Target_Current, 12, 1.5f, 15));
            AddPhaseSpell(1, AddSpell(ILLIDAN_DRAW_SOUL, Target_Self, 6, 1.5f, 40));    // Self-kills are bad =/
            AddPhaseSpell(1, AddSpell(ILLIDAN_FLAME_CRASH, Target_Destination, 12, 1.3f, 25));
            mParasiticDmg = AddSpell(PARASITIC_SHADOWFIEND_WITH_DAMAGE, Target_RandomPlayerNotCurrent, 0, 0, 0);
            mParasitic = AddSpellFunc(&SpellFunc_Illidan_Parasitic, Target_RandomPlayerNotCurrent, 0, 0, 0);
            //Phase 2 spells
            AddPhaseSpell(2, AddSpell(ILLIDAN_THROW_GLAIVE1, Target_Destination, 0, 0.5f, 0));
            AddPhaseSpell(2, AddSpell(ILLIDAN_THROW_GLAIVE2, Target_Self, 0, 0.5f, 0));
            AddPhaseSpell(2, AddSpell(ILLIDAN_FIREBALL, Target_RandomPlayer, 60, 2, 0));
            AddPhaseSpell(2, AddSpell(ILLIDAN_DARK_BARRAGE, Target_RandomPlayer, 40, 10, 40));
            AddPhaseSpell(2, AddSpell(ILLIDAN_GLAIVE_RETURNS, Target_Self, 0, 0.5f, 0));
            //Phase 3 spells
            AddPhaseSpell(3, AddSpell(ILLIDAN_SHEAR, Target_Current, 12, 1.5f, 15));
            AddPhaseSpell(3, AddSpell(ILLIDAN_DRAW_SOUL, Target_Self, 6, 1.5f, 40));    // Self-kills are bad =/
            AddPhaseSpell(3, AddSpell(ILLIDAN_FLAME_CRASH, Target_Destination, 12, 1.3f, 25));
            AddPhaseSpell(3, AddSpell(ILLIDAN_AGONIZING_FLAMES, Target_RandomDestination, 7, 0, 25, 0, 35, true));
            mShadowPrison = AddPhaseSpell(3, AddSpell(ILLIDAN_SHADOW_PRISON, Target_Self, 0, 0, 0));
            //Phase 4 spells
            mShadowBlast = AddPhaseSpell(4, AddSpell(ILLIDAN_SHADOW_BLAST, Target_Destination, 0, 2, 0));
            mFlameBurst = AddPhaseSpell(4, AddSpell(ILLIDAN_FLAME_BURST1, Target_Self, 0, 0, 20));
            mShadowDemons = AddPhaseSpell(4, AddSpell(ILLIDAN_SUMMON_DEMON, Target_Self, 0, 1.3f, 60));
            //Phase 5 spells
            AddPhaseSpell(5, AddSpell(ILLIDAN_SHEAR, Target_Current, 12, 1.5f, 15));
            AddPhaseSpell(5, AddSpell(ILLIDAN_DRAW_SOUL, Target_Self, 6, 1.5f, 40));    // Self-kills are bad =/
            AddPhaseSpell(5, AddSpell(ILLIDAN_FLAME_CRASH, Target_Destination, 12, 1.3f, 25));
            AddPhaseSpell(5, AddSpell(ILLIDAN_AGONIZING_FLAMES, Target_RandomDestination, 7, 0, 25, 0, 35, true));
            mEnrage = AddPhaseSpell(5, AddSpell(ILLIDAN_ENRAGE, Target_Self, 0, 1.3f, 0, 0, 0, true, "You've wasted too much time mortals, now you shall fall!", CHAT_MSG_MONSTER_YELL, 11474));

            AddEmote(Event_OnTargetDied, "Who shall be next to taste my blades?!", CHAT_MSG_MONSTER_YELL, 11473);
            AddEmote(Event_OnTargetDied, "This is too easy!", CHAT_MSG_MONSTER_YELL, 11472);

            SetEnrageInfo(AddSpell(ILLIDAN_BERSERK, Target_Self, 0, 0, 0), 1500000);

            _applyAura(ILLIDAN_SKULL_INTRO);
            setCanEnterCombat(false);
            _setWieldWeapon(false);
            setRooted(true);
            setFlyMode(false);
            SetWaypointMoveType(Movement::WP_MOVEMENT_SCRIPT_NONE);
            SetPhase(1);

            for (uint8 i = 1; i < ILLIDAN_WAYPOINT_SIZE; ++i)
            {
                AddWaypoint(CreateWaypoint(i, 0, Movement::WP_MOVE_TYPE_FLY, ForIllidan[i]));
            }

            _unit->SetBaseAttackTime(RANGED, 1800);
            _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
            _unit->SetDualWield(true);

            mFoA1 = mFoA2 = NULL;
            mAllow = true;

            mPhaseBackup = 0;
            mScenePart = 0;
            mTimeLeft = 0;
            mParasiticTimer = 0;
            mMovementTimer = 0;
            mFireWallTimer = 0;
            mLastFireWall = 0;
            mMiscEventPart = 0;
            mShadowDemonsTimer = 0;
            mFlameBurstTimer = 0;
            mPlaySound = 0;
            mDemonTimer = 0;
            mYellTimer = 0;
            mEnrageTimer = 0;
            mCurrentWaypoint = 0;
        }

        void OnCombatStart(Unit* pTarget)
        {
            GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
            GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
            if (pRightGate != NULL)
            {
                pRightGate->SetState(GO_STATE_CLOSED);
            }
            if (pLeftGate != NULL)
            {
                pLeftGate->SetState(GO_STATE_CLOSED);
            }

            _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);

            ParentClass::OnCombatStart(pTarget);
            CancelAllCooldowns();

            mParasitic->mEnabled = false;
        }

        void OnCombatStop(Unit* pTarget)
        {
            // General
            _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
            SetWaypointMoveType(Movement::WP_MOVEMENT_SCRIPT_NONE);
            SetTargetToChannel(NULL, 0);
            setCanEnterCombat(true);
            _setWieldWeapon(true);
            _setMeleeDisabled(true);
            setFlyMode(false);
            setRooted(false);

            mAIUpdateFrequency = 1000;
            mAllow = true;

            // Phase 2
            if (mFoA1 != NULL)
            {
                mFoA1->despawn(0);
                mFoA1 = NULL;
            }
            if (mFoA2 != NULL)
            {
                mFoA2->despawn(0);
                mFoA2 = NULL;
            }

            Creature* pTrigger = getNearestCreature(677.399963f, 305.545044f, 353.192169f, CN_FACE_TRIGGER);
            if (pTrigger != NULL)
            {
                pTrigger->Despawn(0, 0);
            }
            for (uint8 i = 0; i < 2; ++i)
            {
                Creature* pBlade = getNearestCreature(UnitPos[i].x, UnitPos[i].y, UnitPos[i].z, CN_BLADE_OF_AZZINOTH);
                if (pBlade != NULL)
                {
                    pBlade->SetChannelSpellTargetGUID(0);
                    pBlade->SetChannelSpellId(0);
                    pBlade->Despawn(0, 0);
                }
            }

            // Phase 4
            _setDisplayWeapon(true, true);
            _removeAura(ILLIDAN_DEMON_FORM);

            if (isAlive())
            {
                GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
                GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
                if (pRightGate != NULL)
                {
                    pRightGate->SetState(GO_STATE_OPEN);
                }
                if (pLeftGate != NULL)
                {
                    pLeftGate->SetState(GO_STATE_OPEN);
                }

                Creature* pMaiev = getNearestCreature(CN_MAIEV);
                if (pMaiev != NULL)
                {
                    pMaiev->Despawn(0, 0);
                }
            }

            ParentClass::OnCombatStop(pTarget);
        }

        void OnDied(Unit* pKiller)
        {
            Creature* pMaiev = getNearestCreature(CN_MAIEV);
            if (pMaiev != NULL && pMaiev->isAlive())
            {
                _unit->SetHealth(1);
                setCanEnterCombat(false);
                _applyAura(ILLIDAN_DEATH1);
                _applyAura(ILLIDAN_DEATH2);

                pMaiev->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9);
                pMaiev->GetAIInterface()->setAiState(AI_STATE_IDLE);
                pMaiev->GetAIInterface()->WipeTargetList();
                pMaiev->GetAIInterface()->WipeHateList();
            }
            else
            {
                GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
                GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
                if (pRightGate != NULL)
                {
                    pRightGate->SetState(GO_STATE_OPEN);
                }
                if (pLeftGate != NULL)
                {
                    pLeftGate->SetState(GO_STATE_OPEN);
                }
            }

            ParentClass::OnDied(pKiller);
        }

        // Does not work until it's hooked
        void OnHit(Unit* mVictim, float fAmount)
        {
            sendChatMessage(CHAT_MSG_MONSTER_YELL, 0, "ON HIT1!");
            if (mVictim->IsCreature() && (mVictim->GetEntry() == CN_MAIEV || mVictim->GetEntry() == CN_AKAMA))
            {
                sendChatMessage(CHAT_MSG_MONSTER_YELL, 0, "ON HIT2!");
                Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
                if (pTarget == NULL || !pTarget->IsPlayer())
                {
                    pTarget = _unit->GetAIInterface()->GetSecondHated();
                    if (pTarget == NULL || !pTarget->IsPlayer())
                    {
                        pTarget = GetBestPlayerTarget(TargetFilter_Closest);
                        if (pTarget == NULL)
                            return;
                    }
                }

                _unit->GetAIInterface()->setNextTarget(pTarget);
                _unit->GetAIInterface()->modThreatByPtr(pTarget, (int32)(fAmount * 2));
                _unit->GetAIInterface()->RemoveThreatByPtr(mVictim);
            }
        }

        void OnDamageTaken(Unit* mAttacker, uint32 fAmount)
        {
            if (mAttacker->IsCreature() && (mAttacker->GetEntry() == CN_MAIEV || mAttacker->GetEntry() == CN_AKAMA))
            {
                Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
                if (pTarget == NULL || !pTarget->IsPlayer())
                {
                    pTarget = _unit->GetAIInterface()->GetSecondHated();
                }
                if (pTarget != NULL && pTarget->IsPlayer())
                {
                    _unit->GetAIInterface()->setNextTarget(pTarget);
                    _unit->GetAIInterface()->modThreatByPtr(pTarget, fAmount * 3);
                }
                else
                    return;

                _unit->GetAIInterface()->RemoveThreatByPtr(mAttacker);
            }
        }

        void AIUpdate()
        {
            if (!mAllow)
                return;

            switch (GetPhase())
            {
                case 1:
                    PhaseOne();
                    break;
                case 2:
                    PhaseTwo();
                    break;
                case 3:
                    PhaseThree();
                    break;
                case 4:
                    PhaseFour();
                    break;
                case 5:
                    PhaseFive();
                    break;
                default:
                    return;
            }
        }

        void PhaseOne()
        {
            if (_getHealthPercent() <= 85)
            {
                if (mParasitic->mEnabled == false)
                {
                    mParasitic->mEnabled = true;
                    mParasiticTimer = 30000;
                }

                mParasiticTimer -= mAIUpdateFrequency;
                if (mParasiticTimer <= 0)
                {
                    CastSpellNowNoScheduling(mParasitic);
                    mParasiticTimer = 30000;
                    return;
                }
            }
            if (_unit->GetAIInterface()->isFlying())
            {
                Creature* pAI = spawnCreature(CN_FACE_TRIGGER, 677.399963f, 305.545044f, 353.192169f, 0.0f);
                if (pAI != NULL)
                {
                    pAI->m_noRespawn = true;

                    _unit->GetAIInterface()->setNextTarget(pAI);
                }

                SetPhase(2);
                ForceWaypointMove(1);
                SetAIUpdateFreq(500);

                mTimeLeft = BladeEvent[0];
                mScenePart = 1;
                mAllow = false;
                return;
            }
            else if (_getHealthPercent() <= 65 && !_isCasting())
            {
                Creature* pAkama = getNearestCreature(CN_AKAMA);
                if (pAkama != NULL && pAkama->GetScript() != NULL)
                {
                    AkamaAI* pAkamaAI = static_cast< AkamaAI* >(pAkama->GetScript());
                    if (pAkamaAI->mScenePart <= 2 && pAkamaAI->canEnterCombat())
                    {
                        pAkamaAI->setCanEnterCombat(false);
                        pAkamaAI->GetUnit()->GetAIInterface()->setAiState(AI_STATE_IDLE);
                        // ugly code, trows compile error if left just null, this should do it ~ azolex
                        uint32 nullfix = 0;
                        pAkamaAI->GetUnit()->GetAIInterface()->setNextTarget(nullfix);
                        pAkamaAI->GetUnit()->GetAIInterface()->WipeTargetList();
                        pAkamaAI->GetUnit()->GetAIInterface()->WipeHateList();
                        pAkamaAI->_setMeleeDisabled(false);
                        pAkamaAI->setRooted(true);

                        _unit->GetAIInterface()->RemoveThreatByPtr(pAkamaAI->GetUnit());
                    }
                }

                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11479, "I will not be touched by rabble such as you!");
                moveTo(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ() + 10.0f, false);
                setCanEnterCombat(false);
                _setMeleeDisabled(false);
                setFlyMode(true);

                _unit->SetEmoteState(EMOTE_ONESHOT_NONE);
                _unit->Emote(EMOTE_ONESHOT_LIFTOFF);

                mFireWallTimer = 30000;
                mMovementTimer = 40000;
                mMiscEventPart = 0;
                mLastFireWall = -1;
                return;
            }

            _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
            ParentClass::AIUpdate();
        }

        void PhaseTwo()
        {
            if (mScenePart != 0)
            {
                mTimeLeft -= mAIUpdateFrequency;
                if (mTimeLeft != 0)
                    return;

                switch (mScenePart - 1)
                {
                    case 0:
                        {
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11480, "Behold the flames of Azzinoth!");
                            SpellDesc* mGlaiveThrow = FindSpellById(ILLIDAN_THROW_GLAIVE2);
                            if (mGlaiveThrow != NULL)
                            {
                                CastSpellNowNoScheduling(mGlaiveThrow);
                            }
                        }
                        break;
                    case 1:
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            _unit->CastSpellAoF(LocationVector(UnitPos[i].x, UnitPos[i].y, UnitPos[i].z), sSpellCustomizations.GetSpellInfo(ILLIDAN_THROW_GLAIVE1), false);
                        }
                        _setWieldWeapon(false);
                        break;
                    case 2:
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            Creature* pBlade = spawnCreature(CN_BLADE_OF_AZZINOTH, UnitPos[i].x, UnitPos[i].y, UnitPos[i].z, UnitPos[i].o);
                            if (pBlade != NULL)
                            {
                                pBlade->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pBlade->m_noRespawn = true;
                            }
                        }
                        break;
                    case 3:
                        mFoA1 = SpawnCreature(22997, 672.039246f, 326.748322f, 354.206390f, 0.207343f);
                        mFoA2 = SpawnCreature(22997, 673.008667f, 283.813660f, 354.267548f, 6.203853f);
                        if (mFoA1 != NULL)
                        {
                            Unit* pBlade = getNearestCreature(UnitPos[0].x, UnitPos[0].y, UnitPos[0].z, CN_BLADE_OF_AZZINOTH);
                            if (pBlade != NULL)
                            {
                                pBlade->SetChannelSpellTargetGUID(mFoA1->GetUnit()->GetGUID());
                                pBlade->SetChannelSpellId(TEAR_OF_AZZINOTH_CHANNEL);
                            }
                        }
                        if (mFoA2 != NULL)
                        {
                            Unit* pBlade = getNearestCreature(UnitPos[1].x, UnitPos[1].y, UnitPos[1].z, CN_BLADE_OF_AZZINOTH);
                            if (pBlade != NULL)
                            {
                                pBlade->SetChannelSpellTargetGUID(mFoA2->GetUnit()->GetGUID());
                                pBlade->SetChannelSpellId(TEAR_OF_AZZINOTH_CHANNEL);
                            }
                        }
                        _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_2);
                        break;
                    case 4:
                        {
                            _unit->GetAIInterface()->SetAllowedToEnterCombat(true);
                            SetAIUpdateFreq(1000);
                            mScenePart = 0;

                            _unit->GetAIInterface()->setNextTarget(GetBestPlayerTarget(TargetFilter_Closest));
                            return;
                        }
                        break;
                    case 5:
                        for (uint8 i = 0 ; i < 2; ++i)
                        {
                            Creature* Blade = getNearestCreature(UnitPos[i].x, UnitPos[i].y, UnitPos[i].z, CN_BLADE_OF_AZZINOTH);
                            if (Blade != NULL)
                            {
                                Blade->Despawn(0, 0);
                            }
                        }
                        break;
                    case 6:
                        {
                            SpellDesc* mGlaiveReturns = FindSpellById(ILLIDAN_GLAIVE_RETURNS);
                            if (mGlaiveReturns != NULL)
                                CastSpellNowNoScheduling(mGlaiveReturns);

                            _applyAura(ILLIDAN_RAKE);
                            _removeAura(ILLIDAN_RAKE);
                            _setWieldWeapon(true);
                        }
                        break;
                    case 7:
                        setFlyMode(false);
                        _unit->Emote(EMOTE_ONESHOT_LAND);
                        break;
                    case 8:
                        {
                            Creature* pTrigger = getNearestCreature(677.399963f, 305.545044f, 353.192169f, CN_FACE_TRIGGER);
                            if (pTrigger != NULL)
                            {
                                pTrigger->Despawn(0, 0);
                            }

                            _unit->GetAIInterface()->setNextTarget(GetBestPlayerTarget(TargetFilter_Closest));
                            _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                            setCanEnterCombat(true);
                            _setMeleeDisabled(true);
                            setRooted(false);
                            _clearHateList();
                            SetPhase(3);
                            _unit->setUInt64Value(UNIT_FIELD_FLAGS, 0);

                            SetAIUpdateFreq(1000);

                            mParasiticTimer = 30000;
                            mShadowDemonsTimer = (RandomUInt(25) + 15) * 1000;
                            mDemonTimer = 40000;
                            mPhaseBackup = 3;
                            mScenePart = 0;
                            mPlaySound = false;
                            return;
                        }
                        break;
                }

                mTimeLeft = BladeEvent[mScenePart];
                ++mScenePart;
                return;
            }

            mMovementTimer -= mAIUpdateFrequency;
            mFireWallTimer -= mAIUpdateFrequency;
            if (mFoA1 != NULL)
            {
                if (!mFoA1->isAlive())
                {
                    Unit* pBlade = getNearestCreature(UnitPos[0].x, UnitPos[0].y, UnitPos[0].z, CN_BLADE_OF_AZZINOTH);
                    if (pBlade != NULL)
                    {
                        pBlade->SetChannelSpellTargetGUID(0);
                        pBlade->SetChannelSpellId(0);
                    }

                    mFoA1 = NULL;
                }
            }
            if (mFoA2 != NULL)
            {
                if (!mFoA2->isAlive())
                {
                    Unit* pBlade = getNearestCreature(UnitPos[1].x, UnitPos[1].y, UnitPos[1].z, CN_BLADE_OF_AZZINOTH);
                    if (pBlade != NULL)
                    {
                        pBlade->SetChannelSpellTargetGUID(0);
                        pBlade->SetChannelSpellId(0);
                    }

                    mFoA2 = NULL;
                }
            }
            if (_unit->GetAIInterface()->getWaypointScriptType() != Movement::WP_MOVEMENT_SCRIPT_WANTEDWP && !_isCasting())
            {
                if (_unit->GetChannelSpellId() == 0)
                {
                    if (mFoA1 == NULL && mFoA2 == NULL)
                    {
                        Unit* pTrigger = getNearestCreature(677.399963f, 305.545044f, 353.192169f, CN_FACE_TRIGGER);
                        if (pTrigger != NULL)
                        {
                            _unit->GetAIInterface()->setNextTarget(pTrigger);
                        }

                        ForceWaypointMove(4);
                        SetAIUpdateFreq(500);

                        mTimeLeft = BladeEvent[5];
                        mScenePart = 6;
                        mAllow = false;
                        return;
                    }
                    else if (mMovementTimer <= 0)
                    {
                        if (RandomUInt(1) == 1)    // Move right
                        {
                            ++mCurrentWaypoint;
                            if (mCurrentWaypoint > 3)
                                mCurrentWaypoint = 1;
                        }
                        else                    // Move left
                        {
                            --mCurrentWaypoint;
                            if (mCurrentWaypoint < 1)
                                mCurrentWaypoint = 3;
                        }

                        ForceWaypointMove(mCurrentWaypoint);

                        mMovementTimer = 50000;
                        mAllow = false;
                        return;
                    }
                }
                if (mMiscEventPart != 0)
                {
                    if (mMiscEventPart == 1)
                    {
                        uint32 FireWall = RandomUInt(7);
                        while((int)FireWall == mLastFireWall || (int)FireWall == 7 - mLastFireWall)
                        {
                            FireWall = RandomUInt(7);
                        }

                        Creature* pTrigger = spawnCreature(CN_EYE_BEAM_TRIGGER, EyeBeamPaths[FireWall].x, EyeBeamPaths[FireWall].y, EyeBeamPaths[FireWall].z, EyeBeamPaths[FireWall].o);
                        if (pTrigger != NULL && pTrigger->GetScript() != NULL)
                        {
                            sendChatMessage(CHAT_MSG_MONSTER_YELL, 11481, "Stare into the eyes of the Betrayer!");
                            SetTargetToChannel(pTrigger, ILLIDAN_EYE_BLAST1);
                            _unit->CastSpell(pTrigger, ILLIDAN_EYE_BLAST1, true);
                            _unit->GetAIInterface()->setNextTarget(pTrigger);

                            float Distance = pTrigger->CalcDistance(EyeBeamPaths[7 - FireWall].x, EyeBeamPaths[7 - FireWall].y, EyeBeamPaths[7 - FireWall].z);
                            uint32 TimeToReach = (uint32)(Distance * 1000 / pTrigger->getSpeedForType(TYPE_WALK));
                            EyeBeamTriggerAI* pEyeBeamTriggerAI = static_cast< EyeBeamTriggerAI* >(pTrigger->GetScript());
                            pEyeBeamTriggerAI->mPosition = FireWall;
                            pEyeBeamTriggerAI->despawn(TimeToReach + 1500, 0);
                            mFireWallTimer = TimeToReach + 1000;
                            mLastFireWall = FireWall;
                            mMiscEventPart = 2;
                        }
                        else
                        {
                            mFireWallTimer = 30000;
                            mMiscEventPart = 0;
                            mLastFireWall = -1;
                        }

                        return;
                    }
                    else if (mMiscEventPart == 2 && mFireWallTimer <= 0)
                    {
                        SetTargetToChannel(NULL, 0);
                        _removeAura(ILLIDAN_EYE_BLAST1);
                        _unit->GetAIInterface()->setNextTarget(GetBestPlayerTarget(TargetFilter_Closest));

                        mFireWallTimer = 30000;
                        mMiscEventPart = 0;
                        return;
                    }
                }
                else if (mFireWallTimer <= 0)
                {
                    mMiscEventPart = 1;
                    return;
                }

                ParentClass::AIUpdate();
            }
        }

        void Transform(Transformation* pTransformation, uint32 pMaxPart)
        {
            mTimeLeft -= mAIUpdateFrequency;
            if (mTimeLeft > 0)
                return;

            // Ugly ass code
            if (pTransformation[mMiscEventPart - 1].mEmoteType == 0)
                _unit->Emote((EmoteType)pTransformation[mMiscEventPart - 1].mEmote);
            else
                _unit->SetEmoteState(pTransformation[mMiscEventPart - 1].mEmote);
            sendChatMessage(CHAT_MSG_MONSTER_YELL, pTransformation[mMiscEventPart - 1].mSoundId, pTransformation[mMiscEventPart - 1].mText);
            _applyAura(pTransformation[mMiscEventPart - 1].mAura);
            _removeAura(pTransformation[mMiscEventPart - 1].mUnAura);
            _setWieldWeapon(pTransformation[mMiscEventPart - 1].mEquipWeapons);

            if (pTransformation == Ascend)
            {
                if (mMiscEventPart == 5)
                {
                    _clearHateList();
                    Unit* pTarget = GetBestPlayerTarget();
                    if (pTarget != NULL)
                    {
                        _unit->GetAIInterface()->AttackReaction(pTarget, 5000);
                    }
                    setAIAgent(AGENT_SPELL);
                    stopMovement();                    // readding settings after target switch
                    SetPhase(mPhaseBackup);            // without it he gets back to phase 1 and then immediatly to 2

                    if (mPlaySound)
                    {
                        switch (RandomUInt(2))
                        {
                            case 0:
                                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11469, "You know nothing of power!");
                                mPlaySound = false;
                                break;
                            case 1:
                                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11471, "Such arrogance!");
                                mPlaySound = false;
                                break;
                        }
                    }
                }
                else if (mMiscEventPart == 6)
                {
                    if (mPlaySound)
                    {
                        switch (RandomUInt(2))
                        {
                            case 0:
                                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11469, "You know nothing of power!");
                                mPlaySound = false;
                                break;
                            case 1:
                                sendChatMessage(CHAT_MSG_MONSTER_YELL, 11471, "Such arrogance!");
                                mPlaySound = false;
                                break;
                        }
                    }

                    _setDisplayWeapon(false, false);
                    SetAIUpdateFreq(1000);

                    mShadowDemonsTimer = (RandomUInt(25) + 15) * 1000;
                    mFlameBurstTimer = (RandomUInt(7) + 8) * 1000;
                    mDemonTimer = 60000;
                    mMiscEventPart = 0;

                    SetPhase(4);
                    PhaseFour();
                    return;
                }
            }
            else if (pTransformation == Descend)
            {
                if (mMiscEventPart == 3)
                {
                    _clearHateList();
                    SetPhase(4);            // without it he gets back to phase 1 and then immediatly to 2
                }
                else if (mMiscEventPart == 6)
                {
                    setAIAgent(AGENT_NULL);
                    SetPhase(mPhaseBackup);
                    if (mPhaseBackup == 5)
                    {
                        mEnrageTimer = RandomUInt(5) + 25;
                    }

                    SetAIUpdateFreq(1000);

                    mParasiticTimer = 30000;
                    mEnrageTimer = 25000;
                    mDemonTimer = 60000;
                    mYellTimer = 25000;
                    mMiscEventPart = 0;
                    return;
                }
            }

            ++mMiscEventPart;
            if (mMiscEventPart > pMaxPart)
                return;

            mTimeLeft = pTransformation[mMiscEventPart - 1].mTimer;
        }

        bool SpawnMaiev()    // this doesn't have collision checks! so keep in mind that Maiev can be spawned behind walls!
        {
            float xchange  = RandomFloat(15.0f);
            float distance = 15.0f;

            float ychange = sqrt(distance * distance - xchange * xchange);

            if (RandomUInt(1) == 1)
                xchange *= -1;
            if (RandomUInt(1) == 1)
                ychange *= -1;

            float newposx = _unit->GetPositionX() + xchange;
            float newposy = _unit->GetPositionY() + ychange;

            MoonScriptCreatureAI* pMaievAI = SpawnCreature(CN_MAIEV, newposx, newposy, _unit->GetPositionZ() + 0.5f, 2.177125f);
            if (pMaievAI == NULL)
            {
                UnstuckFromShadowPrison();
                return false;
            }

            pMaievAI->GetUnit()->GetAIInterface()->setNextTarget(_unit);
            return true;
        }

        void UnstuckFromShadowPrison()
        {
            _removeAuraOnPlayers(ILLIDAN_SHADOW_PRISON);
            setCanEnterCombat(true);
            _setMeleeDisabled(true);
            setRooted(false);

            _unit->setUInt64Value(UNIT_FIELD_FLAGS, 0);

            mScenePart = 0;
        }

        void MaievScene()    // thinking about converting Maiev's functions to MoonScriptCreatureAI functions
        {
            mTimeLeft -= mAIUpdateFrequency;
            if (mTimeLeft > 0)
                return;

            MaievAI* pMaievAI = NULL;
            if (mScenePart > 3)
            {
                Creature* pMaiev = getNearestCreature(CN_MAIEV);
                if (pMaiev != NULL && pMaiev->GetScript() != NULL)
                {
                    pMaievAI = static_cast< MaievAI* >(pMaiev->GetScript());
                }
                else
                {
                    UnstuckFromShadowPrison();
                    return;
                }
            }

            switch (mScenePart)
            {
                case 1:
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11476, "Is this it, mortals? Is this all the fury you can muster?");
                    break;
                case 2:
                    _unit->Emote(EMOTE_ONESHOT_QUESTION);
                    break;
                case 3:
                    if (!SpawnMaiev())
                        return;
                    break;
                case 4:
                    pMaievAI->_applyAura(MAIEV_TELEPORT);
                    _unit->GetAIInterface()->setNextTarget(pMaievAI->GetUnit());
                    break;
                case 5:
                    pMaievAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11491, "Their fury pales before mine, Illidan. We have some unsettled business between us.");
                    break;
                case 6:
                    pMaievAI->_setWieldWeapon(true);
                    break;
                case 7:
                    pMaievAI->GetUnit()->Emote(EMOTE_ONESHOT_EXCLAMATION);
                    pMaievAI->_setDisplayWeapon(false, false);
                    break;
                case 8:
                    {
                    sendChatMessage(CHAT_MSG_MONSTER_YELL, 11477, "Maiev... How is it even possible?");
                        pMaievAI->_setDisplayWeapon(true, true);
                    }
                    break;
                case 9:
                    {
                        float Facing = pMaievAI->GetUnit()->calcRadAngle(_unit->GetPositionX(), _unit->GetPositionY(), pMaievAI->GetUnit()->GetPositionX(), pMaievAI->GetUnit()->GetPositionY());
                        _unit->SetFacing(Facing);
                        _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                    }
                    break;
                case 10:
                    pMaievAI->sendChatMessage(CHAT_MSG_MONSTER_YELL, 11492, "Ah, my long hunt is finally over. Today, Justice will be done!");
                    pMaievAI->GetUnit()->Emote(EMOTE_ONESHOT_EXCLAMATION);
                    pMaievAI->_setDisplayWeapon(false, false);
                    break;
                case 11:
                    pMaievAI->GetUnit()->Emote(EMOTE_ONESHOT_YES);
                    pMaievAI->_setDisplayWeapon(true, true);
                    break;
                case 12:
                    pMaievAI->GetUnit()->Emote(EMOTE_ONESHOT_ROAR);
                    break;
                case 13:
                    setCanEnterCombat(true);
                    _setMeleeDisabled(true);
                    setRooted(false);
                    SetPhase(5);

                    _unit->setUInt64Value(UNIT_FIELD_FLAGS, 0);

                    pMaievAI->RegisterAIUpdateEvent(1000);
                    pMaievAI->mYellTimer = pMaievAI->_addTimer((RandomUInt(20) + 20) * 1000);
                    pMaievAI->mTrapTimer = pMaievAI->_addTimer((RandomUInt(5) + 18) * 1000);
                    pMaievAI->GetUnit()->SetEmoteState(EMOTE_ONESHOT_READY1H);
                    pMaievAI->setCanEnterCombat(true);
                    pMaievAI->GetUnit()->GetAIInterface()->setCurrentAgent(AGENT_NULL);
                    pMaievAI->GetUnit()->GetAIInterface()->setAiState(AI_STATE_IDLE);
                    pMaievAI->setRooted(false);
                    pMaievAI->GetUnit()->GetAIInterface()->AttackReaction(_unit, 1, 0);
                    pMaievAI->GetUnit()->SetEmoteState(EMOTE_ONESHOT_READY1H);
                    pMaievAI->_setWieldWeapon(true);
                    pMaievAI->mIllidanAI = this;

                    _clearHateList();
                    _unit->GetAIInterface()->AttackReaction(pMaievAI->GetUnit(), 200);

                    mParasiticTimer = 30000;
                    mDemonTimer = 60000;
                    mPhaseBackup = 5;
                    mEnrageTimer = 25000;
                    mYellTimer = 25000;
                    break;
            }

            ++mScenePart;
            if (mScenePart > 13)
            {
                mScenePart = 0;
                return;
            }

            mTimeLeft = MaievTimers[mScenePart - 1];
        }

        void PhaseThree()
        {
            if (mMiscEventPart != 0)
            {
                Transform(Ascend, 6);
                return;
            }
            else if (mScenePart != 0)
            {
                MaievScene();
                return;
            }
            else if (_unit->GetHealthPct() <= 30 && !_isCasting())
            {
#ifdef USE_SHADOW_PRISON
                stopMovement();
                CastSpellNowNoScheduling(mShadowPrison);
#endif

                setCanEnterCombat(false);
                setAIAgent(AGENT_NULL);
                _setMeleeDisabled(false);
                setRooted(true);

                _unit->SetEmoteState(EMOTE_ONESHOT_NONE);

                mTimeLeft = MaievTimers[0];
                mScenePart = 1;
                return;
            }

            mDemonTimer -= mAIUpdateFrequency;
            mParasiticTimer -= mAIUpdateFrequency;
            if (!_isCasting())
            {
                if (mDemonTimer <= 0)
                {
                    stopMovement();
                    setAIAgent(AGENT_SPELL);
                    SetAIUpdateFreq(250);

                    _unit->SetEmoteState(EMOTE_ONESHOT_NONE);

                    mMiscEventPart = 1;
                    mTimeLeft = Ascend[0].mTimer;
                    return;
                }
                if (mParasiticTimer <= 0)
                {
                    CastSpellNowNoScheduling(mParasitic);
                    mParasiticTimer = 30000;
                    return;
                }

                _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                ParentClass::AIUpdate();
            }
        }

        void PhaseFour()
        {
            if (mPlaySound)
            {
                switch (RandomUInt(2))
                {
                    case 0:
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11469, "You know nothing of power!");
                        mPlaySound = false;
                        break;
                    case 1:
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11471, "Such arrogance!");
                        mPlaySound = false;
                        break;
                }
            }

            mDemonTimer -= mAIUpdateFrequency;
            mFlameBurstTimer -= mAIUpdateFrequency;
            mShadowDemonsTimer -= mAIUpdateFrequency;
            if (_isCasting())
            {
                mPlaySound = false;
                return;
            }

            if (mMiscEventPart != 0)
            {
                Transform(Descend, 6);
                return;
            }

            if (mDemonTimer <= 0 || (_getHealthPercent() <= 30 && mPhaseBackup == 3))
            {
                _setDisplayWeapon(true, true);
                stopMovement();
                setAIAgent(AGENT_SPELL);
                SetAIUpdateFreq(250);

                _unit->SetEmoteState(EMOTE_ONESHOT_NONE);

                mMiscEventPart = 1;
                mTimeLeft = Descend[0].mTimer;
            }
            else if (_unit->GetAIInterface()->getNextTarget() != NULL)
            {
                Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
                if (getRangeToObject(pTarget) <= 80.0f)
                {
                    setAIAgent(AGENT_SPELL);
                    stopMovement();

                    if (mShadowDemonsTimer <= 0)
                    {
                        CastSpellNowNoScheduling(mShadowDemons);
                        SpawnCreature(CN_SHADOW_DEMON, _unit->GetPositionX() + RandomFloat(5), _unit->GetPositionY() + RandomFloat(5), _unit->GetPositionZ() + 2.0f, 0, true);
                        SpawnCreature(CN_SHADOW_DEMON, _unit->GetPositionX() - RandomFloat(5), _unit->GetPositionY() + RandomFloat(5), _unit->GetPositionZ() + 2.0f, 0, true);
                        SpawnCreature(CN_SHADOW_DEMON, _unit->GetPositionX() + RandomFloat(5), _unit->GetPositionY() - RandomFloat(5), _unit->GetPositionZ() + 2.0f, 0, true);

                        mShadowDemonsTimer = 120000;
                        return;
                    }
                    if (mFlameBurstTimer <= 0)
                    {
                        CastSpellNowNoScheduling(mFlameBurst);
                        for (std::set< Object* >::iterator itr = _unit->GetInRangePlayerSetBegin(); itr != _unit->GetInRangePlayerSetEnd(); ++itr)
                        {
                            Unit* pUnit = static_cast< Unit* >(*itr);
                            MoonScriptCreatureAI* pAI = SpawnCreature(CN_FLAME_BURST, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, true);
                            _unit->CastSpell(pUnit, ILLIDAN_FLAME_BURST2, true);
                            if (pAI != NULL)
                            {
                                float Distance = getRangeToObject(pUnit);
                                if (Distance == 0.0f)
                                {
                                    pAI->RegisterAIUpdateEvent(300);        // o'rly?
                                }
                                else
                                {
                                    pAI->RegisterAIUpdateEvent((uint32)(Distance * 1000 / 32.796));
                                }
                            }
                        }

                        mFlameBurstTimer += 20000;
                        return;
                    }

                    uint32 Spell = RandomUInt() % 100;
                    if (Spell <= 80)
                        CastSpellNowNoScheduling(mShadowBlast);
                }
                else
                {
                    setAIAgent(AGENT_NULL);
                }
            }
        }

        void PhaseFive()
        {
            if (mMiscEventPart != 0)
            {
                Transform(Ascend, 6);
                return;
            }

            mYellTimer -= mAIUpdateFrequency;
            mDemonTimer -= mAIUpdateFrequency;
            mEnrageTimer -= mAIUpdateFrequency;
            mParasiticTimer -= mAIUpdateFrequency;
            if (_unit->HasAura(40760) || _unit->HasAura(40695))
            {
                mEnrageTimer = 120000;
            }
            else if (!_isCasting())
            {
                if (mDemonTimer <= 0)
                {
                    stopMovement();
                    setAIAgent(AGENT_SPELL);
                    SetAIUpdateFreq(250);

                    _unit->SetEmoteState(EMOTE_ONESHOT_NONE);

                    mMiscEventPart = 1;
                    mTimeLeft = Ascend[0].mTimer;
                    mPlaySound = true;
                    return;
                }
                if (mEnrageTimer <= 0)
                {
                    CastSpellNowNoScheduling(mEnrage);
                    mEnrageTimer = 120000;
                    mYellTimer += 7000;
                    return;
                }
                if (mParasiticTimer <= 0)
                {
                    CastSpellNowNoScheduling(mParasitic);
                    mParasiticTimer = 30000;
                    return;
                }
                if (mYellTimer <= 0 && _unit->GetAIInterface()->getNextTarget() != NULL)
                {
                    Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
                    if (pTarget->IsCreature() && pTarget->GetEntry() == CN_MAIEV)
                    {
                        sendChatMessage(CHAT_MSG_MONSTER_YELL, 11470, "Feel the hatred of ten thousand years!");
                        mYellTimer = 25000;
                    }
                }

                _unit->SetEmoteState(EMOTE_ONESHOT_READY1H);
                ParentClass::AIUpdate();
            }
        }

        void OnReachWP(uint32 pWaypointId, bool pForwards)
        {
            if (pWaypointId == 1)
            {
                _clearHateList();
                Unit* pTarget = _unit->GetAIInterface()->getNextTarget();
                if (pTarget != NULL && (!pTarget->IsCreature() || pTarget->GetEntry() != CN_FACE_TRIGGER))
                {
                    Creature* pTrigger = getNearestCreature(677.399963f, 305.545044f, 353.192169f, CN_FACE_TRIGGER);
                    if (pTrigger != NULL)
                    {
                        _unit->GetAIInterface()->setNextTarget(pTrigger);
                    }
                }
            }

            StopWaypointMovement();

            mCurrentWaypoint = pWaypointId;
            mAllow = true;
        }

        void Destroy()
        {
            Creature* pAkama = getNearestCreature(CN_AKAMA);
            if (pAkama != NULL && pAkama->GetScript() != NULL)
            {
                AkamaAI* pAI = static_cast< AkamaAI* >(pAkama->GetScript());
                if (pAI != NULL)
                {
                    if (pAI->mIllidanAI == this)
                        pAI->mIllidanAI = NULL;
                }
            }

            Creature* pMaiev = getNearestCreature(CN_MAIEV);
            if (pMaiev != NULL && pMaiev->GetScript() != NULL)
            {
                MaievAI* pAI = static_cast< MaievAI* >(pMaiev->GetScript());
                if (pAI != NULL)
                {
                    if (pAI->mIllidanAI == this)
                        pAI->mIllidanAI = NULL;
                }
            }

            // commented - due to the creature not being in world when this is called, mapmgr == null -> access violation
            /*        GameObject* pRightGate = getNearestGameObject(745.07f, 241.802f, 354.292f, 200000);
                    GameObject* pLeftGate  = getNearestGameObject(744.829f, 369.276f, 354.324f, 200001);
                    if (pRightGate != NULL)
                    {
                        pRightGate->SetState(GO_STATE_OPEN);
                    }
                    if (pLeftGate != NULL)
                    {
                        pLeftGate->SetState(GO_STATE_OPEN);
                    }*/

            delete this;
        }

        // Global variables
        uint32 mPhaseBackup;
        uint32 mScenePart;
        int32 mTimeLeft;
        bool mAllow;

        // Phase 1 variables
        int32 mParasiticTimer;
        SpellDesc* mParasiticDmg;
        SpellDesc* mParasitic;

        // Phase 2 variables
        MoonScriptCreatureAI* mFoA1;
        MoonScriptCreatureAI* mFoA2;
        int32 mMovementTimer;
        int32 mFireWallTimer;
        int32 mLastFireWall;
        uint32 mMiscEventPart;

        // Phase 3 variables
        SpellDesc* mShadowPrison;

        // Phase 4 variables
        int32 mShadowDemonsTimer;
        int32 mFlameBurstTimer;
        SpellDesc* mFlameBurst;
        SpellDesc* mShadowDemons;
        SpellDesc* mShadowBlast;
        bool mPlaySound;

        // Phase 3 & 4 variables
        int32 mDemonTimer;

        // Phase 5 variables
        int32 mYellTimer;
        int32 mEnrageTimer;
        SpellDesc* mEnrage;

        // Temporary variables
        uint32 mCurrentWaypoint;
};

void SpellFunc_Illidan_Parasitic(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    IllidanStormrageAI* Illidan = (pCreatureAI != NULL) ? static_cast< IllidanStormrageAI* >(pCreatureAI) : NULL;
    if (Illidan != NULL)
    {
        Illidan->CastSpell(Illidan->mParasiticDmg);
        if (pTarget != NULL)                        // not sure if target is really added here
        {
            // Workaround - we will spawn 2 Parasitic Shadowfiends on that player place
            Illidan->SpawnCreature(CN_PARASITIC_SHADOWFIEND, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation(), false);
            Illidan->SpawnCreature(CN_PARASITIC_SHADOWFIEND, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation(), false);
        }
    }
}

/*END
            .:Rewritten Illidan Script by M4ksiu:.
END*/

// Cage Trap Trigger AI
const uint32 CN_CAGE_TRAP_DISTURB_TRIGGER = 23304;       // GO data taken from UDB
const uint32 CN_CAGE_TRAP_TRIGGER = 23292;      // 23292 - 9
const uint32 GO_CAGE_TRAP = 185916;

const uint32 CAGE_TRAP = 40760;
const uint32 CAGED1 = 40695;
const uint32 CAGED2 = 40713;       // must find proper spell for it

float PositionAdds[8][2] =
{
    { 0, 1 },
    { 0.5, 0.5 },
    { 1, 0 },
    { 0.5, -0.5 },
    { 0, 1 },
    { -0.5, 0.5 },
    { -1, 0 },
    { -0.5, 0.5 }
};

class CageTrapTriggerAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(CageTrapTriggerAI, MoonScriptCreatureAI);
        CageTrapTriggerAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            _unit->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            _unit->GetAIInterface()->m_canMove = false;
            _unit->m_noRespawn = true;

            RegisterAIUpdateEvent(1000);
            _setMeleeDisabled(false);
            mIsActivated = false;
            mHasTrapped = false;
        }

        void OnCombatStart(Unit* pTarget)
        {}

        void OnCombatStop(Unit* pTarget)
        {
            setAIAgent(AGENT_NULL);
            _unit->GetAIInterface()->setAiState(AI_STATE_IDLE);
        }

        void AIUpdate()
        {
            Unit* pIllidan = getNearestCreature(22917);
            if (pIllidan != NULL)
            {
                IllidanStormrageAI* pAI = static_cast< IllidanStormrageAI* >(static_cast<Creature*>(pIllidan)->GetScript());
                if (pAI->mMiscEventPart != 0 && mTriggerAIList.size() == 0)
                {
                    GameObject* pGameObject = getNearestGameObject(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), GO_CAGE_TRAP);
                    if (pGameObject != NULL)
                    {
                        pGameObject->Despawn(0, 0);
                    }

                    despawn(0);
                    return;
                }

                if (!mIsActivated)
                    return;

                if (mTriggerAIList.size() == 0 && pIllidan->isAlive() && getRangeToObject(pIllidan) <= 5.0f && !pAI->_isCasting())
                {
                    for (uint8 i = 0; i < 8; ++i)
                    {
                        MoonScriptCreatureAI* pTriggerAI = SpawnCreature(CN_CAGE_TRAP_TRIGGER, _unit->GetPositionX() + PositionAdds[i][0], _unit->GetPositionY() + PositionAdds[i][1], _unit->GetPositionZ(), _unit->GetOrientation());
                        if (pTriggerAI != NULL)
                        {
                            pTriggerAI->GetUnit()->setUInt64Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pTriggerAI->GetUnit()->GetAIInterface()->m_canMove = false;
                            pTriggerAI->GetUnit()->m_noRespawn = true;
                            mTriggerAIList.push_back(pTriggerAI);
                        }
                    }

                    pAI->_applyAura(CAGE_TRAP);
                    pAI->setRooted(true);
                    pAI->_setMeleeDisabled(false);
                    pAI->stopMovement();

                    SetAIUpdateFreq(2500);
                    return;
                }
                else if (pIllidan->isAlive() && !mHasTrapped && mTriggerAIList.size() != 0)
                {
                    pAI->_removeAura(ILLIDAN_ENRAGE);
                    pAI->_applyAura(CAGED1);
                    SetAIUpdateFreq(14000);

                    for (size_t i = 0; i < mTriggerAIList.size() / 2; ++i)
                    {
                        MoonScriptCreatureAI* pTriggerAI1 = mTriggerAIList[i];
                        MoonScriptCreatureAI* pTriggerAI2 = mTriggerAIList[mTriggerAIList.size() - i - 1];
                        pTriggerAI1->SetTargetToChannel(pTriggerAI2->GetUnit(), 40708);//CAGED2);
                        pTriggerAI2->SetTargetToChannel(pTriggerAI1->GetUnit(), 40709);//CAGED2);
                    }

                    mHasTrapped = true;
                    return;
                }
                else if (mHasTrapped || !pIllidan->isAlive())
                {
                    GameObject* pGameObject = getNearestGameObject(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), GO_CAGE_TRAP);
                    if (pGameObject != NULL)
                    {
                        pGameObject->Despawn(0, 0);
                    }

                    // Immediatly Illidan, after trap expires, becomes Demon
                    if (pIllidan->isAlive())
                    {
                        pAI->_removeAura(CAGED1);
                        pAI->setRooted(false);
                        pAI->_setMeleeDisabled(true);
                        pAI->stopMovement();
                        pAI->setAIAgent(AGENT_SPELL);

                        pIllidan->SetEmoteState(EMOTE_ONESHOT_NONE);

                        pAI->SetAIUpdateFreq(250);
                        pAI->mMiscEventPart = 1;
                        pAI->mTimeLeft = 250;
                    }
                    for (size_t i = 0; i < mTriggerAIList.size(); ++i)
                    {
                        MoonScriptCreatureAI* pTriggerAI = mTriggerAIList[i];
                        pTriggerAI->SetTargetToChannel(NULL, 0);
                        pTriggerAI->despawn(0);
                    }

                    mTriggerAIList.clear();
                    RemoveAIUpdateEvent();
                    despawn(0);
                    return;
                }
            }
            else
            {
                GameObject* pGameObject = getNearestGameObject(_unit->GetPositionX(), _unit->GetPositionY(), _unit->GetPositionZ(), GO_CAGE_TRAP);
                if (pGameObject != NULL)
                {
                    pGameObject->Despawn(0, 0);
                }

                for (size_t i = 0; i < mTriggerAIList.size(); ++i)
                {
                    MoonScriptCreatureAI* pTriggerAI = mTriggerAIList[i];
                    pTriggerAI->despawn(0);
                }

                mTriggerAIList.clear();
                RemoveAIUpdateEvent();
                despawn(0);
            }
        }

        std::vector<MoonScriptCreatureAI*>    mTriggerAIList;
        bool                                mIsActivated;
        bool                                mHasTrapped;
};

// Animation progress is wrong - it should be closed and OnActive - open

class CageTrapGO : public GameObjectAIScript
{
    public:
        CageTrapGO(GameObject* pGameObject) : GameObjectAIScript(pGameObject)
        {
            _gameobject->setFloatValue(OBJECT_FIELD_SCALE_X, 3);
        }

        void OnActivate(Player* pPlayer)
        {
            _gameobject->SetFlags(GO_FLAG_NONSELECTABLE);
            Creature* pTrigger = _gameobject->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(_gameobject->GetPositionX(), _gameobject->GetPositionY(), _gameobject->GetPositionZ(), CN_CAGE_TRAP_DISTURB_TRIGGER);
            if (pTrigger != NULL && pTrigger->GetScript() != NULL)
            {
                CageTrapTriggerAI* pTriggerAI = static_cast< CageTrapTriggerAI* >(pTrigger->GetScript());
                pTriggerAI->mIsActivated = true;
            }
        }

        static GameObjectAIScript* Create(GameObject* pGameObject) { return new CageTrapGO(pGameObject); }
};

// Udalo and Olum AI
const uint32 CN_UDALO = 23410;
const uint32 CN_OLUM = 23411;

class DranaeiSpiritAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(DranaeiSpiritAI, MoonScriptCreatureAI);
        DranaeiSpiritAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            despawn(45000);
            setCanEnterCombat(false);
            _unit->m_noRespawn = true;
        }

        void Destroy()
        {
            Creature* pAkama = getNearestCreature(22990);
            if (pAkama != NULL && pAkama->GetScript() != NULL)
            {
                AkamaAI* pAI = static_cast< AkamaAI* >(pAkama->GetScript());
                if (pAI != NULL)
                {
                    if (pAI->mUdaloAI == this)
                        pAI->mUdaloAI = NULL;
                    else if (pAI->mOlumAI == this)
                        pAI->mOlumAI = NULL;
                }
            }

            delete this;
        }
};

// Flame of Azzinoth AI
const uint32 CN_FLAME_OF_AZZINOTH = 22997;

const uint32 FLAME_OF_AZZINOTH_BLAZE = 40637;       // this one just summons mob that will trigger that fire cloud - currently not used
const uint32 FLAME_OF_AZZINOTH_FLAME_BLAST = 40631;
const uint32 FLAME_OF_AZZINOTH_ENRAGE = 40683;        // 40683 or 40743
const uint32 FLAME_OF_AZZINOTH_CHARGE = 40602;        // CHAOS_CHARGE 40497 CHARGE 40602

void SpellFunc_FlameOfAzzinothFlameBlast(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);
void SpellFunc_FlameOfAzzinothCharge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType);

class FlameOfAzzinothAI : public MoonScriptCreatureAI
{
        MOONSCRIPT_FACTORY_FUNCTION(FlameOfAzzinothAI, MoonScriptCreatureAI);
        FlameOfAzzinothAI(Creature* pCreature) : MoonScriptCreatureAI(pCreature)
        {
            mFlameBlast = AddSpell(FLAME_OF_AZZINOTH_FLAME_BLAST, Target_Self, 0, 0, 0);
            AddSpellFunc(&SpellFunc_FlameOfAzzinothFlameBlast, Target_Current, 15, 0, 25, 0, 10);

            mCharge = AddSpell(FLAME_OF_AZZINOTH_CHARGE, Target_Current, 0, 0, 0);
            mEnrage = AddSpell(FLAME_OF_AZZINOTH_ENRAGE, Target_Self, 0, 0, 0);
            mChargeSpellFunc = AddSpellFunc(&SpellFunc_FlameOfAzzinothCharge, Target_Current, 0, 0, 60);

            _unit->m_noRespawn = true;

            Unit* pTarget = GetBestPlayerTarget(TargetFilter_Closest);
            if (pTarget != NULL)
            {
                _unit->GetAIInterface()->AttackReaction(pTarget, 200);
                OnCombatStart(pTarget);
            }

            mBlaze = 0;
        }

        void OnCombatStart(Unit* pTarget)
        {
            ParentClass::OnCombatStart(pTarget);
            CancelAllCooldowns();
        }

        void AIUpdate()
        {
            // Uh... so ugly, but that's what Wiki says
            Unit* pBlade1 = getNearestCreature(UnitPos[0].x, UnitPos[0].y, UnitPos[0].z, 22996);
            Unit* pBlade2 = getNearestCreature(UnitPos[1].x, UnitPos[1].y, UnitPos[1].z, 22996);
            if (pBlade1 != NULL && pBlade2 != NULL && mChargeSpellFunc->mLastCastTime + mChargeSpellFunc->mCooldown <= (uint32)time(NULL))
            {
                for (std::set< Object* >::iterator itr = _unit->GetInRangePlayerSetBegin(); itr != _unit->GetInRangePlayerSetEnd(); ++itr)
                {
                    // && or || ? - not sure about details too
                    if ((*itr)->CalcDistance(pBlade1) > 40.0f || (*itr)->CalcDistance(pBlade2) > 40.0f)
                    {
                        Unit* pUnit = static_cast< Unit* >(*itr);
                        _clearHateList();
                        _unit->GetAIInterface()->setNextTarget(pUnit);
                        _unit->GetAIInterface()->AttackReaction(pUnit, 10000);
                        CastSpellNowNoScheduling(mChargeSpellFunc);
                        break;
                    }
                }
            }

            ParentClass::AIUpdate();
        }

        void Destroy()
        {
            Creature* pIllidan = getNearestCreature(22917);
            if (pIllidan != NULL && pIllidan->GetScript() != NULL)
            {
                IllidanStormrageAI* pAI = static_cast< IllidanStormrageAI* >(pIllidan->GetScript());
                if (pAI != NULL)
                {
                    if (pAI->mFoA1 == this)
                        pAI->mFoA1 = NULL;
                    else if (pAI->mFoA2 == this)
                        pAI->mFoA2 = NULL;
                }
            }

            delete this;
        }

        // Flame Blast Spell Function
        SpellDesc* mFlameBlast;
        SpellDesc* mBlaze;

        // Charge Spell Function
        SpellDesc* mChargeSpellFunc;
        SpellDesc* mCharge;
        SpellDesc* mEnrage;
};

void SpellFunc_FlameOfAzzinothFlameBlast(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    FlameOfAzzinothAI* FlameOfAzzinoth = (pCreatureAI) ? static_cast< FlameOfAzzinothAI* >(pCreatureAI) : NULL;
    if (FlameOfAzzinoth)
    {
        FlameOfAzzinoth->CastSpell(FlameOfAzzinoth->mFlameBlast);
        FlameOfAzzinoth->SpawnCreature(CN_BLAZE_EFFECT, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation());
    }
}

void SpellFunc_FlameOfAzzinothCharge(SpellDesc* pThis, MoonScriptCreatureAI* pCreatureAI, Unit* pTarget, TargetType pType)
{
    FlameOfAzzinothAI* FlameOfAzzinoth = (pCreatureAI) ? static_cast< FlameOfAzzinothAI* >(pCreatureAI) : NULL;
    if (FlameOfAzzinoth)
    {
        FlameOfAzzinoth->CastSpell(FlameOfAzzinoth->mCharge);
        FlameOfAzzinoth->CastSpell(FlameOfAzzinoth->mEnrage);
    }
}

void SetupBlackTemple(ScriptMgr* mgr)
{
#ifndef UseNewMapScriptsProject
    mgr->register_instance_script(MAP_BLACK_TEMPLE, &BlackTempleScript::Create);
#endif
    //////////////////////////////////////////////////////////////////////////////////////////
    ///////// Mobs
    mgr->register_creature_script(CN_DRAGON_TURTLE, &DragonTurtleAI::Create);
    mgr->register_creature_script(CN_LEVIATHAN, &LeviathanAI::Create);
    mgr->register_creature_script(CN_MUTANT_WAR_HOUND, &MutantWarHoundAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_RIDING_HOUND, &ShadowmoonRidingHoundAI::Create);
    mgr->register_creature_script(CN_SISTER_OF_PLEASURE, &SisterOfPleasureAI::Create);
    mgr->register_creature_script(CN_SISTER_OF_PAIN, &SisterOfPainAI::Create);
    mgr->register_creature_script(CN_PRIESTESS_OF_DEMENTIA, &PriestessOfDementiaAI::Create);
    mgr->register_creature_script(CN_PRIESTESS_OF_DELIGHT, &PriestessOfDelightAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_NIGHTLORD, &IllidariNightlordAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_HEARTSEEKER, &IllidariHeartseekerAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_FEARBRINGER, &IllidariFearbringerAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_DEFILER, &IllidariDefilerAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_CENTURION, &IllidariCenturionAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_BONESLICER, &IllidariBoneslicerAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_BATTLELORD, &AshtongueBattlelordAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_DEFENDER, &AshtongueDefenderAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_ELEMENTALIST, &AshtongueElementalistAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_MYSTIC, &AshtongueMysticAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_PRIMALIST, &AshtonguePrimalistAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_ROGUE, &AshtongueRogueAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_SPIRITBINDER, &AshtongueSpiritbinderAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_STALKER, &AshtongueStalkerAI::Create);
    mgr->register_creature_script(CN_ASHTONGUE_STORMCALLER, &AshtongueStormcallerAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_BEHEMOTH, &BonechewerBehemothAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_BLADE_FURY, &BonechewerBladeFuryAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_BLOOD_PROPHET, &BonechewerBloodProphetAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_BRAWLER, &BonechewerBrawlerAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_COMBATANT, &BonechewerCombatantAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_SHIELD_DISCIPLE, &BonechewerShieldDiscipleAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_SPECTATOR, &BonechewerSpectatorAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_TASKMASTER, &BonechewerTaskmasterAI::Create);
    mgr->register_creature_script(CN_BONECHEWER_WORKER, &BonechewerWorkerAI::Create);
    mgr->register_creature_script(CN_CHARMING_COURTESAN, &CharmingCourtesanAI::Create);
    mgr->register_creature_script(CN_COILSKAR_GENERAL, &CoilskarGeneralAI::Create);
    mgr->register_creature_script(CN_COILSKAR_HARPOONER, &CoilskarHarpoonerAI::Create);
    mgr->register_creature_script(CN_COILSKAR_SEACALLER, &CoilskarSeacallerAI::Create);
    mgr->register_creature_script(CN_COILSKAR_SOOTHSAYER, &CoilskarSoothsayerAI::Create);
    mgr->register_creature_script(CN_COILSKAR_WRANGLER, &CoilskarWranglerAI::Create);
    mgr->register_creature_script(CN_DRAGONMAW_SKY_STALKER, &DragonmawSkyStalkerAI::Create);
    mgr->register_creature_script(CN_DRAGONMAW_WIND_REAVER, &DragonmawWindReaverAI::Create);
    mgr->register_creature_script(CN_DRAGONMAW_WYRMCALLER, &DragonmawWyrmcallerAI::Create);
    mgr->register_creature_script(CN_ENSLAVED_SERVANT, &EnslavedServantAI::Create);
    mgr->register_creature_script(CN_HAND_OF_GOREFIEND, &HandOfGorefiendAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_ARCHON, &IllidariArchonAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_ASSASSIN, &IllidariAssassinAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_BATTLEMAGE, &IllidariBattlemageAI::Create);
    mgr->register_creature_script(CN_ILLIDARI_BLOOD_LORD, &IllidariBloodLordAI::Create);
    mgr->register_creature_script(CN_IMAGE_OF_DEMENTIA, &ImageOfDementiaAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_BLOOD_MAGE, &ShadowmoonBloodMageAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_CHAMPION, &ShadowmoonChampionAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_DEATHSHAPER, &ShadowmoonDeathshaperAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_FALLEN, &ShadowmoonFallenAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_HOUNDMASTER, &ShadowmoonHoundmasterAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_REAVER, &ShadowmoonReaverAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_SOLDIER, &ShadowmoonSoldierAI::Create);
    mgr->register_creature_script(CN_SHADOWMOON_WEAPON_MASTER, &ShadowmoonWeaponMasterAI::Create);
    mgr->register_creature_script(CN_SPELLBOUND_ATTENDANT, &SpellboundAttendantAI::Create);
    mgr->register_creature_script(CN_TEMPLE_CONCUBINE, &TempleConcubineAI::Create);
    mgr->register_creature_script(CN_STORM_FURY, &StormFuryAI::Create);
    mgr->register_creature_script(CN_AQUEOUS_SURGER, &AqueousSurgerAI::Create);
    mgr->register_creature_script(CN_AQUEOUS_SPAWN, &AqueousSpawnAI::Create);
    mgr->register_creature_script(CN_AQUEOUS_LORD, &AqueousLordAI::Create);
    mgr->register_creature_script(CN_PROMENADE_SENTINEL, &PromenadeSentinelAI::Create);
    mgr->register_creature_script(CN_ANGERED_SOUL_FRAGMENT, &AngeredSoulFragmentAI::Create);
    //mgr->register_creature_script(CN_ASHTONGUE_FERAL_SPIRIT, &AshtongueFeralSpiritAI::Create);
    mgr->register_creature_script(CN_ENSLAVED_SOUL, &EnslavedSoulAI::Create);
    mgr->register_creature_script(CN_HUNGERING_SOUL_FRAGMENT, &HungeringSoulFragmentAI::Create);
    mgr->register_creature_script(CN_SHADOWY_CONSTRUCT, &ShadowyConstructAI::Create);
    mgr->register_creature_script(CN_SUFFERING_SOUL_FRAGMENT, &SufferingSoulFragmentAI::Create);
    mgr->register_creature_script(CN_VANGEFUL_SPIRIT, &VangefulSpiritAI::Create);
    mgr->register_creature_script(CN_WRATHBONE_FLAYER, &WrathboneFlayerAI::Create);

    //Bosses
    //mgr->register_creature_script(CN_SUPREMUS, &SupremusAI::Create);
    mgr->register_creature_script(CN_NAJENTUS, &NajentusAI::Create);
    mgr->register_creature_script(CN_GURTOGG_BLOODBOIL, &GurtoggAI::Create);

    mgr->register_creature_script(CN_RELIQUARY_OF_SOULS, &ReliquaryOfSoulsAI::Create);
    mgr->register_creature_script(CN_ESSENCEOFSUFFERING, &EssenceOfSufferingAI::Create);
    mgr->register_creature_script(CN_ESSENCEOFDESIRE, &EssenceOfDesireAI::Create);
    mgr->register_creature_script(CN_ESSENCEOFANGER, &EssenceOfAngerAI::Create);

    mgr->register_creature_script(CN_MOTHER_SHAHRAZ, &ShahrazAI::Create);
    mgr->register_creature_script(CN_LADY_MALANDE, &MalandeAI::Create);
    mgr->register_creature_script(CN_GATHIOS_THE_SHATTERER, &GathiosAI::Create);
    mgr->register_creature_script(CN_HIGH_NETHERMANCER_ZEREVOR, &ZerevorAI::Create);
    mgr->register_creature_script(CN_VERAS_DARKSHADOW, &VerasAI::Create);
    mgr->register_creature_script(CN_TERON_GOREFIEND, &TeronGorefiendAI::Create);
    //mgr->register_creature_script(CN_SHADE_OF_AKAMA, &ShadeofakamaAI::Create); //test

    //Illidan Stormrage related
    Arcemu::Gossip::Script* AG = new AkamaGossip();
    mgr->register_creature_gossip(CN_AKAMA, AG);

    mgr->register_creature_script(CN_DOOR_EVENT_TRIGGER, &UnselectableTriggerAI::Create);
    mgr->register_creature_script(CN_FACE_TRIGGER, &UnselectableTriggerAI::Create);
    mgr->register_creature_script(CN_CAGE_TRAP_TRIGGER, &UnselectableTriggerAI::Create);
    mgr->register_creature_script(CN_CAGE_TRAP_DISTURB_TRIGGER, &CageTrapTriggerAI::Create);
    mgr->register_creature_script(CN_DEMON_FIRE, &GenericTriggerAI::Create);
    mgr->register_creature_script(CN_EYE_BEAM_TRIGGER, &EyeBeamTriggerAI::Create);
    mgr->register_creature_script(CN_FLAME_CRASH, &GenericTriggerAI::Create);
    mgr->register_creature_script(CN_BLAZE_EFFECT, &GenericTriggerAI::Create);
    mgr->register_creature_script(CN_FLAME_BURST, &GenericTriggerAI::Create);
    mgr->register_creature_script(CN_FLAME_OF_AZZINOTH, &FlameOfAzzinothAI::Create);
    mgr->register_creature_script(CN_SHADOW_DEMON, &ShadowDemonAI::Create);
    mgr->register_creature_script(CN_PARASITIC_SHADOWFIEND, &ParasiticShadowfiendAI::Create);
    mgr->register_creature_script(CN_AKAMA, &AkamaAI::Create);
    mgr->register_creature_script(CN_MAIEV, &MaievAI::Create);
    mgr->register_creature_script(CN_ILLIDAN_STORMRAGE, &IllidanStormrageAI::Create);
    mgr->register_creature_script(CN_UDALO, &DranaeiSpiritAI::Create);
    mgr->register_creature_script(CN_OLUM, &DranaeiSpiritAI::Create);
    mgr->register_gameobject_script(GO_CAGE_TRAP, &CageTrapGO::Create);
}
