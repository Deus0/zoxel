// Counts the biomes used in the map, and sets to highest used
zox_sys2(BiomeMapAvgSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateTunk);
    zox_sys_in(BiomeMap);
    zox_sys_out(BiomeLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateTunk, generate);
        zox_sys_i(BiomeMap, bmap);
        zox_sys_o(BiomeLink, link);
        if (generate->value != zox_generate_tunk_heights) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain) || !zox_has(terrain, RealmLink)) {
            zox_logw("Invalid [Terrain] in Tunks [%s]", zox_get_name(terrain));
            continue;
        }
        zox_geter_value(terrain, RealmLink, entity, realm);
        if (!zox_valid(realm) || !zox_has(realm, BiomeLinks)) {
            zox_logw("Invalid [Realm] in Tunks");
            continue;
        }
        zox_geter(realm, BiomeLinks, biomes);
        if (biomes->length < 2) {
            zox_loge("No biomes on realm");
            continue;
        }
        // TODO: Calculate most popular in bmp generically
        //   uint counts[biomes->length];
        uint used_0 = 0;
        uint used_1 = 0;
        for (int j = 0; j < bmap->length; j++) {
            byte bindex = bmap->value[j];
            if (bindex == 0) {
                used_0++;
            } else {
                used_1++;
            }
        }
        // NOTE: Links a chunk to a biome
        byte popular_biome = used_0 > used_1 ? 0 : 1;
        link->value = biomes->value[popular_biome];
        // zox_log("popular_biome is [%i] [%s]", popular_biome, zox_get_name(link->value));
    }
} zox_sys_end(BiomeMapAvgSystem);
