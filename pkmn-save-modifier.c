// Copyright 2023 Ben Trask. MIT licensed.
/*
// Build and run:
clang -std=c99 -D_POSIX_C_SOURCE=200809L -D_PKMN_SAVE_MODIFIER_STANDALONE -Wall -Wextra -Werror -static pkmn-save-modifier.c -L./lib -lspec -lm -o ./lib/pkmn-save-modifier && ./lib/pkmn-save-modifier

// Run with Cling REPL (interactive):
cling -l ./lib/libspec.so -l ./pkmn-save-modifier.c

// Example interactive usage via Cling:
$ cling -l ./lib/libspec.so -l ./pkmn-save-modifier.c
[cling]$ save_open("/home/user/.config/retroarch/saves/Pokemon - FireRed Version (USA).srm")
(int) 0
[cling]$ pokemon_list x = list_init(_save)
(pokemon_list &) @0x7ffaff61e020
[cling]$ list_filter_by_species(&x, KADABRA)
(unsigned long) 1
[cling]$ pokemon_moveset(x.pokemon[0], CONFUSION, TELEPORT, FLASH, PSYBEAM)
[cling]$ save_store();
[cling]$ .q
*/

#define MMAP_OPEN_IMPL
#include "mmap_open.h"
#include "include/libspec.h"
#include "include/pokemon_data.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define numberof(x) (sizeof(x) / sizeof(*x))

// TODO: For testing only...
uint16_t const GBA_TO_CODEPAGE[] = {
	0x0020, 0x3042, 0x3044, 0x3046, 0x3048, 0x304a, 0x304b, 0x304d,
	0x304f, 0x3051, 0x3053, 0x3055, 0x3057, 0x3059, 0x305b, 0x305d,
	0x305f, 0x3061, 0x3064, 0x3066, 0x3068, 0x306a, 0x306b, 0x306c,
	0x306d, 0x306e, 0x306f, 0x3072, 0x3075, 0x3078, 0x307b, 0x307e,
	0x307f, 0x3080, 0x3081, 0x3082, 0x3084, 0x3086, 0x3088, 0x3089,
	0x308a, 0x308b, 0x308c, 0x308d, 0x308f, 0x3092, 0x3093, 0x3041,
	0x3043, 0x3045, 0x3047, 0x3049, 0x3083, 0x3085, 0x3087, 0x304c,
	0x304e, 0x3050, 0x3052, 0x3054, 0x3056, 0x3058, 0x305a, 0x305c,
	0x305e, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x3070, 0x3073,
	0x3076, 0x3079, 0x307c, 0x3071, 0x3074, 0x3077, 0x307a, 0x307d,
	0xffff, 0x30a2, 0x30a4, 0x30a6, 0x30a8, 0x30aa, 0x30ab, 0x30ad,
	0x30af, 0x30b1, 0x30b3, 0x30b5, 0x30b7, 0x30b9, 0x30bb, 0x30bd,
	0x30bf, 0x30c1, 0x30c4, 0x30c6, 0x30c8, 0x30ca, 0x30cb, 0x30cc,
	0x30cd, 0x30ce, 0x30cf, 0x30d2, 0x30d5, 0x30d8, 0x30db, 0x30de,
	0x30df, 0x30e0, 0x30e1, 0x30e2, 0x30e4, 0x30e6, 0x30e8, 0x30e9,
	0x30ea, 0x30eb, 0x30ec, 0x30ed, 0x30ef, 0x30f2, 0x30f3, 0x30a1,
	0x30a3, 0x30a5, 0x30a7, 0x30a9, 0x30e3, 0x30e5, 0x30e7, 0x30ac,
	0x30ae, 0x30b0, 0x30b2, 0x30b4, 0x30b6, 0x30b8, 0x30ba, 0x30bc,
	0x30be, 0x30c0, 0x30c2, 0x30c5, 0x30c7, 0x30c9, 0x30d0, 0x30d3,
	0x30d6, 0x30d9, 0x30dc, 0x30d1, 0x30d4, 0x30d7, 0x30da, 0x30dd,
	0xffff, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036,
	0x0037, 0x0038, 0x0039, 0x0021, 0x003f, 0x002e, 0x002d, 0xffff,
	0x2026, 0x201c, 0x201d, 0x2018, 0x2019, 0x2642, 0x2640, 0xffff,
	0x002c, 0xffff, 0x002f, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d,
	0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055,
	0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b,
	0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073,
	0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0xffff,
	0xffff, 0x00c4, 0x00d6, 0x00dc, 0x00e4, 0x00f6, 0x00fc, 0xffff,
	0xffff, 0xffff, 0xffff, 0x2192, 0xffff, 0xffff, 0x000a, 0x0000
};

