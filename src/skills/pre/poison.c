// poison: a dot (Dot, Poison), a realm's (spawn_realm_debuff) an aura puts on who it touches (OnHitLinks), while they're in it
entity spawn_prefab_poison(ecs *world) {
    zox_prefab();
    zox_prefab_name("poison");
    zox_add(e, Dot);
    zox_add(e, Poison);
    zox_setv(e, Damage, 0);
    zox_setv(e, Rate, 1);
    zox_setv(e, DamageTime, 0);
    zox_setv(e, DestroyInTime, 0);
    return e;
}
