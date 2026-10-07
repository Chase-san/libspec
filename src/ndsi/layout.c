// Where each Gen 5 game keeps its fields: Black and White, Black 2 and White 2.

#include "ndsi/ndsi_internal.h"

// The block lists match Project Pokémon's BW and B2W2 Save Structure pages and every Gen 5 save in
// hand; the labels are the community's. TODO: Verify the block lists against the carts.
constexpr spec_ndsi_layout_t BLACK_WHITE_LAYOUT = {
    .type = SPEC_GAME_TYPE_BLACK_WHITE,
    .copy_size = 0x24000,
    .block_count = 69,
    .blocks =
        {
            {0x00000, 0x03E0}, // Box Names
            {0x00400, 0x0FF0}, // Box 1
            {0x01400, 0x0FF0}, // Box 2
            {0x02400, 0x0FF0}, // Box 3
            {0x03400, 0x0FF0}, // Box 4
            {0x04400, 0x0FF0}, // Box 5
            {0x05400, 0x0FF0}, // Box 6
            {0x06400, 0x0FF0}, // Box 7
            {0x07400, 0x0FF0}, // Box 8
            {0x08400, 0x0FF0}, // Box 9
            {0x09400, 0x0FF0}, // Box 10
            {0x0A400, 0x0FF0}, // Box 11
            {0x0B400, 0x0FF0}, // Box 12
            {0x0C400, 0x0FF0}, // Box 13
            {0x0D400, 0x0FF0}, // Box 14
            {0x0E400, 0x0FF0}, // Box 15
            {0x0F400, 0x0FF0}, // Box 16
            {0x10400, 0x0FF0}, // Box 17
            {0x11400, 0x0FF0}, // Box 18
            {0x12400, 0x0FF0}, // Box 19
            {0x13400, 0x0FF0}, // Box 20
            {0x14400, 0x0FF0}, // Box 21
            {0x15400, 0x0FF0}, // Box 22
            {0x16400, 0x0FF0}, // Box 23
            {0x17400, 0x0FF0}, // Box 24
            {0x18400, 0x09C0}, // Bag
            {0x18E00, 0x0534}, // Party
            {0x19400, 0x0068}, // Trainer
            {0x19500, 0x009C}, // Trainer Position
            {0x19600, 0x1338}, // Unity Tower and Survey
            {0x1AA00, 0x07C4}, // Pal Pad Player Data
            {0x1B200, 0x0D54}, // Pal Pad Friend Data
            {0x1C000, 0x002C}, // C-Gear Skin
            {0x1C100, 0x0658}, // Gym Badge Data
            {0x1C800, 0x0A94}, // Mystery Gift
            {0x1D300, 0x01AC}, // Dream World Catalog
            {0x1D500, 0x03EC}, // Chatter
            {0x1D900, 0x005C}, // Adventure Info
            {0x1DA00, 0x01E0}, // Trainer Card Records
            {0x1DC00, 0x00A8}, // Unknown
            {0x1DD00, 0x0460}, // Mail
            {0x1E200, 0x1400}, // Overworld State
            {0x1F700, 0x02A4}, // Musical
            {0x1FA00, 0x02DC}, // White Forest and Black City
            {0x1FD00, 0x034C}, // IR
            {0x20100, 0x03EC}, // EventWork
            {0x20500, 0x00F8}, // GTS
            {0x20600, 0x02FC}, // Regulation Tournament
            {0x20900, 0x0094}, // Gimmick
            {0x20A00, 0x035C}, // Battle Box
            {0x20E00, 0x01CC}, // Daycare
            {0x21000, 0x0168}, // Strength Boulder Status
            {0x21200, 0x00EC}, // Misc: Badges, Money, Trainer Sayings
            {0x21300, 0x01B0}, // Entralink Level and Powers
            {0x21500, 0x001C}, // Unknown
            {0x21600, 0x04D4}, // Pokédex
            {0x21B00, 0x0034}, // Encount
            {0x21C00, 0x003C}, // Battle Subway Play Info
            {0x21D00, 0x01AC}, // Battle Subway Score Info
            {0x21F00, 0x0B90}, // Battle Subway Wi-Fi Info
            {0x22B00, 0x009C}, // Online Records
            {0x22C00, 0x0850}, // Entree Forest Pokémon
            {0x23500, 0x0028}, // Unknown
            {0x23600, 0x0284}, // Answered Questions
            {0x23900, 0x0010}, // Unity Tower
            {0x23A00, 0x005C}, // Battle Institute
            {0x23B00, 0x016C}, // Unknown
            {0x23D00, 0x0040}, // Unknown
            {0x23E00, 0x00FC}, // Unknown
        },
    .table_offset = 0x23F00,  // Project Pokémon BW Save Structure Checksum Block; TODO: verify
    .footer_offset = 0x23F8C, // Project Pokémon BW Save Structure Checksum Block; TODO: verify

    .trainer_offset = 0x19400, // Trainer block
    .misc_offset = 0x21200,    // Misc block
    .has_rival_name = false,
    .pokedex_offset = 0x21600, // Pokédex block
    .spinda_offset = 0x4D0,    // community; TODO: verify

    .party_offset = 0x18E00,     // Party block
    .daycare_offset = 0x20E00,   // Daycare block
    .box_info_offset = 0x00000,  // Box Names block
    .first_box_offset = 0x00400, // Box 1 block
    .bag_offset = 0x18400,       // Bag block
};

