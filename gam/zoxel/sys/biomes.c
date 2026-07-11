zox_sys2(BiomesRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    zox_sys_out(BiomeLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        zox_sys_o(BiomeLinks, biomes);
        if (state->value != zox_generate_realm_biomes) {
            continue;
        }
        {
            entity e2 = spawn_biome(world, prefab_biome, e, "grasslands", seed->value + 1);
            zox_set_parent(world, e2, e);
            add_to_BiomeLinks(biomes, e2);
            zox_set(e2, DirtChance, { 0.3f });
            zox_set(e2, GrassChance, { 0.44f });
            zox_set(e2, WeedsChance, { 0.71f });
            zox_set(e2, FlowerChance, { 0.72f });
            zox_set(e2, TreeChance, { 0.73f });
        }
        {
            entity e2 = spawn_biome(world, prefab_biome, e, "desert", seed->value + 2);
            zox_set_parent(world, e2, e);
            add_to_BiomeLinks(biomes, e2);
            zox_set(e2, DirtChance, { 0.6f });
            zox_set(e2, GrassChance, { 0.66f });
            zox_set(e2, WeedsChance, { 0.72f });
            zox_set(e2, FlowerChance, { 0.73f });
            zox_set(e2, TreeChance, { 0.74f });
        }
        zox_logv("At [%f] Realm [Biomes] [%i] spawned.", zox_current_time, biomes->length);
    }
} zox_sys_end(BiomesRealmSpawnSystem);