// Manually converting unicode codepoints into UTF-8 and UTF-16 - https://stackoverflow.com/questions/6240055/manually-converting-unicode-codepoints-into-utf-8-and-utf-16
void utf8_from_ucs2(char *dst, char16_t const *const src, size_t const len) {
	for(size_t i = 0; i < len; i++) {
		uint16_t const c = src[i];
		if(c <= 0x7f) { *dst++ = c & 0xff; }
		else if(c <= 0x7ff) { *dst++ = 0xc0 | (c >> 6 & 0x1f); *dst++ = 0x80 | (c >> 0 & 0x3f); }
		else if(c <= 0xfffd) { *dst++ = 0xe0 | (c >> 12 & 0x0f); *dst++ = 0x80 | (c >> 6 & 0x3f); *dst++ = 0x80 | (c >> 0 & 0x3f); }
		else *dst++ = '?';
	}
	dst[len-1] = '\0'; // Guarantee nul-termination. // TODO?
}
void ucs2_from_utf8(char16_t *const dst, char const *src, size_t const len) {
	char const *const end = src+(len*3); // TODO
	for(size_t i = 0; i < len; i++) {
		assert(src < end);
		uint8_t const c1 = src[0];
		uint8_t const c2 = src+1 < end ? src[1] : 0;
		uint8_t const c3 = src+2 < end ? src[2] : 0;
//		fprintf(stderr, "interpreting %x %x %x\n", c1, c2, c3);
		// TODO: Broken...
		if(c1 <= 0x7f) { dst[i] = c1; src += 1; }
		else if(c1 <= 0xdf) { dst[i] = (c1 & 0x1f) << 6 | (c2 & 0x3f) << 0; src += 2; }
		else if(c1 <= 0xef) { dst[i] = (c1 & 0x0f) << 12 | (c2 & 0x3f) << 6 | (c3 & 0x3f) << 0; src += 3; }
		else { dst[i] = '?'; src += 1; } // TODO?
	}
}

bool pokemon_is_female(pk3_box_t *p) {
	assert(p);
	return true; // TODO: This needs to be calculated from p->pid.
	// https://bulbapedia.bulbagarden.net/wiki/Personality_value
}
bool pokemon_slot_empty(pk3_box_t *const p) {
	assert(p);
	return 0 == p->species; // TODO: Better way to check?
}
void pokemon_set_move(pk3_box_t *p, uint8_t index, uint16_t move) {
	assert(p);
	assert(index >= 1);
	assert(index <= numberof(p->move));
	assert(move < numberof(moves));
	// TODO: Take into account the pp_up field?
	p->move[index-1] = move; p->move_pp[index-1] = moves[move].pp;
}
void pokemon_moveset(pk3_box_t *p, uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
	assert(p);
	pokemon_set_move(p, 1, m1);
	pokemon_set_move(p, 2, m2);
	pokemon_set_move(p, 3, m3);
	pokemon_set_move(p, 4, m4);
}

