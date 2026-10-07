# Gen 6 and 7 data

X, Y, Omega Ruby, Alpha Sapphire, Sun, Moon, Ultra Sun and Ultra Moon.
`tools/generate_3ds_tables.py` turns these tables into C. The [data README](../README.md) explains
the format, how sources are cited and
[what archive paths such as `a/0/1/7` mean](../README.md#where-cartridge-data-lives).

These tables come from the author's US carts.

## `species.tsv`

Base stats, gender ratio, growth rate, types, abilities (the first, the second and the hidden), egg
cycles and base friendship. Rows are by National Dex number, form and `from_game`, as the
[data README](../README.md#from_game) explains.

- Sources: each pair's personal archive, which the two carts of the pair hold alike:
  - X and Y: `a/2/1/8`.
  - Omega Ruby and Alpha Sapphire: `a/1/9/5`. They change nothing X and Y have, and bring in forms
    such as the Primal Reversions and more Mega Evolutions.
  - Sun and Moon: `a/0/1/7`. They change some species' base stats and abilities, and make Pikachu's
    forms the caps.
  - Ultra Sun and Ultra Moon: `a/0/1/7`. They change nothing Sun and Moon have, and bring in the
    species from Poipole on and more forms.
- A form has a row of its own only when its data differs from its species'; otherwise it has its
  species'.
- Arceus and Silvally have no form rows: the games' code gives them the type of the item they hold.
- A species of one type has it as both, and one with a single ability has it as both its first and
  its second, as the games store them.

## `items.tsv`

The bag pocket each game files an item in, by the item number Gen 4 to 7 share.

- Pockets come from each game's item archive, where every item's member holds its pocket in bits
  7–10 of the u16 at byte 8:
  - X and Y: `a/2/2/0`.
  - Omega Ruby and Alpha Sapphire: `a/1/9/7`.
  - Sun and Moon: `a/0/1/9`.
  - Ultra Sun and Ultra Moon: `a/0/1/9`. These games add the Rotom Powers pocket, so their pocket
    numbers for the held Z-Crystals move up by one.
- `-` is an item the game lacks, or a held Z-Crystal, which never sits in the bag.
- Names come from the shared item names.

## `chinese_glyphs.tsv`

Gen 7 stores Chinese species names in private-use glyphs, not as characters. This table reads each
glyph back as a character.

- The units are those in Ultra Sun and Ultra Moon's species-name list: member 60 of the Simplified
  (`a/0/3/8`) and Traditional (`a/0/3/9`) Chinese text archives. Sun and Moon's list holds the same
  units for every species it has.
- Each unit is matched to the character PokeAPI's names put in the same place, by majority over
  every species.

## `chinese_species_names.tsv`

The Chinese species names exactly as Gen 7 stores them, as glyph units.

- Source: the same species-name lists as `chinese_glyphs.tsv`.
- Row 0 is the Egg's name, from line 0 of each list.
