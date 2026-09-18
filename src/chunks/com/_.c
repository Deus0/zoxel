zox_tag(Chunk);
// zox_tag(ChunkMesh);
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

void zox_components_chunks(ecs* world) {
    // Types zoxd_nf_tag
    zoxd_tag(Chunk);
    // zoxd_tag(ChunkMesh);
    zoxd_tag(ChunkTextured);
    // Events
    zoxd_tag_event(BuildChunkSides);
    zoxd_tag_event(FindNeighbors);
    // Properties
    zoxd_nf_tag(NoSplitChunk);
    // States
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_double(ChunkMeshTimer);
    zoxd_entity(ChunkPrefabLink);
    zoxd_entity(ChunkMeshPrefabLink);
}
