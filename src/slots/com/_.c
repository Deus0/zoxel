zox_tag(SlotManager);
zox_tag(Slot);
zoxc_entity(SlotLink);
zoxc_byte(SlotType);
zoxc_byte(SlotAnchor);
zoxc_entity(DataLink);
zoxc_state(DataDirty);

void zox_components_slots(ecs* world) {
    zoxd_tag(SlotManager);
    zoxd_tag(Slot);
    zoxd_entity(SlotLink);
    zoxd_byte(SlotType);
    zoxd_byte(SlotAnchor);
    zoxd_entity(DataLink);
    zoxd_state(DataDirty);
}
