/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (c) 2014-2017 AscEmu Team <http://www.ascemu.org/>
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2005-2007 Ascent Team
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

#include "StdAfx.h"
#include "Storage/DatabaseCleaner.h"
#include "Server/MainServerDefines.h"
#include "Units/Creatures/Corpse.h"
#include "Log.hpp"

initialiseSingleton(DatabaseCleaner);

void DatabaseCleaner::Run()
{
    LogNotice("DatabaseCleaner : Stage 1 of 3: Cleaning characters...");
    CleanCharacters();

    LogNotice("DatabaseCleaner : Stage 2 of 3: Cleaning world...");
    CleanWorld();

    LogNotice("DatabaseCleaner : Stage 3 of 3: Optimizing databases...");
    Optimize();
}

void DatabaseCleaner::CleanWorld()
{}

void DatabaseCleaner::Optimize()
{}

void DatabaseCleaner::CleanCharacters()
{
    std::set<uint32> chr_guids;
    std::set<uint32> chr_guilds;
    std::set<uint32> chr_charters;
    LogNotice("DatabaseCleaner : Loading guids...");

    QueryResult* result = CharacterDatabase.Query("SELECT guid, guildid, charterId FROM characters");
    if (result)
    {
        do
        {
            chr_guids.insert(result->Fetch()[0].GetUInt32());
            if (result->Fetch()[1].GetUInt32() != 0)
                chr_guilds.insert(result->Fetch()[1].GetUInt32());
            if (result->Fetch()[2].GetUInt32() != 0)
                chr_guilds.insert(result->Fetch()[2].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }
    LogNotice("DatabaseCleaner : Got %u guids.", chr_guids.size());
    LogNotice("DatabaseCleaner : Cleaning playeritems...");

    result = CharacterDatabase.Query("SELECT ownerguid, guid FROM playeritems");
    std::vector<uint64> tokill_items;
    if (result)
    {
        do
        {
            if (result->Fetch()[0].GetUInt32() != 0 && chr_guids.find(result->Fetch()[0].GetUInt32()) == chr_guids.end())
            {
                tokill_items.push_back(result->Fetch()[1].GetUInt64());
            }
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint64>::iterator itr = tokill_items.begin(); itr != tokill_items.end(); ++itr)
    {
        CharacterDatabase.WaitExecute("DELETE FROM playeritems WHERE guid = " I64FMTD, *itr);
    }

    LogNotice("DatabaseCleaner : Deleted %u item instances.", tokill_items.size());
    LogNotice("DatabaseCleaner : Cleaning questlog...");

    result = CharacterDatabase.Query("SELECT index, player_guid FROM questlog");
    std::vector<uint32> tokill_quests;
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[1].GetUInt32()) == chr_guids.end())
                tokill_quests.push_back(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_quests.begin(); itr != tokill_quests.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM questlog WHERE index = %u", *itr);

    LogNotice("DatabaseCleaner : Deleted %u questlog entries.", tokill_quests.size());
    LogNotice("DatabaseCleaner : Cleaning corpses...");

    std::vector<uint32> tokill_corpses;
    result = CharacterDatabase.Query("SELECT * FROM corpses");
    if (result)
    {
        do
        {
            Corpse* pCorpse = new Corpse(0, result->Fetch()[0].GetUInt32());
            pCorpse->LoadValues(result->Fetch()[8].GetString());
            pCorpse->SetLowGUID(0);
            if (pCorpse->GetDisplayId() == 0 || GET_LOWGUID_PART(pCorpse->GetOwner()) == 0 || chr_guids.find(GET_LOWGUID_PART(pCorpse->GetOwner())) == chr_guids.end())
            {
                tokill_corpses.push_back(pCorpse->GetLowGUID());
            }
            delete pCorpse;
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_corpses.begin(); itr != tokill_corpses.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM corpses WHERE guid = %u", *itr);

    LogNotice("DatabaseCleaner : Removed %u corpses.", tokill_corpses.size());
    LogNotice("DatabaseCleaner : Cleaning mailbox...");

    result = CharacterDatabase.Query("SELECT message_id, player_guid FROM mailbox");
    std::vector<uint32> tokill_mail;
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[1].GetUInt32()) == chr_guids.end())
                tokill_mail.push_back(result->Fetch()[0].GetUInt32());

        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_mail.begin(); itr != tokill_mail.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM mailbox WHERE message_id = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u mail messages.", tokill_mail.size());
    LogNotice("DatabaseCleaner : Cleaning guilds table...");

    result = CharacterDatabase.Query("SELECT guildId FROM guilds");
    std::vector<uint32> tokill_guilds;
    if (result)
    {
        do
        {
            if (chr_guilds.find(result->Fetch()[0].GetUInt32()) == chr_guilds.end())
            {
                tokill_guilds.push_back(result->Fetch()[0].GetUInt32());
            }
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_guilds.begin(); itr != tokill_guilds.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM guilds WHERE guildId = %u", *itr);

    LogNotice("DatabaseCleaner : Deleted %u guilds.", tokill_guilds.size());
    LogNotice("DatabaseCleaner : Cleaning guild_ranks table...");

    result = CharacterDatabase.Query("SELECT guildId FROM guild_ranks");
    std::set<uint32> tokill_guildranks;
    if (result)
    {
        do
        {
            if (chr_guilds.find(result->Fetch()[0].GetUInt32()) == chr_guilds.end())
            {
                tokill_guildranks.insert(result->Fetch()[0].GetUInt32());
            }
        }
        while (result->NextRow());
        delete result;
    }
    for (std::set<uint32>::iterator itr = tokill_guildranks.begin(); itr != tokill_guildranks.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM guild_ranks WHERE guildId = %u", *itr);

    LogNotice("DatabaseCleaner : Deleted %u guild rank rows.", tokill_guildranks.size());
    LogNotice("DatabaseCleaner : Cleaning social table...");

    result = CharacterDatabase.Query("SELECT * FROM social");
    std::vector<std::pair<uint32, uint32> > tokill_social;
    if (result)
    {
        do
        {
            uint32 g1 = result->Fetch()[0].GetUInt32();
            uint32 g2 = result->Fetch()[1].GetUInt32();
            if (chr_guids.find(g1) == chr_guids.end() || chr_guids.find(g2) == chr_guids.end())
            {
                std::pair<uint32, uint32> x;
                x.first = g1;
                x.second = g2;
                tokill_social.push_back(x);
            }
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<std::pair<uint32, uint32> >::iterator itr = tokill_social.begin(); itr != tokill_social.end(); ++itr)
    {
        CharacterDatabase.WaitExecute("DELETE FROM social WHERE guid = %u and socialguid = %u", itr->first, itr->second);
    }

    LogNotice("DatabaseCleaner : Deleted %u social entries.", tokill_social.size());
    LogNotice("DatabaseCleaner : Cleaning cooldown tables...");

    std::set<uint32> tokill_cool;
    std::vector<std::pair<uint32, uint32> > tokill_cool2;
    result = CharacterDatabase.Query("SELECT OwnerGuid, CooldownTimeStamp FROM playercooldownitems");
    if (result)
    {
        uint32 t = getMSTime();
        do
        {
            uint32 guid = result->Fetch()[0].GetUInt32();
            uint32 cool = result->Fetch()[1].GetUInt32();
            if (chr_guids.find(guid) == chr_guids.end())
                tokill_cool.insert(guid);
            else if (t >= cool)
                tokill_cool2.push_back(std::make_pair(guid, cool));
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<std::pair<uint32, uint32> >::iterator itr = tokill_cool2.begin(); itr != tokill_cool2.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playercooldownitems WHERE OwnerGuid = %u AND CooldownTimeStamp = %u", itr->first, itr->second);
    for (std::set<uint32>::iterator itr = tokill_cool.begin(); itr != tokill_cool.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playercooldownitems WHERE OwnerGuid = %u", *itr);

    LogNotice("DatabaseCleaner : Deleted %u playercooldownitems.", tokill_cool.size() + tokill_cool2.size());
    tokill_cool.clear();
    tokill_cool2.clear();

    result = CharacterDatabase.Query("SELECT OwnerGuid, TimeStamp FROM playercooldownsecurity");
    if (result)
    {
        uint32 t = getMSTime();
        do
        {
            uint32 guid = result->Fetch()[0].GetUInt32();
            uint32 cool = result->Fetch()[1].GetUInt32();
            if (chr_guids.find(guid) == chr_guids.end())
                tokill_cool.insert(guid);
            else if (t >= cool)
                tokill_cool2.push_back(std::make_pair(guid, cool));
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<std::pair<uint32, uint32> >::iterator itr = tokill_cool2.begin(); itr != tokill_cool2.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playercooldownsecurity WHERE OwnerGuid = %u AND TimeStamp = %u", itr->first, itr->second);
    for (std::set<uint32>::iterator itr = tokill_cool.begin(); itr != tokill_cool.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playercooldownsecurity WHERE OwnerGuid = %u", *itr);

    LogNotice("DatabaseCleaner : Deleted %u playercooldownsecurities.", tokill_cool.size() + tokill_cool2.size());
    LogNotice("DatabaseCleaner : Cleaning tutorials...");
    std::vector<uint32> tokill_tutorials;

    result = CharacterDatabase.Query("SELECT playerId FROM tutorials");
    if (result)
    {
        do
        {
            uint32 pi = result->Fetch()[0].GetUInt32();
            if (chr_guids.find(pi) == chr_guids.end())
                tokill_tutorials.push_back(pi);
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_tutorials.begin(); itr != tokill_tutorials.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM tutorials WHERE playerId = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u tutorials.", tokill_tutorials.size());
    LogNotice("DatabaseCleaner : Cleaning playerpets...");
    std::set<uint32> tokill_pet;

    result = CharacterDatabase.Query("SELECT ownerguid, petnumber FROM playerpets");
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[0].GetUInt32()) == chr_guids.end())
                tokill_pet.insert(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }
    for (std::set<uint32>::iterator itr = tokill_pet.begin(); itr != tokill_pet.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playerpets WHERE ownerguid = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u pets.", tokill_pet.size());
    LogNotice("DatabaseCleaner : Cleaning playersummonspells...");
    std::set<uint32> tokill_ss;

    result = CharacterDatabase.Query("SELECT ownerguid FROM playersummonspells");
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[0].GetUInt32()) == chr_guids.end())
                tokill_ss.insert(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }
    for (std::set<uint32>::iterator itr = tokill_ss.begin(); itr != tokill_ss.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playersummonspells WHERE ownerguid = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u summonspells.", tokill_ss.size());
    LogNotice("DatabaseCleaner : Cleaning playerpetspells...");
    std::set<uint32> tokill_ps;

    result = CharacterDatabase.Query("SELECT ownerguid FROM playerpetspells");
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[0].GetUInt32()) == chr_guids.end())
                tokill_ps.insert(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }
    for (std::set<uint32>::iterator itr = tokill_ps.begin(); itr != tokill_ps.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM playerpetspells WHERE ownerguid = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u petspells.", tokill_ps.size());
    LogNotice("DatabaseCleaner : Cleaning gm_tickets...");
    std::set<uint32> tokill_gm;

    result = CharacterDatabase.Query("SELECT guid FROM gm_tickets");
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[0].GetUInt32()) == chr_guids.end())
                tokill_gm.insert(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }
    for (std::set<uint32>::iterator itr = tokill_gm.begin(); itr != tokill_gm.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM gm_tickets WHERE guid = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u gm tickets.", tokill_gm.size());
    LogNotice("DatabaseCleaner : Cleaning charters...");
    std::vector<uint32> tokill_charters;

    result = CharacterDatabase.Query("SELECT * FROM charters");
    if (result)
    {
        do
        {
            if (chr_charters.find(result->Fetch()[0].GetUInt32()) == chr_charters.end() ||
                chr_guids.find(result->Fetch()[1].GetUInt32()) == chr_guids.end())
            {
                tokill_charters.push_back(result->Fetch()[0].GetUInt32());
            }
        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_charters.begin(); itr != tokill_charters.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM charters WHERE charterId = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u charters.", tokill_charters.size());
    LogNotice("DatabaseCleaner : Cleaning charters...");

    result = CharacterDatabase.Query("SELECT auctionId, owner FROM auctions");
    std::vector<uint32> tokill_auct;
    if (result)
    {
        do
        {
            if (chr_guids.find(result->Fetch()[1].GetUInt32()) == chr_guids.end())
                tokill_auct.push_back(result->Fetch()[0].GetUInt32());

        }
        while (result->NextRow());
        delete result;
    }

    for (std::vector<uint32>::iterator itr = tokill_auct.begin(); itr != tokill_auct.end(); ++itr)
        CharacterDatabase.WaitExecute("DELETE FROM auctions WHERE auctionId = %u", *itr);
    LogNotice("DatabaseCleaner : Deleted %u auctions.", tokill_auct.size());
    LogNotice("DatabaseCleaner : Ending...");
}
