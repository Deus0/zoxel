zox_tag(FlatTerrain);
zox_tag(FlatlandChunk);
zox_tag(TerrainWorld);
zox_tag(TerrainChunk);
zox_tag(ChunkTerrain);
zoxc_entity(TerrainLink);

entity get_linked_terrain(ecs* world, const entity e) {
    return zox_valid(e) && zox_has(e, TerrainLink) ? zox_gett_value(e, TerrainLink) : 0;
}

void define_components_terrain(ecs *world) {
    zox_define_tag(FlatTerrain);
    zox_define_tag(FlatlandChunk);
    zox_define_tag(TerrainWorld);
    zox_define_tag(TerrainChunk);
    zox_define_tag(ChunkTerrain);
    zox_define_component_entity(TerrainLink);
}