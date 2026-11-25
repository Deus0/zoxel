// tst: transform2_hierarchy, nonuniform scale and local rotation compose through nested nodes and line endpoints.
static byte zox_tst_transform2_near(float2 actual, float2 expected) {
    return fabsf(actual.x - expected.x) < 1e-4f && fabsf(actual.y - expected.y) < 1e-4f;
}

static entity zox_tst_transform2_child(ecs* world, entity parent, float2 local) {
    const entity child = zox_new();
    zox_setv(child, Position2, float2_zero);
    zox_setv(child, LocalPosition2, local);
    zox_setv(child, Rotation2, 0);
    zox_set_parent_non_fragment(world, child, parent);
    return child;
}

static void zox_tst_transform2_hierarchy_check(ecs* world) {
    const entity root = zox_new();
    const float2 origin = { 10, 20 };
    zox_setv(root, Position2, origin);
    zox_setv(root, Rotation2, M_PI / 2);
    zox_setv(root, Scale2, ((float2) { 2, 3 }));
    zox_setv(root, Scale1, 99);
    const entity child = zox_tst_transform2_child(world, root, (float2) { 2, 1 });
    zox_setv(child, LocalRotation2, -M_PI / 2);
    zox_setv(child, LocalScale2, ((float2) { 2, 0.5f }));
    zox_setv(child, Scale2, float2_one);
    const entity grandchild = zox_tst_transform2_child(world, child, (float2) { 1, -2 });
    zox_setv(grandchild, LocalScale1, 2);
    zox_setv(grandchild, Scale1, 1);

    const entity line = zox_new();
    zox_set(line, LinePoints2, { 0 });
    zox_set(line, LocalLinePoints2, { { 1, 2 }, { -1, 0 } });
    zox_set_parent(world, line, child);

    const entity independent = zox_tst_transform2_child(world, root, (float2) { 4, 2 });
    const float2 independent_position = { 40, 50 };
    zox_setv(independent, Position2, independent_position);
    zox_add(independent, DisableParentTransform);

    const float2 moves[] = { { 0, 0 }, { 5, -6 } };
    for (int i = 0; i < 2; i++) {
        zox_setv(root, Position2, float2_add(origin, moves[i]));
        zox_run(world, zox_id(transform2_children_system));
        zox_tst_check(zox_tst_transform2_near(zox_getv(child, Position2), float2_add(((float2) { 7, 24 }), moves[i])),
            "child position did not compose the root's translation, rotation and XY scale at sample [%i]", i);
        zox_tst_check(zox_tst_transform2_near(zox_getv(grandchild, Position2), float2_add(((float2) { 11, 21 }), moves[i])),
            "grandchild position did not follow both transforms at sample [%i]", i);
        const LinePoints2* endpoints = zox_get(line, LinePoints2);
        zox_tst_check(zox_tst_transform2_near(endpoints->start, float2_add(((float2) { 11, 27 }), moves[i])) &&
            zox_tst_transform2_near(endpoints->end, float2_add(((float2) { 3, 24 }), moves[i])),
            "line endpoints did not follow the child at sample [%i]", i);
        zox_tst_check(float2_equals(zox_getv(independent, Position2), independent_position),
            "DisableParentTransform let the parent move the independent element");
    }
    zox_tst_check(fabsf(zox_getv(child, Rotation2)) < 1e-5f && fabsf(zox_getv(grandchild, Rotation2)) < 1e-5f,
        "the child's local quarter turn did not cancel the parent's");
    zox_tst_check(float2_equals(zox_getv(child, Scale2), ((float2) { 4, 1.5f })) && zox_getv(grandchild, Scale1) == 3,
        "the XY and uniform scales did not compose through the hierarchy");
}
