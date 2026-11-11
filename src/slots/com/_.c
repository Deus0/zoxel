zox_tag(SlotManager);
zox_tag(Slot);
zox_tag(SlotUser);
zox_tag(SlotLink);
zoxc_byte(SlotType);
zoxc_byte(SlotAnchor);
zox_tag(DataDirty);
zox_tag(DataLink);         // links a slot, or a UI, to its data, i.e. an item
zox_tag(DataUpdate);

// When data is unlinked, replaced or deleted, refresh its UI
void on_unlink_DataLink(ecs* world, entity e, entity data) {
    if (!zox_valid(e)) {
        return;
    }
    zox_add(e, DataDirty);
}

void zox_components_slots(ecs* world) {
    zoxd_tag(SlotManager);
    zoxd_tag(Slot);
    zoxd_tag(SlotUser);
    zoxd_nf_link(SlotLink);
    zoxd_byte(SlotType);
    zoxd_byte(SlotAnchor);
    zoxd_tag_event(DataDirty);
    zoxd_tag_event(DataUpdate);
    zoxd_nf_link(DataLink);
    zox_on_unlink(on_unlink_DataLink, DataLink);
}
