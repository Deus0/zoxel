### Zoxel ToDo ###
=> GPU Constrained actually, memory barely used - 200-400mb used

- Make mountains more stone, less dirty
- Make Towns less Grassy, more Rough Dirt + dirt
	- Make Road from middle of town to gate
- Make Walls Thicker
- Make Town Gates more overhanggy
- Aim down when speaking to slime/chicken etc
- Debug missing town wall parts, cuttoff, do minimaps next
-x Make Towns not close to Region Borders
- Fix OOB errors
- Fix chunk mesh reload glitch (flashes, basically missing for one frame)

# Next
- Use Realm Seed + region positions to set Region Seeds
- Use Region Seeds to generate mountain dat
- Add Character Render Texture UI to show your character - mirror taskbar icon too
- Fix the flickering of chunks when I move through terrain
- Windows no longer close taskbar toggle
- Physics clip issue now that my body is bigger than one block size

# Chunk Refactor
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
	
# ChunkTextured Refactor
- Spawn a Terrain chunk as test function
- Spawn a TerrainChunk with no Mesh and child Renderer
	- fix systems to work with new terrain chunk

# Next
- Arm Swing when attacking
- Add humanoids back to npc spawning
- improve dialogue contrast, make the dialogue ui block background
- spawn town based on region + town data
- spawn player in town
- block health quick cube overlay mesh
- pause should pause the npcs too, cherrie wants dat
- if our body is overlapping chunk edges, we shouldnt update the chunk streaming, wait until its not overlapping
- Set UI hightlight color of buttons, and highlight outline colors
	- have it lerp there over time
- consider prompts for inputs when character is less than a day old
- Disable next button until animating dialogue finishes
- Flag if chunk has file, then progressively load the contents per LOD level

# Refactors
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

# Inputs
- Button Mapping
	- Add Button Mapping Module
	- so the player systems use those instead
- Game UI - LB / RB should cycle through taskbar uis
- Navigation on UIs should also auto scroll down on the options menu

# Tests
- Add test function for spawning a Terrain Chunk

## Game

# World Blocks
- Blocks to heal over time
- Simple Block Overlay Effect - alpha from health

# Character
- Status UI
	- Test button first
	- Spawn render texture
	- Spawn render camera facing this
	- Spawn characters mesh object (mesh clone)

# Combat
- Shake camera when hit
- Add haptic feedback rumble when hit

# Stats
- Add Stats panel to the taskbar and give header / window
- Vitality should increase health stat
- Add base stat "Physical Damage" that punch uses, strength should increase this

# Skills #
- Raycast Range should be Skill Range
	- Change when skill is equiped - using actions
	
# Items
- Spawn Vox model items in hand
- Spawn Vox model items in world (flowers drop)
- Add Cookie Item
- Right click to split items
- Write a test function to spawn a world item to pickup

# Body
- Body / Equip Tooltips
	- Add Slot Names to Body Parts + Equipment in tooltips
	- Add tooltip of slot type over empty slots
- Show unuseable slots as grayed out or something
- Check body slot type before placing body items
- Add Cylinder Shape for Fill Nodes
- Add Neck Node for Headblueprint
- Use Cylinder for Bicep / Forearm Items
- Retype ChunkSize as byte3
- Add Neck to head + chest blueprints
	- cylinder shape at bottom+top
- Designate a hand bone
	- Spawn Held Item onto handbone
- Footstep Sounds when walking around
- Head bob to move faster when walking
- use chest shape data from zoxel for the chest
- handle color combos by making head a different color
	- uses lookup table when placing
- Body Part Tooltip
	- show PartLinks
	- show AttachLinks

# Crafting
- Fix Crafting and use Slots for them
- Output Craft Slot, pickup only
- Pickup item to change after any of the other slots change, using recipe data to match

# Pets
- Spawn mr penguin on screen
- Give the ability to change his hat
- Make a penguin module
- Adds a penguin to taskbar
- Adds a penguin render scene and 3 buttons to change his hat
- remove any use of user macros from user data
	- just reuse their prefabs and add tags
- use functions like spawn_window, or spawn_user_grid inside those uis
- Spawn label top left above stats - show block / npc selected
- Generate a mouse texture - arrow
- Refactor Texture Generation into Nodegraphs
- Maximize a window - button on map header
- Resize window grabber at corner
- move sand/wood/stone into biome blocks