bool genes_equal(pk3_genes_t a, pk3_genes_t b) {
	return (
		a.atk == b.atk &&
		a.def == b.def &&
		a.spd == b.spd &&
		a.satk == b.satk &&
		a.sdef == b.sdef &&
	1);
}
pk3_genes_t genes_from_uint32(uint32_t genes) {
	return (pk3_genes_t){
		(uint8_t)((genes >> (25+2)) & 0x1f),
		(uint8_t)((genes >> (20+2)) & 0x1f),
		(uint8_t)((genes >> (15+2)) & 0x1f),
		(uint8_t)((genes >> (10+2)) & 0x1f),
		(uint8_t)((genes >> ( 5+2)) & 0x1f),
		(uint8_t)((genes >> ( 0+2)) & 0x1f),
		// is_egg = 0
		// ability = 0
	};
}

void trainer_init(gba_trainer_t *t, char const *name, char gender, uint16_t id, uint16_t sid) {
	assert(t);
	char16_t tmp[sizeof(t->name)] = {};
	ucs2_from_utf8(tmp, name, sizeof(t->name));
	ucs2_to_gba_text(t->name, tmp, sizeof(t->name));
	t->is_female = 'F' == gender;
	t->id = id;
	t->sid = sid;
	t->time_played = (gba_time_t){0, 0, 0, 0};
}
void pokemon_init_v2(pk3_box_t *p, gba_trainer_t const *ot, uint32_t pid, uint32_t iv, uint16_t sid, char const *name, uint8_t level);
void pokemon_init(pk3_box_t *p, uint32_t genes, uint16_t id, char const *name, char gender, uint32_t nature, uint8_t level, gba_trainer_t const *ot) {
	pokemon_init_v2(p, ot, nature, genes, id, name, level);
}
void pokemon_init_v2(pk3_box_t *p, gba_trainer_t const *ot, uint32_t pid, uint32_t iv, uint16_t sid, char const *name, uint8_t level) {
	assert(p);
//	memset(p, 0, sizeof(*p));

	p->pid = pid;

	p->ot_id = ot->id;
	p->ot_sid = ot->sid;

	char16_t tmp[sizeof(p->nickname)] = {};
	ucs2_from_utf8(tmp, name ? name : species[sid].nameCaps, sizeof(p->nickname));
	ucs2_to_gba_text(p->nickname, tmp, sizeof(p->nickname));
	p->language = ENGLISH;

	p->is_bad_egg = 0;
	p->has_species = 1; // TODO: Is this used?
	p->use_egg_name = 0;
	assert(sizeof(p->ot_name) == sizeof(ot->name));
	memcpy(p->ot_name, ot->name, sizeof(p->ot_name));
	p->markings = (pk3_marking_t){0, 0, 0, 0};
	p->checksum = 0;

	p->species = sid;
	p->held_item = 0;
	p->exp = (level*level*level)+50; // TODO: Expose API for this. Handle growth rates.
	p->pp_up = (pk3_pp_up_t){0, 0, 0, 0};
	p->friendship = 1;

	pokemon_moveset(p, TACKLE, 0, 0, 0);

	p->ev = (pk3_effort_t){20, 20, 20, 20, 20, 20}; // TODO
	p->contest = (pk3_contest_t){0, 0, 0, 0, 0, 0};

	p->pokerus = (pk3_pokerus_t){0, 0};
	p->met_loc = PALLET_TOWN;
	p->level_met = 5;
	p->game = FIRERED_GAME;
	p->pokeball = POKE_BALL_CAUGHT;
	p->is_ot_female = !!ot->is_female;

	p->iv = genes_from_uint32(iv);
	p->is_egg = 0;
	p->ability = 0;
	p->ribbon = (pk3_ribbon_t){};

	pk3_encrypt(p); // This has the effect of computing the checksum. Might help?
	pk3_decrypt(p);
}
void pokemon_clear(pk3_box_t *p) {
	memset(p, 0, sizeof(*p));
}
uint16_t pokemon_calc_stat(pk3_box_t const *p, uint8_t level, stat_stat_t stat) {
	pk3_genes_t const *const iv = &p->iv;
	pk3_effort_t const *const ev = &p->ev;
	uint8_t i, e, b;
	switch(stat) {
	case STAT_HP:         i = iv->hp; e = ev->hp; b = species[p->species].stats.hp; break;
	case STAT_ATTACK:     i = iv->atk; e = ev->atk; b = species[p->species].stats.atk; break;
	case STAT_DEFENSE:    i = iv->def; e = ev->def; b = species[p->species].stats.def; break;
	case STAT_SPEED:      i = iv->spd; e = ev->spd; b = species[p->species].stats.spd; break;
	case STAT_SP_ATTACK:  i = iv->satk; e = ev->satk; b = species[p->species].stats.satk; break;
	case STAT_SP_DEFENSE: i = iv->sdef; e = ev->sdef; b = species[p->species].stats.sdef; break;
	// TODO: STAT_ATTACK is spelled wrong.
	}
	if(STAT_HP == stat) return gba_calc_hp_stat(level, b, i, e);
	return gba_calc_stat(level, b, i, e, stat_get_bonus((stat_nature_t)(p->pid % 25), stat)); // TODO: Special function for working with pid?
}
void pokemon_in_party_init(pk3_t *p) {
	uint8_t level = stat_get_level(STAT_GROWTH_RATE_MEDIUM_FAST, p->box.exp); // TODO: Growth rate?
	uint16_t hp = pokemon_calc_stat(&p->box, level, STAT_HP);
	p->party.status = (pk3_status_t){};
	p->party.level = level;
	p->party.pokerus_time = 0;
	p->party.stats = (pk3_stats_t){
		hp, hp,
		pokemon_calc_stat(&p->box, level, STAT_ATTACK),
		pokemon_calc_stat(&p->box, level, STAT_DEFENSE),
		pokemon_calc_stat(&p->box, level, STAT_SPEED),
		pokemon_calc_stat(&p->box, level, STAT_SP_ATTACK),
		pokemon_calc_stat(&p->box, level, STAT_SP_DEFENSE),
	};
}
void save_encrypt_all(gba_save_t *save, bool encrypt) {
	gba_party_t *party = gba_get_party(save);
	gba_pc_t *pc = gba_get_pc(save);
	for(size_t i = 0; i < party->size; i++) {
		(encrypt ? pk3_encrypt : pk3_decrypt)(&party->pokemon[i].box);
	}
	for(size_t i = 0; i < numberof(pc->box); i++) {
		for(size_t j = 0; j < numberof(pc->box[i].pokemon); j++) {
			(encrypt ? pk3_encrypt : pk3_decrypt)(&pc->box[i].pokemon[j]);
		}
	}
}

