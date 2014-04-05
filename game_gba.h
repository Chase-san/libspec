/**
 * The GBA games, Generation 3 games. These games include Ruby, Sapphire, Emerald, Fire Red and Leaf Green.
 *
 * @file game_gba.h
 * @brief Contains the structures and functions for editing GBA pokemon save games.
 */

#ifndef __GBA_H__
#define __GBA_H__

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Enum containing the different gba game types.
 */
typedef enum {
	/** An unknown GBA game, no functions will work on this save type. */
	GBA_TYPE_UNKNOWN,
	/** Pokemon Ruby and Pokemon Sapphire */
	GBA_TYPE_RS,
	/** Pokemon Emerald */
	GBA_TYPE_E,
	/** Pokemon Fire Red and Pokemon Leaf Green */
	GBA_TYPE_FRLG
} gba_savetype_t;

enum {
	/** The size in bytes of the GBA save we expect. */
	GBA_SAVE_SIZE = 0x20000,
	/** The unpacked size of a GBA save slot. */
	GBA_UNPACKED_SIZE = 0xD900
};

/**
 * @brief A structure used for handling gba save types.
 */
typedef struct {
	/** @brief The unpacked data for this save. Will always be GBA_UNPACKED_SIZE in length. */
	uint8_t *data;
	/** @brief The savetype of the save. */
	gba_savetype_t type;
	/** @brief Internal data used by the library. */
	void *internal;
} gba_save_t;

/* Generation 3 Pokemon Data Structure */

/**
 * @brief Defines constants related to the GBA pokemon structure.
 */
enum {
	/** @brief The size of an individual block in the pokemon structure. */
	PK3_BLOCK_SIZE = 0xC,
	/** @brief The size of the pk3_box_t structure, the box storage structure. */
	PK3_BOX_SIZE = 0x50,
	/** @brief The size of the pk3_t structure, the party storage structure. */
	PK3_PARTY_SIZE = 0x64,
	/** @brief The length of a pokemons nickname. */
	PK3_NICKNAME_LENGTH = 10,
	/** @brief The length of a pokemon's original trainers name. */
	PK3_OT_NAME_LENGTH = 7
};

/**
 * @brief Defines constants related to GBA pokemon storage.
 */
enum {
	/** The number of boxes in the PC. */
	GBA_BOX_COUNT = 14,
	/** The number of pokemon in a box. */
	GBA_POKEMON_IN_BOX = 30,
	/** The length of a pc box's name. */
	GBA_BOX_NAME_LENGTH = 9,
};

enum {
	GBA_RS_ITEM_COUNT = 216,
	GBA_E_ITEM_COUNT = 236,
	GBA_FRLG_ITEM_COUNT = 216
};


// packed structs
#pragma pack(push, 1)
/**
 * @brief The pokemon's markings that you see in the party or box. Used for searching.
 */
typedef struct { //0x8
	bool circle : 1;
	bool square : 1;
	bool triangle : 1;
	bool heart : 1;
	uint8_t : 4; //unused
} pk3_marking_t;

/**
 * @brief The pokemon's effort values.
 */
typedef struct {
	uint8_t hp;
	uint8_t atk;
	uint8_t def;
	uint8_t spd;
	uint8_t satk;
	uint8_t sdef;
} pk3_effort_t;

/**
 * @brief The pokemon's contest stats.
 */
typedef struct {
	uint8_t cool;
	uint8_t beauty;
	uint8_t cute;
	uint8_t smart;
	uint8_t tough;
	uint8_t sheen;
} pk3_contest_t;

/**
 * @brief The pokemon's pokerus infection/strain.
 */
typedef struct {
	uint8_t days : 4;
	uint8_t strain : 4;
} pk3_pokerus_t;

/**
 * @brief The ppup of each move of the pokemon.
 */
typedef struct {
	uint8_t move_0 : 2;
	uint8_t move_1 : 2;//4
	uint8_t move_2 : 2;//6
	uint8_t move_3 : 2;//8
} pk3_pp_up_t;

/**
 * @brief The pokemon's individual values. It's unchangable genes.
 */
typedef struct {
	uint8_t hp : 5;
	uint8_t atk : 5;
	uint8_t def : 5;
	uint8_t spd : 5;
	uint8_t satk : 5;
	uint8_t sdef : 5;
	uint8_t : 2;
} pk3_genes_t;

