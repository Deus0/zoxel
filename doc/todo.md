# Todo

- Refactor all 'SettingsSystem)' into events on prefab_app
- doesnt spawn new chunk mesh ... again
	- test by building upwards generating, the chunk initialization issue
- Refactor Restore/Dispose into Query Functions
- Refactor Settings to an event - InitializeEvent on App
- Refactor ([in] players.PlayerState,) with new PlayerStateEvent listener
- Fix Depth Invalid issues - when i added queue
	- perhaps its due to lod setting earlier
- Make chunk datas more self contained
    - dont use terraindepth, just use maxdepth property

Refactor Events
	- LayoutPositionDirty
	- LayoutSizeDirty
	- TextDirty
	- Refactor RenderDisabled to DF Tag

UIs
- Centralize different uis
    - atm theyre a bit spread through different state events
    - For example screen fader is in 3 events
- Make tooltip an event instead of systems
- Remove old links
	- CanvasLink (mostly done)
	- CharacterLink
	- CameraLink
	- EntityLinks
	- BlockLinks
	- BiomeLinks
	- ModelLinks (half done)
	- CharacterLinks
	- PlaylistLinks
	- Remove clear systems realm_clear_system
- Remove old structs
	- CanvasData
	- LayoutParentData
	- ElementSpawnData

Input Refactor
- Make input actions events instead of systems
    - Pause key - PlayerPauseSystem

Character Refactors
- Refactor character mesh out of character - as child
- Add character CombatState

-x Add listener component on Game
    -x GameStateEvent
    -x Passes in game event with function
- Add loading screen event
- Move the key events out of the test functions
    - just add to a KeyListener component

- Refactor Events to tags
	- GenerateTexture
	- RenderLodDirty
- Refactor GenerateChunk to Generate, Generating, etc
	- GenerateModel
- Fix destruction material
- Add quest on npc - 5% chance
- When remove body part
	- remove the associated item slots
	- refresh the body ui
    
- Add links back for active mesh, fuck the rules

- Streaming gets 20ms spikes atm...
	- debug with mangohud

- if theres multiple streamers with different levels
    - idk... why this breaks for new game
    - oh i think it uses position for has, we should remove previous position if level is higher then
    - we clear atm which is fine for now
- Remove state components and just use tags + dontfrag
- Refactor our timings
    - spawn a profile entity (dataset)
- GenerateChunk not running when increasing depths
- Landfill LOD issues
	- When loading map?
	- missmatch of tunk / chunk
- Add VoxelNodeLock for Octree Safety
	- Crashed on load...!
- Write test to spawn realm
- Write test to spawn streamer + terrain
- Test to spawn query and print the tables

## Regression again
- Lower FPS, investigate the chunk meshes tables
    - we can see by debugging pipelines

Target: 230fps at the current position
- Make MeshDirty + and other bytes tags
- keep testing against old builds

## After
- add min, max to fps display
- We should not have restore/dispose systems
    - just use query
    - as systems cache their results per frame

Maybe we just keep refactoring stuff
- fix prior refactor bugs...!
    - issue is initialize needs time to process
    - i tried spawn at start of ppipeline but it breaks

Did
- VoxelDirty made into tag
- used links for chunks active mesh instead of children
- Also lag spikes, huge
- Now... chunk meshses lag the game
    - disable meshes - normal fps
    - enabled meshes - 40 fps... ffujckc
- Lod Increase - Generation isnt triggering
    - probably state not detected
- placing crashes / freezes it

## Custom Phases
- Make custom phases
    - input
    - state
    - physics
        - forces
        - apply
        - collision
    - transforms
    - cameras
    - rendering
- remove pre/post loops and just use systems as functions

## --------------------
## TOP PRIORITY
    - Glitchy Ass Terrain Meshes
        - Fix Chunks from mesh updating before their neighbors do
            - use Ready systems and timers
        - Fix Chunks Lights flashing when updating
    - Loading screen now that we've slowed systems down
        - Add events for game state
        - Add loading screen spawn/destroy on those
    