typedef struct {
	gba_save_t *save; // For use by filters.
	size_t size;
	pk3_box_t *pokemon[6+(GBA_BOX_COUNT*GBA_POKEMON_IN_BOX)];
} pokemon_list;
void list_clear_remainder(pokemon_list *list) {
	for(size_t i = list->size; i < numberof(list->pokemon); i++) {
		list->pokemon[i] = NULL;
	}
}
pokemon_list list_init(gba_save_t *const save) {
	assert(save);
	pokemon_list list[1];
	list->save = save;
	gba_party_t *party = gba_get_party(save);
	gba_pc_t *pc = gba_get_pc(save);
	size_t n = 0;
	for(size_t i = 0; i < party->size; i++) {
		list->pokemon[n++] = &party->pokemon[i].box;
	}
	for(size_t i = 0; i < numberof(pc->box); i++) {
		for(size_t j = 0; j < numberof(pc->box[i].pokemon); j++) {
			pk3_box_t *const p = &pc->box[i].pokemon[j];
			if(pokemon_slot_empty(p)) continue;
			list->pokemon[n++] = p;
		}
	}
	list->size = n;
	list_clear_remainder(list);
	return *list;
}
size_t list_filter_by_genes(pokemon_list *const list, pk3_genes_t const genes) {
	assert(list);
	size_t n = 0;
	for(size_t i = 0; i < list->size; i++) {
		if(!genes_equal(list->pokemon[i]->iv, genes)) continue;
		list->pokemon[n++] = list->pokemon[i];
	}
	list->size = n;
	list_clear_remainder(list);
	return list->size;
}
size_t list_filter_by_box(pokemon_list *const list, uint8_t box) {
	assert(list);
	gba_pc_t *pc = gba_get_pc(list->save);
	size_t n = 0;
	for(size_t i = 0; i < list->size; i++) {
		for(size_t j = 0; j < numberof(pc->box[box].pokemon); j++) {
			if(list->pokemon[i] != &pc->box[box].pokemon[j]) continue;
			list->pokemon[n++] = list->pokemon[i];
		}

		// TODO: Somehow this misses valid results. WTF?
//		pk3_box_t *const start = &pc->box[box].pokemon[0];
//		pk3_box_t *const end = &pc->box[box].pokemon[GBA_BOX_COUNT-1];
//		if(list->pokemon[i] < start || list->pokemon[i] > end) continue;
//		list->pokemon[n++] = list->pokemon[i];
	}
	list->size = n;
	list_clear_remainder(list);
	return list->size;
}
size_t list_filter_by_species(pokemon_list *const list, uint16_t const id) {
	assert(list);
	size_t n = 0;
	for(size_t i = 0; i < list->size; i++) {
		if(id != list->pokemon[i]->species) continue;
		list->pokemon[n++] = list->pokemon[i];
	}
	list->size = n;
	list_clear_remainder(list);
	return list->size;
}
#ifdef NOTYET // TODO
void list_filter_by_name() {}
void list_filter_by_ot() {}
void list_filter_by_gender() {}
void list_filter_by_level() {}
void list_filter_by_type() {}
#endif

