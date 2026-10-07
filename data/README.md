# Data

The tables libSPEC is built from. At build time, `tools/generate_*_tables.py` turns each folder's
TSV files into C. Nothing generated is committed.

This folder holds what every console shares: names in every language the games use, each game's
moves, and the highest numbers each game uses. Each console has a folder of its own, with its own
README:

- [`gb/`](gb/README.md): Gen 1
- [`gbc/`](gbc/README.md): Gen 2
- [`gba/`](gba/README.md): Gen 3
- [`nds/`](nds/README.md): Gen 4
- [`ndsi/`](ndsi/README.md): Gen 5
- [`3ds/`](3ds/README.md): Gen 6 and 7

## Reading the files

- Every file is tab-separated, with a header row first and then one row per entry.
- `-` in a cell means there is nothing there: no name in that language, or no pocket in that game.

### `from_game`

Where the games disagree about something, its table has a `from_game` column, and a row for each
game type that changes it:

- `-`: every game type the table covers.
- A game type, such as `sun_moon`: that game type and every later one, until a later row for the
  same entry takes over.

For a given game type, the row in force is the one with the latest `from_game` that isn't after it.
Game types are `spec_game_type_t`'s, which count up generation by generation. An entry a game type
lacks has no row in force for it, so its first row names the game type that brings it in.

### Column order

Every file follows the same column order, so its short, fixed columns line up and its names are
easy to find.

1. The key: the number each row is for, such as `item` or `national`.
2. The other short columns: numbers, flags, pockets and codes.
3. Last, the names and other text, one column per language, in this order:
   1. `japanese` (kana)
   2. `english`
   3. `french`
   4. `italian`
   5. `german`
   6. `spanish`
   7. `korean`
   8. `chinese_simplified`
   9. `chinese_traditional`

A file leaves out the languages it has no names for, but keeps the rest in this order. Some files
lack a language because no source for it is on hand yet, such as Gen 2's French, German and
Italian item names.

A column that serves more than one language sits where its first language would. For example, in
the Game Boy character maps, `japanese_hiragana` follows `japanese`, and `french_german` and
`italian_spanish` take French's and Italian's places. Gen 3's `international` character set, which
the English, Italian and Spanish games share, takes English's place.

### Citations

Sources are named precisely enough to find again:

- pret and the other disassemblies are cited by repository and path, such as
  `pokecrystal data/items/names.asm`.
- Cartridge data is cited by its place on the cart, as the next section explains. The carts are
  the author's own, and US carts unless a note names another, such as Korean Pearl for Gen 4's
  Korean text.

## Where cartridge data lives

The DS and 3DS games keep almost all their data in numbered archives in the cart's file system.
Game Freak named every folder and file there with a single digit, so a path like `a/0/1/6` is the
folder `a`, then the folder `0`, then the folder `1`, then the file `6`. The path is only a slot:
it says nothing about what the file holds, and each game puts the same kind of data in a different
slot.

- On a DS cart the file system is the ROM's own, and each of these files is a NARC archive. On a
  3DS cart it is the RomFS, and each file is a GARC archive.
- An archive holds numbered members, counted from 0: "member 55 of `a/0/3/2`" is the 56th file in
  that archive.
- A text archive's members are message files. Each holds one list of lines, such as every move's
  name, where line n is the name of entry n.
- A text archive holds one language. A game with several languages has one text archive per
  language, and a given list is the same member in each of them.
- A data archive has one member per entry. The personal archive has one member per species and
  form (Game Freak's "personal" data: base stats, types, abilities, gender ratio, egg cycles, base
  friendship and growth rate), and the item archive one member per item.
- From Black and White on, a species' personal member says where its forms' members start and how
  many forms it has. Gen 4's form members sit after the species' and the two eggs', in an order its
  code knows.

The archives these tables cite:

