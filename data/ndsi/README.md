# Gen 5 data

Black, White, Black 2 and White 2. `tools/generate_ndsi_tables.py` turns these tables into C. The
[data README](../README.md) explains the format, how sources are cited and
[what archive paths such as `a/0/1/6` mean](../README.md#where-cartridge-data-lives).

The tables come from the author's US carts.

## `species.tsv`

Species by National Dex number, then the forms whose data differs from their species'.

- Columns: base stats, growth rate, gender ratio, types, abilities (the first, the second and the
  hidden), egg cycles and base friendship.
- Source: the personal archives, `a/0/1/6`, which have a member for each species and form: Black's
  for Black and White, Black 2's for Black 2 and White 2. White's and White 2's agree with them.
- The games agree on every species and form they share. Black 2 and White 2 bring in the Therian
  Formes and White and Black Kyurem, so their rows begin with `black2_white2`. `from_game` is as
  the [data README](../README.md#from_game) explains.
- A form with no row of its own has its species' data, as Resolute Keldeo, whose data is Keldeo's,
  has. Every form has its species' gender ratio and growth rate.
- Arceus has no form rows: the games' code gives it the type of the plate it holds.
- A species of one type has it as both, and an ability a species lacks is 0, as the games store
  them.
- `gender_ratio` is the byte the game stores:
  - 0 means always male, 254 always female and 255 genderless.
  - Otherwise a Pokémon is female when its personality's low byte is below this value.

## `items.tsv`

Items by item number: the English name and the pocket in each pair of games.

- Pockets: the item archive, `a/0/2/4`. Each item's member holds its pocket in bits 7–10 of byte 8.
- English names: the item name list in the text archive, `a/0/0/2`: member 54 in Black and White,
  64 in Black 2 and White 2.
- `-` in a pocket column means that pair lacks the item.
- Other languages use the shared item names, which Gen 5 numbers the same way.
