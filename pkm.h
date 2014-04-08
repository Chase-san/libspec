/**
 * @file pkm.h
 */

#ifndef __PKM_H__
#define __PKM_H__

#include "types.h"
#include "prng.h"
#include "checksum.h"

/* PKM covers both generation 4 and 5 (nds/dsi), so it has it's own file. */

//PKM
enum {
	PKM_LENGTH = 136,        //!< PKM_LENGTH
	PKM_PARTY_LENGTH = 236,  //!< PKM_PARTY_LENGTH
	PKM_BLOCK_SIZE = 32,     //!< PKM_BLOCK_SIZE
	PKM_NICKNAME_LENGTH = 11,//!< PKM_NICKNAME_LENGTH
	PKM_OT_NAME_LENGTH = 8   //!< PKM_OT_NAME_LENGTH
};

#pragma pack(push, 1)

typedef struct { //0x2
	//byte 1
	bool sinnoh_champ : 1;
	bool ability	 : 1;
	bool great_ability : 1;
	bool double_ability : 1;
	bool multi_ability : 1;
	bool pair_ability : 1;
	bool world_ability : 1;
	bool alert : 1;
	//byte 2
	bool shock : 1;
	bool downcast : 1;
	bool careless : 1;
	bool relax : 1;
	bool snooze : 1;
	bool smile : 1;
	bool gorgeous : 1;
	bool royal : 1;
} pkm_ribbon_sinnoh1_t;

typedef struct { //0x2
	//byte 1
	bool gorgeous_royal : 1;
	bool footprint : 1;
	bool record : 1;
	bool history : 1;
	bool legend : 1;
	bool red : 1;
	bool green : 1;
	bool blue : 1;
	//byte 2
	bool festival : 1;
	bool carnival : 1;
	bool classic : 1;
	bool premier : 1;
	uint8_t : 4;
} pkm_ribbon_sinnoh2_t;

typedef struct { //0x2
	//byte 1
	bool gorgeous_royal : 1;
	bool footprint : 1;
	bool record : 1;
	bool event : 1;
	bool legend : 1;
	bool world_champion : 1;
	bool birthday : 1;
	bool special : 1;
	//byte 2
	bool souvenir : 1;
	bool wishing : 1;
	bool classic : 1;
	bool premier : 1;
	uint8_t : 4;
} pkm_ribbon_unova_t;

typedef struct { //0x2
	//byte 1
	bool cool_normal : 1;
	bool cool_super : 1;
	bool cool_hyper : 1;
	bool cool_master : 1;
	bool beauty_normal : 1;
	bool beauty_super : 1;
	bool beauty_hyper : 1;
	bool beauty_master : 1;
	//byte 2
	bool cute_normal : 1;
	bool cute_super : 1;
	bool cute_hyper : 1;
	bool cute_master : 1;
	bool smart_normal : 1;
	bool smart_super : 1;
	bool smart_hyper : 1;
	bool smart_master : 1;
} pkm_ribbon_hoenn1_t;

typedef struct { //0x2
	//byte 1
	bool tough_normal : 1;
	bool tough_super : 1;
	bool tough_hyper : 1;
	bool tough_master : 1;
	bool champion : 1;
	bool winning : 1;
	bool victory : 1;
	bool artist : 1;
	//byte 2
	bool effort : 1;
	bool marine : 1;
	bool land : 1;
	bool sky : 1;
	bool country : 1;
	bool national : 1;
	bool earth : 1;
	bool world : 1;
} pkm_ribbon_hoenn2_t;

typedef struct { //0x2
	//byte 1
	bool cool_normal : 1;
	bool cool_super : 1;
	bool cool_hyper : 1;
	bool cool_master : 1;
	bool beauty_normal : 1;
	bool beauty_super : 1;
	bool beauty_hyper : 1;
	bool beauty_master : 1;
	//byte 2
	bool cute_normal : 1;
	bool cute_super : 1;
	bool cute_hyper : 1;
	bool cute_master : 1;
	bool smart_normal : 1;
	bool smart_super : 1;
	bool smart_hyper : 1;
	bool smart_master : 1;
} pkm_ribbon_sinnoh3_t;

typedef struct { //0x2
	//byte 1
	bool tough_normal : 1;
	bool tough_super : 1;
	bool tough_hyper : 1;
	bool tough_master : 1;
	bool : 4;
	//byte 2
	uint8_t : 8;
} pkm_ribbon_sinnoh4_t;

typedef struct { //0x8
	uint32_t pid;
	union {
		struct {
			uint8_t version : 4;
			uint8_t sort : 4;
			uint8_t : 5;
			bool is_party_encrypted : 1;
			bool is_encrypted : 1;
			bool is_egg : 1;
		};
		uint16_t padding;
	};
	uint16_t checksum;
} pkm_header_t;

