// Counts the biomes used in the map, and sets to highest used
zox_sys2(BiomeMapAvgSystem) {
    const uint seed = global_seed;
    double height_frequency = terrain_frequency;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(VoxLink);
    zox_sys_in(BiomeMap);
    zox_sys_out(BiomeLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(BiomeLink, link);

        if (generate->value != zox_dirty_end) {
            continue;
        }

        if (!zox_valid(terrain->value) || !zox_has(terrain->value, RealmLink)) {
            zox_logw("Invalid [Terrain] in Tunks [%s]", zox_get_name(terrain->value));
            continue;
        }

        zox_geter_value(terrain->value, RealmLink, entity, realm);

        if (!zox_valid(realm) || !zox_has(realm, BiomeLinks)) {
            zox_logw("Invalid [Realm] in Tunks");
            continue;
        }

        zox_geter(realm, BiomeLinks, biomes);

        // TODO: Calculate most popular in bmp generically
        uint counts[biomes->length];

        uint used_0 = 0;
        uint used_1 = 0;
        for (int j = 0; j < bmap->length; j++) {
            byte bindex = bmap->value[j];

            if (bindex == 0) used_0++;
            else used_1++;
        }

        byte popular_biome = used_0 > used_1 ? 0 : 1;
        link->value = biomes->value[popular_biome];

        // zox_log("popular_biome is [%i] [%s]", popular_biome, zox_get_name(link->value));
    }
} zox_sys_end(BiomeMapAvgSystem);