//pulled from pkm, may not be accurate
/**
 * @brief The pokemon's ribbon data.
 */
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
	//byte 3
	bool tough_normal : 1;
	bool tough_super : 1;
	bool tough_hyper : 1;
	bool tough_master : 1;
	bool champion : 1;
	bool winning : 1;
	bool victory : 1;
	bool artist : 1;
	//byte 4
	bool effort : 1;
	bool marine : 1;
	bool land : 1;
	bool sky : 1;
	bool country : 1;
	bool national : 1;
	bool earth : 1;
	bool world : 1;
} pk3_ribbon_t;

/**
 * @brief A GBA pokemon's box data. 80 bytes in size.
 */
typedef struct { //80 bytes for box data
	/** @brief Header */
	struct { //32 bytes
		/** Personality Value */
		uint32_t pid; //4
		union {
			/** Original Trainer Full ID */
			uint32_t ot_fid; //full id
			struct {
				/** Original Trainer ID */
				uint16_t ot_id; //6
				/** Original Trainer Secret ID */
				uint16_t ot_sid; //8
			};
		};
		/** Pokemon's Nickname */
		char8_t nickname[PK3_NICKNAME_LENGTH]; //18
		/** Original Language */
		uint16_t language; //20
		/** Original Trainer's Name */
		char8_t ot_name[PK3_OT_NAME_LENGTH]; //27
		/** Pokemon's Markings */
		pk3_marking_t markings; //28
		/** Checksum of all 4 blocks */
		uint16_t checksum; //30
		uint16_t : 16; //32
	};

	/* data */
	union { //48 bytes
		/** Raw Block Access */
		uint8_t block[4][PK3_BLOCK_SIZE];
		struct {
			/** @brief Block A */
			struct {
				/** Pokemon Species */
				uint16_t species;
				/** Held Item ID */
				uint16_t held_item;
				/** Experience Points */
				uint32_t exp;
				/** Pokemon Move PPUPs */
				pk3_pp_up_t pp_up;
				/** Friendship Value / Steps to Hatch */
				uint8_t friendship;
				uint16_t : 16;
			};
			/** @brief Block B */
			struct {
				/** Move ID (4) */
				uint16_t move[4];
				/** Move PP Remaining (4) */
				uint8_t move_pp[4];
			};
			/** @brief Block C */
			struct {
				/** Effort Values */
				pk3_effort_t ev;
				/** Contest Stats */
				pk3_contest_t contest;
			};
			/** @brief Block D */
			struct {
				/** Poke'R US Virus */
				pk3_pokerus_t pokerus;
				/** Location Met */
				uint8_t met_loc;
				struct {
					/** Level Met At */
					uint8_t level_met : 7;
					/** Original Game ID */
					uint8_t game : 4;
					/** Pokeball Caught In */
					uint8_t pokeball : 4;
					/** Original Trainer's Gender */
					bool is_ot_female : 1;
				};
				union {
					/** Pokemon's Individual Values */
					pk3_genes_t iv;
					struct {
						uint32_t : 30;
						/** Is this pokemon an Egg? */
						bool is_egg : 1;
						/** Which of the two possible abilities does this pokemon have? */
						bool ability : 1;
					};
				};
				/** Pokemon's Ribbons */
				pk3_ribbon_t ribbon;
			};
		};
	};
} pk3_box_t;

typedef struct {
	uint16_t hp;
	uint16_t max_hp;
	uint16_t atk;
	uint16_t def;
	uint16_t spd;
	uint16_t satk;
	uint16_t sdef;
} pk3_stats_t;

typedef struct {
	/** @brief Turns of sleep status remaining */
	uint8_t status_sleep : 3;
	bool status_poison : 1;
	bool status_burn : 1;
	bool status_freeze : 1;
	bool status_paralysis : 1;
	bool status_toxic : 1;
} pk3_status_t;

typedef struct {
	pk3_status_t status;
	uint32_t : 24; //padding?
	uint8_t level;
	uint8_t pokerus_time;
	pk3_stats_t stats;
} pk3_party_t;

/**
 * @brief A GBA pokemon's box and party data. 100 bytes in size.
 */
typedef struct {
	pk3_box_t box;
	pk3_party_t party;
} pk3_t;

/**
 * @brief GBA Party Structure.
 */
typedef struct {
	/** @brief The number of pokemon currently in the party. */
	uint32_t size;
	/** @brief The individual pokemon in the party. */
	pk3_t pokemon[POKEMON_IN_PARTY];
} gba_party_t;

/**
 * @brief GBA PC Box Structure.
 */
typedef struct {
	/** @brief The individual pokemon in the box. Indexed visually from Left to Right and Top to Bottom. */
	pk3_box_t pokemon[GBA_POKEMON_IN_BOX];
} gba_pc_box_t;

/**
 * @brief GBA PC Pokemon Storage Structure.
 */
