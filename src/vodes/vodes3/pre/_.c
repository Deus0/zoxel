entity spawn_prefab_block_health(ecs* world) {
    zox_prefab();
    zox_prefab_name("block_health");
    zox_add(e, WorldBlock);
    zox_setv(e, StatValue, 1);
    zox_setv(e, StatValueMax, 1);
    return e;
}

entity prefab_block_health;

void zox_prefabs_vodes3(ecs* world) {
    prefab_block_health = spawn_prefab_block_health(world);
}
