### Zoxel ToDo ###
=> GPU Constrained actually, memory barely 
used - 200-400mb used

- Fix chunk bounds debug visuals

- Make close button smaller square with a circle pattern
    - the X is ugly there need icon

- After it works, lets fade between the lod meshes
	- handle interupts so it can fade the other direction again
	- also switch chunk depth to 5 + double scale so its 32x32x32
	
Bug!
- Lesser chunks dissapear
- UVs get messed up when flying around on chunks

Causes
- Maybe it uploads before it's done generating?

Solution:
- Inside Chunk to work on camera instead of player character
- fly around to outer chunks to debug them
- display more data
- render lines over mesh on chunk - as a debug method to show mesh itself - tests whether its mesh issue, or the renderer itself

Lets assume
- Lod Updates
- Spawns Mesh
- Mesh updated
- Somehow not rendering

Setting all chunks to build sides again
- it rebuilds and shows mesh
- then neighbor updates
- then it dissapears

Disabling Lods means:
- Chunk Mesh spawns once
- No need to spawn new mesh links

- Issue must be, sides is created per lod level - so if new sides is made and chunk generates, at wrong depth, it will create these issues?
	- but the data was the same so it doesnt support this - mesh was still there at full depth

- Test Method:
	- fly around until chunk dissapears
	- fly into it
	- note the position
	- check debug logs for system processes
	- didnt seem to be rendering?
10x1x-3

- Issue occurs only when switching LODs
	- spawning a new one didn't appear to work..
	- it disabled last, spawned a new one butt...

-x Change all opengl data to gint and gint2 and guint and guint2
	- this might be issue

- Okay its probaly race conditions with Side updates, mesh updates, etc
- Make a toggle for disabling .. etc
- Sides issues
- Arm swing brokens
- Sometimes chunk didnt update until we destroyed a block from it

- When i spawn new chunk meshes, it breaks the old ones...!
- New Test: Spawns MeshViewer ui based on inside chunk data

- Shrink Game Viewport and put Editor Buttons as icon buttons around side, like gizmos - docked

- For soil vox addition, while it digs down, use a random chance to alter the color?
- Zigel Positions not always set atm for UIs
-x InitializeEntity - make byte from state - removing Increment systems
- when I turn off toggling, it doesnt break
- Also when i disable lods
- It still glitches though, when mesh built, it shows at wrong place
    - only on new chunks though

Regressions:
- Fix item drop from terrain, so it works in same pipeline as others (sync point reduction)
-x LAG - 150 ms before now 50...
- Dissapearing chunk meshes
- Meshes that randomly render
- Mesh flickers off and on when rebuilds
    - it shouldn't need to rebuild only when depth updates
    - Maybe make a swap lod mesh state that waits for the enabled chunk to finish loading
-x Crashes Sometimes
    
    
- Pretty sure loading chunks was removed - test IO
- Landfill not scaling well looks like - tested with low map render distance
-o Lag with transforms due to new meshes?
    -x Disable ChunkMesh and just set Transform + position data
    -x same for chunk
    -o Position - transform updates lagging - add a dirty flag for this

- Fix Loading of settings
	- make load in system
	- if spawning, check if exists or not yet
	- Add state for Settings Load, and Settings Spawn, for Initializing app settings
Current Terrain Chunk Refactor:
- Move spawn gpu stuff to initialize mesh systems
- Frustum should account for sub meshes
- We should set bounds of chunk sepertate to mesh

Next:
- Add extra stage for Sides - ChunkMaterial
- Add second block with lava material for test

Systems
	- GenerateChunk
	- GenerateSides
	- Generate Mesh (children)
	
New Systems:
	- ChunkMeshSpawnSystem
	- Chunk3MeshTrigger2System
	- Chunk3TexturedBuild2System
	- Chunk3Sides2System
	- ChunkMeshSlow2System
	
Hard:
- Refactor Layouts by using LocalPosition2 instead of CanvasLayout
- Fix P hysics collision bug, by choosing collision side based on previous position
- Refactor 3D UI to use layouts
- Refactor Terrain Chunks to use LOD mesh entities instead of singular chunk

- Save Options to disk from a system - OptionsDirty
- Fix arm, needs to not instantly rotate
- Pause NPCs when press escape
- Fix physics bug
- Fix Unstuck system
- Make Camera Z + and remove maps ui FLIP hacks code
- Prioritize Chunks higher lod first
- Also process double if the lod is less, per depth
    - use a point system for counting processing based on depth levels
