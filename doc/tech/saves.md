# Saves

How a game is saved and loaded: the folders, each file's layout, and what writes and reads it. Zoxel is in alpha:
save formats change without keeping older saves loading. Every format has an io test.

## Folders

- Saves root: `~/.local/share/zoxel` (Linux), `%USERPROFILE%\AppData\Local\zoxel` (Windows), the app's files dir
  (Android). Tests use `/tmp/zoxel_nosaves_<pid>` (`--nosaves`, src/saves/fun/terminal.c), deleted after the run.
- One folder a realm, named by its seed: `<root>/zoxel/<seed>/`. Its path is the realm's `FolderPath`.
- `~/.config/zoxel/settings.cfg`: the player's settings (src/_/settings), not part of a realm.

## A realm's files

| File | What | Written | Read |
|---|---|---|---|
| `realm.txt` | seed, name, seconds played, last played | a new realm, loading it, ending a game | the load menu, loading |
| `blocks.txt` | each block index's key (name and seed), a line each | after the realm's blocks generate | the same, to remap chunks |
| `chunk_<x>_<y>_<z>.dat` | a chunk's voxels and block data | edited chunks, on save | the chunk streaming in |
| `player.dat` | the player's character: position, rotation | when it moves (its hash changes) | a continued game |

### realm.txt

Plain text, a `key value` line each (src/saves/fun/realm_info.c, `RealmInfo`):

```
seed 2105344050
name Zokamel
played 3600
last 1763200000
```

- `played` adds the time since entering or loading the realm when a game ends (`realm_info_played`).
- The load menu lists realms by `last`, newest first, its name on the button, its seed and time played in its
  tooltip.
- tst: realm_info, menu_flow (the typed name saved).

### blocks.txt

A line an index: `<name> <seed>`. Voxels store their block's index in the realm's `BlockLinks`; when blocks are
added or reordered the indices move, so a realm compares its saved table with its blocks and rewrites its saved
chunks through a remap (src/chunks/chunks3/io/sys/block_table.c). A block gone from the realm loads as air.
tst: block_table, block_table_remap.

### chunk files

Binary (src/chunks/chunks3/io): the octree, then sections.

```
octree    each node, depth first: value (1 byte), has children (1 byte), then its 8 children if it has
sections  zero or more, until the file ends:
          1: id (1 byte), size (2 bytes), then size bytes
          2: id (1 byte), then an octree as above
```

- Section 1, block data: the chunk's `BlockDatas` buffer as it is in memory: entries of position (3 bytes), length
  (1 byte), data (1 to 255 bytes). For the few voxels that need data: a bed's or door's direction (its base), a
  filler's offset back to its base, later a door's state or a sign's text. A larger thing (a chest's items) would
  keep an id here and its own file.
- Section 2, what's switched on: the chunk's `ActiveOctree` (a door open, a gravity cube on), as the voxels' octree.
- A chunk with no block data, or nothing on, writes no section for it: its file is its octree only.
- An unknown section id stops the read (a warning), so new sections can follow.
- Only edited chunks save (`Edited`, chunk3_save_system); unedited ones regenerate from the realm's seed.
- An edited chunk is written when a voxel changes (`ChunkUpdate`), or when only its block data changes (a door
  toggled: `SaveDirty`, chunk3_save_dirty_system, written once).
- Block data is written with its voxel (`VoxelNodeUpdate.data`, voxel_update_queue_system), under the chunk's
  octree lock, and cleared when the voxel changes.
- tst: chunk_io (octrees), block_datas_io (sections, sizes), block_data_queue (in game: set, saved, loaded, cleared),
  doors and door_save (what's on, through a file).

### player.dat

A raw `SaveDataCharacter`: position (3 floats) and euler rotation (3 floats), rounded to 0.01
(src/characters/characters3/sys/saving.c). Items, stats and quests aren't saved yet (todo.md).

## Sizes

- realm.txt: a few dozen bytes. blocks.txt: a line a block (~50 blocks).
- A chunk: 2 bytes a node; a flat or empty chunk is a few bytes, a busy one a few kb. Block data adds 3 bytes and
  4 + length an entry, only for chunks that have any.
