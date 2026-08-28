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
    // Types
    zoxd_tag(Chunk);
    zoxd_tag(ChunkMesh);
    zoxd_nf_tag(ChunkTextured);
    // Events
    zoxd_nf_tag(BuildChunkSides);
    zoxd_nf_tag(NoSplitChunk);
    zoxd_nf_tag(FindNeighbors);
    // States
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_double(ChunkMeshTimer);
    zoxd_entity(ChunkPrefabLink);
    zoxd_entity(ChunkMeshPrefabLink)
}
