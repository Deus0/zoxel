# Editor: zox as a general purpose engine

The goal: zox usable as an engine for games other than zoxel, like godot, with an editor to inspect and later edit
them. The foundations exist (flecs, modules, ui, `src/editor`'s hierarchy, inspector and profiler, `gam/zedit`).
This is a checklist of features, then a detailed plan per feature with notes as each is started.

Order: inspection flows first, perfect them, then editing, then the engine wide changes.

## Checklist

### 1. Inspection (first)
- [ ] File window: browse `res/` and the save folders, a list of folders and files, open a file to inspect it
- [ ] Foldable hierarchy: arrows to fold and unfold children, indented rows instead of "-"
- [ ] Hierarchy scrollbar fixed, scroll area masks rows that overflow
- [ ] Tag filter on the hierarchy (blocks, items, stats..)
- [ ] Inspector: data components readable (TextureData, mesh data open their own window)
- [ ] Close buttons on every editor window
- [ ] Editor style: high contrast text, one style function for colours and font sizes

### 2. Windows and docks
- [ ] Resizable windows (corner drag)
- [ ] UI docks: hierarchy left, inspector right, the game viewport shrinks between them
- [ ] Detachable windows: a window dragged out becomes its own SDL window

### 3. Editing
- [ ] Edit component values in the inspector (numbers, colours, bools)
- [ ] Gizmos: selection outline, move and rotate handles, picking with raycasts
- [ ] Scenes: how they work with procedural generation is open (save the seed and generators' settings, plus edits?)
- [ ] Play and stop in `gam/zedit`

### 4. Engine
- [ ] Better io: loading and saving resources and data, tested (doc/tech/saves.md)
- [ ] Modularity: modules compiled separately (doc/plans/modularity.md)
- [ ] Refactored games: each game in `gam/` picks its modules, engine core vs kits (voxel, rpg)
- [ ] Reloading modules from core: rebuild a module and reload it without restarting
- [ ] Node graph ui for nodes and blueprints

## Plans

Detailed notes per item go here as each starts.

### File window
(todo)

### Foldable hierarchy (first)
Other tools call it a tree view: godot's Tree (a TreeItem is collapsed or not), unity's foldout, imgui's TreeNode,
qt's TreeView (expand / collapse), apple's disclosure triangle. Here: a **fold arrow** on a hierarchy row.

How it looks: each row that has children starts with an arrow, right (folded) or down (unfolded); a row without
children has the same space empty, so names line up. Clicking the arrow folds or unfolds; clicking the name still
inspects it.

What it needs:
1. Fold state, per hierarchy window: the entities unfolded, a list on the window (two hierarchies can differ, and
   game entities get no tags, no new flecs tables). The root starts unfolded, everything else folded.
   The rows are cleared and respawned on every refresh, so the state can't live on a row.
2. `get_hierarchy_rows` only walks an entity's children when it's unfolded, and marks which rows have children.
   Folded by default also fixes the 128 row cut off for big trees.
3. Two arrow textures, generated once (textures/gen, like icon.c's): a right and a down triangle, white so the
   button's colour tints them. Shared by every row, not one per row.
4. The arrow is an icon button (spawn_button_icon) to the left of the row's name, a child of the row, placed by its
   ListIndent. Its ClickEvent flips its row's Target in the window's list and adds HierarchyUIDirty.
5. Keep the scroll position on a refresh from a fold (refresh_editor_list sets ListStart back to 0 now).
6. Test (editor module): a parent with two children, hierarchy open: one row, unfold: three rows, fold: one row,
   the arrow only on the parent, no errors.

Later: the same fold arrow for the inspector's component list, and for the file window's folders, so it should be
a ui element (ui/elements2), not hierarchy code.

### Docked windows and the game view (after the hierarchy)
- Hierarchy docked left, inspector right; the game's viewport shrinks to the space between (its camera's screen size,
  not the window's), and grows back when they close.
