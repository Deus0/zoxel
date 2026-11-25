zox_tag(Chunk);
// zox_tag(ChunkMesh);
zox_tag(ChunkTextured);
zox_tag(FindNeighbors);
zoxc_byte(GenerateChunk);
zoxc_byte(GenerateModel);
zoxc_byte(NodeDepth);
zoxc_double(ChunkMeshTimer);

void zox_components_chunks(ecs* world) {
    // Types zoxd_nf_tag
    zoxd_tag(Chunk);
    // zoxd_tag(ChunkMesh);
    zoxd_tag(ChunkTextured);
    // Events
    zoxd_tag_event(FindNeighbors);
    // States
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_double(ChunkMeshTimer);
}
