# Bugs #

- Fix light face on bigger lods, needs tojust pick brightest if at a lower division

# Regressions #
- now that body is bigger, we are seeing more physics glitching
- make run should check gam directory too for changes
- Higher Resolution mode is bugged for player model
	- skinning issues
	- max length chunk is 128, increase length..
	- put depth to 5
- Music load error on linux (somehow loads on windows)
- Music Missing from main menu as no realm then
# Others #
- Item in hand dissapears when updating body
- Redo skinning, has overlapping issues if parts close atm 
	- Cellular automata voxels around bone joints for better rotation
	- use octree data for bone indexes, so we can remember what part we placed the voxels from, then we can use this when building up the weights
	- we can probably blend them better between as well
- At start, terrain lights don't update in some chunks
- Fix dungeon core texture issues
- Fix DungeonCore not showing texture properly
- Audio missing from main menu - need to spawn a realm for it
- Fix music file not importing from LMM
- Decor (SDL window edge) missing on khadas - try compile sdl together?
	- test sdl window
	- test wayland window
- fix text label size when resizing - button size
Polish:
- Fix little teleport when loading player character
- set the slider text block depth [5] when sliding

# Fixed #
-x Fix set_octree_value Warnings
-x Fix Raycast Error: Invalid Vox warning
-x Sometimes npcs spawn in same chunk twice
-x Sometimes character missing a mesh too
-x Characters sometimes small - due to block depth
-x Characters sometimes fall through map
-x Segfault has been introduced when i added hat
	- occasionally breaks during games load now
	- its breaking when body rebuilds
	- disable body rebuilding and see if crashes
	- issue is it only sometimes breaks
	- memory leak somewhere
	- now it only crashes when not in gdb when re-enter game
-x Crashes atm if any other model swaps the chest - when shoulder is added
