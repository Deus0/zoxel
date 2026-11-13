zox_tag(Biome);
zox_tag(BiomeLink);
zox_tag(BiomeLinker);
zoxc_entities(BiomeLinks);
// vegetation (vegetation/fun): ground cover shares of columns (the rest grass), and a tree's chance a cell
zoxc_float(DirtChance);     // bare dirt
zoxc_float(GrassChance);    // grass topped soil
zoxc_float(WeedsChance);    // tall grass on top
zoxc_float(TreeChance);     // chance a tree cell (zox_tree_cell) has a tree
zoxc_float(FlowerChance);   // flowers on top
zoxc_float(BiomeHeightFrequency);
zoxc_color_rgb(BiomeSkyColor);

void zox_components_biomes(ecs *world) {
    zoxd_tag(Biome);
    zoxd_nf_link(BiomeLink);
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
