// tst: prefab_variants, a prefab of a prefab (a realm's character prefab of the skeleton prefab) passes
//  on its base's children (the skeleton prefab's body slot) to its instances
//  - instance first, then made a prefab (spawn_character3_meta did): its instances lost the body slot,
//      humanoid npcs had no chest slot ("No chest slot on character"), the player crashed equipping
//  - made a prefab first, then its base linked (zox_prefab_variant): they keep it
//  - a variant with a non fragmenting child of its own (a character prefab with its stats): its instances
//      get only that child, not the base's ChildOf slot (flecs doesn't mix them, humanoids lost their bodies)
//  - with the base's slot non fragmenting too (one kind through the chain): its instances get both
//  - (made an instance then a prefab, adding a non fragmenting child asserted in flecs too)
//  - run: bash bsh/tst.sh prefab_variants
zox_tag(TstVariantSlot);

void zox_components_tst_prefab_variants(ecs* world) {
    zoxd_tag(TstVariantSlot);
}

byte zox_tst_prefab_variants_ready(ecs* world) {
    (void) world;
    return 1;
}

static inline byte zox_tst_has_slot_child(ecs* world, entity e) {
    return zox_valid(zox_get_child_by_id(world, e, zox_id(TstVariantSlot)));
}

void zox_tst_prefab_variants_check(ecs* world) {
    // the base, like the skeleton prefab, with its body slot child (a prefab child)
    const entity base = zox_new();
    zox_make_prefab(base);
    const entity slot = zox_new();
    zox_make_prefab(slot);
    zox_add(slot, TstVariantSlot);
    zox_set_parent(world, slot, base);
    // instance first, then made a prefab (like spawn_character3_meta)
    const entity late = zox_ins(world, base);
    zox_make_prefab(late);
    // a prefab first, then its base (zox_prefab_variant)
    const entity early = zox_prefab_variant(world, base);
    const entity from_base = zox_ins(world, base);
    const entity from_late = zox_ins(world, late);
    const entity from_early = zox_ins(world, early);
    zox_log("[tst] the slot on: an instance of the base [%i], of a late variant [%i], of an early variant [%i]",
        zox_tst_has_slot_child(world, from_base), zox_tst_has_slot_child(world, from_late), zox_tst_has_slot_child(world, from_early));
    zox_tst_check(zox_tst_has_slot_child(world, from_base), "an instance of the base didn't get its slot");
    zox_tst_check(zox_tst_has_slot_child(world, from_early), "an instance of a prefab variant didn't get its base's slot");
    // a variant with a non fragmenting child of its own (a realm character prefab with its stats)
    const entity with_stat = zox_prefab_variant(world, base);
    const entity stat = zox_new();
    zox_set_parent_non_fragment(world, stat, with_stat);
    const entity from_with_stat = zox_ins(world, with_stat);
    const int stats = zox_get_children_count(world, from_with_stat);
    zox_log("[tst] an instance of a variant with a non fragmenting child: the base's slot [%i], children [%i]",
        zox_tst_has_slot_child(world, from_with_stat), stats);
    // flecs: an instance gets only the variant's own non fragmenting children, not its base's ChildOf ones
    zox_tst_check(!zox_tst_has_slot_child(world, from_with_stat) && stats == 1,
        "mixing kinds of prefab children works now ([%i] children), doc/children.md can say so", stats);
    zox_delete(from_with_stat);
    zox_delete(with_stat);
    // the same, the base's slot non fragmenting too (one kind of children through the prefab chain)
    const entity base_nf = zox_new();
    zox_make_prefab(base_nf);
    const entity slot_nf = zox_new();
    zox_make_prefab(slot_nf);
    zox_add(slot_nf, TstVariantSlot);
    zox_set_parent_non_fragment(world, slot_nf, base_nf);
    const entity variant_nf = zox_prefab_variant(world, base_nf);
    const entity stat_nf = zox_new();
    zox_set_parent_non_fragment(world, stat_nf, variant_nf);
    const entity from_nf = zox_ins(world, variant_nf);
    zox_log("[tst] the same, the base's slot non fragmenting too: the base's slot [%i], children [%i]",
        zox_tst_has_slot_child(world, from_nf), zox_get_children_count(world, from_nf));
    zox_tst_check(zox_tst_has_slot_child(world, from_nf) && zox_get_children_count(world, from_nf) == 2,
        "with one kind of children through the prefab chain, an instance didn't get both");
    zox_delete(from_nf);
    zox_delete(variant_nf);
    zox_delete(base_nf);
    zox_delete(from_base);
    zox_delete(from_late);
    zox_delete(from_early);
    zox_delete(late);
    zox_delete(early);
    zox_delete(base);
}
