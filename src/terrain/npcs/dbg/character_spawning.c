// todo: pass player entity through here
entity zox_tst_character3_npc;

void zox_tst_spawn_character3_npc(ecs *world, ClickEventData data) {

    if (zox_valid(zox_tst_character3_npc)) {
        zox_log("Cleaning Test [zox_tst_character3_npc]");
        zox_delete(zox_tst_character3_npc);
        zox_tst_character3_npc = 0;
        return;
    }

    // zox_log("keydown %i\n", keycode)
    // spawn at here
    int3 chunk_position = int3_zero;
    // float3 position = (float3) { 4, 12, 4 };

    entity terrain = local_terrain;
    if (!terrain) {
        return;
    }

    zox_geter(terrain, ChunkLinks, chunk_links);
    entity chunk = int3_hashmap_get(chunk_links->value, chunk_position);
    if (!chunk) {
        return;
    }

    // zox_log("+ spawning npc!\n")
    float4 rotation = quaternion_identity;
    byte character_lod = 0;
    byte render_disabled = 0;

    /*int vox_index = rand() % npc_vox_index_count;
    entity vox = string_hashmap_get(files_hashmap_voxes, new_string_data(npc_voxes[vox_index]));

    spawn_character3D_data spawn_data = {
        .prefab = prefab_character3_npc,
        .vox = vox,
        .position = position,
        .rotation = rotation,
        .lod = character_lod,
        .render_disabled = render_disabled,
    };
    entity e = spawn_character3(world, spawn_data);
    zox_set(e, ChunkLink, { chunk });
    zox_tst_character3_npc = e;*/
}
