entity spawn_user_item_body(
    ecs *world,
    entity parent,
    entity base_item,
    byte slot)
{
    entity e = spawn_user_item(world, parent, base_item);
    zox_setv(e, SlotType, slot);
    zox_setv(e, BlockPosition, int3_zero);
    zox_setv(e, PartPosition, byte3_zero);
    return e;
}
