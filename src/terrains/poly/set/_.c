// Number of cells along x and z.
int2 poly_terrain_cells = { 256, 256 };      // 2m cells
// Size along x and z in metres.
float2 poly_terrain_size = { 512, 512 };
// Maximum hill height in metres.
float poly_terrain_height = 64;
// Sea height in metres.
float poly_terrain_sea = 2;
// Base Perlin noise frequency.
float poly_terrain_frequency = 0.012f;      // per metre: a hill every ~80m, more of them on a bigger island
// How much the hills (noise) and the middle's mountain make of the height, 0 to 1 each.
float poly_terrain_hills = 0.55f;
float poly_terrain_peak = 0.6f;
// How far noise moves the coast in or out, of the island's half width: bays and headlands, not a round blob.
float poly_terrain_coast = 0.3f;
// Number of Perlin noise octaves.
int poly_terrain_octaves = 4;
