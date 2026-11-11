// tst: skeleton_render
//  - skeleton meshes are drawn by skeleton3_render_system
//  - every visible skeleton has one enabled, drawable ActiveMesh with valid bone data
#define zox_tst_skeleton_meshes_max 512
zox_tst_draws zox_tst_skeleton_draws;
// the probe collects every skeleton mesh each frame, ready swaps to the finished frame
entity zox_tst_skeleton_meshes[zox_tst_skeleton_meshes_max];
entity zox_tst_skeleton_meshes_counting[zox_tst_skeleton_meshes_max];
int zox_tst_skeleton_meshes_count = 0;
int zox_tst_skeleton_meshes_counting_count = 0;
char zox_tst_skeleton_waiting[256];

byte zox_tst_skeleton_ready(ecs* world) {
    memcpy(zox_tst_skeleton_meshes, zox_tst_skeleton_meshes_counting, sizeof(entity) * zox_tst_skeleton_meshes_counting_count);
    zox_tst_skeleton_meshes_count = zox_tst_skeleton_meshes_counting_count;
    zox_tst_skeleton_meshes_counting_count = 0;
    int render_disabled = 0;
    int no_indicies = 0;
    int disabled = 0;
    int visible = 0;
    for (int i = 0; i < zox_tst_skeleton_meshes_count; i++) {
        entity e = zox_tst_skeleton_meshes[i];
        if (!zox_valid(e)) {
            continue;
        }
        const byte is_render_disabled = zox_getv(e, RenderDisabled) != 0;
        const byte is_disabled = zox_has(e, Disabled);
        zox_geter(e, MeshIndicies, indicies);
        render_disabled += is_render_disabled;
        disabled += is_disabled;
        no_indicies += !indicies->length;
        visible += !is_render_disabled && !is_disabled && indicies->length;
    }
    snprintf(zox_tst_skeleton_waiting, sizeof(zox_tst_skeleton_waiting),
        "a visible SkeletonMesh with indicies, last frame: [%i] skeleton meshes, "
        "[%i] RenderDisabled, [%i] no indicies, [%i] Disabled",
        zox_tst_skeleton_meshes_count, render_disabled, no_indicies, disabled);
    // checked this frame, so one must be visible now (frustum culling hides npcs as the camera turns)
    if (!visible) {
        return 0;
    }
    return zox_tst_draws_ready(
        world,
        &zox_tst_skeleton_draws,
        zox_drawn_skeleton_meshes,
        zox_tst_skeleton_waiting);
}

// a mesh skeleton3_render_system will draw, and draw correctly
void zox_tst_skeleton_check_mesh(ecs* world, entity skeleton, entity e) {
    zox_tst_check(!zox_has(e, Disabled), "[%s] active mesh [%s] is Disabled", zox_getn(skeleton), zox_getn(e));
    zox_tst_check(!zox_has(e, Initialize), "[%s] active mesh [%s] still has Initialize", zox_getn(skeleton), zox_getn(e));
    zox_tst_check(!zox_getv(e, RenderDisabled), "[%s] active mesh [%s] is RenderDisabled", zox_getn(skeleton), zox_getn(e));
    const guint2 mesh = zox_getv(e, MeshGPULink);
    zox_tst_check(mesh.x && mesh.y, "[%s] MeshGPULink [%u:%u]", zox_getn(e), mesh.x, mesh.y);
    zox_tst_check(zox_getv(e, ColorsGPULink), "[%s] has no ColorsGPULink", zox_getn(e));
    zox_tst_check(zox_getv(e, BoneIndexGPULink), "[%s] has no BoneIndexGPULink", zox_getn(e));
    zox_tst_check_camera_filter(world, e);
    zox_geter(e, MeshIndicies, indicies);
    zox_geter(e, MeshVertices, vertices);
    zox_geter(e, BoneIndexes, bone_indexes);
    zox_geter(skeleton, BoneLinks, bones);
    zox_tst_check(indicies->length, "[%s] has 0 indicies", zox_getn(e));
    int bad_indicies = 0;
    for (int i = 0; i < indicies->length; i++) {
        bad_indicies += indicies->value[i] < 0 || indicies->value[i] >= vertices->length;
    }
    zox_tst_check(!bad_indicies, "[%s] has [%i] indicies past [%i] vertices",
        zox_getn(e), bad_indicies, vertices->length);
    zox_tst_check(bone_indexes->length == vertices->length,
        "[%s] has [%i] bone indexes for [%i] vertices", zox_getn(e), bone_indexes->length, vertices->length);
    int bad_bones = 0;
    for (int i = 0; i < bone_indexes->length; i++) {
        bad_bones += bone_indexes->value[i] >= bones->length;
    }
    zox_tst_check(!bad_bones, "[%s] has [%i] vertices weighted past [%i] bones",
        zox_getn(e), bad_bones, bones->length);
}

// still building: no ActiveMesh picked and no mesh enabled yet (i.e. an npc spawned during the check)
byte zox_tst_skeleton_pending(ecs* world, entity skeleton) {
    if (zox_valid(zox_get_link(world, skeleton, ActiveMesh))) {
        return 0;
    }
    iter it2 = zox_children(world, skeleton);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (zox_has(mesh, SkeletonMesh) && !zox_has(mesh, Disabled)) {
                zox_children_end(it2);
                return 0;
            }
        }
    }
    return 1;
}