| Game | Archive | What it holds |
|---|---|---|
| Platinum | `poketool/personal/pl_personal.narc` | Personal data (this cart's files have names) |
| | `poketool/waza/pl_waza_tbl.narc` | Move data, 16 bytes per move |
| HeartGold and SoulSilver | `a/0/0/2` | Personal data |
| | `a/0/1/1` | Move data, as Platinum's |
| Black, White, Black 2, White 2 | `a/0/0/2` | Text in the cart's language: menus and name lists |
| | `a/0/1/6` | Personal data |
| | `a/0/2/1` | Move data, 36 bytes per move |
| | `a/0/2/4` | Item data, 36 bytes per item |
| X and Y | `a/0/7/2` to `a/0/7/9` | Text: Japanese kana, Japanese kanji, English, French, Italian, German, Spanish, Korean |
| | `a/2/1/2` | Move data, 36 bytes per move |
| | `a/2/1/8` | Personal data |
| | `a/2/2/0` | Item data |
| Omega Ruby and Alpha Sapphire | `a/0/7/1` to `a/0/7/8` | Text, in X and Y's language order |
| | `a/1/8/9` | Move data: one member holding every move's 36 bytes |
| | `a/1/9/5` | Personal data |
| | `a/1/9/7` | Item data |
| Sun and Moon | `a/0/3/0` to `a/0/3/9` | Text, in Ultra Sun and Ultra Moon's order |
| | `a/0/1/1` | Move data: one member holding every move's 40 bytes |
| | `a/0/1/7` | Personal data |
| | `a/0/1/9` | Item data |
| Ultra Sun and Ultra Moon | `a/0/3/0` to `a/0/3/9` | Text, in X and Y's order, then Simplified and Traditional Chinese |
| | `a/0/1/1` | Move data, as Sun and Moon's |
| | `a/0/1/7` | Personal data |
| | `a/0/1/9` | Item data |

## The name tables

The names are Ultra Sun and Ultra Moon's, the last 3DS games, which have every name the others have.
Each name list is the member below in each of their text archives, `a/0/3/0` to `a/0/3/9`. The two
carts' lists are identical, and Sun and Moon's agree with them wherever both have a line.

| File | Numbered by | Member |
|---|---|---|
| `species_names.tsv` | National Dex number, 1–807 | 60 |
| `form_names.tsv` | National Dex number and form | 119 |
| `move_names.tsv` | Move number, 1–728 | 118 |
| `ability_names.tsv` | Ability number, 1–233 | 101 |
| `item_names.tsv` | Gen 4–7 item number, 1–959 | 40 |
| `nature_names.tsv` | `spec_nature_t`, 0–24 | 92 |
| `type_names.tsv` | `spec_type_t`, 0–18 | 112 |

### `species_names.tsv`

- The names as the 3DS games show them. Every 3DS game agrees on every species it has.
- Italian and Spanish games use the English names, except Type: Null.
- The carts store Chinese species names in glyphs of their own, not as characters. Each glyph is
  read as the character PokeAPI's names put in the same place, by majority over every species
  (PokeAPI commit `421247a969939f793c52bf8105f3c4e35517d8c1`). The glyphs as Gen 7 stores them are in
  [`3ds/chinese_species_names.tsv`](3ds/README.md).
- The generator derives the spellings older games store from these names:
  - Gen 1 to 4: upper case, with French accents dropped.
  - Gen 1 and 2: Mr. Mime without its space.
  - Gen 5: Farfetch'd with a plain apostrophe.

### `form_names.tsv`

Most games number a species' forms the same way, but a few numbers mean different forms in
different games. `from_game` works as it does in every table, except that `-` here is every game
that has the form, which nearly every row is.

Only Pikachu needs another value today: in Omega Ruby and Alpha Sapphire, forms 1 to 6 are the
dressed-up Pikachu, and from Sun and Moon on the same numbers are the caps. X and Y get neither.

Where the rows come from:

- Ultra Sun and Ultra Moon's form list, for every row but the dressed-up Pikachu. A species' own line
  names its first form; the lines after the last species name the other forms, species by species.
  Each species' form count, in its member of the personal archive (`a/0/1/7`), says how many lines
  it takes. Sun and Moon's list names every form it has the same way.
- Omega Ruby and Alpha Sapphire's form list, member 5 of their text archives (`a/0/7/1` to
  `a/0/7/8`), for the dressed-up Pikachu. Those games have no Chinese.

Also:

- Forms the lists leave unnamed, such as totems, are left out.
- The carts call every Unown form "One form", and name Arceus's and Genesect's forms after the
  species.

### `item_names.tsv`

- Items by the number Gen 4 to 7 share. Gen 1 to 3 number their items their own way, so each of
  those consoles maps its items here where it can.
- Dummies, which the lists name "???", are left out.

### `move_names.tsv`, `ability_names.tsv`, `nature_names.tsv` and `type_names.tsv`

- Moves are named as Ultra Sun and Ultra Moon name them.
- Natures follow `spec_nature_t`, and types follow `spec_type_t`: Gen 5 to 7's order, then ???.
- ???, which Gen 2 to 4 give Curse, is in no 3DS game. Its names come from the Game Boy games:
  - English: pret pokecrystal `data/types/names.asm`.
  - Japanese: Narishma-gb's pokesilver.
  - German: pret pokeruby's German build, `data-de/text/type_names.inc`.
  - Spanish: erosunica's pokecrystal-es.
  - Korean: pokegold-kr.
  - French, Italian and Chinese: no source on hand.

## `moves.tsv`

Each move's type and base PP, in every game type that has the move.

- A move's first row names the game type that brings it in, and each later row a game type that
  changes it.
- `type` is a `spec_type_t` name. `mystery` is ???.

Each game type's moves come from its own source. The two games of each pair agree, and each game
type keeps every earlier one's moves under the same numbers.

- Red, Blue and Yellow: pret pokered and pokeyellow, `data/moves/moves.asm`.
- Gold, Silver and Crystal: pret pokegold and pokecrystal, `data/moves/moves.asm`.
- Ruby, Sapphire, Emerald, FireRed and LeafGreen: pret pokeruby `src/data/battle_moves.c`, and
  pokeemerald and pokefirered `src/data/battle_moves.h`.
- Diamond and Pearl: pret pokediamond `files/poketool/waza/waza_tbl.json`.
- Platinum, HeartGold and SoulSilver: the move archives, where byte 4 is the type, with ??? as 9,
  and byte 6 the PP. pret pokeplatinum's `res/moves` agrees with Platinum's.
- Gen 5 to 7: the move archives, where byte 0 is the type and byte 5 the PP.

## `game_limits.tsv`

For each game type, the highest species (by National Dex number), move and ability number its games
have. Gen 1 and 2 have no abilities, so theirs is 0.

- Moves: each game type's move list, as in `moves.tsv`.
- Species: each console's species table. Sun and Moon's species-name list, member 55 of their text
  archives, ends at Marshadow, 802.
- Abilities:
  - Gen 3: pret pokeemerald `include/constants/abilities.h`. Gen 3 numbers abilities as later games
    do except at the end: it has the unused Cacophony as 76 and Air Lock as 77, where later games
    have Air Lock as 76. The table uses the later numbering, which names them.
  - Gen 4: pret pokeplatinum's ability list.
  - Gen 5 to 7: each game type's ability-name list: member 182 of Black's `a/0/0/2` and 374 of
    Black 2's, then member 34 in X and Y, 37 in Omega Ruby and Alpha Sapphire, 96 in Sun and Moon
    and 101 in Ultra Sun and Ultra Moon.