void pokemon_move(pk3_box_t *dst, pk3_box_t *src) {
	if(src == dst) return;
	memcpy(dst, src, sizeof(*dst));
	memset(src, 0, sizeof(*src));
}
pk3_box_t *box_first_empty_slot(gba_pc_box_t *const box) {
	for(size_t i = 0; i < numberof(box->pokemon); i++) {
		pk3_box_t *p = &box->pokemon[i];
		if(pokemon_slot_empty(p)) return p;
	}
	return NULL;
}
typedef enum {
	DUP_ABORT = 0,
	DUP_SKIP = 1,
	DUP_UPGRADE = 2,
	DUP_OVERWRITE = 3,
	DUP_DONTCHECK = 4,
} dup_action; // TODO ???
int save_add_pokemon(gba_save_t *const save, pk3_box_t *const pokemon, dup_action ondup) {
	assert(DUP_UPGRADE != ondup);
	if(DUP_DONTCHECK != ondup) {
		pokemon_list list = list_init(save);
		list_filter_by_genes(&list, pokemon->iv);
		if(list.size > 0) {
			if(DUP_ABORT == ondup) return -1;
			if(DUP_OVERWRITE == ondup) pokemon_move(list.pokemon[0], pokemon);
			return 0;
		}
	}

	gba_party_t *party = gba_get_party(save);
	if(party->size < 6) {
		pokemon_move(&party->pokemon[party->size].box, pokemon);
		pokemon_in_party_init(&party->pokemon[party->size]);
		party->size++;
		return 0;
	}

	gba_pc_t *pc = gba_get_pc(save);
	for(uint32_t i = 0; i < numberof(pc->box); i++) {
		uint32_t j = (pc->current_box+i) % numberof(pc->box);
		pk3_box_t *slot = box_first_empty_slot(&pc->box[j]);
		if(!slot) continue;
		pokemon_move(slot, pokemon);
		return 0;
	}
	return -1;
}

