// tst: transform3_scale_hierarchy, XYZ scale affects offsets before rotation, with explicit inheritance opt-outs.
static byte zox_tst_transform3_near(float3 actual, float3 expected) {
    return float3_length(float3_subtract(actual, expected)) < 1e-4f;
}

static entity zox_tst_transform3_scaled_child(ecs* world, entity parent, float3 local, float3 scale) {
    const entity child = zox_new();
    zox_setv(child, Position3D, float3_zero);
    zox_setv(child, Rotation3D, quaternion_identity);
    zox_setv(child, LocalPosition3D, local);
    zox_setv(child, LocalScale3, scale);
    zox_setv(child, Scale3, float3_one);
    zox_set_parent_non_fragment(world, child, parent);
    return child;
}

static void zox_tst_transform3_scale_hierarchy_check(ecs* world) {
    const entity root = zox_new();
    const float3 origin = { 10, 20, 30 };
    zox_setv(root, Position3D, origin);
    zox_setv(root, Rotation3D, quaternion_from_axis_angle(float3_up, M_PI / 2));
    zox_setv(root, Scale1, 99);
    zox_setv(root, Scale3, ((float3) { 2, 3, 4 }));

    const float3 local_scale = { 0.5f, 2, 0.25f };
    const entity scaled = zox_tst_transform3_scaled_child(world, root, (float3) { 1, 2, 3 }, local_scale);
    const entity own_scale = zox_tst_transform3_scaled_child(world, root, float3_one, local_scale);
    zox_add(own_scale, DisableParentScale);

    const entity independent = zox_tst_transform3_scaled_child(world, root, (float3) { 1, 1, 4 }, float3_one);
    const float3 independent_position = { 40, 50, 60 };
    const float3 independent_scale = { 3, 2, 1 };
    zox_add(independent, DisableParentTransform);
    zox_setv(independent, Position3D, independent_position);
    zox_setv(independent, Scale3, independent_scale);
    const entity nested = zox_tst_transform3_scaled_child(world, independent, (float3) { 1, -2, 3 }, float3_one);

    const float3 moves[] = { { 0, 0, 0 }, { -5, 1, 8 } };
    for (int i = 0; i < 2; i++) {
        zox_setv(root, Position3D, float3_add(origin, moves[i]));
        zox_run(world, zox_id(transform3_children_system));
        zox_tst_check(zox_tst_transform3_near(zox_getv(scaled, Position3D), float3_add(((float3) { 22, 26, 28 }), moves[i])),
            "XYZ scale and yaw produced the wrong child offset at sample [%i]", i);
        zox_tst_check(zox_tst_transform3_near(zox_getv(own_scale, Position3D), float3_add(((float3) { 14, 23, 28 }), moves[i])),
            "DisableParentScale changed how the child's offset followed the parent");
        zox_tst_check(float3_equals(zox_getv(independent, Position3D), independent_position) &&
            float3_equals(zox_getv(independent, Scale3), independent_scale), "DisableParentTransform let the root change an independent transform");
        zox_tst_check(zox_tst_transform3_near(zox_getv(nested, Position3D), ((float3) { 43, 46, 63 })),
            "the grandchild did not follow its independent parent's transform");
    }
    zox_tst_check(float3_equals(zox_getv(scaled, Scale3), ((float3) { 1, 6, 1 })), "the local and parent XYZ scales did not compose");
    zox_tst_check(float3_equals(zox_getv(own_scale, Scale3), local_scale), "DisableParentScale did not preserve the child's own scale");
    zox_tst_check(float3_equals(zox_getv(nested, Scale3), independent_scale), "the grandchild did not inherit its independent parent's scale");
    zox_tst_check(zox_tst_transform3_near(float4_rotate_float3(zox_getv(scaled, Rotation3D), float3_right), ((float3) { 0, 0, -1 })) &&
        zox_tst_maths_error4(zox_getv(independent, Rotation3D), quaternion_identity) < 1e-5f,
        "the inherited and independent rotations did not follow their settings");
}
