// tst: parts, every body part item's model generated voxels, and its icon texture has pixels
//  - bash bsh/tst.sh parts
#define zox_tst_parts_max 128
entity zox_tst_parts[zox_tst_parts_max];
int zox_tst_parts_count = 0;

int zox_tst_count_voxels(const VoxelNode* node) {
    if (is_closed_VoxelNode(node)) {
        return node->value != 0;
    } else if (is_linked_VoxelNode(node)) {
        return 1;
    } else if (has_children_VoxelNode(node)) {
        const VoxelNode* kids = (const VoxelNode*) node->ptr;
        int count = 0;
        for (int i = 0; i < octree_length; i++) {
            count += zox_tst_count_voxels(&kids[i]);
        }
        return count;
    }
    return 0;
}

// done: its model generated (not Generating) and its icon baked (pixels, not waiting to run)
//  (was 5s after the first part was seen, guessing they'd be done by then)
byte zox_tst_part_done(ecs* world, entity item) {
    const entity model = zox_get_link(world, item, ModelLink);
    const entity texture = zox_get_link(world, item, TextureLink);
    if (!zox_valid(model) || zox_has(model, Generating) || !zox_valid(texture) || !zox_has(texture, TextureData)) {
        return 0;
    }
    return zox_get(texture, TextureData)->length && zox_getv(texture, GenerateTexture) != zox_generate_texture_run;
}

byte zox_tst_parts_ready(ecs* world) {
    int done = 0;
    for (int i = 0; i < zox_tst_parts_count; i++) {
        done += !zox_valid(zox_tst_parts[i]) || zox_tst_part_done(world, zox_tst_parts[i]);
    }
    static char waiting[128];
    snprintf(waiting, sizeof(waiting), "body part items generated with their icons: [%i/%i]", done, zox_tst_parts_count);
    zox_tst_waiting = waiting;
    zox_tst_progress(zox_tst_parts_count * 1000 + done);
    return zox_tst_parts_count && done == zox_tst_parts_count;
}

void zox_tst_parts_check(ecs* world) {
    int checked = 0;
    // each part's model seeded its own (its colours and sprinkles come from it): all seeds were 0, 2026-11-14
    lint first_seed = 0;
    byte seeds_differ = 0;
    for (int i = 0; i < zox_tst_parts_count; i++) {
        entity item = zox_tst_parts[i];
        if (!zox_valid(item)) {
            continue;
        }
        checked++;
        entity model = zox_get_link(world, item, ModelLink);
        zox_tst_check(zox_valid(model), "part [%s] has no ModelLink", zox_getn(item));
        if (zox_valid(model)) {
            const lint seed = zox_has(model, Seed) ? zox_getv(model, Seed) : 0;
            seeds_differ |= checked > 1 && seed != first_seed;
            if (checked == 1) {
                first_seed = seed;
            }
            zox_tst_check(zox_has(model, VoxelNode), "part [%s] model [%s] has no VoxelNode",
                zox_getn(item), zox_getn(model));
            if (zox_has(model, VoxelNode)) {
                const VoxelNode* voxels = zox_get(model, VoxelNode);
                const int count = zox_tst_count_voxels(voxels);
                zox_log("[tst] part [%s] model [%s] voxels [%i] GenerateModel [%i] seed [%lld]",
                    zox_getn(item), zox_getn(model), count,
                    zox_has(model, GenerateModel) ? zox_getv(model, GenerateModel) : -1, (long long) seed);
                zox_tst_check(count, "part [%s] model [%s] has no voxels", zox_getn(item), zox_getn(model));
            }
        }
        // icon, rendered from the model
        entity texture = zox_get_link(world, item, TextureLink);
        zox_tst_check(zox_valid(texture), "part [%s] has no TextureLink", zox_getn(item));
        if (zox_valid(texture) && zox_has(texture, TextureData)) {
            const TextureData* data = zox_get(texture, TextureData);
            int pixels = 0;
            for (int j = 0; j < data->length; j++) {
                pixels += data->value[j].a != 0;
            }
            zox_tst_check(pixels, "part [%s] texture [%s] has no pixels ([%i] length)",
                zox_getn(item), zox_getn(texture), data->length);
        }
    }
    zox_log("[tst] body parts [%i]", checked);
    zox_tst_check(checked, "no body parts");
    zox_tst_check(checked < 2 || seeds_differ, "every body part model has the same seed [%lld]", (long long) first_seed);
}

// collects realm body part items, only while the test runs
void tst_parts_probe_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_active || zox_tst_active->check != zox_tst_parts_check) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (!zox_valid(zox_get_link(world, e, ModelLink))) {
            continue;
        }
        byte seen = 0;
        for (int j = 0; j < zox_tst_parts_count && !seen; j++) {
            seen = zox_tst_parts[j] == e;
        }
        if (!seen && zox_tst_parts_count < zox_tst_parts_max) {
            zox_tst_parts[zox_tst_parts_count++] = e;
        }
    }
    zox_sys_on_end();
} zoxd_system(tst_parts_probe_system);

void zox_tsts_parts(ecs* world) {
    zox_tst_add("parts", zox_tst_parts_ready, zox_tst_parts_check, 90);
    // main thread, in zoxp_render with the other main thread systems: no merges of its own (tst pipeline_syncs)
    zox_system_1(
        tst_parts_probe_system,
        zoxp_render,
        [none] parts.BodyItem
    );
}
