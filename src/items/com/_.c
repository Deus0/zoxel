zox_tag(Item);
zox_tag(ItemBlock);
zox_tag(ItemVox);
zox_tag(ItemConsumable);
zoxc_state(ItemDirty);
zoxc_byte(Quantity);
zox_tag(QuantityDirty);
// Consumables
zoxc_float(ConsumeValue);   // amount added to the consumed stat
// Links
zox_tag(ItemLink);
zox_tag(ConsumeStat);       // links consumable to the realm stat it restores

void zox_components_items(ecs* world) {
    zoxd_tag(Item);
    zoxd_tag(ItemBlock);
    zoxd_tag(ItemVox);
    zoxd_tag(ItemConsumable);
    zoxd_state(ItemDirty);
    zoxd_byte(Quantity);
    zoxd_tag(QuantityDirty);
    // Consumables
    zoxd_float(ConsumeValue);
    // Links
    zoxd_nf_link(ItemLink);
    zoxd_nf_link(ConsumeStat);
}
