zox_tag(ItemBlock);
zoxc_userdata(Item);
zoxc_byte(ItemDirty);
zoxc_byte(QuantityDirty);

void define_components_items(ecs* world) {
    zoxd_tag(ItemBlock);
    zoxd_userdata(Item);
    zoxd_byte(ItemDirty);
    zoxd_byte(QuantityDirty);
}