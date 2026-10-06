# Gen 5 data

Black, White, Black 2 and White 2. `tools/generate_ndsi_tables.py` turns these tables into C. The
[data README](../README.md) explains the format, how sources are cited and
[what archive paths such as `a/0/1/6` mean](../README.md#where-cartridge-data-lives).

Both tables come from the author's US carts.

## `species.tsv`

Species by National Dex number, then the forms whose base stats differ from their species'.

- Source: Black 2's personal archive, `a/0/1/6`, which has a member for each species and form.
  Black's has the same values in these columns.
- Gender ratio and growth rate always come from form 0.
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
