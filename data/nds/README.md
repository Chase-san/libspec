# Gen 4 data

Diamond, Pearl, Platinum, HeartGold and SoulSilver. `tools/generate_nds_tables.py` turns these
tables into C. The [data README](../README.md) explains the format and how sources are cited.

## `species.tsv`

Species by National Dex number, then the forms whose base stats differ from their species'.

- Source: pret pokeplatinum `res/pokemon/<species>/data.json` and `forms/<form>/data.json`.
- Gender ratio and growth rate always come from form 0.
- `gender_ratio` is the byte the game stores:
  - 0 means always male, 254 always female and 255 genderless.
  - Otherwise a Pokémon is female when its personality's low byte is below this value.

## `charmap.tsv`

Characters by 16-bit code.

- Source: pret's msgenc `charmap.txt`. pokeplatinum, pokeheartgold and pokediamond agree.
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
