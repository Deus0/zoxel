// tst: gizmo_owner, a raycast gizmo is deleted with its owner (it is not a child, so it used to outlive a dead player)
//  - a gizmo links to its owner (GizmoOwnerLink, owned), the owner is deleted
//  - the gizmo must be gone, and an unowned gizmo is not touched
//  - run: bash bsh/tst.sh gizmo_owner
entity zox_tst_gizmo_owner_gizmo = 0;
entity zox_tst_gizmo_owner_other = 0;

byte zox_tst_gizmo_owner_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_gizmo_owner_check(ecs* world) {
    zox_defer_begin();
    const entity owner = zox_new();
    zox_add(owner, GizmoOwner);
    zox_tst_gizmo_owner_gizmo = zox_new();
    zox_link(world, owner, Gizmo, zox_tst_gizmo_owner_gizmo);
    zox_link(world, zox_tst_gizmo_owner_gizmo, GizmoOwnerLink, owner);
    zox_tst_gizmo_owner_other = zox_new();
    zox_delete(owner);
    zox_defer_end();
    zox_tst_check(!zox_alive(zox_tst_gizmo_owner_gizmo), "the gizmo outlived its owner");
    zox_tst_check(zox_alive(zox_tst_gizmo_owner_other), "an unowned gizmo was deleted");
    zox_delete_safe(zox_tst_gizmo_owner_other);
}

void zox_tsts_gizmo_owner(ecs* world) {
    zox_tst_add("gizmo_owner", zox_tst_gizmo_owner_ready, zox_tst_gizmo_owner_check, 30);
}