## --------------------

- Link chunk to active mesh
- fix docker build and test onsteamdeck
    - uses new pathing
        
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


- Profiler should show exact frames
    - a second graph over top can show max system times
- Calculate the camera transform before uploading to shader
    - atm it calculates in the vert shader, not good
- I really should just link player to a canvas 3D - then i can spawn/remove from that normally
- Why datagrid?? why not simply ui_grid??
- Optimize the npc spawning
- Optimize the different block generation
	- lags on realm load (for my arm pc)
- Fix logs in build system, should become alot cleaner

## Next
- Spawn 2 XR Eye entities that are cameras
    - XRRender Systems for them
- Seperate character depth from block depth in settings
- Refactor ListPositionDirty and ListSizeDirty into tags
- remove StatLinks just use children and queries

- Fix overlay of destruction materials for block destruction
- Fix taskbar linking
- Make lines in plotgraph use a set value of the graph
    - set the graph based on max value in that frame, but use multiple curves
    - display frame time behind, and system time in front
- Add ui scale to settings
- Im getting neighbor updates not triggering at map edges
- Fall through map - fix unstuck - or make sure collision checks deltas

## Important Refactor
- Why is player state 'playing' when i exit game..
- Player State Refactor
    - Remove just use disables etc
- Game States
    - Controller all players centrally from game states
    - single state machine for games
    - Remove GameStateTarget
    - Use a listener event for modules
- game state where is thou
- do 2D uis get restored? missing any 2D vert data?
- Remove the mandatory arguments on ./bsh/android.sh
- Make sure to tag docker libs properly as release
- When low on damage - add more downscale on the camera
    - make it a camera property too
- Vox item textures... broken again... only sometimes
- just a simple component macro that creates an expanding number ofevents
    - just arrayd for now?
- Add event listeners that can handle multiple events
    - use this for game state
    - modules can add too prefab_game's state change listener
    - use this for special stuffs
    - use this for options
    - can massively reduce systems
- Refactor Punch mechanic as just the way damage is added
- Make a AOE skill that npc can use when its low health
    - adds particles
- Flying in disabled chunk - disabled due to no voxels - causes player to be disabled
- if npc is being baited, by the player keep running around them
    - they can run away
- Mouse scrollwheel broken for switching items
- start game with 3 skills, so you can try the combat styles of the skill tree
- When place above chunk - no mesh spawns
- Add particles when you land
- Add damage and sound if you land strongly
- Add red screen overlay when you are hit
- Test Dialogue function
    - animate scale for each new character 0->1.2->1 (3 frames)
    - remove the glitchy first letter - probably due to it spawning
- Dialogue Generation
    - Greetings
    - based sentence gen on their personality types
    - give each npc a personality type
- When return to main menu - set camera back to origin
- Spawn load text / ui during fade out / game event
    - just show loading for now
- Add loading screen
    - with cancel button
    - show loading chunks count
    
## Polish
- Add ui scale onto options
- Slider UI isnt accounted for in list ui
- Refactor the taskbar button to window using links
    - Fix taskbars again properly linking to windows
- Fix flickering issue of raycast
    - maybe we should use a fixed rotation integral
- Move map UI as player moves
    - mask the map ui
    - or just disable when goes over edge for now
- Add Version of build date to main menu bottom right
- stats panel didnt spawn when respawn
    
## Gameplay
- Each character type - spawn dialogue there

## KeyboardMouse
- We need a target window for player, whichis the last selected one

## Gamepads
- Tooltip should just be at top right of screen when finger is off ui
- add controller vibration support

## Touchscreen
- Add auto jump option - for touchscreen + option to disable
- Fix post procesing on android
- make jump button use - ClickingEvent instead of ClickEvent
    - so we can hold jump button
    - Make button activate while holding - on clicked rather than first click

