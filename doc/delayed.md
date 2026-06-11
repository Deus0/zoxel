# Until Zoxel 2030
	
# 3D UI Refactor
- Create a simple Canvas with Elements in each corner
- Use same data but pass through layout3Ds, that use 3D rendering and transform data
- Test will render the canvas in front of camera at different distances and rotations

# Low Priority
- Player Respawn Point Gizmos
- Stream Level - Region only for initial game town spawning!
- Tunk LOD Levels
	- So we dont generate maps for far away chunks
- Update frame texture outline when icon is selected!
- Save/Load Realm Blocks + data to future proof updates
- NPC RaycastRange add ActiveAction to NPCs - ActiveActionRangeSystem
- Long pause breaks physics - test big frame times
- Refactor spawn_block_vox_meta and remove structs
- Add Tooltips to Empty Slot Frames
- Selected Text option - update the Zigel Colors
- Refactor OverlapEvent to Overlapped state with a system in items
- Add instancing to lines so we can test it easier t he lights
- Debug Town Gizmo
- Debug House Gizmo


- Make Glut Build work
	- Glut should spawn a window
	- We should wrap all other sdl functions properly for glut
- Fix No post processing mode / Rawdog Camera Mode
	- add to settings
- tools (this will help fix bugs)
  - press 'f' while raycasting chunk to select it as debug target
    - this will show the 'chunk ui' which shows all chunk info
    - itll have a close button on it
    - Stats
      - Verts
      - States?
      - Idk
# Rendering
- test vulkan build
- push voxel data and generate mesh on gpu
