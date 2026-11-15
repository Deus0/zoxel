// the bone a slot's voxels move with: its CombineList entry (PartIndex) to the bone (doc/done/skeletons.md)
static inline void part_bone_set(ecs* world, CombineBones* table, entity slot, byte bone) {
    const byte index = zox_has(slot, PartIndex) ? zox_getv(slot, PartIndex) : 0;
    if (table && index && index <= table->length) {
        table->value[index - 1] = bone;
    }
}

// Spawns a bone for the item part, and children
entity spawn_part_bones(
    ecs* world,
    entity skeleton,
    BoneLinks* bones,
    float3 half_bounds,
    float bscale,
    entity parent,
    float3 parent_position,
    entity slot,
    entity part,
    CombineBones* table,
    byte parent_bone,
    entity* bone_slots,
    byte* bone_parents)
{
    byte dbg_log = 0;
    if (!zox_valid(slot)) {
        zox_logw("Invalid Slot for Bones");
        return 0;
    }
    // Many slots on bodys can have no parts
    if (!zox_valid(part)) {
        // zox_logw("Invalid Part for Slot [%s]", zox_get_name(slot));
        return 0;
    }
    if (!zox_has(slot, PartPosition)) {
        zox_logw("Invalid Part [%s]", zox_get_name(part));
        return 0;
    }
    // clothes (a shirt) move with what they cover: no bone, else a voxel picks the last bone around it
    //  (generate_weights) and a shirt's bone, wider than the chest, would hold the shoulders still
    if (zox_getv(slot, SlotAnchor) == body_anchor_over) {
        part_bone_set(world, table, slot, parent_bone);
        return 0;
    }
    byte3 part_position = zox_getv(slot, PartPosition);
    byte3 part_size = zox_getv(slot, PartSize);
    byte3 part_centre_position = byte3_add(part_position, byte3_half(part_size));
    entity vox = get_item_model(world, part);
    if (!zox_valid(vox) || !zox_has(vox, ChunkSize)) {
        zox_logw("Model components invalid for part [%s]", zox_get_name(part));
        return 0;
    }
    int3 vsize = get_item_model_size(world, part, vox);
    float3 size = float3_scale(int3_to_float3(vsize), bscale * 0.5f);
    size = float3_add(size, float3_single(bscale * 0.5f));
    float3 position = float3_scale(byte3_to_float3(part_centre_position), bscale);
    position = float3_sub(position, half_bounds);
    position = float3_add(position, float3_single(bscale * 0.5f));
    float3 local_position = float3_sub(position, parent_position);
    // TODO: calculate local position by subtracting parent position
    entity bone = spawn_body_bone(world, prefab_bone, skeleton, parent, position, local_position, size);
    zox_set_unique_name(bone, "bbone");
    add_to_BoneLinks(bones, bone);
    const byte bone_index = bones->length - 1;
    // its slot (what it is, kept over rebuilds) and parent, by index: zox_setv is deferred, read back as it spawns
    zox_setv(bone, BoneSlot, slot);
    bone_slots[bone_index] = slot;
    bone_parents[bone_index] = parent == skeleton ? bone_index : parent_bone;
    part_bone_set(world, table, slot, bone_index);
    // TODO: FLip models around
    byte is_right_side = position.x < 0;
    if (zox_has(part, Head)) {
        zox_add(bone, HeadBone);
        zox_link(world, skeleton, HeadBoneLink, bone);
    }
    if (zox_has(part, ArmPart)) {
        zox_add(bone, ArmBone);
    }
    if (zox_has(part, Hips)) {
        zox_add(bone, HipsBone);
    }
    if (zox_has(part, Shoulder)) {
        zox_add(bone, ShoulderBone);
        zox_setv(bone, SwingState, 1);
        zox_setv(bone, SwingAngle, 110);
        // NOTE: For Right Shoulder
        if (is_right_side) {
            zox_link(world, skeleton, ShoulderBoneLink, bone);
        }
    }
    if (zox_has(part, Hand)) {
        zox_add(bone, HandBone);
        // NOTE: For Right Hand
        if (is_right_side) {
            zox_link(world, skeleton, HandBoneLink, bone);
            // zox_log("Added RightHandBone to Part's Bone [%s]", zox_get_name(part));
        }
    }
    if (zox_has(part, Thigh)) {
        zox_add(bone, ThighBone);
        zox_set(bone, SwingState, { 1 });
        zox_set(bone, SwingAngle, { 60 });
        // zox_log("Added ThighBone to Part's Bone [%s]", zox_get_name(part));
    }
    if (dbg_log) {
        zox_log("Spawned Part [%s] Bone:", zox_get_name(part));
        zox_log("   # size  b [%ix%ix%i] f [%fx%fx%f]", vsize.x, vsize.y, vsize.z, size.x, size.y, size.z);
        zox_log("   @ position b [%ix%ix%i] f [%fx%fx%f] l [%fx%fx%f]", part_centre_position.x, part_centre_position.y, part_centre_position.z, position.x, position.y, position.z, local_position.x, local_position.y, local_position.z);
    }
    // Now Recursively add parts
    iter it2 = zox_children(world, slot);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, Slot)) {
                continue;
            }
            entity child_part = zox_get_link(world, e2, DataLink);
            // its parent is set as it spawns (spawn_body_bone, non fragmenting): a ChildOf here too gave every
            //  humanoid ~45 tables of its own (tst tables_stream)
            spawn_part_bones(world, skeleton, bones, half_bounds, bscale, bone, position, e2, child_part, table, bone_index,
                bone_slots, bone_parents);
        }
    }
    return bone;
}

