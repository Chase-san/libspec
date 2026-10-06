# libSPEC

Specialized Pokemon Editing in C

This library is designed to make an highly accurate and safe editing library. The following information may be asperational rather than actual.

## Target Games

This library is targets pokemon games in their entirety (including non-english) on the following systems

- Nintendo Gameboy
- Nintendo Gameboy Color
- Nintendo Gameboy Advance
- Nintendo DS
- Nintendo DSi
- Nintendo 3DS

## Capabilities

This library has the following capabilities.

- Pokemon Editing
  - Boxes
  - Party
  - Daycare
- Pokedex Editing
- Trainer Editing
- Names in every language the games use: species, forms, moves, abilities, items, natures and types

## Data

The tables the library is built from are in `data/`. [data/README.md](data/README.md) describes what
each file holds and where it comes from, including what cartridge paths such as `a/0/1/6` mean.

## Contributions

This project makes significant use of Claude Code and Claude Opus 5.5, under careful direction and review by the original author.

Its information comes from community sources, original research and decompilation efforts. Only information that can be independently verified or extracted is used, and no third-party code is included.

- The pret projects: pokered, pokeyellow, pokegold, pokecrystal, pokeruby, pokeemerald, pokefirered, pokediamond, pokeplatinum and pokeheartgold
- Community disassemblies of other releases:
  - Narishma-gb (pokegreen, pokeblue, pokeyellow-jp, pokeyellow-fr, pokesilver, pokegold-kr)
  - einstein95 (pokered-fr, pokered-de, pokered-es)
  - Brianum (pokeyellow-de)
  - erosunica (pokecrystal-es)
- PokéAPI, for the Chinese species names and what only Ultra Sun and Ultra Moon add
- Project Pokémon, PKHeX's contributors and SciresM, whose research documented the Gen 6 and 7 saves and Gen 7's save signature.
- Bulbapedia, Serebii.net, and Pokemon Database.
- The author's own cartridges and applications from which the Gen 5, Gen 6 and Gen 7 data is extracted.
  - This includes all english games from owned cartridges and a number of foreign language games across various platforms.
- The author's own older programs and libraries such as PPSE-DS, libspec (old), and PokeLib.

PokéAPI data is used under its BSD 3-Clause licence: Copyright (c) 2013–2023 Paul Hallett and PokéAPI contributors.

libSPEC is not affiliated with or endorsed by Nintendo, Creatures, GAME FREAK or The Pokémon Company. Pokémon and Pokémon character names are trademarks of Nintendo.
