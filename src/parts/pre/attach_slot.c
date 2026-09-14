// NOTE: Base slot for body and equipment
entity spawn_attach_slot(
    ecs* world,
    entity parent,
    byte anchor)
{
    entity e = spawn_slot(
        world,
        parent);
    zox_set_unique_name(e, "slot_attach");
    zox_setv(e, PartPosition, byte3_zero);
    zox_setv(e, PartSize, byte3_zero);
    zox_setv(e, PartOffset, int3_zero);
    zox_setv(e, SlotAnchor, anchor);
    return e;
}