- Make the bars dissapear when not used
- When chunk updating, create an update group
    - first check neighbrs, if it has an update group
    - if any do, we use that instead
    - When pushing update, just check the group if all chunks on are done
    - This basically keeps them updating in groups without the need to check entire terrains Chunks
- Fix Scale2 working on UIs
- Make a better slow chunk system - perhaps a queue to terrain, when theyre done just remove them from it, to track
- make some npcs attack you
- Queue new chunks - so it doesnt lag
    - TerrainStreamSystem is basically just spawning new chunks
    # Tunks Lod updates happen elsewhere in threads-=
- optimize npcs
    - just spawn closer to player, so not as many for now

# Map
- Zoom in / out buttons / scroll wheel
- Keep track of chunks player has explored - fog of war
- Show percentage explored of Region - name the region
- When you open map, fade it in from player, one piece at a time
- show name label over town on map
- Blend in heights with voxel colors to give more of a mappy feel
- Add a compass symbol to show north etc

-----

- Improve Dirt Model
- Improve Wood Model
- Add Town Dirt Model - more like stone ish, rounded pebbles
- Make compile shell work per module!
- Refactor the sdl mixer into audio - using my own mixer code
- Test WaveFunctionCollapse

# log: 
- Refactor BlocksDirty to non state
- Refactor Statbar3D to use Element2D data, merge the ui code
    - we simply translate the layout2D information into the 3D transform space
- Add WindowTitle property and spawn Header of window in system
    - decouples the header code from spawning
    - keeps spawning purely for data setting!
- Refactor: Tunk Generation should have lodding as it currently lags
    - if still lag at per system limitations
- Map Arrow is backwards, but its pointing right way, when we move right in character, the map ui goes left... So rotation is flipped of map and arrow
- CharacterLoadSystem - Just load character from system using load state
    - streaming should just work seperately to character positioning

Editor:
- Add a Toggle when entity has children in hierarchy, toggle will remove/add their children uis
- Add mask on ScrollArea, so it hides off buttons when they overflow
- Let mouse scrollwheel work on the scrollbar
- Indent the buttons instead of adding "-" to them
- For Edit mode - remove main menu
    - add canvas lines to show
- Dock window - inspector on right, hierarchy on left of canvas

New:
- When Level up + 3 stat points
- When you have stat points, add + button on Attributes
- When attributes go up, increase base stats too!
- When equiping gear, add particles buff around character
- Add dust particles when we jump
- Spawn NPCs based on spawn maps in tunks

Fixes:
- Move NPC dialogue distance back 1 units

Optimize:
- Opt: HeightMapSystem lagging, use LODin g for HeightMaps / Tunk Generation
- Opt: SoundGenerateSystem by using preset sound data + mods
- Opt: Spawn Sound Samples on Realm Start, then use this data in generated sounds - optimized sound generation
- Opt: also add back the color uploads for chunk meshes - MeshColorsDirty
- Opt: ChunkNeighborSystem
    - On Init - add to neighbor directly from the init chunk
    - On Spawn - grab directly there from dictionary on terrain
- Add a queue for spawning new positions, do like 256 checks per frame for terrain for spawning, so its delayed a bit? only if still lagging with new stream systems
- SoundGenerateSystem - uses too much still

Refactor:
- Remove Realm Links components, just use Children
- Make GenerateTexture a byte again - so we set per system
- Ref: Make models just use one vox with multiple Renders Underneath, atm it spawns 5 models and regenerates everytime
- Rft: Make GenerateCharacter states and byte type
- Ref: Remake RenderDepthDirty for TerrainChunks, setting per system instead of zoxc_state
- Finish removning all spawn_block_vox
- Refactor Models to nodegraphs for soil, stone, etc

Regressions:
- Bug: Fix music on Main Menu
- Bug: Fix music loading properly
- [02:12:59] [WARNING] [character3_player_9369]'s Exceeded Capacity [64] [zox_get_children] 
- [02:13:07] [WARNING] [taskbar_icon_4294977265] Invalid GPU Link [0]
- Fix Android Build - on arm pcs
- Fix Slemz eyes - missing

Bugs:
- Bug: Edge of map UI is glitchy
- Bug: Taskbar toggle not deactivated when window closes
- Bug: NPCs can spawn before the chunk is finished, which makes them fall through the chunk
- Bug: The 3D uis are flickering due to depth fighting atm
- Bug: Sometimes when streaming, chunks will flicker randomly
- Bug: Fix Grass placement on outer chunks, it places + 1 but using lower depth so goes too high
- Bug: died holding block, stuck to model?

