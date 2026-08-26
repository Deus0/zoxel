zox_tag(Chunk);
zox_tag(ChunkMesh);
zox_tag(NoSplitChunk);
zox_tag(BuildChunkSides);
zox_tag(ChunkTextured);
zox_tag(FindNeighbors);
zoxc_byte(GenerateChunk);
zoxc_byte(GenerateModel);
zoxc_byte(NodeDepth);
zoxc_entity(ChunkPrefabLink);
zoxc_entity(ChunkMeshPrefabLink);
zoxc_double(ChunkMeshTimer);

void define_components_chunks(ecs* world) {
    zoxd_tag(Chunk);
    zoxd_tag(ChunkMesh);
    zoxd_tag(ChunkTextured);
    zoxd_tag(BuildChunkSides);
    zoxd_tag(NoSplitChunk);
    zoxd_tag(FindNeighbors);
    // Fragmentation
    zox_dont_fragment(ChunkTextured);
    zox_dont_fragment(BuildChunkSides);
    zox_dont_fragment(FindNeighbors);
    // zox_dont_fragment(GenerateChunk);
    // zox_dont_fragment(GenerateModel);
    zox_dont_fragment(NoSplitChunk);
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_double(ChunkMeshTimer);
    zoxd_entity(ChunkPrefabLink);
    zoxd_entity(ChunkMeshPrefabLink)
}
