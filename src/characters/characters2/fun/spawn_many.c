//! Testing, spawn more!
void spawn_many_characters2D(ecs *world, entity prefab) {
    for (int i = 0; i < spawn_characters2D_count; i++) {
        float2 position = (float2) { -0.5f + (rand() % 100) * 0.2f, -0.5f + (rand() % 100) * 0.2f };
        spawn_character2(world, prefab, position);
    }
}
