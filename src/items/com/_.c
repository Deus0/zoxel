zox_tag(Item);
zox_tag(ItemBlock);
zox_tag(ItemVox);
zox_tag(ItemConsumable);
zoxc_byte(Quantity);
zox_tag(QuantityDirty);
// Links
zox_tag(ItemLink);

void zox_components_items(ecs* world) {
    zoxd_tag(Item);
    zoxd_tag(ItemBlock);
    zoxd_tag(ItemVox);
    zoxd_tag(ItemConsumable);
    zoxd_byte(Quantity);
    zoxd_tag(QuantityDirty);
    // Links
    zoxd_nf_link(ItemLink);
}
