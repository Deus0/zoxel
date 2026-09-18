zox_tag(Chunk2);
zoxc_int2(Chunk2Position);
zoxc_int2(Chunk2Size);
#define chunk2_neighbors_length 4
zoxc_array(Chunk2Neighbors, entity, chunk2_neighbors_length);

void zox_components_chunks2(ecs* world) {
    zoxd_tag(Chunk2);
    zoxd_int2(Chunk2Position);
    zoxd_int2(Chunk2Size);
    zoxd(Chunk2Neighbors);
}