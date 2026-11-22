# Imports

What the game loads from `res/` at boot, and how to add your own.

## Voxes (`res/voxes`)

MagicaVoxel `.vox` files, in any folder under `res/voxes` (`characters`, `gear`, `hats`, `blocks`). Each is known by
its file name without the extension (`res/voxes/gear/top_hat.vox` is `top_hat`), so names must be unique across
the folders. Only the first model in a file is read (`src/models/io/files.c`).

- **Size** sets its depth: the smallest octree that holds its longest side (16 voxels is depth 4, 32 is depth 5).
- **Depth it was made at**: a hat or body part is sampled into a body at its own depth, so one made at depth 4 is half
  the size of a depth 5 one on a Block Res 5 body. A `.txt` with the same name beside the `.vox` says the depth it
  was made at, and it's scaled up to depth 5 (each voxel a 2x2x2 cube), so it fits at every Block Res:

  ```
  depth 4
  ```

  (`-depth 4` works too.) `top_hat`, `cavalier_hat` and `bowler_hat` have one; `hatty` is made at depth 5.
- **Colours** come from the file's palette.
- Tests: `bash bsh/tst.sh vox_import`, and `bash bsh/model.sh <name>` to look at a realm model.