#ifdef NOTYET // TODO
typedef enum {
	MISSING = 0,
	SEEN = 1,
	OWNED = 2,
} pokedex_state_t;
pokedex_state_t pokedex_get_state(gba_save_t *save, uint16_t id) {
	if(gba_pokedex_get_owned(save, id-1)) return OWNED;
	if(gba_pokedex_get_seen(save, id-1)) return SEEN;
	return MISSING;
}
void pokedex_set_state(gba_save_t *save, uint16_t id, pokedex_state_t state) {
	gba_pokedex_set_seen(save, id-1, 'O' == state || 'S' == state);
	gba_pokedex_set_owned(save, id-1, 'O' == state);
	bool s, o;
	switch(state) {
	case MISSING: s = false; o = false; break;
	case SEEN:
	}
}
#endif
void pokedex_set_owned(gba_save_t *save, uint16_t id, bool owned) {
	if(owned) gba_pokedex_set_seen(save, id-1, 1);
	gba_pokedex_set_owned(save, id-1, owned);
}
uint16_t pokedex_count(gba_save_t *save, char type) {
	uint16_t count = 0;
	for(uint16_t i = 0; i < numberof(species); i++) { // TODO: Number of Pokemon entries???
		if(('O' == type ? gba_pokedex_get_owned : gba_pokedex_get_seen)(save, i)) count++;
	}
	return count;
}

gba_item_slot_t *item_slot(gba_save_t *save, gba_item_pocket_t pocket, uint16_t item) {
	uint16_t const max = gba_get_pocket_size(save, pocket);
	gba_item_slot_t *out = NULL;
	for(uint16_t i = 0; i < max; i++) {
		gba_item_slot_t *slot = gba_get_pocket_item(save, pocket, i);
		if(slot->index == item && slot->amount) {
			out = slot;
			break;
		}
		if((!slot->index || !slot->amount) && !out) {
			slot->index = item;
			slot->amount = 0;
			out = slot;
			// Don't break, try to find the existing slot for this item.
		}
	}
	return out;
}

