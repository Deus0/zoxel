# Bugs

Unsorted
- Body lagging still behind camera
    - Only sometimes?
    - Suddenly, body lagging again
    - pipelines should be fine though
- Big Terrain Disposal Bug
    - terrain very big, and delete, it causes it to hang
    - actually it did end up working
    - also deleting while its spawning, causes crash
    - I think we can just add a destroy tag, and then move destroy to another phase
- top hat doesn't scale up when imported to depth of 5!
- doesnt spawn new chunk mesh ... again
	- test by building upwards generating, the chunk initialization issue
- Lighting flashes when chunks update
	- stop mesh updating until its finished building lights
        
## RareBugs
- Placing twice in a row can crash it still
- Sometimes block item textures dont load for vox items
    - flowers, grass, etc
- new game loaded chunks... check that
    - broke the selections
- Sometimes flying around we see missing faces now in chunks
    - probably didnt trigger neighbors..
- placed block texture stayed in actionbar
- bug - place block, alt tab at same time
- Fix physics clipping - falling through terrain
- Voxel Sides
	- They arnt being culled between lods, perhaps this is a trigger issue
- Im getting neighbor updates not triggering at map edges
- Fall through map
	- fix unstuck - 
	- Make sure collision checks deltas

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

