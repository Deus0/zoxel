// turns a character's head to look at a target (its head, if it has one), 0 looks ahead again
//  - i.e. both speakers in a dialogue, the player's camera turns with its head
void look_at(ecs* world, entity character, entity target) {
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    if (zox_valid(head)) {
        zox_setv(head, LookTarget, target);
    }
}

// where a character's eyes are, its head bone, else its position
float3 look_point(ecs* world, entity e) {
    const entity skeleton = get_skeleton(world, e);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    return zox_getv(zox_valid(head) ? head : e, Position3D);
}