typedef struct {
	/**
	 * This defines what box pokemon will go into when captured with a full party as well as the box you start on when accessing the PC.
	 * @brief The index of the currently active box, starting at 0.
	 */
	uint32_t current_box;
	/** @brief The individual boxes in the PC. */
	gba_pc_box_t box[GBA_BOX_COUNT];
	/** @brief The names of each box in the PC. */
	char8_t name[GBA_BOX_COUNT][GBA_BOX_NAME_LENGTH];
	/** @brief The wallpaper index for each box of the PC. */
	uint8_t wallpaper[GBA_BOX_COUNT];
} gba_pc_t;

/**
 * @brief GBA Item Slot Structure.
 */
typedef struct {
	/** The item index in this slot. */
	uint16_t index;
	/** The total number of that item. */
	uint16_t amount;
} gba_item_slot_t;

/**
 * @brief Ruby/Sapphire Item Storage
 */
typedef struct { //216 items
	gba_item_slot_t all[GBA_RS_ITEM_COUNT];
	struct {
		gba_item_slot_t pc[50];
		gba_item_slot_t item[20];
		gba_item_slot_t keyitem[20];
		gba_item_slot_t ball[16];
		gba_item_slot_t tmhm[64];
		gba_item_slot_t berry[46];
	};
} gba_rs_storage_t;

/**
 * @brief Emerald Item Storage
 */
typedef union { //236 items E
	gba_item_slot_t all[GBA_E_ITEM_COUNT];
	struct {
		gba_item_slot_t pc[50];
		gba_item_slot_t item[30];
		gba_item_slot_t keyitem[30];
		gba_item_slot_t ball[16];
		gba_item_slot_t tmhm[64];
		gba_item_slot_t berry[46];
	};
} gba_e_storage_t;

/**
 * @brief Fire Red/Leaf Green Item Storage
 */
typedef union { //216 items FRLG
	gba_item_slot_t all[GBA_FRLG_ITEM_COUNT];
	struct {
		gba_item_slot_t pc[30];
		gba_item_slot_t item[42];
		gba_item_slot_t keyitem[30];
		gba_item_slot_t ball[13];
		gba_item_slot_t tmhm[58];
		gba_item_slot_t berry[43];
	};
} gba_frlg_storage_t;

/**
 * Combined Item Storage
 */
typedef struct {
	uint32_t money;
	uint32_t : 32;
	union {
		gba_rs_storage_t rs_items;
		//It's okay if a data structure goes beyond the end of the data, as long as they don't try and access it!
		gba_e_storage_t e_items;
		gba_frlg_storage_t frlg_items;
	};
} gba_storage_t;

/**
 * @brief GBA Time Played Structure
 */
typedef struct {
	uint16_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t frames; //about 1/60 of a second
} gba_time_t;

/**
 * @brief GBA Trainer Data Structure
 */
typedef struct {
	char8_t name[7];
	uint8_t : 8; //padding
	uint8_t gender;
	uint8_t : 8; //padding
	union {
		uint32_t fid;
		struct {
			uint16_t id;
			uint16_t sid;
		};
	};
	gba_time_t time_played;
} gba_trainer_t;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

void gba_text_to_ucs2(char16_t *dst, char8_t *src, size_t size);
void ucs2_to_gba_text(char8_t *dst, char16_t *src, size_t size);

gba_save_t *gba_read_main_save(const uint8_t *);
gba_save_t *gba_read_backup_save(const uint8_t *);
void gba_write_main_save(uint8_t *, const gba_save_t *);
void gba_write_backup_save(uint8_t *, const gba_save_t *);
void gba_save_game(uint8_t *, gba_save_t *);

void gba_free_save(gba_save_t *);
uint8_t *gba_create_data();

void pk3_decrypt(pk3_box_t *);
void pk3_encrypt(pk3_box_t *);

gba_storage_t *gba_get_storage(gba_save_t *);
gba_trainer_t *gba_get_trainer(gba_save_t *);
gba_party_t *gba_get_party(gba_save_t *);
gba_pc_t *gba_get_pc(gba_save_t *);

bool gba_pokedex_get_national(gba_save_t *);
void gba_pokedex_set_national(gba_save_t *, bool);
bool gba_pokedex_get_owned(gba_save_t *, size_t);
void gba_pokedex_set_owned(gba_save_t *, size_t, bool);
bool gba_pokedex_get_seen(gba_save_t *, size_t);
void gba_pokedex_set_seen(gba_save_t *, size_t, bool);

//TODO rival name, badges, day care pokemon (then GBA is done :D)

#ifdef __cplusplus
}
#endif

#endif //__GBA_H__
