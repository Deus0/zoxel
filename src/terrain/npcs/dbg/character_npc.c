// todo: pass player entity through here
extern entity prefab_character3_npc;
entity zox_tst_character3_npc;
extern entity spawn_inspector(ecs*, entity, entity, entity);

// TODO: Debug inspector for placement chunk, just to check its added to entities there
// TODO: Remove InstanceLink, just use ModelLink and RendererInstance Tag
// TODO: ChunkLink didn't seem to update, create an initial ChunkLinked state so it makes sure to update, remove -666, -666, -666 thingo

void zox_tst_spawn_character3_npc(ecs *world, ClickEventData data) {
    if (zox_valid(zox_tst_character3_npc)) {
        zox_log("Cleaning Test [zox_tst_character3_npc]");
        zox_delete(zox_tst_character3_npc);
        zox_tst_character3_npc = 0;
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
    entity prefab = is_characters_instanced ? prefab_character3_instanced_npc : prefab_character3_npc;
    zox_geter_value(pcharacter, Position3D, float3, sposition);
    zox_geter_value(pcharacter, Rotation3D, float4, srotation);
    zox_geter_value(pcharacter, RenderDepth, byte, render_depth);
    zox_geter(realm, CharacterLinks, characters);
    lint seed = rand_range(0, 10000);
    uint mindex = rand_range(0, characters->length - 1);
    entity meta = characters->value[mindex];
    entity model = zox_getv(meta, ModelLink);
    // cchar name[64]; // = "TS-G391";
    // csprintf(name, "TS-G%lu", seed);
    // char* name = generate_name();
    char* name = NULL; // generate_name(seed);
    zox_log("Running Test: Spawn [character3_npc]");
    zox_log("   - Meta [%s:%i]:[%lu]", zox_get_name(meta), mindex, seed);
    entity e = spawn_character3(world, prefab, realm, terrain, seed, model, render_depth, 0, sposition, srotation, name);
    free(name);
    zox_tst_character3_npc = e;
    zox_geter_value(player, CanvasLink, entity, canvas);
    spawn_inspector(world, canvas, player, e);
}
