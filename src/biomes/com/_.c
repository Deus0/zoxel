zox_tag(Biome);
zox_tag(BiomeLinker);
zoxc_entities(BiomeLinks);
zoxc_float(DirtChance);
zoxc_float(GrassChance);
zoxc_float(WeedsChance);
zoxc_float(TreeChance);
zoxc_float(FlowerChance);
zoxc_float(BiomeHeightFrequency);
zoxc_color_rgb(BiomeSkyColor);

void define_components_biomes(ecs *world) {
    zoxd_tag(Biome);
    zoxd_tag(BiomeLinker);
    zoxd_entities(BiomeLinks)
    zoxd_float(DirtChance);
    zoxd_float(GrassChance);
    zoxd_float(WeedsChance);
    zoxd_float(TreeChance);
    zoxd_float(FlowerChance);
    zoxd_float(BiomeHeightFrequency);
    zoxd_color_rgb(BiomeSkyColor);
}
