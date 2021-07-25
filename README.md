gfxdata
===

Various helper scripts that compile the sprites in FSROOT/PICS/SPRITES.

To compile the data, run `make LOCAL=1`; this compiles the various configuration and data files in the `data` folder and produces an `out` folder with the compiled data files.
Note that the actual pictures are not included and need to be supplied separately. In
particular, you should create a `data` folder with subfolders `pkmn`, `pkmn-back`, and
`pkmn-icon` containing 96x96, 96x96, and 32x32 sprites of the mons you wish to include;
the files should be named `%hu[f][s][f][_%hhu][f][s][f].png` (as an example, `41sf.png` is the sprite of shiny, female Zubat; `050_1s.png` is the sprite of shiny Alolan Diglett.)