# Placing
- Show a gizmo where block would be if holding block - 3D cube gizmo
- Animate particles fade in when placing over time
- Lock in place position from when clicked
	- Slow rotation of camera during lock

## Worlds

# Blocks
- Make Grass Taller, 2-3 blocks tall sometimes
- Refactor Model Spawning out of block_vox_meta functions
- seperate block spawning more from the realm,break it into biomes and modules
- remove use of global voxel indexes and use BlockLinks from realm

# Regions
- destroy regions
- region lods
- region town/mountain links
- spawn straight highways between region towns

# Biomes
- base biome color on prior biomes so it stands out too

# Towns

# Tunks
  - tunks to handle increasing resolution when needed - using the depth update
  - refactor: towns tunk and chunk3 systems into its own module, self contained addons

## UI

# Huds
- Seperate Pause UI from Game Menu
- Pause UI to use blacked out screen, game ui to just use blur
- Sometimes namelabel didnt load on npcs
- Pause ui toggle not showing on spawn taskbar + pause
- Give game uis the realm colors so feels different to the main menu

# Editor #
- Terminal UI
- test key for keyboard navigation mode

# Unsorted #
- Vox Frames - different vox models we swap between
- Use a HighlightColor - instead of just adjusting brightness
- add quantity label as child of icon - for mouse pickup too
- event for pickup we can latch onto in system
	- trigger the body dirty if body item picked up
- spawn item model as child of character - when switch action
- Render Lines at chunk when action happens
	- using an entity called ChunkBeamDebugger
	- One when it updates the Voxels
	- One when it updates the Lighting
	- One when it updates the Render Depth
- add a delete all savegames to the test window
- Make layout positions recursive too - so it updates in a single frame
- Make all ui shaders use transforms like render texture does
	- need to refactor the shaders and element render system
- Handle player death by removing camera on death state
- F6 key to toggle bone render debugs
- Use realm colors for UI
- Spawn Status UI + player model through the UI
- Click TextureData label to open a texture window
- Click SoundData to play the sound
- Arrows to fold/unfold children in hierarchy
- Click MeshIndicies or MeshVertices label to open a Mesh Window
- Slider UI to show Text of Number
- Slider UI to snap on integer points - when integer - on release
- Fix music importer
- Fix import workflow - make import call

# Position Debugger
- Debug Component for Position Curve (float3 curve)
- Position Y chart - showing fluctuations - use to stabilize physics
- Position atm shakes when grounded
- If unstuck disabled, it will fall through ground when loading

# Editor Delayed
- TextureData UI in Inspector Label
- Scrollbar horizontal
- bar at bottom of texture to resize individual component
- Component Element for a list (MeshIndicies, etc)
	- using folded button group
- Resizable Windows
	- add corner button to bottom right
	- debug when release + hold system for the dragging
- Editor Style function that sets:
	- window_color
	- font_size
	- element_font_color + outline
	- header_font_size
	+ header_font_color + outline
- dock hierarchy/window on left of window
	- left drag to edge and release
	- add a button on header to restore from docked
- tag filter dropdown on header of hierarchy
	- Block, Item, Stat, Etc
	- Just add a list of tags for now to it
- Slider UI - visually show points to with little up arrows
Engine [Builds]:
- windows build in zelder, make a bsh/windows.sh
- make builder:
	- build android
	- push to itch io
- move import modules into a new module - which will only be included when building zengine workflow (with another define)
- prefabs should spawn children too
Module [Nodes]:
- create generic variables for our nodes
- our head blueprint needs a l_eye_size r_eye_size
- set these variables on process
- for eye size, its a paint sphere, we can hook a input node up to the fill node


# Tools #
- terminal log text list
- chunk debugger - show lods of chunks etc
- character debugger - show number per tunk2D of npcs

# Chunks
- seperate Chunk into Chunk + Render
- after chunk3 refactor, use a render per lod, instead of regenerating everytime
- add local chunk lookups for block indexes
	- Prevents Updates messing up Chunk Save Data

# Realms
- fix end game fade out, really bad atm
- remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho
- Shows play time on Load Game
- delete option Confirm Screen
- use node system

# Nodes
- Nodegraph UI - show a node graph full of nodes and connections
- simple print blueprint function that logs a blueprint nodes

# Maps
- map UI - show tunk heightmap for now
- map icons - and character ones - show player arrow overlay
- drop item button
