# Optimizations


## --------------------
## TOP PRIORITY
    - Glitchy Ass Terrain Meshes
        - Fix Chunks from mesh updating before their neighbors do
            - use Ready systems and timers
        - Fix Chunks Lights flashing when updating
## --------------------

## Regression again

## Fix Lag Spikes

-> Use the flecs profiler to make sure chunk components are reduced properly
-> Focus on terrain chunks for now

- We going to need to make two systems for voxel dirty and for when lod updates, to spawn new meshes - just use same iterator and pass it in

- Remove components when done:
    -x ChunkLodDirty
        - ChunkMeshToggleSystem
        - VodesSpawnSystem
        - ChunkMeshSpawnSystem
    - VoxelNodeDirty
        - VodesSpawnSystem

- More Components (to remove)
    - VoxelNodeDirty
    - BuildChunkSides
    - GenerateModel
    - FindNeighbors

- Remove systems when idle, by removing components
    - Chunks
        - ChunkTexturedBuildSystem [6ms]
        - ChunkMeshToggleSystem [6ms]
        - VodesSpawnSystem [2ms]
    - Lighting
        - ChunkMeshColorsTriggerSystem [9ms]
        - SmoothLightsBuildSystem [8ms]
- The next system in timing is Camera3RenderSystem [2ms]

+ Chunk3TexturedRenderSystem (11ms) + ElementRenderSystem (3ms)
- Can we hide behind mountains to improve culling?
- Streaming has a spike, make sure to display the biggest system spike per graph

- Refactor the shaders and element render system
- Refactor Block Model Spawning out of block_vox_meta functions
- Refactor Texture Generation into Nodegraphs
