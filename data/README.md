# Data

The tables libSPEC is built from. At build time, `tools/generate_*_tables.py` turns each folder's
TSV files into C. Nothing generated is committed.

This folder holds what every console shares: names in every language the games use. Each console
has a folder of its own, with its own README:

- [`gb/`](gb/README.md): Gen 1
- [`gbc/`](gbc/README.md): Gen 2
- [`gba/`](gba/README.md): Gen 3
- [`nds/`](nds/README.md): Gen 4
- [`ndsi/`](ndsi/README.md): Gen 5
- [`3ds/`](3ds/README.md): Gen 6 and 7

## Reading the files

- Every file is tab-separated, with a header row first and then one row per entry.
- `-` in a cell means there is nothing there: no name in that language, or no pocket in that game.

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
  the author's own US carts.

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
  form (Game Freak's "personal" data: base stats, types, abilities, gender ratio and growth rate),
  and the item archive one member per item.

The archives these tables cite:

| Game | Archive | What it holds |
|---|---|---|
| Black, White, Black 2, White 2 | `a/0/0/2` | Text in the cart's language: menus and name lists |
| | `a/0/1/6` | Personal data |
| | `a/0/2/4` | Item data, 36 bytes per item |
| X and Y | `a/0/7/2` to `a/0/7/9` | Text: Japanese kana, Japanese kanji, English, French, Italian, German, Spanish, Korean |
| | `a/2/2/0` | Item data |
| Omega Ruby and Alpha Sapphire | `a/0/7/1` to `a/0/7/8` | Text, in X and Y's language order |
| | `a/1/9/5` | Personal data |
| | `a/1/9/7` | Item data |
| Sun and Moon | `a/0/1/9` | Item data |
| Ultra Sun and Ultra Moon | `a/0/3/0` to `a/0/3/9` | Text, in X and Y's order, then Simplified and Traditional Chinese |
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
| `type_names.tsv` | `spec_type_t`, 0–17 | 112 |

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
different games. The `from_game` column says which games a row is for:

- `-`: every game that has the form. Nearly every row is this.
- A game type, such as `sun_moon`: that game type and every later one, until a later row for the
  same species and form takes over. Game types are `spec_game_type_t`'s, which count up generation
  by generation.

For a game type, a form's name is the row with the latest `from_game` that isn't after it. Today
only Pikachu needs this: in Omega Ruby and Alpha Sapphire, forms 1 to 6 are the dressed-up Pikachu,
and from Sun and Moon on the same numbers are the caps. X and Y get neither.

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
- Natures follow `spec_nature_t`, and types follow `spec_type_t`, which is Gen 4 to 7's order.