typedef struct { //0x8
	bool circle : 1;
	bool triangle : 1;
	bool square : 1;
	bool star : 1;
	bool diamond : 1;
	uint8_t : 3; //unused
} pkm_marking_t;

typedef struct {
	uint8_t hp;
	uint8_t atk;
	uint8_t def;
	uint8_t spd;
	uint8_t satk;
	uint8_t sdef;
} pkm_effort_t;

typedef struct {
	uint8_t cool;
	uint8_t beauty;
	uint8_t cute;
	uint8_t smart;
	uint8_t tough;
	uint8_t sheen;
} pkm_contest_t;

//typedef  pkm_blocka_t;

typedef struct {
	uint8_t hp : 5;
	uint8_t atk : 5;
	uint8_t def : 5;
	uint8_t spd : 5;
	uint8_t satk : 5;
	uint8_t sdef : 5;
	uint8_t : 2;
} pkm_genes_t;


typedef struct { //0x3
	uint8_t year; //since 2000
	uint8_t month;
	uint8_t day;
} pkm_date_t;

typedef struct { //0x1
	uint8_t days : 4;
	uint8_t strain : 4;
} pkm_pokerus_t;

typedef struct {
	pkm_header_t header;
	union {
		uint8_t block[4][PKM_BLOCK_SIZE];
		struct {
			/** @brief Block A */
			struct { //0x32
				uint16_t species;
				uint16_t held_item;
				uint16_t ot_id;
				uint16_t ot_sid;
				uint32_t exp;
				uint8_t friendship;
				uint8_t ability;
				pkm_marking_t markings;
				uint8_t country;
				pkm_effort_t ev;
				pkm_contest_t contest;
				pkm_ribbon_sinnoh1_t ribbon_sinnoh1;
				union {
					pkm_ribbon_sinnoh2_t ribbon_sinnoh2;
					pkm_ribbon_unova_t ribbon_unova;
				};
			};

			/** @brief Block B */
			struct { //0x32
				uint16_t move[4]; //8
				uint8_t move_pp[4]; //4
				uint8_t move_pp_up[4]; //4
				union {
					pkm_genes_t iv;
					struct {
						uint32_t : 30;
						bool is_egg : 1;
						bool is_nicknamed : 1;
					};
				};
				pkm_ribbon_hoenn1_t ribbon_hoenn1;
				pkm_ribbon_hoenn2_t ribbon_hoenn2;

				bool is_fateful_encounter : 1;
				bool is_female : 1;
				bool is_genderless : 1;
				uint8_t forme : 5; //25
				union {
					//shining leaf stuff
					struct {
						uint8_t : 2;
						bool has_crown : 1;
						uint8_t shining_leaf : 5;
					};
					//unova nature stuff
					uint8_t nature;
				};
				struct {
					bool is_abil_dreamworld : 1;
					bool is_n_pokemon : 1;
					uint8_t : 6;
				};
				uint8_t : 8;
				uint16_t egg_loc_plat;
				uint16_t met_loc_plat;
			};

			/** @brief Block C */
			struct { //0x32
				char16_t nickname[PKM_NICKNAME_LENGTH];
				uint16_t hometown; //24
				pkm_ribbon_sinnoh3_t ribbon_sinnoh3; //26
				pkm_ribbon_sinnoh4_t ribbon_sinnoh4; //28
				uint32_t unknown; //32
			};

			/** @brief Block D */
			struct { //0x32
				char16_t ot_name[PKM_OT_NAME_LENGTH];
				pkm_date_t egg_met_date;
				pkm_date_t met_date;
				uint16_t egg_loc_dp;
				uint16_t met_loc_dp;
				pkm_pokerus_t pokerus;
				uint8_t pokeball;
				struct {
					uint8_t level_met : 7;
					bool is_ot_female : 1;
				};
				uint8_t encounter_type;
				uint8_t pokeball_hgss;
				uint8_t : 8;
			};
		};
	};
} pkm_box_t;

typedef struct { //0x100
	struct { //0x1
		uint8_t sleep_turns : 3;
		bool is_poisoned : 1;
		bool is_burned : 1;
		bool is_frozen : 1;
		bool is_paralyzed : 1;
		bool is_toxic : 1;
	};//status
	uint32_t : 24;
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
} pkm_nds_party_t;

typedef struct {
	pkm_box_t box;
	pkm_nds_party_t party;
} pkm_nds_t;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

void pkm_decrypt(pkm_box_t *);
void pkm_encrypt(pkm_box_t *);
void pkm_crypt_nds_party(pkm_nds_t *);

#ifdef __cplusplus
}
#endif

#endif //__PKM_H__
