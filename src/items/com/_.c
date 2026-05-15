zox_tag(ItemBlock);
// zoxc_userdata(Item);
zox_tag(Item);
zoxc_entity(ItemLink);
zoxc_entitiesw(ItemLinks);
zoxc_state(ItemDirty);
zoxc_state(QuantityDirty);

void define_components_items(ecs* world) {
    zoxd_tag(ItemBlock);
    // zoxd_userdata(Item);
    zoxd_tag(Item);
    zoxd_entity(ItemLink);
    zoxd_entitiesw(ItemLinks);
    zoxd_state(ItemDirty);
    zoxd_state(QuantityDirty);
}
