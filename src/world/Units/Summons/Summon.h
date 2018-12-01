/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team
 * Copyright (C) 2008-2011 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SUMMON_HPP_
#define SUMMON_HPP_

#include "SummonDefines.hpp"
#include "Units/Creatures/Creature.h"

// Base class for Summoned creatures
class Summon : public Creature
{
    // MIT START
private:
    Unit* owner;

public:
    Unit* getUnitOwner() { return owner; }
    Object* getPlayerOwner() override;

    // MIT END

        Summon(uint64 GUID);
        ~Summon();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //virtual void Load(CreatureProto *proto, Unit* owner, LocationVector &position, uint32 spellid, int32 summonslot);
        //  Loads and sets up the basic properties of the summoned creature, like displayid, health, powers (mana and co.)
        //
        //Parameter(s)
        //  CreatureProto *proto      -   Reference to the CreatureProto structure that belongs to this creature (basically a template)
        //  Unit* owner               -   Owner unit of this Creature, usually the unit that summoned it, or the unit that summoned the summoner
        //  LocationVector &position  -   Position of the summoned creature in the game world
        //  uint32 spellid            -   The spell that summoned this creature
        //  int32 summonslot          -   The summon slot of the creature, -1 means no slot
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void Load(CreatureProperties const* properties_, Unit* owner, LocationVector & position, uint32 spellid, int32 summonslot);
        void OnPushToWorld();
        void OnPreRemoveFromWorld();
        bool SummonedToSlot()
        {
            if (summonslot != -1)
                return true;
            return false;
        }
        bool isSummon() const override { return true; }

        
        void Die(Unit* pAttacker, uint32 damage, uint32 spellid);
        void onRemoveInRangeObject(Object* object);

    private:

        int32 summonslot;  // Summon slot of the creature in the owner's summonhandler, -1 means no slot
};

#endif      // SUMMON_HPP_
