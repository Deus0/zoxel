entity spawn_body_slot(
    ecs* world,
    entity parent,
    byte anchor)
{
    entity e = spawn_attach_slot(
        world,
        parent,
        anchor);
    zox_set_unique_name(e, "body_slot");
    zox_add(e, BodySlot);
    return e;
}
