# Editor: zox as a general purpose engine

The goal: zox usable as an engine for games other than zoxel, like godot, with an editor to inspect and later edit
them. The foundations exist (flecs, modules, ui, `src/editor`'s hierarchy, inspector and profiler, `gam/zedit`).
This is a checklist of features, then a detailed plan per feature with notes as each is started.

Order: inspection flows first, perfect them, then editing, then the engine wide changes.

## Checklist

### 1. Inspection (first)
- [x] File window: browse `res/` and the save folders and inspect file metadata
- [x] Foldable hierarchy: arrows to fold and unfold children, indented rows instead of "-"
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
- [ ] Play controls in `gam/zedit`: a play / pause toggle button (pauses the realm's clock, like the game's pause)
  and a stop button (ends the play, back to the editor's scene), icon buttons at the top of the viewport

### 4. Engine
- [ ] Better io: loading and saving resources and data, tested (doc/tech/saves.md)
- [ ] Modularity: modules compiled separately (doc/plans/modularity.md)
- [ ] Refactored games: each game in `gam/` picks its modules, engine core vs kits (voxel, rpg)
- [ ] Reloading modules from core: rebuild a module and reload it without restarting
- [ ] Node graph ui for nodes and blueprints

## Plans

Detailed notes per item go here as each starts.

### File window
Press `n` to toggle Files on the debug player’s canvas. The window opens on the resources and game save roots.
It lists one folder at a time, with `..` first, then folders and files sorted by name within each group.
Click a file to add an info row with its name, byte size and extension. File viewers are a later task.
Folders that exceed the shared list capacity show a `More entries...` row.

The list rebuilds on core `Dirty` after a deferred spawn or a click. The `file_window` test checks deferred
spawning, row order, folder navigation and file metadata using a temporary folder.

### Foldable hierarchy (first)
Other tools call it a tree view: godot's Tree (a TreeItem is collapsed or not), unity's foldout, imgui's TreeNode,
qt's TreeView (expand / collapse), apple's disclosure triangle. Here: a **fold arrow** on a hierarchy row.

How it looks: each row that has children starts with an arrow, right (folded) or down (unfolded); a row without
children has the same space empty, so names line up. Clicking the arrow folds or unfolds; clicking the name still
inspects it.

The hierarchy list holds UI's `Unfolded` array and the root stays open. The hierarchy walks open branches and
marks rows with children. Each fold arrow clones one of two triangle textures generated at startup. Arrow clicks
toggle the row target in the list's array and add core `Dirty` to the list. The hierarchy observer rebuilds the
rows and keeps the scroll position within the available range.

The `hierarchy_fold` test checks arrows on the root and parent, leaf rows with no arrow, and the
two-to-four-to-two row sequence through the arrow click handlers. It also checks scroll preservation and errors.

Later: the same fold arrow for the inspector's component list, and for the file window's folders, so it should be
a ui element (ui/elements2), not hierarchy code.

### Docked windows and the game view (after the hierarchy)
- Hierarchy docked left, inspector right; the game's viewport shrinks to the space between (its camera's screen size,
  not the window's), and grows back when they close.
