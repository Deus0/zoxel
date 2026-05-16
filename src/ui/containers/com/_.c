zox_tag(SlotManager);
zox_tag(Slot);
zox_tag(DataFrame);
zoxc_entity(SlotLink);
zoxc_entity(DataLink);
zoxc_state(DataDirty);

void define_components_ui_containers(ecs* world) {
    zoxd_tag(SlotManager);
    zoxd_tag(Slot);
    zoxd_tag(DataFrame);
    zoxd_entity(SlotLink);
    zoxd_entity(DataLink);
    zoxd_state(DataDirty);
}