void zox_tst_skeleton_check_skeleton(ecs* world, entity skeleton) {
    // skeleton3_render_system needs bones with transforms
    zox_tst_check(zox_has(skeleton, BoneLinks), "[%s] has no BoneLinks", zox_getn(skeleton));
    if (!zox_has(skeleton, BoneLinks)) {
        return;
    }
    zox_geter(skeleton, BoneLinks, bones);
    zox_tst_check(bones->length, "[%s] has 0 bones", zox_getn(skeleton));
    for (int i = 0; i < bones->length; i++) {
        entity bone = bones->value[i];
        zox_tst_check(zox_valid(bone) && zox_has(bone, TransformMatrix),
            "[%s] bone [%i] invalid or has no TransformMatrix", zox_getn(skeleton), i);
    }
    // one enabled mesh, which is the ActiveMesh
    entity active = zox_get_link(world, skeleton, ActiveMesh);
    zox_tst_check(zox_valid(active), "[%s] has no ActiveMesh", zox_getn(skeleton));
    int enabled = 0;
    byte active_is_child = 0;
    iter it2 = zox_children(world, skeleton);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity mesh = it2.entities[j];
            if (!zox_has(mesh, SkeletonMesh)) {
                continue;
            }
            enabled += !zox_has(mesh, Disabled);
            active_is_child |= mesh == active;
        }
    }
    zox_tst_check(enabled == 1, "[%s] has [%i] enabled skeleton meshes, expected 1",
        zox_getn(skeleton), enabled);
    if (!zox_valid(active)) {
        return;
    }
    zox_tst_check(active_is_child, "[%s] ActiveMesh [%s] is not its child",
        zox_getn(skeleton), zox_getn(active));
    zox_tst_skeleton_check_mesh(world, skeleton, active);
}

void zox_tst_skeleton_check(ecs* world) {
    zox_tst_draws* draws = &zox_tst_skeleton_draws;
    const uint drawn = zox_drawn_skeleton_meshes - draws->baseline;
    zox_tst_check(drawn, "no skeleton meshes drawn in [%.0fs]", zox_tst_draws_window);
    entity material = transparent_meshes ? material_bonet : material_bone;
    zox_tst_check(zox_valid(material) && zox_getv(material, MaterialGPULink),
        "bone material invalid or has no MaterialGPULink");
    // every visible skeleton, once
    entity checked[zox_tst_skeleton_meshes_max];
    int checked_count = 0;
    int hidden = 0;
    int pending = 0;
    for (int i = 0; i < zox_tst_skeleton_meshes_count; i++) {
        if (!zox_valid(zox_tst_skeleton_meshes[i])) {
            continue;
        }
        entity skeleton = zox_get_parent(world, zox_tst_skeleton_meshes[i]);
        if (!zox_valid(skeleton)) {
            continue;
        }
        byte seen = 0;
        for (int j = 0; j < checked_count && !seen; j++) {
            seen = checked[j] == skeleton;
        }
        if (seen) {
            continue;
        }
        checked[checked_count++] = skeleton;
        // the owner (character) hides its skeleton, i.e. out of view distance
        entity owner = zox_get_parent(world, skeleton);
        if (zox_valid(owner) && zox_has(owner, RenderDisabled) && zox_getv(owner, RenderDisabled)) {
            hidden++;
            continue;
        }
        if (zox_tst_skeleton_pending(world, skeleton)) {
            pending++;
            continue;
        }
        zox_tst_skeleton_check_skeleton(world, skeleton);
    }
    zox_log("[tst] skeletons [%i] (hidden [%i], pending [%i]), meshes [%i], drawn [%u] in [%.0fs]",
        checked_count, hidden, pending, zox_tst_skeleton_meshes_count, drawn, zox_tst_draws_window);
    zox_tst_check(checked_count - hidden - pending, "no visible built skeletons");
}

zox_sys2(TstSkeletonProbeSystem) {
    if (!zox_tst_active || zox_tst_active->check != zox_tst_skeleton_check) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(RenderDisabled, disabled);
        if (zox_tst_skeleton_meshes_counting_count < zox_tst_skeleton_meshes_max) {
            zox_tst_skeleton_meshes_counting[zox_tst_skeleton_meshes_counting_count++] = e;
        }
        if (!disabled->value &&
            indicies->length &&
            !zox_has(e, Disabled) &&
            !zox_valid(zox_tst_skeleton_draws.candidate))
        {
            zox_tst_skeleton_draws.candidate = e;
        }
    }
} zox_sys_end(TstSkeletonProbeSystem);

void zox_tsts_skeleton_render(ecs* world) {
    zox_tst_add("skeleton_render", zox_tst_skeleton_ready, zox_tst_skeleton_check, 90);
    zox_system_1(
        TstSkeletonProbeSystem,
        zoxp_update,
        [in] rendering.MeshIndicies,
        [in] rendering.RenderDisabled,
        [none] rendering3.SkeletonMesh
    );
}