# Misc
- Tool: Make a region debugger, show the region bounds with a cross through each corner (X) A big X
- Show max system time's graph, in red, overlay over the regular graph
- Bug: lines of selected block should render over destruction overlay
- Bug: Realm Generation: Memory leak somewhere, breaks during VoxGenerationSystem - color spawning - test with valgrind on x86 later
- Bug: Character shakes - smooth the physics - put to sleep when not moving
- Refactor: load files on boot and not per realm

# Towns
- TownLinkSystem for Characters - Log when entering a town
- Towns: Average out heights between chunk neighbors
- Towns: Spawn roads between houses - simple entity, road, start and end points, thickness
- Towns: Place Roads during Maps based on Roads Children of Towns
- Towns: Spawn Highways between towns
- Towns: Set Block Links inside Homes, for Home Floor, Roof, Wall, Door blocks

# Textures
- Refactor texture generation to use nodegraph + nodeprocess
- Add fancier frames to the uis
- Use a MaskTexture in the generation data, use this to apply different materials on frames (mask just black and white array)

# UI
- Add Glyphs for Key/Buttons on UI
- UI: Toast UI when you enter Town
- Add Character Render Texture UI to show your character - mirror taskbar icon too
- Add Logs UI at bottom left
- Status UI
	- Test button first
	- Spawn render texture
	- Spawn render camera facing this
	- Spawn characters mesh object (mesh clone)
- Use realm colors for UI
- Spawn Status UI + player model through the UI
- Click TextureData label to open a texture window
- Slider UI to snap on integer points - when integer - on release
- Add quantity label as child of icon - for mouse pickup too

# Lights
- One when it updates the Render Depth
- Make layout positions recursive too - so it updates
- Lights: Add smooth lights for terrain faces, so the corners are darkened

# Blocks
- Make Grass Taller, 2-3 blocks tall sometimes
- seperate block spawning more from the realm,break it into biomes and modules
- remove use of global voxel indexes and use BlockLinks from realm
- Add Regen to BlockHealths
- Use BlockHealth prefabs with health stat as children
- Vodes: Destroy Block Health when full for >= 3 seconds

# Npcs
- Add humanoids back to npc spawning
- Npcs: Add Monster spawn zones for types - remove around towns

# Bodys
- FIX: Make arms never snap, just lerp the rotation over time
- Animations: Add Sine Scale on chest for breathing animation
- Animations: Sine Bob + rotate the World Items
- Footstep Sounds
- Footstep Prints on ground
- Dust Particles for jumping and landing
- Add standard ground dust particles

# Animations
- Disable Switching Action when one is active
- Global Cooldown When Switching
- When place last block, dont release arm raise until after animation is done
- Base Speed of swing off walk speed and state - WalkSpeed (byte based on max speed)

# Projectiles
- Add Projectiles Damaging Terrain
- Add Projects Damaging NPCs
- Add Color of projectile, based on skill

# Dialogue
- improve dialogue contrast, make the dialogue ui block background
- Aim down when speaking to slime/chicken etc
- Change music track when dialogue starts too
- Fade it in when you get closer to them, like within a chunk distance
- Reduce ai's dialogue distance a little
- Aim head at the npc when talking to them
	- making sure the dialogue is above them
- Black bars to animate from top and bottom when dialogue mode is dirty (remove the fade effect)

# Gizmos
- Gizmos: Selected Block Gizmo - is flickering - only when attached to character, if detatch is stops...
- FIX: Set CubeLines Faces based on VoxelSides thats generated, then make sure it Renders Over Top of the overlays - disabled depth

# Cameras
- Shake camera when hit

# Rendering
- Lines: Line Instancing
- PostProcessing: Add a Post Processor Noise option - Slider - we can reduce it or turn off
- PostProcessing: Add a Post Processor Vignette option - Slider - we can reduce it or turn off
- FIX: Create a new Unstuck system
- Smooth Lighting for terrain lights - smoothed edges

# Generation
- Use Realm Seed + region positions to set Region Seeds
- Use Region Seeds to generate mountain dat
- Fix the flickering of chunks when I move through terrain
- Windows no longer close taskbar toggle
- Physics clip issue now that my body is bigger than one block size
- base biome color on prior biomes so it stands out too

# Regions
- destroy regions
- Region lods
- region town/mountain links
- spawn straight highways between region towns

# Emojis
- Add little vox model above npc if you can talk to them - little cube that animates
	- Animate it more when talking to the npc

# Saves
- SaveLoad Items - using new Saves Module for better workflow
- Add Statistics Overlay
	- SHow counts for many things
- Remove Debug options and just put in Gizmos
	- Transparent Meshes
	- Render Bones
	- Paint Bone Weights
	- Disable NPCs
