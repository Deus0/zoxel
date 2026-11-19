zoxc_float(ConsumeValue);   // amount added to the consumed stat
zox_tag(ConsumeStat);       // links a consumable to the realm stat it restores

void zox_components_consumables(ecs* world) {
    zoxd_float(ConsumeValue);
    zoxd_nf_link(ConsumeStat);
}
