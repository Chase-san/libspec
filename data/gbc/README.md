# Gen 2 data

Gold, Silver and Crystal. `tools/generate_gbc_tables.py` turns these tables into C, reusing parts
of the Gen 1 generator. The [data README](../README.md) explains the format and how sources are
cited.

## `species.tsv`

Species by National Dex number, with base stats, growth rate, gender ratio, types and egg cycles.

- Source: pret pokecrystal `data/pokemon/base_stats/*.asm`, in `BaseData`'s order. pokegold has
  the same rows, and the author's Gold, Silver, Crystal (both revisions) and Japanese Crystal carts
  agree.
- A species of one type has it as both, as the games store it.
- `egg_cycles` is the base data's step cycles to hatch, which an egg counts down in its friendship.
- Every species' base friendship is 70 (`BASE_HAPPINESS`, `constants/pokemon_data_constants.asm`),
  so the table has none.
- `gender_ratio` is the byte the game stores:
  - 0 means always male, 254 always female and 255 genderless.
  - Otherwise a Pokémon is female when `Attack DV << 4 | Speed DV` is at most this value.

## `charmap.tsv`

Characters by byte, in the same columns as Gen 1's: `japanese`, `japanese_hiragana`, `english`,
`french_german` and `italian_spanish`.

The characters come from the name fonts:

- English: `constants/charmap.asm` of pret pokecrystal and pokegold.
- Japanese: Narishma-gb's pokesilver. The Japanese Crystal cart draws them the same way.
- French, German, Italian and Spanish:
  - 0xBA–0xDF and 0xE4–0xE5 come from pokecrystal's copies of their fonts
    (`gfx/font/french_german.png` and `spanish_italian.png`).
  - erosunica's pokecrystal-es agrees on Spanish.

The table reads the same way as Gen 1's:

- 0x50 ends a name and 0x49–0x5F are control codes.
- A ligature byte is two characters.
- `~` marks a byte that is read but never written.
- Glyphs only the games have use Game Freak's private-use code points.
- `japanese_hiragana` is the other reading of a tile both kana share.

## `items.tsv`

Items by item number: names, the pocket in each pair of games, machine numbers and mail.

- English: pret pokecrystal `data/items/names.asm`. `#` is written as POKé, the way `PlacePOKe`
  prints it (`home/text.asm`).
- Pockets (`gold_silver_pocket`, `crystal_pocket`):
  - Each is the type `ItemAttributes` gives the item (`data/items/attributes.asm`), filed the way
    `_ReceiveItem` files it, in pokegold and in pokecrystal.
  - `-` is a dummy, such as Gold and Silver's TERU-SAMA.
- `machine`: the TM or HM number under which the TM/HM pocket keeps the item's quantity.
- `is_mail`: follows `data/items/mail_items.asm`.
- `migration_id`: the item's number in Gen 4 to 7, found as Gen 1's is
  ([`gb/README.md`](../gb/README.md#itemstsv)). Gen 2's berries, apricorns and mail have none.
- The other languages:
  - Japanese: the Japanese Crystal cart.
  - Spanish: pokecrystal-es.
  - Korean: pokegold-kr, which is Gold and Silver only.
  - French, German and Italian: no retail source on hand.
