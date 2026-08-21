# Refactoring

## Macro Use
- Remove zox_sys2 and use regular function names
- just call zox_sys_begin that will add timings there

## Events
- Move sound systems to events too
    - PickupSoundSystem
    - LandSoundSystem
- Remove state systems and just use event components with listeners

## Unsorted
- SDL should be a module again
    - it will just add onto the App prefab and events
    - This makes it easier for Glut to be implemented or others 
- Rename Cubes - Geometry
- Move Bodys and Equipment to Parts with tags
-x Make Core its own module, and _ will just be helpers, flecs macros, data structs
- Move Texture tag from rendering to Textures
    - Just wrap gpu stuff in graphics, then do it all in textures before our rendering
- Rename Rendering: Geometry, used for meshes
- Rename LayoutSize as PixelSize, as well as ScreenSize, add to Core (generic af)
- Refactor settings spawning back to hooks
- Make Initialize a tag - dont fragment
    - remove at end of frame
- Make ChunkEntities use links instead of components
- Remove DeviceLink from zevices, use parent
- Remove Camera Datas - main_cameras ui_cameras
- Remove DeviceLink from Zevice, use parent
- Spawn finger entity during finger down - instead of just setting
- We can refactor all the links between enttiies as pairs instead of components

## Devops
- We need docker libraries to be seperated from regular ones
    - for docker.sh - add docker tag in linux.sh
    - then make libs use seperate folder

## Tooltips
- Refactor Tooltips as events
    - Make the event store multiple functions
    # Probably the tooltips can all just be events - systems arnt needed for these

## Apps
- Move sdl/glut/glfw/headless into core as wrappers or not idk... we havnt actually wrapped yet!

## Data
- Make DeviceDisabled a tag - Generic Disabled
- Remove build states when not building (60ms)
    - ChunkMeshColorsTriggerSystem
    - ChunkTexturedBuildSystem
    - SmoothLightsBuildSystem
    - BuildMesh

# Refactors
- Refactor: Stream Terrain Chunks from terrain, not chunks, just spawn there
	- Keep setting the lods in chunk systems though as can be multithreaded

## Nodes
- Add color hsv limits like the size later to the color node
- Color node should set a reference name
    - fill can use this instead of hard values

## UI
- Refactor taskbar as entities and add the Mirror UI there
    - so its easier to add the new mirror button

## Skinning
- Add BoneOctree
- Set Bone values when we place items into body
- Convert Bone values into weights instead of using sizes
	
## ChunkTextured Refactor
- Spawn a Terrain chunk as test function
- Spawn a TerrainChunk with no Mesh and child Renderer
	- fix systems to work with new terrain chunk

## Chunks
- seperate Chunk into Chunk + Render
- after chunk3 refactor, use a render per lod, instead of regenerating everytime
- add local chunk lookups for block indexes
	- Prevents Updates messing up Chunk Save Data

## Chunk Refactor
- Fade in Chunks
- Spawn Hierarchy + inspector when we spawn our Vox Test
- Spawn the Vox test without renderer, and use child as render
	- refactor the mesh building functions for this use
- Switching LOD Meshes should
	- fade in and out the switch
	- when fadeout completely they get disabled
	- when fading in, they get enabled (entity enabling)
- Refactor Chunk to Chunk + BlockMaterial + Lod Render
- chunk = chunk + renders (seperate)
- Generate sides per Chunk3Material (sub entity of Chunk3)
- Spawn Chunk3Render entity per LOD level when RenderDepth set
- Refactor models to just spawn one vox model and multiple render objects as children
	- Make lods just use the same model, no need to create 5 models per slime
	- just generate per each level - set with shapes per node level
	
## Extras

- Refactor Settings to Entities
- Move Damage outside of MeleeSystem and add a DamageQueue onto the terrain chunk
- Change Settings / Options UI as entities data
- Change Taskbar as entities data
- Move TexturesSize + TexturesDirty to Textures module from Rendering
- Remove any Hook use and just use system states
- Reduce systems total

## Refactors
- Remove refactor Vodes spawn code
- Replace Chunk VoxLinks with Parent calls
- Refactor Model Lods -> one model, just create mesh per lod
- Refactor 3D UIs to use 2D UI stuff
- Remove struct use from spawn_block_vox_meta
- Remove Duplicate UI prefabs
- Remove structs from UI prefab use
- Remove spawn_window_users use
- Refactor Header Spawning to system
- Remove all spawn_window_users's
- Spawn character data from biomes
- Compare raycasts based on priority
	- Raycast Solid Blocks, Character, Non Solid Blocks (Grass)
- make shape type and centering part of painting as well - use fill system just with diff byte for checks
- refactor soil/blocks into nodegraphs for models
- link nodegraphs to realm's nodegraphLinks
- list uis should just reposition inside system when children dirty
- taskbar data into entities
- settings data into entities
- remove sdl_image and use BMP imports
- remove sdl_mixer and use another simpler audio lib
- Refactor texture generation to blueprints
- upgrade the texture for the item frames
- Terrain loading and finishing should be overseen by GameState and not Player events
- Move character/block name generation to system under new module Names
- Remove any random delays, and make the state changes timed instead
- Add a tooltip that just stores a string - no need for event every time
