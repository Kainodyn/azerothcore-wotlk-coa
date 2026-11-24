/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */
#ifndef ASCENSION_CLIENT_SPELL_ROWS_H
#define ASCENSION_CLIENT_SPELL_ROWS_H

#include "Define.h"
#include <array>
#include <functional>
#include <optional>
#include <string>
#include <vector>

class Player;

struct AscensionSpellRow
{
    static constexpr uint32 DWORDS = 170;
    static constexpr uint32 FIRST_STRING_FIELD = 136;
    static constexpr uint32 FIRST_FIELD_AFTER_STRINGS = 204;
    static constexpr uint32 FIELD_COUNT = 234;

    enum Text : uint8
    {
        NAME = 0,
        DESCRIPTION = 1,
        RANK = 2,
        TOOLTIP = 3,
    };

    std::array<uint32, DWORDS> Values{};
    std::array<std::string, 4> Strings;

    static constexpr std::optional<uint32> Slot(uint32 dbcField)
    {
        if (dbcField < FIRST_STRING_FIELD)
            return dbcField;
        if (dbcField >= FIRST_FIELD_AFTER_STRINGS && dbcField < FIELD_COUNT)
            return FIRST_STRING_FIELD + 4 + (dbcField - FIRST_FIELD_AFTER_STRINGS);
        return std::nullopt;
    }

    uint32 SpellId() const { return Values[0]; }

    uint32 Get(uint32 dbcField) const
    {
        std::optional<uint32> const slot = Slot(dbcField);
        return slot ? Values[*slot] : 0;
    }

    bool Set(uint32 dbcField, uint32 value)
    {
        std::optional<uint32> const slot = Slot(dbcField);
        if (!slot || !*slot)
            return false;
        Values[*slot] = value;
        return true;
    }
};

using AscensionSpellRowOverride = void (*)(Player* player, AscensionSpellRow& row);

void AscensionSetSpellRowOverride(AscensionSpellRowOverride hook);
std::optional<AscensionSpellRow> AscensionOriginalSpellRow(uint32 spellId);
bool AscensionSendPlayerSpellRow(Player* player, uint32 spellId,
    std::function<void(AscensionSpellRow&)> const& edit = {});

#endif
