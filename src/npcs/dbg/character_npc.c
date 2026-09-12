// todo: pass player entity through here
entity zox_tst_character3_npc;

// TODO: Debug inspector for placement chunk, just to check its added to entities there
// TODO: Remove InstanceLink, just use ModelLink and RendererInstance Tag
// TODO: ChunkLink didn't seem to update, create an initial ChunkLinked state so it makes sure to update, remove -666, -666, -666 thingo

void zox_tst_spawn_character3_npc(
    ecs *world,
    ClickEventData data)
{
    byte dbg_inspector = 0;
    if (zox_valid(zox_tst_character3_npc)) {
        zox_log("Cleaning Test [zox_tst_character3_npc]");
        zox_delete(zox_tst_character3_npc);
        zox_tst_character3_npc = 0;
        return;
    }
    entity player = dbg_player;
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        zox_logw("No Player Character.");
        return;
    }
    entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        return;
    }
    entity realm = zox_get_link(world, terrain, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    // NOTE: Non instanced is broken atm?
    entity prefab = is_characters_instanced ?
        prefab_character3_instanced_npc :
        prefab_character3_npc;
    zox_geter_value(character, Position3D, float3, sposition);
    zox_geter_value(character, Rotation3D, float4, srotation);
    // zox_geter_value(character, RenderDepth, byte, render_depth);
    zox_geter(realm, CharacterLinks, characters);
    lint seed = rand_range(0, 10000);
    uint mindex = rand_range(0, characters->length - 1);
    entity meta = characters->value[mindex];
    // entity model = zox_getv(meta, ModelLink);
    // cchar name[64]; // = "TS-G391";
    // csprintf(name, "TS-G%lu", seed);
    // char* name = generate_name();
    zox_log("Running Test: Spawn [character3_npc]");
    zox_log("   - Meta [%s:%i]:[%lu]", zox_get_name(meta), mindex, seed);
    entity e = spawn_character3(
        world,
        prefab,
        realm,
        terrain,
        seed,
        0,
        0,
        sposition,
        srotation,
        NULL);
    zox_tst_character3_npc = e;
    if (dbg_inspector) {
        entity canvas = zox_get_link(world, player, Canvas);
        spawn_inspector(
            world,
            canvas,
            player,
            e);
    }
}
