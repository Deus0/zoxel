# Plan: the last dirty pairs

The six `zoxc_dirty(T)` pairs left (`TDirty`, `TUpdate`, 2 remover systems each), audited 2026-11-21 by the rules in
[tech/states.md](../tech/states.md): fewer tags, systems and observers together; light reactions observers on a
non fragmenting tag; heavy or bulk ones systems (on workers), a fragmenting pair kept for them.

## The audit

| Pair | Writers | Readers | Verdict |
|---|---|---|---|
| LayoutPosition | 21 | none on Update (its one term is commented out); grid.c checks Dirty on children | **remove** |
| Grid | 1 (spawn) | grid_system (69 lines, needs sizes settled: an observer broke navigator_inventory) | **fold into LayoutSize** |
| InsideBlock | 1 | 2 observers, inside_block_sound_system (main thread, spawns a sound) | **one nf tag, observers** |
| LayoutSize | 16 | 8 systems: anchors, layout and 3D meshes, render texture and texture sizes | keep: heavy, in bulk |
| ListPosition | 5 | list_system (74 lines), list_render_dirty_system | keep: layout work |
| Skeleton | 2 | head_animate, bone_index_generate, place_camera_head (characters stream in bulk) | keep |

## 1. LayoutPosition: remove the pair

- 21 places add `LayoutPositionDirty` (drags, the mouse ui, tooltips, map markers each frame, lists, grids, text):
  each add moves the entity's table twice, for nothing reading it but grid.c.
- grid.c skips children without it (`!zox_has(child, LayoutPositionDirty)`): check what that guards (only newly
  laid out children?) and replace it with what it means, or drop it.
- Then delete the 21 adds, the tags and their 2 systems. Positions still flow: the layout systems read
  `LayoutPosition` every frame.
- Saves: 2 tags, 2 systems, 21 lines; no table moves on every position change.
- Tests: navigator_grid, navigator_inventory, scroll_wheel, tooltips, tooltip_flick, map markers (coop_map_markers),
  window drag, game_log, menu_flow.

## 2. Grid: fold into LayoutSize

- `GridDirty` is added once, at spawn; a grid needs laying out when it's made or resized, which `LayoutSizeDirty`
  already says. grid_system matches `[none] layouts.LayoutSizeUpdate, [in] containers.GridSize` instead, its spawn
  adds `LayoutSizeDirty`.
- Saves: 2 tags, 2 systems. Risk: a grid resized lays out again (it should anyway).
- Tests: navigator_inventory, navigator_grid, vox_mesh_grid, item_split.

## 3. InsideBlock: one non fragmenting tag

- Its readers are light (a sound, a splash, a callback): `inside_block_sound_system` becomes an observer, the tag
  non fragmenting, a cleanup observer removes it; `InsideBlockUpdate` and the 2 removers go.
- `terrain_callback_inside_block` reads Update (a frame after): check it doesn't need the frame (the collision
  settled), else it reads Dirty with the others.
- Saves: 1 tag; 3 systems out, 2 observers in (the sound reader, the cleanup): 1 fewer together.
- Tests: water_splash / swim tests, footsteps, inside block tests, player_spawn.

## Kept

LayoutSize, ListPosition, Skeleton: their readers are system work that runs in bulk (ui layouts and meshes, many
characters streaming), the case where a fragmenting pair and systems win (flecs_event_scaling).

## Totals, if all three land

| | now | after |
|---|---|---|
| pairs | 6 | 3 |
| tags | 12 | 7 (3 pairs, 1 nf tag) |
| systems + observers | 566 + 96 = 662 | ~559 + 97 = ~656 |
| table moves on a position change | 2 | 0 |

Measured before and after as in states.md's refactor log (lint_systems, pipeline_syncs, the tests above' own time).
Order: 1 (the biggest win, least risk), then 2, then 3; a commit and the tests each.
