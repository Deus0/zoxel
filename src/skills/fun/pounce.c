// a melee user without a skeleton (a slime) pounces, not swings: it squashes, springs at its aim and hits what it
//  lands on (sys/pounce.c); its shape a Scale3 while it does
void pounce_begin(ecs* world, entity user, entity skill) {
    if (zox_has(user, PounceStart)) {
        return;
    }
    zox_set(user, PounceStart, { zox_current_time });
    zox_set(user, PounceStage, { 0 });
    zox_set(user, PounceSkill, { skill });
    zox_set(user, Scale3, { float3_one });
}
