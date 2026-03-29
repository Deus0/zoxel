### todo ###

Finish Editor:
- Left Align Hierarchy / Inspector Buttons / List
- Keep hierarchy elements selected - active state
- Component Tooltips, e.g.:
	- IconType [byte] [3]
- Component Element for a list (MeshIndicies, etc)
	- using folded button group
	- for now just spawn a multiline text that shows list
- Scrollbar horizontal
- Resizable Windows
	- add corner button to bottom right
	- debug when release + hold system for the dragging
- Texture UI in component list - TextData
- Editor Style function that sets:
	- window_color
	- font_size
	- element_font_color + outline
	- header_font_size
	+ header_font_color + outline
- SoundData play button to test sound

Tools:
- dock hierarchy/window on left of window
	- left drag to edge and release
	- add a button on header to restore from docked
- arrows to fold/unfold children in hierarchy
- tag filter dropdown on header of hierarchy
	- Block, Item, Stat, Etc
	- Just add a list of tags for now to it
- Spawn RenderCamera + Render Texture + Render UI
- Position Y chart - showing fluctuations - use to stabilize physics
- Slider UI to show Text of Number
- Slider UI to snap on integer points
	- visually show points to with little up arrows

Improvements:
- Spawn camera onto headbone
- Update body when picking up head
	- only allow to pickup shoulder if bicep is up
- Set body positions based on item anchor types dynamically
- Combine vox TopHat onto body as well - Equipment layer
- Spawn Hips Body Part onto character
	- Spawn Shoulders Body Parts
	- Spawn Biceps Body Parts
- Center the vox textures for body parts
- Use realm colors for UI
- Spawn Status UI + player model through the UI
- bar at bottom of texture to resize individual component (later)

Bugs:
- Audio missing from main menu - need to spawn a realm for it
- Npcs spawning in lesser chunks - just disable this for now, collision broken there
- Slimes not adapting to block vox depths
- Decor (SDL window edge) missing on khadas - try compile sdl together?
	- test sdl window
	- test wayland window
- fix collisions in lesser resolution terrain chunks
	- test by forcing all terrain chunks at a sub res for easier testing
- fix text label size when resizing - button size
	

Refactors:
- refactor models to just spawn one vox model and multiple render objects as children
- make shape type and centering part of painting as well - use fill system just with diff byte for checks
- Make lods just use the same model, no need to create 5 models per slime
	- just generate per each level - set with shapes per node level
- move sand/wood/stone into biome blocks
- refactor soil/blocks into nodegraphs for models
- link nodegraphs to realm's nodegraphLinks
- list uis should just reposition inside system when children dirty
- chunk = chunk + renders (seperate)
- taskbar data into entities
- settings data into entities
- remove sdl_image and create a seperate image import
- remove sdl_mixer and use another simpler audio lib

Engine [Builds]:
- windows build in zelder, make a bsh/windows.sh
- make builder:
	- build linux
	- build windows
	- build android
	- push to itch io
- move import modules into a new module - which will only be included when building zengine workflow (with another define)
- prefabs should spawn children too

Module [Nodes]:
- create generic variables for our nodes
- our head blueprint needs a l_eye_size r_eye_size
- set these variables on process
- for eye size, its a paint sphere, we can hook a input node up to the fill node

Module [Bodys]:
- disable pickup part if it has attached parts
- if pickup part - refresh body model
- handle color combos by making head a different color
	- uses lookup table when placing
- generate head position from slot position of chest, and slot position of head
- handle increasing depth for > 32^3 max size
- use chest shape data from zoxel for the chest
- Spawn bones from item slot positions
- Body Part Tooltip
	- show PartLinks
	- show AttachLinks

Module [Biomes]:
- Generate unique color per biome
	- base on prior biomes so it stands out too
	- primary color dirt - others based off this
- Refactor more blocks into per biome

Module [Terrain/Regions]:
- region stream position / detect system
- spawn / destroy regions
- region lods
- region town/mountain links

Module [Blocks]:
- when hit block - create entity for block health
- block damage overlay effect

Module [Rendering]:
- test vulkan build
- push voxel data and generate mesh on gpu

Module [Tools]:
- terminal log text list
- chunk debugger - show lods of chunks etc
- character debugger - show number per tunk2D of npcs

Module [Sounds]:
- fix music file not importing anymore

Module [UI]:
- remove use of list window / list ui, just spawn elements into a listview and have system handle the rest

Module [Blocks]:
- seperate block spawning more from the realm,break it into biomes and modules
- remove use of global voxel indexes and use BlockLinks from realm

Module [Chunks]:
- first chunk / tunk to move from controllers to stream begin systems
- seperate Chunk into Chunk + Render
- after chunk3 refactor, use a render per lod, instead of regenerating everytime
- add local chunk lookups to make the terrain chunks be more future proof to dynamic changes in the blocks list

Module [Realms]: (+ games)
- fix end game fade out, really bad atm
- remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho

Module [Terrain/Tunks]:
  - tunks to handle increasing resolution when needed - using the depth update
  - refactor: towns tunk and chunk3 systems into its own module, self contained addons


new
- load ui for a save game
	- window with confirm
	- shows play time
	- delete option w confirm
- town layers walls
- spawn/destroy regions per 32x32 tunks
- spawn slime model from shapes3 data
- use node system
- link to ModelLinks in realm spawn system
- shape data on a node will go through with a vox data to generate sphere for now
- add eyes after using a fill node
- simple print blueprint function that logs a blueprint nodes
- spawn chest and head items and add to body ui
- spawn arm bones
- spawn held item into hand bone (use HandLink and SecondaryHandLink)
- map UI - show tunk heightmap for now
- map icons - and character ones - show player arrow overlay
- block damages, heal, and destroy feature
- terminal ui
- drop item button


tools (this will help fix bugs)
  - press 'f' while raycasting chunk to select it as debug target
    - this will show the 'chunk ui' which shows all chunk info
    - itll have a close button on it
    - Stats
      - Verts
      - States?
      - Idk
      

done:
-x ui off on khadas
-x Generate a Chest Item for the realm bodys
-x generate texture for the chest item
-x create a head model / item
-x Unique Character Tag
-x Link the body parts together
-x combine the two character items into one model for use
-x add position and sizes to the combine list
-x bug: player body still smaller during low res mode
-x bug: hangs atm, due to paint size of eyes
-x bug: low res mode has eye issue, just sets entire head green
-x finish eyes on head of body
-x bug: blockdepth not savings / loading
-x bug: eye painting still not right
-x bug: flower block vox scaled wrong when lower depth
-x play sound when enter grass
-x unstuck should perfectly reposition above nearest ground
	-x atm if you flymode and teleport under, it moves you up randomly + 1
-x option for transparent meshes
-x option for painting weight
-x Sides Generation to use new sides data
-x Slice mesh properly for voxel faces
-x spawn particles when hit grass
-x Grass sound cuts off midway
-x add option for rendering bones
-x Render Part Position / Sizes
-x Spawn Part Position / Sizes from Parts Vox Combining
-X Hierarchy UI for realm and children
-x Inspector UI
	-x fix opening with target entity
	-x fix refreshing buttons like hierarchy system
-x color coding on component types
-x update component text system
	-x watch character position update in real-time
-x debug key to open inspector for player