## New
- Raycast x 4, for player, and compare results
    - so we dont get shuffling between cubes when camera shakes
    - like squarecast
- Check when spawning if we are inside blocks

## Whimsy
- Add cracked bricks, a weaker version of bricks, 1 / 8 chance to spawn on wall
- Why cant i touch and drag two windows at once???
- Make a mushroom we can throw on ground - does disease damage once it hits an npc
- Create jingle, a bunch of sounds that play in sequence, can be spawned from events like sounds, when a new ui opens etc
- Stress test mode - add a multiplying slime that dies fast



## Game UI
- Redo Realm Select UI
    - add date in tooltip
    - Sort latest to top
    - Add Realm Names as labels instead of seed

## Android
- Terrain Material doesnt restore on minimize + restore
- Post Processing Broken
- Screen Rotation event not working

## Windows
- SDL3Mixer not compiling for windows

## Mirror UI
- Make it so camera can filter an object itself, instead of just the object filtering camera?
    - Make a renderer clone that just copies another entities mesh.. somehow
- Destroy Render Camera when the ui dies - just parent it?

## Body
- arm doesnt swing when not selected

## Weather
- Fix the fog, make look nicer

## Projectiles
- Projectiles get destroyed on hit characters (use same system as pickups)

## Models
- Node (Runner) Process Entity should shift the seed its using
- Add SetSizeNode with min max properties
    - min max get shifted depth to the depth of the model
    - this lets us use same blueprint for all slimes
    - This way we wont need float3 ratio in the model spawning too
- Add paint detail - clamp options - for tiny details
- Once its done, get ai to spawn chickens and cookies
- add spawn spots node that adds some variation

## Cookies
- Refactor Model Blueprints to vary size based on seed - set size node
- Refactor the model to generate all chunk meshes
    - refactor vox color builder to use parent

Later
- Crosshair will flicker siize too fast
- Animate Text test
- Note: Atm the material is the same on all render textures
    - we can just create a child of the ui as material if we need unique values
        
Bugs
- Destruction Material Flickering - increase depth until i fix it
- List ui calculating wrong cause sliders different heights then buttons!
- Fix Skinning issues of humanoids at max depth
- Fix unstuck system and test it with no clip

### Refactoring ###
- Move particle and bone rendering systems into rendering module
- Remove crafting users reference and use normal item ui for craft ui
- Refactor Chunk Render systems back into Rendering module
- Replace Generate components with core one
- Refactor Colored Vox to use same ChunkSides system
- Convert Vox Model to use ChunkMeshes
- remove GenerateTexture after use
- Replace Generate components (GenerateTexture, GenerateRealm, GenerateTunk)
- Add GenerateProgress, and add to it when generate state begins, remove when ends
- Redo all my destruction hooks like MeshGPULink - remove DTOR for OBSR
- Refactor Character - Vox model can be underneath them
- Refactor Vox Model so meshes can be underneath them
- Move chunk render back to rendering module, and just link to material
	- keep all rendering stuff there

Polish
- When dialogue animates - first fontt will flash...!
- Smooth the camera or sleep the physics cause it flickers the lighting
- make voxel item textures 2 pixels larger, one is outline, one is air
- Make map move around player position
- Fix timing of arm swing
- item ++ shhould occur after it reaches you (the world item)
- Add loading screen

# Note we need to use pipelines to isolate reading from voxels / writing to

- Make a easy way to add hotkey functionality
	- zox_on_key_pressed(key_z, test_thingo);

Quick Wins
- set particle grass color to that of grass itself - block color
- arm should only slerp rotation - no setting
- save player character slot data
	- items, skills, etc whatever is there
- load player character slot data
- Add chicken head item
- Vox Items - hold chicken head in hands
- 10% chance for Boney npc to have a quest to give
	- change dialogue if no quest
- simple kill slime quest 
	- hand in once you killed 10

