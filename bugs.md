# Bugs #
- Segfault has been introduced when i added hat
	- occasionally breaks during games load now
	- its breaking when body rebuilds
	- disable body rebuilding and see if crashes
	- issue is it only sometimes breaks
	- memory leak somewhere
	- now it only crashes when not in gdb when re-enter game
- Higher Resolution mode is bugged for player model
	- skinning issues
	- max length chunk is 128, increase length..
	- put depth to 5
- Item in hand dissapears when updating body

- At start a terrain chunk lights didn't load
- Fix dungeon core texture issues
- Fix DungeonCore not showing texture properly
- Audio missing from main menu - need to spawn a realm for it
- Fix music file not importing from LMM
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
