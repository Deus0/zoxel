entity zox_tst_character3_npc_skeleton;

void zox_tst_spawn_character3_npc_skeleton(
    ecs *world,
    ClickEventData data)
{
    if (zox_valid(zox_tst_character3_npc_skeleton)) {
        zox_log("Cleaning Test [zox_tst_character3_npc_skeleton]");
        zox_delete(zox_tst_character3_npc_skeleton);
        zox_tst_character3_npc_skeleton = 0;
        return;
    }
    entity player = dbg_player;
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        zox_logw("No Player Character.");
        return;
    }
    float3 sposition = zox_getv(character, Position3D);
    float4 srotation = zox_getv(character, Rotation3D);
    entity terrain = zox_get_link(world, character, TerrainLink);
    if (!zox_valid(terrain)) {
        return;
    }
    entity realm = zox_get_link(world, terrain, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    // byte render_depth = zox_getv(pcharacter, RenderDepth);
    entity characters[zox_realm_characters_max];
    const uint characters_count = zox_get_links(
        world,
        realm,
        CharacterLinks,
        characters,
        zox_realm_characters_max);
    if (!characters_count) {
        zox_logw("Realm has no characters.");
        return;
    }
    lint seed = rand_range(0, 10000);
    uint mindex = rand_range(0, characters_count - 1);
    entity meta = characters[mindex];
    // entity model = zox_get_link(world, meta, ModelLink);
    zox_log("Running Test [zox_tst_character3_npc_skeleton]");
    zox_log("   - Meta [%s:%i]:[%lu]",
            zox_getn(meta),
            mindex,
            seed);
    entity e = spawn_character3(
        world,
        prefab_character3_skeleton_npc,
        realm,
        terrain,
        seed,
        0,
        0,
        sposition,
        srotation,
        "Testyman");
    zox_tst_character3_npc_skeleton = e;
    inspect_entity(world, e);
}
