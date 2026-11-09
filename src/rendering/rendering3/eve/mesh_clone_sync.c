// Source mesh became MeshDirty -> copy its data to every linked clone
#define mesh_clone_sync_max 64

void mesh_clone_sync_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshColorRGBs, colors);
        if (!indicies->value ||
            !verts->value ||
            !colors->value)
        {
            continue;
        }
        // gather clones first, so we don't modify while iterating
        entity clones[mesh_clone_sync_max];
        int clones_count = 0;
        iter it2 = ecs_each_id(
            world,
            ecs_pair(zox_id(MeshCloneLink), e));
        while (zox_each_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                if (clones_count < mesh_clone_sync_max) {
                    clones[clones_count++] = it2.entities[j];
                }
            }
        }
        for (int j = 0; j < clones_count; j++) {
            entity clone = clones[j];
            if (!zox_valid(clone) ||
                clone == e ||
                !zox_has(clone, MeshIndicies) ||
                !zox_has(clone, MeshVertices) ||
                !zox_has(clone, MeshColorRGBs))
            {
                continue;
            }
            clone_MeshIndicies(zox_get_mut(clone, MeshIndicies), indicies);
            clone_MeshVertices(zox_get_mut(clone, MeshVertices), verts);
            clone_MeshColorRGBs(zox_get_mut(clone, MeshColorRGBs), colors);
            zox_add(clone, MeshDirty);
            if (dbg_log) {
                zox_log("Synced Mesh Clone [%s] from [%s] [%i]",
                    zox_getn(clone),
                    zox_sys_e_name,
                    verts->length);
            }
        }
    }
}
