#define zox_biome_earthy_forest 0
#define zox_biome_martian 1
#define zox_biome_toxic_swamp 2
#define zox_biome_fungal_alien 3
#define zox_biome_volcanic 4
#define zox_biome_artic_tundra 5
#define zox_biome_radioactive_wasteland 6
#define zox_biome_underground_mushroom 7
#define zox_biome_crystal_valley 8
#define zox_biome_wind_scarred_cliffs 9
#define zox_biome_neon_void 10
#define zox_biome_end 11

typedef struct {
    const char* name;
    float6 dirt;
    float6 grass;
    float6 stone;
    float6 sand;
    float6 obsidian;
    float6 sky;
    float chance;
    float frequency;
} BiomeData;