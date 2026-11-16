// tst: transform_hierarchy, children follow their parents (transforms3/sys/children.c)
//  - a root (turned, moved, scaled 2), a child (local position, turn), a grandchild (local position, scale 0.5)
//  - a few frames later: each's Position3D is its parent's plus its local position turned by and scaled by its
//      parent's; its Rotation3D its parent's then its own; its Scale1 its parent's times its own
//  - TransformMatrix (zoxp_transform_end) moves a point as Position3D, Rotation3D and Scale1 do
//  - moving the root moves both the next frames
//  - an IgnoreParentRotation child keeps its own rotation, its position still turned
//  - run: bash bsh/tst.sh transform_hierarchy
entity zox_tst_th_root = 0, zox_tst_th_child = 0, zox_tst_th_grandchild = 0, zox_tst_th_ignore = 0;
int zox_tst_th_frames = 0;
byte zox_tst_th_stage = 0;

static inline entity zox_tst_th_spawn(ecs* world, entity parent, float3 local, float4 rotation, float scale) {
    const entity e = zox_new();
    zox_set(e, Position3D, { parent ? float3_zero : local });
    zox_set(e, Rotation3D, { parent ? quaternion_identity : rotation });
    zox_set(e, Scale1, { scale });
    zox_set(e, TransformMatrix, { float4x4_identity });
    if (parent) {
        zox_set(e, LocalPosition3D, { local });
        zox_set(e, LocalRotation3D, { rotation });
        zox_set(e, LocalScale1, { scale });
        zox_set_parent(world, e, parent);
    }
    return e;
}

byte zox_tst_transform_hierarchy_ready(ecs* world) {
    zox_tst_waiting = "a few frames of transforms";
    zox_tst_th_frames++;
    if (!zox_tst_th_root) {
        zox_tst_th_root = zox_tst_th_spawn(world, 0, (float3) { 10, 2, -4 }, quaternion_from_axis_angle(float3_up, M_PI / 2), 2);
        zox_tst_th_child = zox_tst_th_spawn(world, zox_tst_th_root, (float3) { 1, 0, 0 }, quaternion_from_axis_angle(float3_right, M_PI / 2), 1);
        zox_tst_th_grandchild = zox_tst_th_spawn(world, zox_tst_th_child, (float3) { 0, 0, 3 }, quaternion_identity, 0.5f);
        zox_tst_th_ignore = zox_tst_th_spawn(world, zox_tst_th_root, (float3) { 0, 0, 1 }, quaternion_identity, 1);
        zox_add(zox_tst_th_ignore, IgnoreParentRotation);
        zox_tst_th_frames = 0;
        return 0;
    }
    return zox_tst_th_frames > 5;
}

// e's world transform from its parent's, as the hierarchy should
static inline void zox_tst_th_expect(ecs* world, entity e, entity parent, float3* position, float4* rotation, float* scale) {
    const float3 pp = zox_getv(parent, Position3D);
    const float4 pr = zox_getv(parent, Rotation3D);
    const float ps = zox_getv(parent, Scale1);
    *position = float3_add(pp, float4_rotate_float3(pr, float3_scale(zox_getv(e, LocalPosition3D), ps)));
    *rotation = quaternion_rotate(pr, zox_getv(e, LocalRotation3D));
    *scale = ps * zox_getv(e, LocalScale1);
}

static inline void zox_tst_th_check_one(ecs* world, entity e, entity parent, const char* label) {
    float3 position;
    float4 rotation;
    float scale;
    zox_tst_th_expect(world, e, parent, &position, &rotation, &scale);
    const float3 p = zox_getv(e, Position3D);
    const float4 r = zox_getv(e, Rotation3D);
    zox_tst_check(float3_length(float3_subtract(p, position)) < 1e-4f, "[%s] at [%.3f %.3f %.3f], not [%.3f %.3f %.3f]",
        label, p.x, p.y, p.z, position.x, position.y, position.z);
    zox_tst_check(zox_tst_maths_error4(r, rotation) < 1e-4f, "[%s] rotation off by [%f]", label, zox_tst_maths_error4(r, rotation));
    zox_tst_check(fabsf(zox_getv(e, Scale1) - scale) < 1e-5f, "[%s] scale [%f], not [%f]", label, zox_getv(e, Scale1), scale);
    // its matrix moves a point as its transform does
    const float3 v = { 0.3f, -1.2f, 2 };
    const float3 by_matrix = float4x4_multiply_float3(zox_getv(e, TransformMatrix), v);
    const float3 by_transform = float3_add(p, float3_scale(float4_rotate_float3(r, v), zox_getv(e, Scale1)));
    zox_tst_check(float3_length(float3_subtract(by_matrix, by_transform)) < 1e-3f, "[%s] TransformMatrix moves a point to [%.3f %.3f %.3f], its transform to [%.3f %.3f %.3f]",
        label, by_matrix.x, by_matrix.y, by_matrix.z, by_transform.x, by_transform.y, by_transform.z);
}