constexpr spec_ndsi_layout_t BLACK2_WHITE2_LAYOUT = {
    .type = SPEC_GAME_TYPE_BLACK2_WHITE2,
    .copy_size = 0x26000,
    .block_count = 73,
    .blocks =
        {
            {0x00000, 0x03E0}, // Box Names
            {0x00400, 0x0FF0}, // Box 1
            {0x01400, 0x0FF0}, // Box 2
            {0x02400, 0x0FF0}, // Box 3
            {0x03400, 0x0FF0}, // Box 4
            {0x04400, 0x0FF0}, // Box 5
            {0x05400, 0x0FF0}, // Box 6
            {0x06400, 0x0FF0}, // Box 7
            {0x07400, 0x0FF0}, // Box 8
            {0x08400, 0x0FF0}, // Box 9
            {0x09400, 0x0FF0}, // Box 10
            {0x0A400, 0x0FF0}, // Box 11
            {0x0B400, 0x0FF0}, // Box 12
            {0x0C400, 0x0FF0}, // Box 13
            {0x0D400, 0x0FF0}, // Box 14
            {0x0E400, 0x0FF0}, // Box 15
            {0x0F400, 0x0FF0}, // Box 16
            {0x10400, 0x0FF0}, // Box 17
            {0x11400, 0x0FF0}, // Box 18
            {0x12400, 0x0FF0}, // Box 19
            {0x13400, 0x0FF0}, // Box 20
            {0x14400, 0x0FF0}, // Box 21
            {0x15400, 0x0FF0}, // Box 22
            {0x16400, 0x0FF0}, // Box 23
            {0x17400, 0x0FF0}, // Box 24
            {0x18400, 0x09EC}, // Bag
            {0x18E00, 0x0534}, // Party
            {0x19400, 0x00B0}, // Trainer
            {0x19500, 0x00A8}, // Trainer Position
            {0x19600, 0x1338}, // Unity Tower and Survey
            {0x1AA00, 0x07C4}, // Pal Pad Player Data
            {0x1B200, 0x0D54}, // Pal Pad Friend Data
            {0x1C000, 0x0094}, // Options and C-Gear Skin
            {0x1C100, 0x0658}, // Trainer Card
            {0x1C800, 0x0A94}, // Mystery Gift
            {0x1D300, 0x01AC}, // Dream World Catalog
            {0x1D500, 0x03EC}, // Chatter
            {0x1D900, 0x005C}, // Adventure Info
            {0x1DA00, 0x01E0}, // Trainer Card Records
            {0x1DC00, 0x00A8}, // Unknown
            {0x1DD00, 0x0460}, // Mail
            {0x1E200, 0x1400}, // Overworld State
            {0x1F700, 0x02A4}, // Musical
            {0x1FA00, 0x00E0}, // White Forest and Black City, Fused Reshiram or Zekrom
            {0x1FB00, 0x034C}, // IR
            {0x1FF00, 0x04E0}, // EventWork
            {0x20400, 0x00F8}, // GTS
            {0x20500, 0x02FC}, // Regulation Tournament
            {0x20800, 0x0094}, // Gimmick
            {0x20900, 0x035C}, // Battle Box
            {0x20D00, 0x01D4}, // Daycare
            {0x20F00, 0x01E0}, // Strength Boulder Status
            {0x21100, 0x00F0}, // Misc: Badges, Money, Trainer Sayings
            {0x21200, 0x01B4}, // Entralink Level and Powers
            {0x21400, 0x04DC}, // Pokédex
            {0x21900, 0x0034}, // Encount
            {0x21A00, 0x003C}, // Battle Subway Play Info
            {0x21B00, 0x01AC}, // Battle Subway Score Info
            {0x21D00, 0x0B90}, // Battle Subway Wi-Fi Info
            {0x22900, 0x00AC}, // Online Records
            {0x22A00, 0x0850}, // Entree Forest Pokémon
            {0x23300, 0x0284}, // Answered Questions
            {0x23600, 0x0010}, // Unity Tower
            {0x23700, 0x00A8}, // Battle Institute and PWT
            {0x23800, 0x016C}, // Unknown
            {0x23A00, 0x0080}, // Unknown
            {0x23B00, 0x00FC}, // Hidden Hollow and Rival
            {0x23C00, 0x16A8}, // Join Avenue
            {0x25300, 0x0498}, // Medal
            {0x25800, 0x0060}, // Key System
            {0x25900, 0x00FC}, // Festa Missions
            {0x25A00, 0x03E4}, // Pokéstar Studios
            {0x25E00, 0x00F0}, // Unknown
        },
    .table_offset = 0x25F00,  // Project Pokémon B2W2 Save Structure Mirror; TODO: verify
    .footer_offset = 0x25F94, // community; TODO: verify

    .trainer_offset = 0x19400, // Trainer block
    .misc_offset = 0x21100,    // Misc block
    .has_rival_name = true,
    .rival_name_offset = 0x23BA4, // community; TODO: verify
    .pokedex_offset = 0x21400,    // Pokédex block
    .spinda_offset = 0x4D8,       // community; TODO: verify

    .party_offset = 0x18E00,     // Party block
    .daycare_offset = 0x20D00,   // Daycare block
    .box_info_offset = 0x00000,  // Box Names block
    .first_box_offset = 0x00400, // Box 1 block
    .bag_offset = 0x18400,       // Bag block
};

const spec_ndsi_layout_t *spec_ndsi_get_layout(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_BLACK_WHITE:
            return &BLACK_WHITE_LAYOUT;
        case SPEC_GAME_TYPE_BLACK2_WHITE2:
            return &BLACK2_WHITE2_LAYOUT;
        default:
            return nullptr;
    }
}
