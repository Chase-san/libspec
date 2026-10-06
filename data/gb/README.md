# Gen 1 data

Red, Blue, Green and Yellow. `tools/generate_gb_tables.py` turns these tables into C. The
[data README](../README.md) explains the format and how sources are cited.

## `species.tsv`

Species by the game's own index, which is not the National Dex number.

- Columns: the National Dex number, the base stats (HP, Attack, Defense, Speed, Special) and the
  growth rate.
- From pret pokered:
  - The index and National Dex number: `constants/pokemon_constants.asm` and
    `data/pokemon/dex_order.asm`.
  - The rest: `data/pokemon/base_stats/*.asm`; Mew's is `data/pokemon/mew.asm`.
- Indices that name no species are left out.

## `charmap.tsv`

Characters by byte, one column per character set: `japanese`, `japanese_hiragana`, `english`,
`french_german` and `italian_spanish`.

The characters come from the name fonts in each disassembly's `constants/charmap.asm`:

- English: pret pokered and pokeyellow.
- French and German: einstein95's pokered-fr and pokered-de, Narishma-gb's pokeyellow-fr and
  Brianum's pokeyellow-de.
- Italian and Spanish: einstein95's pokered-es. The Italian Blue cart has the same font.
- Japanese: Narishma-gb's pokegreen, pokeblue and pokeyellow-jp.

Reading the table:

- 0x50 ends a name and 0x49–0x5F are control codes, so neither is listed.
- A ligature byte is two characters. Writing matches them greedily.
- `~` marks a byte that is read but never written, because another byte writes the same character.
- Glyphs only the games have use Game Freak's private-use code points: PK U+E0A7, MN U+E0A8, the
  ellipsis U+E08D and × U+E088. The Pokédollar is `$`.
- Where both kana share a tile, `japanese` reads it as katakana and `japanese_hiragana` as
  hiragana.

## `items.tsv`

The items the bag and PC can hold, by item number, with a name in each language Gen 1 shipped in.

- English: pret pokered `constants/item_constants.asm` and `data/items/names.asm`.
- The other languages, from each game's `data/items/names.asm`:
  - Japanese: pokegreen.
  - French: pokered-fr.
  - German: pokered-de.
  - Spanish: pokered-es.
  - Italian: the Italian Blue cart.
- Machines are named the way `GetMachineName` builds them (`home/names.asm`).
- Left out: dummies (named "?????" or only an `ITEM_` constant), badges and elevator floors.
