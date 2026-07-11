zox_tag(Biome);
zoxc_entity(BiomeLink);
zoxc_entities(BiomeLinks);
zoxc_float(DirtChance);
zoxc_float(GrassChance);
zoxc_float(WeedsChance);
zoxc_float(TreeChance);
zoxc_float(FlowerChance);

void define_components_biomes(ecs *world) {
    zoxd_tag(Biome);
    zoxd_entity(BiomeLink);
    zoxd_entities(BiomeLinks)
    zoxd_float(DirtChance);
    zoxd_float(GrassChance);
    zoxd_float(WeedsChance);
    zoxd_float(TreeChance);
    zoxd_float(FlowerChance);
}
