# Bugs #
- Sometimes crashes still

## Rendering
- Cube mesh doesnt restore on alt tab

## Lighting
- corners of light still off (needs check solids)
- when moving between chunks can throw lights off
    - i think due to light at lower depth is more sunny
-x Lighting node at top suddenly dark - only when sunlight and reduce are on too
-x When the light octree increases scale, it doesnt get blacked out? this causes bigger lights to lightup house sides

## Blocks
- SoilGrass color at bottom wrong

## Inputs
- New gamepads turned on mid game - dont get added

## Physics
- Fix low frame rates - clip through ground - by checking deltas between intersections

## UI
- Line2Ds are broken, need to add to new 2D render stack that sorts layers

## Next
- cant talk / raycast npcs when no action selected
- Fix body (body parts) icon didnt generate
    - check events
- Fix music importer

- Fix light face on bigger lods, needs tojust pick brightest if at a lower division
- make run should check gam directory too for changes
- Music load error on linux (somehow loads on windows)
- Music Missing from main menu as no realm then
- Fix the destruction material
    - use depth level and keep at same position as voxel
- Crashed on start a few times randomly.. then stopped

## Apps
- Fix viewport not resizing

## Unsorted
- Item in hand dissapears when updating body
- Redo skinning, has overlapping issues if parts close atm 
	- Cellular automata voxels around bone joints for better rotation
	- use octree data for bone indexes, so we can remember what part we placed the voxels from, then we can use this when building up the weights
	- we can probably blend them better between as well
- At start, terrain lights don't update in some chunks
- Audio missing from main menu - need to spawn a realm for it
- Fix music file not importing from LMM
- Decor (SDL window edge) missing on khadas - try compile sdl together?
	- test sdl window
	- test wayland window
- fix text label size when resizing - button size
Polish:
- Fix little teleport when loading player character
- set the slider text block depth [5] when sliding