Fix:
- Sometimes humanoids dissapear (when talkin go them?)
- aura particles are broken, they only spawn in same spot now
- PNG errors - convert rest to bmp
- 'Sides is true with Air'
- 'child_index_to_text_array_index: index ' warning
- 'OOB [set_octree_value] [0x2x0] depth [1] vlength [2]' warnings
- Glitchy depth fighting on 3D UIs, and on Block Destruction Materials
- Smooth out camera at low fps - its very glitchy
- skybox doesnt follow to main menu?
- "Terrain Chunk Mesh Builder: Sides is true with Air" error
- "OOB [set_octree_value] " error with vox building
- PNG errors
- Eventually we need multi chunk characters, cause their updates lag
	- max depth 4-5 for mesh build times
- Top of world bugs
	- blocks placed on top of world cause no face to show
	- slime npcs arnt hitting back
		- they cannot raycast me on top of the world at 16 height
		- npc cant raycast me
			- only small ones
			- when im far away from origin
			- super weird
	
- add destruction particles
- debug npc voxel positions - porcupine style
- particles for skeleton death
- bleed effect

Atm:
- time punch animation when first click?
	- warmup of skill first
	- then punch begins
	- punch swings
	- punch hits
	- punch winds back
	- skill coolsdown
- Animate ui for punch, overlay

Fix:
- The errors about ChunkSides
- Set editor text contrast alot higher, black and white
- Make raycast from fixed position, not off moving head
- Fix the layering of the 3D UIs, text and such has depth issues
- Fix npcs falling through map

New:
- Add slight directional light over the AO on colored voxes

Refactor:
- Big one is making Vox + terrain use same ChunkSides code
	- make vox use chunksides
	- refactor vox mesh outside of vox itself
	- Seperate out UVs generation from Faces, so they both use same Mesh Building
- Make the chunk colors use same system to build mesh
    - we can just add uvs in for textured system
- MeshDirty should set for diff data - bitfield - Mesh + UVs + Colors etc
    - and keep set in systems that upload them
    - keep them all modular
    - keep it all centralized in rendering so we dont have to do the same work everywhere

- Spawn/Despawn npc labels when they enter near player
- SkeletonMesh accidently dissapeared whoops, check changing states
- debug label - the font will dissapear sometimes and reappear
	- Looks like a memory issue causing the mashes to dissapear? when streaming
- sort load games by latest played
- Add lookouts around map - just big ole scary towers
    - make system so all new region placers will a void prior ones
- Add different rarity on items and frame them differently
- Make homes built with different materials / Blocks
- Add strange names for blocks
    - just show label as Soil type of block

Refactors
- Make 3D ui use layouts
    - test the canvas
    - layout to local position
    - Position2 on prefab stack gets shifted to Position3
    - TextSpawnSystem just uses a ZigelPrefabLink instead, handling both types

Gameplay
- Add countdown timer on respawn
- Add a timer for new items to be picked up
- Spawn NPC - Humanoid -Test
- Give npc in town a quest
- Give option of dialogue to give user a quest
- Quest accept UI shows between dialogue
- Escape key should exit dialogue
- Finish converting icons to bmp
- Add mirror UI for viewing player model

Fixes
- Add delays to queue so its frame timing independent 
- if building a tower up - shows invisible face
    - Its when its next to a chunk with no chunk meshes - it doesnt build the edge
- Jumps should work independent of time delta
- Third Person Camera a bit buggy
- Fix Vegation not building at same scale
- Npcs fall through ground durin load..
- Fix jumps working same at diff time deltas
    - physics, we just need to give it the same accelaration juice
- Fix Lighting Seems between Chunks, only sometimes??
- Wait is grass missing now??
    - it didnt spawn in time
