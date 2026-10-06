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
- Ultra Sun and Ultra Moon are not dumped yet. Their additions come from Pokémon Bank 6.8, whose
  text matches Sun and Moon's line for line wherever both have a line.

## Where cartridge data lives

The DS and 3DS games keep almost all their data in numbered archives in the cart's file system.
Game Freak named every folder and file there with a single digit, so a path like `a/0/1/6` is the
folder `a`, then the folder `0`, then the folder `1`, then the file `6`. The path is only a slot:
it says nothing about what the file holds, and each game puts the same kind of data in a different
slot.

- On a DS cart the file system is the ROM's own, and each of these files is a NARC archive. On a
  3DS cart it is the RomFS, and each file is a GARC archive. Pokémon Bank's RomFS works the same
  way.
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
| Sun and Moon | `a/0/3/0` to `a/0/3/9` | Text, in X and Y's order, then Simplified and Traditional Chinese |
| | `a/0/1/7` | Personal data |
| | `a/0/1/9` | Item data |
| Pokémon Bank 6.8 | `a/0/0/4` to `a/0/1/3` | Text, in Sun and Moon's language order |

## The name tables

Each name list is one member of the text archives: the Sun and Moon member below in each of
`a/0/3/0` to `a/0/3/9`, and the Pokémon Bank member in each of `a/0/0/4` to `a/0/1/3`.

| File | Numbered by | Sun and Moon member | Pokémon Bank member |
|---|---|---|---|
| `species_names.tsv` | National Dex number, 1–807 | 55 | 12, for 803–807 |
| `form_names.tsv` | National Dex number and form | 114 | none |
| `move_names.tsv` | Move number, 1–728 | 113 | 52, for 720–728 |
| `ability_names.tsv` | Ability number, 1–233 | 96 | 27, for 233 |
| `item_names.tsv` | Gen 4–7 item number, 1–959 | 36 | 8, for 921–959 |
| `nature_names.tsv` | `spec_nature_t`, 0–24 | 87 | none |
| `type_names.tsv` | `spec_type_t`, 0–17 | 107 | none |

### `species_names.tsv`

- The names as the 3DS games show them. X, Y, Omega Ruby, Alpha Sapphire, Sun and Moon agree on
  every species they share.
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

Forms are numbered as Gen 7 numbers them. The `source` column says where each row comes from:

- `sun_moon`: Sun and Moon's form list. A species' own line names its first form; the lines after
  the last species name the other forms, species by species. Each species' form count, in its
  member of the personal archive (`a/0/1/7`), says how many lines it takes.
- `pokeapi`: the five forms only Ultra Sun and Ultra Moon have (Partner Cap Pikachu, Dusk Form
  Lycanroc and Necrozma's three), from PokeAPI at the commit above.
- `omega_ruby_alpha_sapphire`: the dressed-up Pikachu, which those games number their own way, from
  their form list, member 5 of their text archives (`a/0/7/1` to `a/0/7/8`). Those games have no
  Chinese.

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
