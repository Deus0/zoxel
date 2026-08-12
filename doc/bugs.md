# Bugs #
- Sometimes crashes still

## Physics
- Fix low frame rates - clip through ground - by checking deltas between intersections

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

