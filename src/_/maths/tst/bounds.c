// tst: maths_bounds, boxes (src/_/maths/bounds): points in them, rays through them, pure logic
//  - point_in_aabb: inside, on the faces, just outside each face
//  - ray_intersects_aabb: rays from outside at the box's middle hit it at its face's distance; rays aimed away,
//      or passing beside it, miss; a ray from inside hits (its entry clamped to 0)
//  - run: bash bsh/tst.sh maths_bounds
void zox_tst_maths_bounds_check(ecs* world) {
    (void) world;
    zox_tst_maths_seed = 777;
    int inside_wrong = 0, outside_wrong = 0, hit_wrong = 0, miss_wrong = 0, from_inside_wrong = 0;
    float distance_worst = 0;
    for (int i = 0; i < zox_tst_maths_samples; i++) {
        const float3 extents = { 0.5f + zox_tst_maths_rand() * 0.4f + 0.4f, 0.5f + zox_tst_maths_rand() * 0.4f + 0.4f, 0.5f + zox_tst_maths_rand() * 0.4f + 0.4f };
        const bounds box = { .center = zox_tst_maths_rand3(20), .extents = extents };
        // inside, at 90% of its extents
        const float3 in = float3_add(box.center, (float3) { extents.x * 0.9f * zox_tst_maths_rand(), extents.y * 0.9f * zox_tst_maths_rand(), extents.z * 0.9f * zox_tst_maths_rand() });
        inside_wrong += !point_in_aabb(in, box);
        // past a face by 1%
        for (byte axis = 0; axis < 3; axis++) {
            float3 out = box.center;
            float* o = (float*) &out;
            const float* e = (const float*) &extents;
            o[axis] += (i % 2 ? 1 : -1) * e[axis] * 1.01f;
            outside_wrong += point_in_aabb(out, box);
        }
        // a ray from 5 to 10 away, along an axis, at the middle
        const byte axis = i % 3;
        float3 direction = float3_zero;
        ((float*) &direction)[axis] = i % 2 ? 1 : -1;
        const float away = 5 + (zox_tst_maths_rand() + 1) * 2.5f;
        const float3 origin = float3_subtract(box.center, float3_scale(direction, away));
        // nudged off the axis, so no direction is exactly 0
        const float3 aimed = float3_normalize(float3_add(direction, (float3) { 1e-4f, 1e-4f, 1e-4f }));
        float tmin = 0, tmax = 0;
        if (!ray_intersects_aabb(origin, aimed, box, &tmin, &tmax)) {
            hit_wrong++;
        } else {
            const float face = away - ((const float*) &extents)[axis];
            const float d = fabsf(tmin - face);
            distance_worst = d > distance_worst ? d : distance_worst;
        }
        // aimed away
        miss_wrong += ray_intersects_aabb(origin, float3_scale(aimed, -1), box, &tmin, &tmax);
        // beside it: moved 3 extents off on another axis
        float3 beside = origin;
        ((float*) &beside)[(axis + 1) % 3] += 3 * 2;
        miss_wrong += ray_intersects_aabb(beside, aimed, box, &tmin, &tmax);
        // from inside
        from_inside_wrong += !ray_intersects_aabb(in, aimed, box, &tmin, &tmax) || tmin != 0;
    }
    zox_log("[tst] [metric] maths_bounds hits off by up to [%f]", distance_worst);
    zox_tst_check(!inside_wrong, "[%i] points inside a box weren't in it", inside_wrong);
    zox_tst_check(!outside_wrong, "[%i] points past a face were in the box", outside_wrong);
    zox_tst_check(!hit_wrong, "[%i] rays at a box's middle missed it", hit_wrong);
    zox_tst_check(distance_worst < 1e-3f, "a ray's hit was [%f] off its face", distance_worst);
    zox_tst_check(!miss_wrong, "[%i] rays aimed away or beside a box hit it", miss_wrong);
    zox_tst_check(!from_inside_wrong, "[%i] rays from inside a box didn't hit it at 0", from_inside_wrong);
}
