### todo ###

architecture & refactors

engine
  - move import modules into a new module - which will only be included when building zengine workflow (with another define)
  - windows build in zelder, make a bsh/windows.sh
  - fix music file not importing anymore

ui
  - remove use of list window / list ui, just spawn elements into a listview and have system handle the rest

blocks
  - seperate block spawning more from the realm,break it into biomes and modules
  - remove use of global voxel indexes and use BlockLinks from realm

chunks
  - Sides Generation to use new sides data
  - Slice mesh properly for voxel faces
  - first chunk / tunk to move from controllers to stream begin systems
  - seperate Chunk into Chunk + Render
  - after chunk3 refactor, use a render per lod, instead of regenerating everytime
  - add local chunk lookups to make the terrain chunks be more future proof to dynamic changes in the blocks list

realms (+ games)
  - fix end game fade out, really bad atm
  - remove all realm macros and just use state in their own systems (RealmItemsSpawnSystem, ClearSystem, etc) - wait clear can be a macro tho

tunks
  - tunks to handle increasing resolution when needed - using the depth update
  - refactor: towns tunk and chunk3 systems into its own module, self contained addons

nice to haves
  - prefabs should spawn children too

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