- Sometimes still crashes - fix that
- Font sometimes scrambled on ui
- Chunk Mesh failed deactivating... ? Keep eye on them
- Npcs be falling through ground
- Text font just scrambled positions again - maybe it repositions before child index is set
- Occasional Crashes
- Map UI slightly off still - the flipping part
    - wait i think its just lods updating stuff
    - Vegetation seems off, when moving between LODs

Optimize
- ChunkTexturedBuildSystem - 28ms idle??
    - should we be removing states after?
- Maybe we create entities for events, this will save the idle times?
- Remove DeactivateDelay on chunk meshes and build new system for this

Crashes:
- crashes when destroying grass - only in run
- put more entity component checks everywhere
- crashed randommly on block place

Ideas
- Add chunks on edge that are Simulation only? for map? idk
- Create a biomemap at the region level that helps coordinate

- Generated slime colors should be the same every time loads
- Begin animating arm when attack starts
	- atm it is delayed until mid attack
- Can we give each chunk a seed
	- then use seed for spawning npcs so it spawns the same every time

Fixes (older bugs)

Opptimize
- ChunkFrustumSystem [20ms]
- ChunkColoredBuildSystem [
- Test Pipeline sync points before i finish

- Add a loading screen - for bigger loads
- for ui we should keep a Size2 property - real float - this is for the mesh resizing

Improve
- Chunk meshes should only be spawning if materials per side > 0
    - Ignore spawn mesh if air or if no visible edges

New:
- Spawn / Respawn point should be in a Home - facing outward
- Add Terrain Depth to options - debug options
- Draw X inside chunk after it builds sides - to debug  this
- Make close button smaller square with a circle pattern
    - the X is ugly there need icon
- Add material + system for Fading chunks in
	- Tag then Remove tag after faded for TransparentRendering
- After it works, lets fade between the lod meshes
	- handle interupts so it can fade the other direction again
- also switch chunk depth to 5 + double scale so its 32x32x32
- Create a fullscreen ui scroll view, render textures, of spinning vox models
	- each one will be a part of a population
	- population breeds based on rules
	- the final population becomes the biome blocks

-o Test entity table issues by adding tag to chunk im in and seeing if it breaks their rendering... weird but might be why meshes dissapearing
- Fix chunk bounds debug visuals - the lines debugger
- for some reason when it lods lod again it will rebuild mesh

	
- Sometimes chunk didnt update until we destroyed a block from it

- When i spawn new chunk meshes, it breaks the old ones...!
- New Test: Spawns MeshViewer ui based on inside chunk data

- Shrink Game Viewport and put Editor Buttons as icon buttons around side, like gizmos - docked
- For soil vox addition, while it digs down, use a random chance to alter the color?
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

## Textures
- Refactor texture generation to use nodegraph + nodeprocess
- Add fancier frames to the uis
- Use a MaskTexture in the generation data, use this to apply different materials on frames (mask just black and white array)

## UI
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

## Lights
- One when it updates the Render Depth
- Make layout positions recursive too - so it updates
- Lights: Add smooth lights for terrain faces, so the corners are darkened

## Blocks
- Make Grass Taller, 2-3 blocks tall sometimes
- seperate block spawning more from the realm,break it into biomes and modules
- remove use of global voxel indexes and use BlockLinks from realm
- Add Regen to BlockHealths
- Use BlockHealth prefabs with health stat as children
- Vodes: Destroy Block Health when full for >= 3 seconds

## Npcs
- Add humanoids back to npc spawning
- Npcs: Add Monster spawn zones for types - remove around towns

## Bodys
- FIX: Make arms never snap, just lerp the rotation over time
- Animations: Add Sine Scale on chest for breathing animation
- Animations: Sine Bob + rotate the World Items
- Footstep Sounds
- Footstep Prints on ground
- Dust Particles for jumping and landing
- Add standard ground dust particles

## Animations
- Disable Switching Action when one is active
- Global Cooldown When Switching
- When place last block, dont release arm raise until after animation is done
- Base Speed of swing off walk speed and state - WalkSpeed (byte based on max speed)

## Projectiles
- Add Projectiles Damaging Terrain
- Add Projects Damaging NPCs
- Add Color of projectile, based on skill

## Dialogue
- improve dialogue contrast, make the dialogue ui block background
- Aim down when speaking to slime/chicken etc
- Change music track when dialogue starts too
- Fade it in when you get closer to them, like within a chunk distance
- Reduce ai's dialogue distance a little
- Aim head at the npc when talking to them
	- making sure the dialogue is above them
- Black bars to animate from top and bottom when dialogue mode is dirty (remove the fade effect)

## Gizmos
- Gizmos: Selected Block Gizmo - is flickering - only when attached to character, if detatch is stops...
- FIX: Set CubeLines Faces based on VoxelSides thats generated, then make sure it Renders Over Top of the overlays - disabled depth

## Cameras
- Shake camera when hit

## Rendering
- Lines: Line Instancing
- PostProcessing: Add a Post Processor Noise option - Slider - we can reduce it or turn off
- PostProcessing: Add a Post Processor Vignette option - Slider - we can reduce it or turn off
- FIX: Create a new Unstuck system
- Smooth Lighting for terrain lights - smoothed edges

## Generation
- Use Realm Seed + region positions to set Region Seeds
- Use Region Seeds to generate mountain dat
- Fix the flickering of chunks when I move through terrain
- Windows no longer close taskbar toggle
- Physics clip issue now that my body is bigger than one block size
- base biome color on prior biomes so it stands out too

## Regions
- destroy regions
- Region lods
- region town/mountain links
- spawn straight highways between region towns

## Emojis
- Add little vox model above npc if you can talk to them - little cube that animates
	- Animate it more when talking to the npc

## Saves
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

## Inputs
- Button Mapping
	- Add Button Mapping Module
	- so the player systems use those instead

## Stats
- Add Stats panel to the taskbar and give header / window
- Vitality should increase health stat
- Add base stat "Physical Damage" that punch uses, strength should increase this

## Skills
- NPC - Raycast Range should be Skill Range
	
## Items
- Drop item button
- Spawn Vox model items in hand
- Spawn Vox model items in world (flowers drop)
- Add Cookie Item
- Right click to split items
- Write a test function to spawn a world item to pickup

## Body
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

## Crafting
- Fix Crafting and use Slots for them
- Output Craft Slot, pickup only
- Pickup item to change after any of the other slots change, using recipe data to match

## Pets
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

## Placing
- Show a gizmo where block would be if holding block - 3D cube gizmo
- Animate particles fade in when placing over time
- Lock in place position from when clicked
	- Slow rotation of camera during lock

## Tunks
- Tunks to use Quadtrees and LODing

## Huds
- Seperate Pause UI from other Game UIs (Taskbar etc)
- Pause UI to use blacked out screen, game ui to just use blur
- Sometimes namelabel didnt load on npcs
- Pause ui toggle not showing on spawn taskbar + pause
- Give game uis the realm colors so feels different to the main menu

## Realms
- fix end game fade out, really bad atm
- remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho
- Shows play time on Load Game
- delete option Confirm Screen
- use node system

## Game
- pause should pause the npcs too, cherrie wants dat

## Unsorted
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

# Engine
- Fix import workflow - make import call
- Engine: Compile modules seperately into .o and then combine, during debug builds - developing
- Look into compiling parts modularly so it compiles faster - important for development speed

# Tests
- Add test function for spawning a Terrain Chunk

# Experimental
- Vox Frames - different vox models we swap between
- We could have MegaChunks again, 4x4 chunks, and then cull them first with the Frustum + bounds checks (this is just a quicker sweep)

# Notes
+ I removed render_depth_invisible, we are just going to set depth to 0 from now on or destroy the chunk - or we can disable but it should be seperate from depth variable
+ Remove more VoxLink use if its from chunk/tunk/region / use parents
