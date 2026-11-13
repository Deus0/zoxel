void biomes_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
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
        uint seed_shift = 3690;
        uint seed_offset = 999 + seed_shift;
        {
            lint biome_seed = seed->value + (seed_offset+=seed_shift);
            entity e2 = spawn_biome(
                world,
                prefab_biome,
                e,
                "grasslands",
                biome_seed);
            zox_set_parent(world, e2, e);
            add_to_BiomeLinks(biomes, e2);
            // ground cover shares (the rest is grass) and a tree's chance a cell (vegetation/fun)
            zox_set(e2, DirtChance, { 0.02f });
            zox_set(e2, GrassChance, { 0.28f });
            zox_set(e2, WeedsChance, { 0.55f });
            zox_set(e2, FlowerChance, { 0.10f });
            zox_set(e2, TreeChance, { 0.45f });
            zox_set(e2, BiomeHeightFrequency, { 0.13f });
        }
        {
            lint biome_seed = seed->value + (seed_offset+=seed_shift);
            entity e2 = spawn_biome(
                world,
                prefab_biome,
                e,
                "desert",
                biome_seed);
            zox_set_parent(world, e2, e);
            add_to_BiomeLinks(biomes, e2);
            zox_set(e2, DirtChance, { 0.70f });
            zox_set(e2, GrassChance, { 0.20f });
            zox_set(e2, WeedsChance, { 0.06f });
            zox_set(e2, FlowerChance, { 0.01f });
            zox_set(e2, TreeChance, { 0.04f });
            zox_set(e2, BiomeHeightFrequency, { 0.07f });
        }
        {
            lint biome_seed = seed->value + (seed_offset+=seed_shift);
            entity e2 = spawn_biome(
                world,
                prefab_biome,
                e,
                "jungle",
                biome_seed);
            zox_set_parent(world, e2, e);
            add_to_BiomeLinks(biomes, e2);
            zox_set(e2, DirtChance, { 0.02f });
            zox_set(e2, GrassChance, { 0.18f });
            zox_set(e2, WeedsChance, { 0.62f });
            zox_set(e2, FlowerChance, { 0.14f });
            zox_set(e2, TreeChance, { 0.85f });
            zox_set(e2, BiomeHeightFrequency, { 0.369f });
        }
        zox_logv("Realm [Biomes] [%i] spawned", biomes->length);
    }
    zox_sys_on_end();
} zoxd_system(biomes_realm_spawn_system);
