# Bugs
- Part of the [todo](todo.md), done items go in [done.md](done.md). Tests: `bash bsh/tst.sh list`


- healthbar is wrong orderin stats panel
- top hat doesn't scale up when imported to depth of 5!
	- add a top_hat.txt that has a -depth 4 value
	- importer can then know to scale it up
- cant talk / raycast to npcs when no action selected
	- give raycast range for no actions

## Crashes
- Big Terrain Disposal Bug
	- set render distance to 32
	- terrain very big, and delete, it causes it to hang
	- actually it did end up working
	- also deleting while its spawning, causes crash
	- I think we can just add a destroy tag, and then move destroy to another phase initialization issue

## Terrain & Chunks
- Terrain Chunk faces missing when streaming
	- update neighbors directly from the queue
	- just rebuild their mesh, no need for voxelnodeupdate etc
	- When streaming terrain - flag neighbor of updated chunks dirty
		- the outside chunks have missing faces now
		- move a little around chunks
		- look at LOD horizon and notice chunk faces can be missing
	- Sometimes flying around we see missing faces now in chunks
		- probably didnt trigger neighbors..
	- Im getting neighbor updates not triggering at map edges
- Voxel Sides
	- They arnt being culled between lods, perhaps this is a trigger issue

## Lighting
- corners of light still off (needs check solids)
- when moving between chunks can throw lights off
	- i think due to light at lower depth is more sunny
- Fix light face on bigger lods, needs tojust pick brightest if at a lower division
- At start, terrain lights don't update in some chunks
- Smooth Lights Error: Colors Past Limits [n] (logged during terrain load, seen in most tst runs)
	- [17:32:51.562] [ERROR] Smooth Lights Error: Colors Past Limits [1200] - Face [5] at Pos [12x8x13] Depth [4] of [4]

## Characters
- Item in hand dissapears when updating body
- Skeleton pose resets when we rebuild skeleton
	- it should be more dynamic when regrowing its pose / bones
- Redo skinning, has overlapping issues if parts close atm
	- Cellular automata voxels around bone joints for better rotation
	- use octree data for bone indexes, so we can remember what part we placed the voxels from, then we can use this when building up the weights
	- we can probably blend them better between as well

## Physics
- Fix little teleport when loading player character

## UI
- Tooltips: If ui selects while didnt deselect..!
	- select main menu header after button selects
	- the tooltip updates to wrong one - shows load game when you  mouse move fast between uis
	- it can show nothing selected even though log shows it set, teh issue therefor must be that it gets set and unset in same frame
- Line2Ds are broken, need to add to new 2D render stack that sorts layers
- fix text label size when resizing - button size
- set the slider text block depth [5] when sliding
- 16 printable ascii have no glyphs, i.e. ' (dialogue "I'm" shows ?), needs font art (tst text_data)

## Audio
-x Main Menu music missing (spawn_menu_music, on the game, crossfades with the realm's)
-x Fix music importer (make convert, tst music_file)
-x Music load error on linux (the note count was read as a size_t, tst music_file)
-x Sound effects quiet in game (too low for small speakers, tst sound_balance)
-x Quest givers say "You did well..." without giving a quest (NodeLink was exclusive, tst quest_dialogue)

## Rendering
- Cube mesh doesnt restore on alt tab - gpu restore issue?
- camera position off - can see through walls?
- Fix the destruction material
	- use depth level and keep at same position as voxel

## Blocks
- SoilGrass color at bottom wrong

## Inputs
- New gamepads turned on mid game - dont get added

## Apps
- Fix viewport not resizing
- make run should check gam directory too for changes

### Web
- no save games
- no stats
- npcs missing
- Dev version  stuck on load game

### Android
- Touch UI not showing up in game
	- Fix touchscreen touch ui [player_state_touch_ui]
	- device mode not set if not spawn
- NOTE: Zoxel android release working now, was issue  with no safety checks Crash

### Steam Deck
- Gamepad navigation faulty neighbors
	- Implement position checking for any interactable items in canvas tree

### Windows
- sdl3 window mode - it dissapears
	- windowed mode bug

### Linux (Khadas)
- Decor (SDL window edge) missing on khadas - try compile sdl together?
	- test sdl window
	- test wayland window

## Features
- Add a sphere collider under npc
	- detect nearby characters
	- event useful when spawning/despawning name labels or markers