void fprint_trainer_card(FILE *out, gba_save_t *save) {
	assert(save);
	gba_trainer_t *trainer = gba_get_trainer(save);
	char16_t ucs2[sizeof(trainer->name)] = {};
	gba_text_to_ucs2(ucs2, trainer->name, sizeof(trainer->name));
	char utf8[sizeof(trainer->name)] = {};
	utf8_from_ucs2(utf8, ucs2, sizeof(trainer->name));
	fprintf(out, "Trainer Card\n");
	fprintf(out, "     Name: %s (%c)\n", utf8, trainer->is_female ? 'F' : 'M');
	fprintf(out, "    IDNo.: %05d\n", trainer->id);
	fprintf(out, "    Money: $P %d\n", gba_get_money(save));
	fprintf(out, "  Pokédex: %d\n", pokedex_count(save, 'O'));
	fprintf(out, "     Time: %dh%02dm%02ds\n", trainer->time_played.hours, trainer->time_played.minutes, trainer->time_played.seconds);
	fprintf(out, "   Badges: (unknown)\n"); // TODO
}
void fprint_location_story_progress(FILE *out, gba_save_t *save) {
	fprintf(out, "Location and Story Progress\n");
	fprintf(out, "  (not implemented)\n"); // TODO

	(void)save;

//	fprintf(

}
void fprint_stats(FILE *out, gba_save_t *save) {
	fprintf(out, "Stats\n");
	fprintf(out, "  (not implemented)\n"); // TODO
	(void)save;
}
void fprint_party(FILE *out, gba_party_t *party) {
	assert(party);
	fprintf(out, "Party\n");
	for(uint32_t i = 0; i < party->size; i++) {
		pk3_t *const p = &party->pokemon[i];
		char16_t tmp[sizeof(p->box.nickname)] = {};
		char name[sizeof(p->box.nickname)*3] = {};
		gba_text_to_ucs2(tmp, p->box.nickname, sizeof(p->box.nickname));
		utf8_from_ucs2(name, tmp, sizeof(p->box.nickname));
		fprintf(out, "  %d: %-10s (%c) lv.%d HP:%d/%d %s\n",
			i+1, name, pokemon_is_female(&p->box) ? 'F' : 'M', p->party.level,
			p->party.stats.hp, p->party.stats.max_hp,
			p->box.held_item ? items[p->box.held_item].name : "");
	}
}
void fprint_boxes(FILE *out, gba_save_t *save) {
	assert(save);
	fprintf(out, "Pokémon in PC\n");
	gba_pc_t *pc = gba_get_pc(save);
	bool pc_empty = true;
	for(uint32_t i = 0; i < numberof(pc->box); i++) {
		pokemon_list list = list_init(save);
		list_filter_by_box(&list, i);
		if(!list.size) continue;

		char16_t tmp[GBA_BOX_NAME_LENGTH];
		char name[GBA_BOX_NAME_LENGTH];
		gba_text_to_ucs2(tmp, pc->name[i], GBA_BOX_NAME_LENGTH);
		utf8_from_ucs2(name, tmp, GBA_BOX_NAME_LENGTH);
		fprintf(out, "  %-9s %2u Pokémon\n", name, (unsigned)list.size);
		pc_empty = false;
	}
	if(pc_empty) fprintf(out, "  (none)\n");
}
void fprint_pokemon_summary(FILE *out, pk3_t *full, pk3_box_t *in_box) {
	pk3_t storage[1];
	pk3_t *p = NULL;
	if(full) {
		p = full;
	} else {
		p = storage;
		memcpy(&p->box, in_box, sizeof(*in_box)); // TODO: Kind of ugly.
		pokemon_in_party_init(p);
	}

	char16_t tmp[sizeof(p->box.nickname)] = {};
	char name[sizeof(p->box.nickname)*3] = {};
	gba_text_to_ucs2(tmp, p->box.nickname, sizeof(p->box.nickname));
	utf8_from_ucs2(name, tmp, sizeof(p->box.nickname));
	fprintf(out, "%s (%c), lv.%d HP:%d/%d %s\n", // TODO: Improve this format.
		name, pokemon_is_female(&p->box) ? 'F' : 'M', p->party.level,
		p->party.stats.hp, p->party.stats.max_hp,
		p->box.held_item ? items[p->box.held_item].name : "");

	fprintf(out, "Moves\n");
	for(uint8_t i = 0; i < 4; i++) {
		fprintf(out, "  %u: %-15s PP:%d/%d\n",
			i+1, moves[p->box.move[i]].name,
			(int)p->box.move_pp[i], (int)moves[p->box.move[i]].pp);
	}
}
void fprint_items(FILE *out, gba_save_t *save) {
	assert(save);
	for(uint8_t i = 0; i < numberof(pockets); i++) {
		bool pocket_empty = true;
		fprintf(out, "%s\n", pockets[i].label);
		for(size_t j = 0; j < gba_get_pocket_size(save, (gba_item_pocket_t)i); j++) {
			gba_item_slot_t *slot = gba_get_pocket_item(save, (gba_item_pocket_t)i, j);
			if(!slot->index || !slot->amount) continue; // TODO: break?
			fprintf(out, "  %3u x %s\n", (unsigned)slot->amount, items[slot->index].name);
			pocket_empty = false;
		}
		if(pocket_empty) fprintf(out, "  (none)\n");
		if(i+1 < numberof(pockets)) fprintf(out, "\n");
	}
}
void fprint_pokedex(FILE *out, gba_save_t *save) {
	fprintf(out, "Pokédex (%u owned / %u seen)\n", pokedex_count(save, 'O'), pokedex_count(save, 'S'));
	for(size_t i = 1; i < numberof(species); i++) {
		bool owned = gba_pokedex_get_owned(save, i-1);
		bool seen = gba_pokedex_get_seen(save, i-1);
		if(!(owned || seen)) continue;
		fprintf(out, "  %c %c %03zu %s\n", owned ? 'O' : ' ', seen ? 'S' : ' ', i, species[i].name);
	}
}
void fprint_save(FILE *out, gba_save_t *save) {
	assert(save);

	fprint_trainer_card(out, save);
	fprintf(out, "\n");
	fprint_location_story_progress(out, save);
	fprintf(out, "\n");
	fprint_stats(out, save);
	fprintf(out, "\n");
	fprint_party(out, gba_get_party(save));
	fprintf(out, "\n");
	fprint_boxes(out, save);
	fprintf(out, "\n");
	fprint_items(out, save);
	fprintf(out, "\n");
	fprint_pokedex(out, save);

	// TODO: For testing:
/*	fprintf(out, "\n");
	char test[sizeof(GBA_TO_CODEPAGE)*3] = {};
	utf8_from_ucs2(test, (char16_t *)GBA_TO_CODEPAGE, sizeof(GBA_TO_CODEPAGE));
	fprintf(out, "test: %s\n", test);

	char16_t test2[sizeof(GBA_TO_CODEPAGE)] = {};
	ucs2_from_utf8(test2, test, sizeof(GBA_TO_CODEPAGE));
	char test3[sizeof(GBA_TO_CODEPAGE)*3] = {};
	utf8_from_ucs2(test3, test2, sizeof(GBA_TO_CODEPAGE));
	fprintf(out, "test: %s\n", test3);*/
}

