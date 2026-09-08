void initialize_light_lock(
    ecs* world,
    entity e)
{
    spinlock llock;
    spinlock_init(&llock);
    zox_setv(e, LightLock, llock);
}
