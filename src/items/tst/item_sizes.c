// tst: item_sizes, vox items (cookies, grass, mushrooms) held or dropped are as big as each other
//  - each realm item with a vox model: its model's solid voxels' extent (item_vox_extent, what scales it) and its
//      mesh's vertices' extent, logged; they match, or it's scaled by the wrong size (grass was half a mushroom)
//  - run: bash bsh/tst.sh item_sizes
byte zox_tst_item_sizes_ready(ecs* world) {
    return zox_tst_player_spawn_ready(world);
}

void zox_tst_item_sizes_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const entity realm = zox_get_link(world, character, RealmLink);
    int count = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity item = it.entities[i];
            if (!zox_has(item, Item) || !zox_has(item, ItemVox)) {
                continue;
            }
            const entity model = zox_get_link(world, item, ModelLink);
            const entity mesh = get_max_model_mesh(world, model);
            if (!zox_valid(mesh) || !zox_has(mesh, MeshVertices)) {
                continue;
            }
            const MeshVertices* vertices = zox_get(mesh, MeshVertices);
            if (!vertices->length) {
                continue;
            }
            float3 low = vertices->value[0], high = vertices->value[0];
            for (int j = 1; j < vertices->length; j++) {
                const float3 v = vertices->value[j];
                low = (float3) { fminf(low.x, v.x), fminf(low.y, v.y), fminf(low.z, v.z) };
                high = (float3) { fmaxf(high.x, v.x), fmaxf(high.y, v.y), fmaxf(high.z, v.z) };
            }
            const float3 mesh_extent = float3_sub(high, low);
            const float3 extent = item_vox_extent(world, item);
            const float mesh_biggest = fmaxf(mesh_extent.x, fmaxf(mesh_extent.y, mesh_extent.z));
            const float biggest = fmaxf(extent.x, fmaxf(extent.y, extent.z));
            zox_log("[tst] item [%s] model extent [%.3f %.3f %.3f] mesh [%.3f %.3f %.3f], scaled to [%.2f]", zox_getn(item),
                extent.x, extent.y, extent.z, mesh_extent.x, mesh_extent.y, mesh_extent.z,
                biggest > 0 ? mesh_biggest / biggest : 0);
            zox_tst_check(biggest > 0 && fabsf(mesh_biggest / biggest - 1) < 0.1f,
                "item [%s]'s mesh is [%.3f] across, its model [%.3f]: scaled to [%.2f] of the others",
                zox_getn(item), mesh_biggest, biggest, biggest > 0 ? mesh_biggest / biggest : 0);
            count++;
        }
    }
    zox_tst_check(count >= 2, "only [%i] vox items", count);
}

void zox_tsts_item_sizes(ecs* world) {
    zox_tst_add("item_sizes", zox_tst_item_sizes_ready, zox_tst_item_sizes_check, 120);
}
