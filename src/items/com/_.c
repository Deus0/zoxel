zox_tag(Item);
zox_tag(ItemBlock);
zoxc_entity(ItemLink);
zoxc_entitiesw(ItemLinks);
zoxc_state(ItemDirty);
zoxc_byte(Quantity);
zoxc_state(QuantityDirty);

void define_components_items(ecs* world) {
    zoxd_tag(Item);
    zoxd_tag(ItemBlock);
    zoxd_entity(ItemLink);
    zoxd_entitiesw(ItemLinks);
    zoxd_state(ItemDirty);
    zoxd_byte(Quantity);
    zoxd_state(QuantityDirty);
}
