#ifndef __PKM_H__
#define __PKM_H__

#include "types.h"
#include "prng.h"
#include "checksum.h"

//PKM
enum {
	PKM_LENGTH = 136,
	PKM_PARTY_LENGTH = 236,
	PKM_BLOCK_SIZE = 32
};

#pragma pack(push, 1)
typedef struct { //0x2
	//byte 1
	uint8_t sinnoh_champ : 1;
	uint8_t ability	 : 1;
	uint8_t great_ability : 1;
	uint8_t double_ability : 1;
	uint8_t multi_ability : 1;
	uint8_t pair_ability : 1;
	uint8_t world_ability : 1;
	uint8_t alert : 1;
	//byte 2
	uint8_t shock : 1;
	uint8_t downcast : 1;
	uint8_t careless : 1;
	uint8_t relax : 1;
	uint8_t snooze : 1;
	uint8_t smile : 1;
	uint8_t gorgeous : 1;
	uint8_t royal : 1;
} pkm_ribbon_sinnoh1_t;

typedef struct { //0x2
	//byte 1
	uint8_t gorgeous_royal : 1;
	uint8_t footprint : 1;
	uint8_t record : 1;
	uint8_t history : 1;
	uint8_t legend : 1;
	uint8_t red : 1;
	uint8_t green : 1;
	uint8_t blue : 1;
	//byte 2
	uint8_t festival : 1;
	uint8_t carnival : 1;
	uint8_t classic : 1;
	uint8_t premier : 1;
	uint8_t : 4;
} pkm_ribbon_sinnoh2_t;

typedef struct { //0x2
	//byte 1
	uint8_t cool_normal : 1;
	uint8_t cool_super : 1;
	uint8_t cool_hyper : 1;
	uint8_t cool_master : 1;
	uint8_t beauty_normal : 1;
	uint8_t beauty_super : 1;
	uint8_t beauty_hyper : 1;
	uint8_t beauty_master : 1;
	//byte 2
	uint8_t cute_normal : 1;
	uint8_t cute_super : 1;
	uint8_t cute_hyper : 1;
	uint8_t cute_master : 1;
	uint8_t smart_normal : 1;
	uint8_t smart_super : 1;
	uint8_t smart_hyper : 1;
	uint8_t smart_master : 1;
} pkm_ribbon_sinnoh3_t;

typedef struct { //0x2
	//byte 1
	uint8_t tough_normal : 1;
	uint8_t tough_super : 1;
	uint8_t tough_hyper : 1;
	uint8_t tough_master : 1;
	uint8_t : 4;
	//byte 2
	uint8_t : 8;
} pkm_ribbon_sinnoh4_t;

typedef struct { //0x2
	//byte 1
	uint8_t gorgeous_royal : 1;
	uint8_t footprint : 1;
	uint8_t record : 1;
	uint8_t event : 1;
	uint8_t legend : 1;
	uint8_t world_champion : 1;
	uint8_t birthday : 1;
	uint8_t special : 1;
	//byte 2
	uint8_t souvenir : 1;
	uint8_t wishing : 1;
	uint8_t classic : 1;
	uint8_t premier : 1;
	uint8_t : 4;
} pkm_ribbon_unova_t;

typedef struct { //0x2
	//byte 1
	uint8_t cool_normal : 1;
	uint8_t cool_super : 1;
	uint8_t cool_hyper : 1;
	uint8_t cool_master : 1;
	uint8_t beauty_normal : 1;
	uint8_t beauty_super : 1;
	uint8_t beauty_hyper : 1;
	uint8_t beauty_master : 1;
	//byte 2
	uint8_t cute_normal : 1;
	uint8_t cute_super : 1;
	uint8_t cute_hyper : 1;
	uint8_t cute_master : 1;
	uint8_t smart_normal : 1;
	uint8_t smart_super : 1;
	uint8_t smart_hyper : 1;
	uint8_t smart_master : 1;
} pkm_ribbon_hoenn1_t;

typedef struct { //0x2
	//byte 1
	uint8_t tough_normal : 1;
	uint8_t tough_super : 1;
	uint8_t tough_hyper : 1;
	uint8_t tough_master : 1;
	uint8_t champion : 1;
	uint8_t winning : 1;
	uint8_t victory : 1;
	uint8_t artist : 1;
	//byte 2
	uint8_t effort : 1;
	uint8_t marine : 1;
	uint8_t land : 1;
	uint8_t sky : 1;
	uint8_t country : 1;
	uint8_t national : 1;
	uint8_t earth : 1;
	uint8_t world : 1;
} pkm_ribbon_hoenn2_t;

typedef struct { //0x3
	uint8_t year; //since 2000
	uint8_t month;
	uint8_t day;
} pkm_date_t;

typedef struct { //0x1
	uint8_t days : 4;
	uint8_t strain : 4;
} pkm_pokerus_t;

