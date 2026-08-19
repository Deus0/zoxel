zox_tag(Chunk);
zox_tag(ChunkMesh);
zox_tag(NoSplitChunk);
zoxc_byte(GenerateChunk);
zoxc_byte(GenerateModel);
zoxc_byte(NodeDepth);
zox_tag(BuildChunkSides);
zoxc_byte(FindNeighbors);
zoxc_entity(ChunkPrefabLink);
zoxc_entity(ChunkMeshPrefabLink);

void define_components_chunks(ecs* world) {
    zoxd_tag(Chunk);
    zoxd_tag(ChunkMesh);
    zoxd_tag(NoSplitChunk);
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_tag(BuildChunkSides);
    zoxd_byte(FindNeighbors);
    zoxd_entity(ChunkPrefabLink);
    zoxd_entity(ChunkMeshPrefabLink);
    zox_dont_fragment(NoSplitChunk);
    zox_dont_fragment(BuildChunkSides);
    // zox_dont_fragment(GenerateChunk);
    // zox_dont_fragment(GenerateModel);
    // zox_dont_fragment(BuildChunkSides);
}