- SaveTransformSystem using States
- Make Road from middle of town to gate
- Make Walls Thicker
- Make Town Gates more overhanggy
- Debug missing town wall parts, cuttoff, do minimaps next
- Fix OOB errors
- Fix chunk mesh reload glitch (flashes, basically missing for one frame)

# Inputs
- Button Mapping
	- Add Button Mapping Module
	- so the player systems use those instead

# Stats
- Add Stats panel to the taskbar and give header / window
- Vitality should increase health stat
- Add base stat "Physical Damage" that punch uses, strength should increase this

# Skills
- NPC - Raycast Range should be Skill Range
	
# Items
- Drop item button
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
- Maximize a window - button on map header
- Resize window grabber at corner
- move sand/wood/stone into biome blocks

# Placing
- Show a gizmo where block would be if holding block - 3D cube gizmo
- Animate particles fade in when placing over time
- Lock in place position from when clicked
	- Slow rotation of camera during lock

# Tunks
- Tunks to use Quadtrees and LODing

# Huds
- Seperate Pause UI from other Game UIs (Taskbar etc)
- Pause UI to use blacked out screen, game ui to just use blur
- Sometimes namelabel didnt load on npcs
- Pause ui toggle not showing on spawn taskbar + pause
- Give game uis the realm colors so feels different to the main menu

# Realms
- fix end game fade out, really bad atm
- remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho
- Shows play time on Load Game
- delete option Confirm Screen
- use node system

# Game
- pause should pause the npcs too, cherrie wants dat

# Unsorted
- if our body is overlapping chunk edges, we shouldnt update the chunk streaming, wait until its not overlapping
- Set UI hightlight color of buttons, and highlight outline colors
	- have it lerp there over time
- consider prompts for inputs when character is less than a day old
- Flag if chunk has file, then progressively load the contents per LOD level
- Clean: Remove the OOB warnings in octree, print the stack maybe?
- Rendering: Make it easier to add a custom material / shader
	- dynamically gets/adds components per shader file?
- We can even raycast check against chunks to further reduce whats rendered...? - fade them in and out?
- Add dictionary to chunk data, so it maps the blocks to the block ids, and then make the block ids generate from realm ids so it stays consistent after changes
- Physics Clip issue for big body
- Use a HighlightColor - instead of just adjusting brightness
- Event for pickup we can latch onto in system
	- trigger the body dirty if body item picked up
- spawn item model as child of character - when switch action
- Make layout positions recursive too - so it updates in a single frame
- Make all ui shaders use transforms like render texture does
- Handle player death by removing camera on death state
- F6 key to toggle bone render debugs
- Arrows to fold/unfold children in hierarchy
- Click MeshIndicies or MeshVertices label to open a Mesh Window

# Tools
- Add a delete all savegames to the test window
- Render Lines at chunk when action happens
	- using an entity called ChunkBeamDebugger
	- One when it updates the Voxels
	- One when it updates the Lighting
	- One when it updates the Render Depth
- Click SoundData to play the sound

# Gamepads
- Add GameMenu RB and LB to switch between UIs
- Add haptic feedback rumble when hit
- Navigation on UIs should also auto scroll down on the options menu

# Refactors
- Refactor: Stream Terrain Chunks from terrain, not chunks, just spawn there
	- Keep setting the lods in chunk systems though as can be multithreaded

# Engine
- Fix music importer
- Fix import workflow - make import call
- Engine: Compile modules seperately into .o and then combine, during debug builds - developing
- Look into compiling parts modularly so it compiles faster - important for development speed

# Android
- Android Build - Fix on x86 machine
- Add auto jump option - for touch

# Tests
- Add test function for spawning a Terrain Chunk

# Experimental
- Vox Frames - different vox models we swap between
- We could have MegaChunks again, 4x4 chunks, and then cull them first with the Frustum + bounds checks (this is just a quicker sweep)

# Bugs
- Bug: Sometimes map arrow is off - the rotation is wrong...
- BUG: Crashed on start a few times randomly.. then stopped
- BUG: Mesh didn't show on NPC randomly
- FIX: Main issue with collisions is how it decides on which face we collided with, using a distance check, so you can teleport a little if its the wrong face, best to use velocity to determine face, last position
- Collisions: When going underneath a block, itll detect the collision of DOWN face first before the proper face - Use velocity to decide face? - can calculate last position from velocity

# Notes
+ I removed render_depth_invisible, we are just going to set depth to 0 from now on or destroy the chunk - or we can disable but it should be seperate from depth variable
+ Remove more VoxLink use if its from chunk/tunk/region / use parents
