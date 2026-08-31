// zox_tst_spawn_character3_npc_skeleton
// todo: pass player entity through here
extern entity prefab_character3_npc;
entity zox_tst_character3_npc_skeleton;
extern entity spawn_inspector(ecs*, entity, entity, entity);

void zox_tst_spawn_character3_npc_skeleton(ecs *world, ClickEventData data) {
    byte dbg_inspector = 1;
    if (zox_valid(zox_tst_character3_npc_skeleton)) {
        zox_log("Cleaning Test [zox_tst_character3_npc_skeleton]");
        zox_delete(zox_tst_character3_npc_skeleton);
        zox_tst_character3_npc_skeleton = 0;
        return;
    }
    entity player = dbg_player;
    zox_geter_value(player, CharacterLink, entity, pcharacter);
    if (!zox_valid(pcharacter)) {
        zox_logw("No Player Character.");
        return;
    }
    zox_geter_value(pcharacter, TerrainLink, entity, terrain);
    if (!zox_valid(terrain)) {
        return;
    }
    zox_geter_value(terrain, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        return;
    }
    // NOTE: Non instanced is broken atm?
    entity prefab = prefab_character3_skeleton_npc;
    zox_geter_value(pcharacter, Position3D, float3, sposition);
    zox_geter_value(pcharacter, Rotation3D, float4, srotation);
    // zox_geter_value(pcharacter, RenderDepth, byte, render_depth);
    zox_geter(realm, CharacterLinks, characters);
    lint seed = rand_range(0, 10000);
    uint mindex = rand_range(0, characters->length - 1);
    entity meta = characters->value[mindex];
    // entity model = zox_getv(meta, ModelLink);
    zox_log("Running Test [zox_tst_character3_npc_skeleton]");
    zox_log("   - Meta [%s:%i]:[%lu]", zox_get_name(meta), mindex, seed);
    entity e = spawn_character3(world, prefab, realm, terrain, seed, 0, 0, sposition, srotation, "Boney");
    zox_tst_character3_npc_skeleton = e;
    if (dbg_inspector) {
        entity canvas = zox_get_link(world, player, Canvas);
        spawn_inspector(world, canvas, player, e);
    }
}
