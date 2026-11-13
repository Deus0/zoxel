// tst: population, a region's npc density by its towns (fun/population.c)
//  - inside a town full (255), out in the wild the base, fading between over population_falloff blocks
//  - the busiest of two towns counts, no towns is the base everywhere
//  - spawn weights: towns mostly villagers and few monsters, the wild as each type's SpawnChance, animals the same
//  - run: bash bsh/tst.sh population
byte zox_tst_population_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_population_check(ecs* world) {
    (void) world;
    const int2 centers[2] = { { 0, 0 }, { 200, 0 } };
    const byte2 sizes[2] = { { 20, 20 }, { 10, 10 } };
    zox_tst_check(population_at((int2) { 3, -4 }, centers, sizes, 1) == population_town, "inside a town isn't full");
    zox_tst_check(population_at((int2) { 500, 500 }, centers, sizes, 2) == population_base, "the wild isn't the base");
    zox_tst_check(population_at((int2) { 3, 3 }, centers, sizes, 0) == population_base, "no towns isn't the base");
    // fading away from the town's wall, never rising
    byte last = 255;
    int rising = 0;
    for (int x = 10; x < 10 + (int) population_falloff + 8; x++) {
        const byte p = population_at((int2) { x, 0 }, centers, sizes, 1);
        rising += p > last;
        last = p;
    }
    zox_tst_check(!rising, "population rose [%i] times walking away from a town", rising);
    zox_tst_check(last == population_base, "past the falloff it's [%i], not the base", last);
    // halfway out is between
    const byte half = population_at((int2) { 10 + (int) population_falloff / 2, 0 }, centers, sizes, 1);
    zox_tst_check(half > population_base && half < population_town, "halfway out is [%i]", half);
    // the busier of two towns counts
    zox_tst_check(population_at((int2) { 200, 0 }, centers, sizes, 2) == population_town, "the second town isn't full");
    // who spawns: villagers fill towns, monsters keep to the wild, animals anywhere (the realm: a villager 2,
    //  monsters 75, animals 20)
    zox_tst_check(population_town_share(population_base) == 0 && population_town_share(-1) == 0, "the wild or unknown is a town");
    zox_tst_check(population_town_share(population_town) == 1, "a town isn't all town");
    float wild[3], town[3];
    const byte chances[3] = { 2, 75, 20 };
    for (int i = 0; i < 3; i++) {
        wild[i] = npc_spawn_weight(chances[i], i == 0, i == 1, 0);
        town[i] = npc_spawn_weight(chances[i], i == 0, i == 1, 1);
    }
    const float wild_total = wild[0] + wild[1] + wild[2], town_total = town[0] + town[1] + town[2];
    zox_log("[tst] [metric] towns spawn villagers [%.0f%%] monsters [%.0f%%], the wild villagers [%.0f%%] monsters [%.0f%%]",
        100 * town[0] / town_total, 100 * town[1] / town_total, 100 * wild[0] / wild_total, 100 * wild[1] / wild_total);
    zox_tst_check(wild[0] == 2 && wild[1] == 75 && wild[2] == 20, "the wild doesn't roll as the spawn chances");
    zox_tst_check(town[0] / town_total > 0.5f, "towns spawn [%.0f%%] villagers, most should be", 100 * town[0] / town_total);
    zox_tst_check(town[1] / town_total < 0.1f, "towns spawn [%.0f%%] monsters, few should be", 100 * town[1] / town_total);
    zox_tst_check(town[2] == wild[2], "animals spawn differently in towns");
}

void zox_tsts_npcs() {
    zox_tst_add("population", zox_tst_population_ready, zox_tst_population_check, 30);
}
