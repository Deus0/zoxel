# Bugs

## Next
- Let body parts meshes generate
	- so we can loot them!
- noticed the item label didnt reduce when placing item
- Add a sphere collider under npc
	- detect nearby characters
	- event useful when spawning/despawning name labels or markers
- healthbar is wrong orderin stats panel
- Add ? and ! markers on the map
	- ? new quest
	- ! hand in quest
- Add x marker on map for enemies
- Quest tracker ui at top right
- top hat doesn't scale up when imported to depth of 5!
- cant talk / raycast npcs when no action selected
- Sometimes the npcs labels dont spawn - name labels
- Main Menu music missing
- Terrain Chunk faces missing when streaming
	- update neighbors directly from the queue
	- just rebuild their mesh, no need for voxelnodeupdate etc
-o Map UI doesnt initialize properly
- doesnt spawn new chunk mesh ... again
	- test by building upwards generating, the chunk
- Fall through map
	- fix unstuck - 
	- Make sure collision checks deltas

## WEB
    - no save games
    - no stats
    - npcs missing
    - Dev version  stuck on load game

## ANDROID
   - Touch UI not showing up in game
	- Fix touchscreen touch ui [player_state_touch_ui]
    	- device mode not set if not spawn
   
## STEAM DECK
   - Gamepad navigation faulty neighbors
	- Implement position checking for any interactable items in canvas tree
   
## WINDOWS
   - sdl3 window mode - it dissapears
   	- windowed mode bug
	
## Unsorted
- camera position off - can see through walls?
- When streaming terrain - flag neighbor of updated chunks dirty
    - the outside chunks have missing faces now
    - move a little around chunks
    - look at LOD horizon and notice chunk faces can be missing
-x vox item textures did not generate
    -x pickup grass
    -x notice no texture
- Block didnt update when spawning on top of world
- Big Terrain Disposal Bug
    - set render distance to 32
    - terrain very big, and delete, it causes it to hang
    - actually it did end up working
    - also deleting while its spawning, causes crash
    - I think we can just add a destroy tag, and then move destroy to another phase initialization issue
- Lighting flashes when chunks update
	- stop mesh updating until its finished building lights
- Sometimes flying around we see missing faces now in chunks
    - probably didnt trigger neighbors..

## Characters
- npcs not despawning now
    - changed recently the states
-o Fix - Npcs not spawning atm
    - maybe the ordering i did change that
- Item in hand dissapears when updating body

## Rendering
- Cube mesh doesnt restore on alt tab

## Lighting
- corners of light still off (needs check solids)
- when moving between chunks can throw lights off
    - i think due to light at lower depth is more sunny
- we shouldn't update mesh when terrain updates until lighting does
    - atm it flashes white during updates

## Blocks
- SoilGrass color at bottom wrong

## Inputs
- New gamepads turned on mid game - dont get added

## Physics
- Fix low frame rates - clip through ground - by checking deltas between intersections
- Integral test on falling through map
	- fly up
	- fall
	- lag spike
	- wham into ground

## UI
- Line2Ds are broken, need to add to new 2D render stack that sorts layers

## Next
-x Fix body (body parts) icon didnt generate
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
	
## RareBugs
- Add VoxelNodeLock for Octree Safety
	- Crashed on load...!
- Placing twice in a row can crash it still
-x Sometimes block item textures dont load for vox items
    - flowers, grass, etc
-x new game loaded chunks... check that
    - broke the selections
-x placed block texture stayed in actionbar
- bug - place block, alt tab at same time
- Voxel Sides
	- They arnt being culled between lods, perhaps this is a trigger issue
- Im getting neighbor updates not triggering at map edges

# NOTE: Zoxel android release working now, was issue  with no safety checks Crash
- Fix lagspikes in [chunk_sides_system] > 200ms
    - 200+ ms with locks..
    - 80ms without locks..
    - just make it generate at depth we are rendering instead of per level
        - flag levels as dirty when building, just dont update them until render lod changes
        
