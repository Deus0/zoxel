entity spawn_realm_block_solid(ecs* world, entity prefab, entity parent, lint seed, const char* name, color primary, entity model) {
    byte is_collision = 1;
    byte is_bake = 1;
    byte model_type = zox_block_solid;
    return spawn_realm_block_vox(world, prefab, parent, seed, name, primary, is_collision, model, model_type, is_bake);
}
