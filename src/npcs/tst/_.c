// tst: population, a region's npc density by its towns (fun/population.c)
//  - inside a town full (255), out in the wild the base, fading between over population_falloff blocks
//  - the busiest of two towns counts, no towns is the base everywhere
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
}

void zox_tsts_npcs() {
    zox_tst_add("population", zox_tst_population_ready, zox_tst_population_check, 30);
}
