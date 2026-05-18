zox_tag(Item);
zoxc_entity(ItemLink);
zoxc_entitiesw(ItemLinks);
zox_tag(ItemBlock);
zoxc_state(ItemDirty);

void define_components_items(ecs* world) {
    zoxd_tag(Item);
    zoxd_entity(ItemLink);
    zoxd_entitiesw(ItemLinks);
    zoxd_tag(ItemBlock);
    zoxd_state(ItemDirty);
}