typedef struct { //0x8
	uint32_t pid;
	union {
		struct {
			uint8_t version : 4;
			uint8_t sort : 4;
			uint8_t : 5;
			uint8_t party : 1;
			uint8_t box : 1;
			uint8_t egg : 1;
		};
		uint16_t padding;
	};
	uint16_t checksum;
} pkm_header_t;

typedef struct { //0x8
	uint8_t circle : 1;
	uint8_t triangle : 1;
	uint8_t square : 1;
	uint8_t star : 1;
	uint8_t diamond : 1;
	uint8_t : 2; //unused
} pkm_marking_t;

typedef struct { //0x32
	uint16_t species;
	uint16_t held_item;
	uint16_t ot_id;
	uint16_t ot_sid;
	uint32_t exp;
	uint8_t friendship;
	uint8_t ability;
	pkm_marking_t markings;
	uint8_t country;
	uint8_t ev_hp;
	uint8_t ev_atk;
	uint8_t ev_def;
	uint8_t ev_spd;
	uint8_t ev_satk;
	uint8_t ev_sdef;
	uint8_t contest_cool;
	uint8_t contest_beauty;
	uint8_t contest_cute;
	uint8_t contest_smart;
	uint8_t contest_tough;
	uint8_t contest_sheen;
	pkm_ribbon_sinnoh1_t ribbon_sinnoh1;
	union {
		pkm_ribbon_sinnoh2_t ribbon_sinnoh2;
		pkm_ribbon_unova_t ribbon_unova;
	};
} pkm_blocka_t;

typedef struct { //0x32
	uint16_t move[4]; //8
	uint8_t movePP[4]; //4
	uint8_t movePPUP[4]; //4
	uint8_t iv_hp : 5;
	uint8_t iv_atk : 5;
	uint8_t iv_def : 5;
	uint8_t iv_spd : 5;
	uint8_t iv_satk : 5;
	uint8_t iv_sdef : 5;
	uint8_t is_egg : 1;
	uint8_t is_nicknamed : 1;
	pkm_ribbon_hoenn1_t ribbon_hoenn1;
	pkm_ribbon_hoenn1_t ribbon_hoenn2;

	uint8_t is_fateful_encounter : 1;
	uint8_t is_female : 1;
	uint8_t is_genderless : 1;
	uint8_t forme : 5; //25
	union {
		//shining leaf stuff
		struct {
			uint8_t : 2;
			uint8_t has_crown : 1;
			uint8_t shining_leaf : 5;
		};
		//unova nature stuff
		uint8_t nature;
	};
	uint8_t is_abil_dreamworld : 1;
	uint8_t is_n_pokemon : 1;
	uint8_t : 6;
	uint8_t : 8;
	uint16_t egg_loc_plat;
	uint16_t met_loc_plat;
} pkm_blockb_t;

typedef struct { //0x32
	union {
		char16_t nickname16[11];
		char8_t nickname8[22];
	};//22
	uint16_t hometown; //24
	pkm_ribbon_sinnoh3_t ribbon_sinnoh3; //26
	pkm_ribbon_sinnoh4_t ribbon_sinnoh4; //28
	uint32_t unknown; //32
} pkm_blockc_t;

typedef struct { //0x32
	union {
		char16_t ot_name16[8];
		char8_t ot_name8[16];
	};
	pkm_date_t egg_met_date;
	pkm_date_t met_date;
	uint16_t egg_loc_dp;
	uint16_t met_loc_dp;
	pkm_pokerus_t pokerus;
	uint8_t pokeball;
	uint8_t leve_met : 7;
	uint8_t is_ot_female : 1;
	uint8_t encounter_type;
	uint8_t pokeball_hgss;
	uint8_t : 8;
} pkm_blockd_t;

typedef struct { //0x100
	uint8_t sleep_turns : 3;
	uint8_t is_poisoned : 1;
	uint8_t is_burned : 1;
	uint8_t is_frozen : 1;
	uint8_t is_paralyzed : 1;
	uint8_t is_toxic : 1;
	uint8_t : 8;
	uint16_t : 16;
	uint8_t level;
	uint8_t capsule;
	uint8_t hp;
	uint8_t maxhp;
	uint8_t atk;
	uint8_t def;
	uint8_t spd;
	uint8_t satk;
	uint8_t sdef;
	uint8_t unknown[56];
	uint8_t seal_loc[24];
} pkm_party_t;

typedef struct {
	pkm_header_t header;
	union {
		uint8_t block[4][PKM_BLOCK_SIZE];
		struct {
			pkm_blocka_t block_a;
			pkm_blockb_t block_b;
			pkm_blockc_t block_c;
			pkm_blockd_t block_d;
		};
	};
	pkm_party_t party;
} pkm_t;
#pragma pack(pop)

void pkm_shuffle(void *);
void pkm_unshuffle(void *);
void pkm_crypt(void *);
void pkm_crypt_party(void *);

#endif //__PKM_H__
