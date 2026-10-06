# Gen 3 data

Ruby, Sapphire, Emerald, FireRed and LeafGreen. `tools/generate_gba_tables.py` turns these tables
into C. The [data README](../README.md) explains the format and how sources are cited.

## `species.tsv`

Species by the game's own index, which is not the National Dex number.

- Columns: the National Dex number, base stats, growth rate and gender ratio.
- Source: pret pokeemerald `gSpeciesInfo` and `sSpeciesToNationalPokedexNum`.
- `gender_ratio` is the byte the game stores:
  - 0 means always male, 254 always female and 255 genderless.
  - Otherwise a Pokémon is female when its personality's low byte is below this value.

## `charmap.tsv`

Characters by byte, one column per character set: `japanese`, `international`, `french` and
`german`.

- Source: pret's `charmap.txt`.
- Bytes from 0xF7 up are control codes.

## `items.tsv`

Items by item number: names, the pocket in each game and whether the PC refuses them.

- Pockets come from pret's item tables in pokeruby, pokeemerald and pokefirered.
- Names:
  - English: those same tables.
  - German: pokeruby's German build. Only Ruby and Sapphire were built in German, so items only
    Emerald, FireRed and LeafGreen have show `-` there.
- `is_important`: the item is one the PC refuses.
- `later_item`: the item's number in Gen 4 to 7, which names it in the other languages.
  - Usually it is the number pret pokeplatinum gives the item when Pal Park brings it over
    (`res/items/data`, its `gbaID`).
  - Otherwise it is the 3DS item with the same English name.
  - `-` means there is none, as with Gen 3's mail.
