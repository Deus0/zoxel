// tst: parts, every body part item's model generated voxels, and its icon texture has pixels
//  - bash bsh/tst.sh parts
const double zox_tst_parts_settle = 5.0;   // seconds after parts spawn, for their model processes
#define zox_tst_parts_max 128
entity zox_tst_parts[zox_tst_parts_max];
int zox_tst_parts_count = 0;
double zox_tst_parts_first_seen = 0;

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

byte zox_tst_parts_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "body part items (BodyItem) with a ModelLink";
    if (!zox_tst_parts_count) {
        return 0;
    }
    if (!zox_tst_parts_first_seen) {
        zox_tst_parts_first_seen = zox_current_time;
    }
    return zox_current_time - zox_tst_parts_first_seen >= zox_tst_parts_settle;
}

void zox_tst_parts_check(ecs* world) {
    int checked = 0;
    for (int i = 0; i < zox_tst_parts_count; i++) {
        entity item = zox_tst_parts[i];
        if (!zox_valid(item)) {
            continue;
        }
        checked++;
        entity model = zox_get_link(world, item, ModelLink);
        zox_tst_check(zox_valid(model), "part [%s] has no ModelLink", zox_getn(item));
        if (zox_valid(model)) {
            zox_tst_check(zox_has(model, VoxelNode), "part [%s] model [%s] has no VoxelNode",
                zox_getn(item), zox_getn(model));
            if (zox_has(model, VoxelNode)) {
                zox_geter(model, VoxelNode, voxels);
                const int count = zox_tst_count_voxels(voxels);
                zox_log("[tst] part [%s] model [%s] voxels [%i] GenerateModel [%i]",
                    zox_getn(item), zox_getn(model), count,
                    zox_has(model, GenerateModel) ? zox_getv(model, GenerateModel) : -1);
                zox_tst_check(count, "part [%s] model [%s] has no voxels", zox_getn(item), zox_getn(model));
            }
        }
        // icon, rendered from the model
        entity texture = zox_get_link(world, item, TextureLink);
        zox_tst_check(zox_valid(texture), "part [%s] has no TextureLink", zox_getn(item));
        if (zox_valid(texture) && zox_has(texture, TextureData)) {
            zox_geter(texture, TextureData, data);
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
}

// collects realm body part items, only while the test runs
zox_sys2(TstPartsProbeSystem) {
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
} zox_sys_end(TstPartsProbeSystem);

void zox_tsts_parts(ecs* world) {
    zox_tst_add("parts", zox_tst_parts_ready, zox_tst_parts_check, 90);
    zox_system_1(
        TstPartsProbeSystem,
        zoxp_update,
        [none] parts.BodyItem
    );
}