void fprint_list(FILE *out, pokemon_list *const list) {
	assert(list);
	fprintf(out, "List of %zu Pokémon\n", list->size);
	for(size_t i = 0; i < list->size; i++) {
		char16_t tmp[sizeof(list->pokemon[i]->nickname)];
		char name[sizeof(list->pokemon[i]->nickname)];
		gba_text_to_ucs2(tmp, list->pokemon[i]->nickname, sizeof(name));
		utf8_from_ucs2(name, tmp, sizeof(name));
		fprintf(out, "  %s\n", name);
	}
}
void print_list(pokemon_list *const list) { return fprint_list(stdout, list); }

void fprint_hex(FILE *out, char const *buf, size_t len) {
	for(size_t i = 0; i < len; i++) {
		if(i && 0 == i % 8) fprintf(out, " ");
		if(i && 0 == i % 32) fprintf(out, "\n");
		fprintf(out, "%02x", (unsigned char)buf[i]);
	}
	if(0 != len % 24) fprintf(out, "\n");
}

unsigned char *_data = NULL;
gba_save_t *_save = NULL;
void save_close(void) {
	if(_save) { gba_free_save(_save); _save = NULL; }
	mmap_close(_data); _data = NULL;
}
int save_open(char const *path) {
	if(_data || _save) save_close();
	if(!path) return 0;
	_data = mmap_open(path);
	if(!_data) return -1;
	_save = gba_read_main_save(_data);
	if(!_save) { save_close(); return -1; }
	save_encrypt_all(_save, 0);
	return 0;
}
int save_store(void) {
	save_encrypt_all(_save, 1); // TODO: Build this into the API? Is it safe?
	gba_save_game(_data, _save);
	gba_save_game(_data, _save); // TODO: For some reason saving once doesn't work?
	// TODO: fsync?
	save_encrypt_all(_save, 0);
	return 0;
}

#ifdef _PKMN_SAVE_MODIFIER_STANDALONE
int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "\n");
		fprintf(stderr, "  Usage: %s save-file.srm\n", argv[0]);
		fprintf(stderr, "  Prints information about the save.\n");
		fprintf(stderr, "  For editing, try loading pkmn-save-modifier.c with Cling.\n");
		fprintf(stderr, "\n");
		return -1;
	}
	save_open(argv[1]);
	fprint_save(stdout, _save);
	save_close();
	return 0;
}
#endif