// Spawns bones under the characters skeleton child, from its body parts
// NOTE: We could delete only bones that are linked to updated parts here, but that is a refactor, perhaps later
void character_bone_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodySize);
    zox_sys_in(BlockScale);
    zox_sys_out(BodyDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodySize, bsize);
        zox_sys_i(BlockScale, bscale);
        zox_sys_o(BodyDirty, body_dirty);
        if (body_dirty->value != zox_generate_body_bones) {
            continue;
        }
        // NOTE: Waits for the skeleton to spawn
        entity skeleton = zox_get_link(world, e, SkeletonLink);
        if (!zox_valid(skeleton) || !zox_has(skeleton, BoneLinks)) {
            continue;
        }
        BoneLinks* bones = zox_getm(skeleton, BoneLinks);
        // NOTE: Deletes old bones, and old parts, spawns new ones
        // a bone keeps its index over a rebuild (matched by its slot), the index of one gone (a hat taken off) is its
        //  parent's, and new ones go on the end: the old skeleton mesh draws until the new one swaps in, its weights
        //  are by index, and every bone after the hat shifting down one drew it all glitched (tst bone_rebuild)
        const int old_length = bones->length;
        entity old_bones[zox_bones_max];
        entity old_slots[zox_bones_max];
        // the pose the old bones were in, and the world transform the skeleton draws with (TransformMatrix), else the
        //  new bones draw a frame at identity before the transform systems reach them
        typedef struct { float4 local; float4 rotation; float3 position; float4x4 matrix; } bone_pose;
        bone_pose old_pose[zox_bones_max];
        for (int k = 0; k < old_length && k < zox_bones_max; k++) {
            const entity old_bone = bones->value[k];
            old_bones[k] = old_bone;
            old_slots[k] = 0;
            old_pose[k] = (bone_pose) { quaternion_identity, quaternion_identity, float3_zero, float4x4_identity };
            if (zox_valid(old_bone) && zox_has(old_bone, TransformMatrix)) {
                old_slots[k] = zox_getv(old_bone, BoneSlot);
                old_pose[k] = (bone_pose) {
                    zox_getv(old_bone, LocalRotation3D),
                    zox_getv(old_bone, Rotation3D),
                    zox_getv(old_bone, Position3D),
                    zox_getv(old_bone, TransformMatrix)
                };
            }
        }
        // what rides on the old bones (the camera on the head, a held item on the hand) is taken off them, and put on
        //  the bone at the same index right away: the HeadBoneLink to the new head is deferred (still the old one), the
        //  camera fell off, place_camera_head_system reset its look and the arm that follows it snapped
        enum { riders_max = 16 };
        entity riders[riders_max];
        int rider_bones[riders_max];
        int riders_length = 0;
        for (int k = 0; k < old_length && k < zox_bones_max; k++) {
            const entity old_bone = old_bones[k];
            // an index that was its parent's (a bone gone) has its riders found at the parent's own index
            byte first = 1;
            for (int j = 0; j < k && first; j++) {
                first = old_bones[j] != old_bone;
            }
            if (!first || !zox_valid(old_bone)) {
                continue;
            }
            iter rit = zox_children(world, old_bone);
            while (zox_children_next(rit)) {
                for (int j = 0; j < rit.count; j++) {
                    const entity rider = rit.entities[j];
                    if (zox_has(rider, Bone) || riders_length >= riders_max) {
                        continue;
                    }
                    riders[riders_length] = rider;
                    rider_bones[riders_length++] = k;
                }
            }
        }
        for (int k = 0; k < riders_length; k++) {
            zox_set_parent(world, riders[k], 0);
        }
        // the camera is kept even when it wasn't found on a bone
        entity camera = zox_get_child_by_id_recursive(world, e, zox_id(Camera));
        if (zox_valid(camera)) {
            zox_set_parent(world, camera, 0);
        }
        entity root_bone = zox_get_child_by_id(world, skeleton, zox_id(Bone));
        if (zox_valid(root_bone)) {
            zox_delete(root_bone);
        }
        resize_BoneLinks(bones, 0);
        float3 half_bounds = float3_scale(byte3_to_float3(bsize->value), bscale->value * 0.5f);
        if (dbg_log) {
            zox_log("+ [%s] is Spawning Bones", zox_get_name(e));
            zox_log("   - Size [%ix%ix%i]", bsize->value.x, bsize->value.y, bsize->value.z);
            zox_log("   - Bounds [%fx%fx%f]", half_bounds.x, half_bounds.y, half_bounds.z);
        }
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot)) {
            continue;
        }
        entity chest_part = zox_get_link(world, chest_slot, DataLink);
        // each body entry's bone (0, the chest, until set), for the meshes' weights
        const entity vox = zox_get_link(world, e, VoxLink);
        CombineBones* table = zox_valid(vox) && zox_has(vox, CombineBones) ? zox_getm(vox, CombineBones) : NULL;
        if (table) {
            resize_CombineBones(table, zox_get(vox, CombineList)->length);
            memset(table->value, 0, table->length);
        }
        entity new_slots[256];
        byte new_parents[256];
        spawn_part_bones(world, skeleton, bones, half_bounds, bscale->value, skeleton, float3_zero, chest_slot, chest_part,
            table, 0, new_slots, new_parents);
        // the spawned bones (in spawn order) placed at their old indexes
        const int new_length = bones->length;
        entity spawned[256];
        memcpy(spawned, bones->value, sizeof(entity) * new_length);
        int placed[256 + zox_bones_max];    // a final index's spawned bone
        byte matched[256 + zox_bones_max];  // it was the same bone (its pose carries over)
        int primary[256];                   // a spawned bone's final index
        for (int j = 0; j < new_length; j++) {
            primary[j] = -1;
        }
        int length = 0;
        if (old_length <= zox_bones_max) {
            for (int k = 0; k < old_length; k++) {
                int found = -1;
                for (int j = 0; j < new_length && old_slots[k] && found < 0; j++) {
                    if (new_slots[j] == old_slots[k] && primary[j] < 0) {
                        found = j;
                    }
                }
                matched[k] = found >= 0;
                if (found < 0) {
                    // gone: its parent's bone (before it, so already placed), the chest if none
                    //  an index already its parent's (gone before) stays with that bone
                    found = k ? placed[0] : 0;
                    int from = -1;
                    for (int j = 0; j < k && from < 0; j++) {
                        if (old_bones[j] == old_bones[k]) {
                            from = j;
                        }
                    }
                    const entity old_parent = from < 0 && zox_valid(old_bones[k]) ? zox_get_parent(world, old_bones[k]) : 0;
                    for (int j = 0; j < k && from < 0 && old_parent; j++) {
                        if (old_bones[j] == old_parent) {
                            from = j;
                        }
                    }
                    if (from >= 0) {
                        found = placed[from];
                    }
                } else {
                    primary[found] = k;
                }
                placed[length++] = found;
            }
        }
        for (int j = 0; j < new_length; j++) {
            if (primary[j] < 0) {
                matched[length] = 0;
                primary[j] = length;
                placed[length++] = j;
            }
        }
        // too many for the shader (zox_bones_max), or it went wrong: the spawn order (the old mesh glitches until swapped)
        byte stable = length <= zox_bones_max;
        for (int j = 0; j < new_length && stable; j++) {
            stable = primary[j] >= 0;
        }
        if (!stable) {
            length = new_length;
            for (int j = 0; j < new_length; j++) {
                placed[j] = j;
                primary[j] = j;
                matched[j] = 0;
            }
        }
        resize_BoneLinks(bones, length);
        for (int k = 0; k < length; k++) {
            bones->value[k] = spawned[placed[k]];
        }
        // the parts table points at the final indexes
        for (int k = 0; table && k < table->length; k++) {
            if (table->value[k] < new_length) {
                table->value[k] = primary[table->value[k]];
            }
        }
        // the pose carried over, a new bone (a hat put on) starts where its parent is
        for (int j = 0; j < new_length; j++) {
            const int k = primary[j];
            int from = k < old_length && matched[k] ? k : -1;
            if (from < 0) {
                const int parent_final = primary[new_parents[j]];
                from = parent_final < old_length && matched[parent_final] ? parent_final : -1;
                if (from < 0) {
                    continue;
                }
                zox_setv(spawned[j], Rotation3D, old_pose[from].rotation);
                zox_setv(spawned[j], Position3D, old_pose[from].position);
                zox_setv(spawned[j], TransformMatrix, old_pose[from].matrix);
                continue;
            }
            zox_setv(spawned[j], LocalRotation3D, old_pose[from].local);
            zox_setv(spawned[j], Rotation3D, old_pose[from].rotation);
            zox_setv(spawned[j], Position3D, old_pose[from].position);
            zox_setv(spawned[j], TransformMatrix, old_pose[from].matrix);
        }
        // the riders (the camera too) on the bone at their index (a hat's on the head)
        for (int k = 0; k < riders_length; k++) {
            if (stable && rider_bones[k] < length) {
                zox_set_parent(world, riders[k], bones->value[rider_bones[k]]);
            } else if (riders[k] != camera) {
                zox_delete(riders[k]);  // the hold system makes a held item again
            }
            // else the camera is put on the new head when the skeleton updates (its look reset)
        }
        body_dirty->value = zox_generate_body_end;
        zox_add(skeleton, SkeletonDirty);
    }
    zox_sys_on_end();
} zoxd_system(character_bone_spawn_system);
