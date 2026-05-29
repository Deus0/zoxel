zox_tag(EquipItem);
zox_tag(EquipSlot);
zox_tag(HatSlot);
zox_tag(MenuEquipment);
zox_tag(EquipFrame);
zox_tag(EquipIcon);

void add_tag_equip_slot(ecs* world, entity e) {
    zox_add_tag(e, EquipSlot);
}

void add_tag_hat_slot(ecs* world, entity e) {
    zox_add_tag(e, HatSlot);
}

void define_components_equipment(ecs* world) {
    zoxd_tag(EquipItem);
    zoxd_tag(EquipSlot);
    zoxd_tag(HatSlot);
    zoxd_tag(MenuEquipment);
    zoxd_tag(EquipFrame);
    zoxd_tag(EquipIcon);
}
