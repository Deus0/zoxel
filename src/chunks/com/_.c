zox_tag(Chunk);
zox_tag(ChunkMesh);
zoxc_byte(GenerateChunk);
zoxc_byte(GenerateModel);
zoxc_byte(NodeDepth);
zoxc_byte(BuildChunkSides);
zoxc_entity(ChunkPrefabLink);
zoxc_entity(ChunkMeshPrefabLink);

void define_components_chunks(ecs* world) {
    zoxd_tag(Chunk);
    zoxd_tag(ChunkMesh);
    zoxd_byte(GenerateChunk);
    zoxd_byte(GenerateModel);
    zoxd_byte(NodeDepth);
    zoxd_byte(BuildChunkSides);
    zoxd_entity(ChunkPrefabLink);
    zoxd_entity(ChunkMeshPrefabLink);
}
