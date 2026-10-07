# Gen 4 data

Diamond, Pearl, Platinum, HeartGold and SoulSilver. `tools/generate_nds_tables.py` turns these
tables into C. The [data README](../README.md) explains the format, how sources are cited and
[what archive paths such as `a/0/0/2` mean](../README.md#where-cartridge-data-lives).

## `species.tsv`

Species by National Dex number, then the forms whose data differs from their species'.

- Columns: base stats, growth rate, gender ratio, types, abilities, egg cycles and base friendship.
- Sources:
  - Diamond and Pearl: pret pokediamond `files/poketool/personal/personal.json`.
  - Platinum: its personal archive, `poketool/personal/pl_personal.narc`. pret pokeplatinum's
    `res/pokemon/<species>/data.json` and `forms/<form>/data.json` agree.
  - HeartGold and SoulSilver: their personal archive, `a/0/0/2`. The two carts agree.
- The games agree on every species and form they share. Platinum brings in Giratina's Origin Forme,
  Shaymin's Sky Forme and Rotom's five forms, so their rows begin with `platinum`. `from_game` is as
  the [data README](../README.md#from_game) explains.
- A form with no row of its own has its species' data. Every form has its species' gender ratio and
  growth rate.
- Arceus has no form rows: the games' code gives it the type of the plate it holds.
- A species of one type has it as both, and `ability_2` is 0 for a species with one ability, as the
  games store them.
- `gender_ratio` is the byte the game stores:
  - 0 means always male, 254 always female and 255 genderless.
  - Otherwise a Pokémon is female when its personality's low byte is below this value.

## `charmap.tsv`

Characters by 16-bit code.

- Source: pret's msgenc `charmap.txt`. pokeplatinum, pokeheartgold and pokediamond agree.
- Except the first 15 Korean syllables, 가 to 갗, which are at 0x0401–0x040F, one code above
  `charmap.txt`. Korean Pearl's species names (member 357 of `msgdata/msg.narc`) decode to the
  Korean names only so, and its text never uses 0x0400.
- Each character is the one the cart's font draws.
- Glyphs only the games have use Game Freak's private-use code points, U+E081–U+E0A8.
- Korean jamo decode as compatibility jamo.
- Codes that are not listed have no character.

## `items.tsv`

Items by item number: the English name and the pocket in each game.

- Pockets, from pret:
  - Diamond and Pearl: pokediamond `item_data.json`.
  - Platinum: pokeplatinum `res/items/data`.
  - HeartGold and SoulSilver: pokeheartgold `item_data.csv`.
- English names: the games' item text.
- `-` in a pocket column means that game lacks the item.
- Other languages use the shared item names, which Gen 4 numbers the same way.