void zox_tst_transform_hierarchy_check(ecs* world) {
    // known answers: the root turned a quarter about y, scaled 2: the child's local x 1 is at x 10, z -4 - 2
    const float3 child = zox_getv(zox_tst_th_child, Position3D);
    zox_log("[tst] [metric] transform_hierarchy child at [%.3f %.3f %.3f]", child.x, child.y, child.z);
    zox_tst_check(float3_length(float3_subtract(child, (float3) { 10, 2, -6 })) < 1e-4f, "the child is at [%.3f %.3f %.3f], not [10 2 -6]",
        child.x, child.y, child.z);
    zox_tst_th_check_one(world, zox_tst_th_child, zox_tst_th_root, "child");
    zox_tst_th_check_one(world, zox_tst_th_grandchild, zox_tst_th_child, "grandchild");
    // ignores its parent's rotation: keeps its own, its position still turned by it
    const float4 own = zox_getv(zox_tst_th_ignore, Rotation3D);
    zox_tst_check(zox_tst_maths_error4(own, quaternion_identity) < 1e-4f, "the IgnoreParentRotation child turned with its parent");
    zox_delete(zox_tst_th_root);
}

// tst: euler_limits, Euler clamped to EulerLimitX / EulerLimitZ, then made its Rotation3D (EulerOverride)
//  - three entities: over the limits, under them, inside them; a few frames later each is clamped (or kept) and
//      its Rotation3D is quaternion_from_euler of it
//  - run: bash bsh/tst.sh euler_limits
entity zox_tst_el[3];
int zox_tst_el_frames = 0;
const float3 zox_tst_el_start[3] = { { 2, 0.3f, 2 }, { -2, 0.3f, -2 }, { 0.2f, 0.3f, -0.1f } };
const float3 zox_tst_el_expect[3] = { { 1, 0.3f, 0.5f }, { -1, 0.3f, -0.5f }, { 0.2f, 0.3f, -0.1f } };

byte zox_tst_euler_limits_ready(ecs* world) {
    zox_tst_waiting = "a few frames of euler limits";
    if (!zox_tst_el[0]) {
        for (int i = 0; i < 3; i++) {
            const entity e = zox_new();
            zox_set(e, Euler, { zox_tst_el_start[i] });
            zox_set(e, EulerLimitX, { { -1, 1 } });
            zox_set(e, EulerLimitZ, { { -0.5f, 0.5f } });
            zox_set(e, Rotation3D, { quaternion_identity });
            zox_add(e, EulerOverride);
            zox_tst_el[i] = e;
        }
        return 0;
    }
    return ++zox_tst_el_frames > 5;
}

void zox_tst_euler_limits_check(ecs* world) {
    const char* labels[3] = { "over", "under", "inside" };
    for (int i = 0; i < 3; i++) {
        const float3 euler = zox_getv(zox_tst_el[i], Euler);
        zox_tst_check(float3_length(float3_subtract(euler, zox_tst_el_expect[i])) < 1e-5f, "[%s] Euler [%.3f %.3f %.3f], not [%.3f %.3f %.3f]",
            labels[i], euler.x, euler.y, euler.z, zox_tst_el_expect[i].x, zox_tst_el_expect[i].y, zox_tst_el_expect[i].z);
        const float error = zox_tst_maths_error4(zox_getv(zox_tst_el[i], Rotation3D), quaternion_from_euler(euler));
        zox_tst_check(error < 1e-5f, "[%s] Rotation3D off its Euler by [%f]", labels[i], error);
        zox_delete(zox_tst_el[i]);
    }
}
