// hook to spawning
entity spawn_character3_npc(ecs* world, entity prefab, entity realm, entity terrain, entity model, byte render_depth, byte render_disabled, float3 position, float4 rotation, const char* name) {
    entity e = spawn_character3(world, prefab, realm, terrain, model, render_depth, render_disabled, position, rotation, name);
    if (rand() % 100 <= 6) {
        zox_set(e, DefaultBehaviour, { zox_behaviour_idle });
    }
    if (rand() % 100 <= 14) {
        zox_add_tag(e, Coward);
    }
    return e;
}
