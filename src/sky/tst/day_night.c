// tst: day_night, a realm's sun lights its cameras by the realm's time (sys/camera_sky.c, doc/plans/day_night.md)
//  - a realm with a sun (spawn_sun, its game spawns it as the realm generates: gam/zoxel/sys/solar.c)
//  - a camera in it (its streamer's terrain's realm): noon full daylight from above, dawn dimmer, its sunset's
//    colour, low in the east,
//    midnight the night's light with the sun under the ground
//  - paused (its clock's scale 0) the time of day holds
//  - no player, no game: a realm, a terrain linking it, a camera streaming from that terrain
//  - run: bash bsh/tst.sh day_night
int zox_tst_day_night_frame = 0;
entity zox_tst_day_night_realm = 0;
entity zox_tst_day_night_camera = 0;
uint zox_tst_day_night_suns = 0;
float3 zox_tst_day_night_dawn_color;
const color_rgb zox_tst_day_night_sunset = { 255, 60, 200 };     // pink, its sun's sunset
float3 zox_tst_day_night_noon;
float3 zox_tst_day_night_noon_sun;
float3 zox_tst_day_night_dawn;
float3 zox_tst_day_night_dawn_sun;
float3 zox_tst_day_night_night;
float3 zox_tst_day_night_night_sun;
double zox_tst_day_night_held = -1;
double zox_tst_day_night_later = -2;

// the realm's clock at a time of day, by its sun's day
static inline void zox_tst_day_night_at(ecs* world, float time_of_day) {
    zox_setv(zox_tst_day_night_realm, ClockTime, (time_of_day - sun_day_start + 1) * sun_day_length);
}

byte zox_tst_day_night_ready(ecs* world) {
    const int f = ++zox_tst_day_night_frame;
    zox_tst_progress(f);
    const entity camera = zox_tst_day_night_camera;
    if (f == 1) {
        const entity realm = zox_ins(world, prefab_realm);
        zox_setv(realm, ClockScale, 0);     // paused: the time only moves as set here
        const entity terrain = zox_new();
        zox_link(world, terrain, RealmLink, realm);
        const entity e = zox_new();
        zox_add(e, Camera);
        zox_setv(e, SkyTint, float3_one);
        zox_setv(e, SunDirection, float3_up);
        zox_setv(e, SunColor, float3_one);
        zox_link(world, e, StreamLink, terrain);
        spawn_sun(world, realm, zox_tst_day_night_sunset);
        zox_tst_day_night_realm = realm;
        zox_tst_day_night_camera = e;
        entity suns[4];
        zox_tst_day_night_suns = zox_get_children_by_id(world, realm, suns, 4, zox_id(Sun));
        zox_tst_day_night_at(world, 0.5f);
    } else if (f == 4) {
        zox_tst_day_night_noon = zox_getv(camera, SkyTint);
        zox_tst_day_night_noon_sun = zox_getv(camera, SunDirection);
        zox_tst_day_night_at(world, 0.27f);
    } else if (f == 7) {
        zox_tst_day_night_dawn = zox_getv(camera, SkyTint);
        zox_tst_day_night_dawn_sun = zox_getv(camera, SunDirection);
        zox_tst_day_night_dawn_color = zox_getv(camera, SunColor);
        zox_tst_day_night_at(world, 0);
    } else if (f == 10) {
        zox_tst_day_night_night = zox_getv(camera, SkyTint);
        zox_tst_day_night_night_sun = zox_getv(camera, SunDirection);
        zox_tst_day_night_held = zox_getv(zox_tst_day_night_realm, ClockTime);
    } else if (f == 40) {
        zox_tst_day_night_later = zox_getv(zox_tst_day_night_realm, ClockTime);
    }
    zox_tst_waiting = "the camera's sky through the day";
    return f >= 40;
}

void zox_tst_day_night_check(ecs* world) {
    (void) world;
    const float3 n = zox_tst_day_night_noon, d = zox_tst_day_night_dawn, m = zox_tst_day_night_night;
    zox_log("[tst] day_night: noon [%.2f %.2f %.2f] sun up [%.2f], dawn [%.2f %.2f %.2f] sun [%.2f %.2f], night [%.2f %.2f %.2f] sun up [%.2f]",
        n.x, n.y, n.z, zox_tst_day_night_noon_sun.y, d.x, d.y, d.z, zox_tst_day_night_dawn_sun.x, zox_tst_day_night_dawn_sun.y,
        m.x, m.y, m.z, zox_tst_day_night_night_sun.y);
    zox_tst_check(zox_tst_day_night_suns == 1, "the realm has [%u] suns, not its one", zox_tst_day_night_suns);
    // dawn's sun its sunset's colour (pink): its green the least
    const float3 c = zox_tst_day_night_dawn_color;
    zox_tst_check(c.y < c.x && c.y < c.z, "dawn's sun [%.2f %.2f %.2f] isn't its sunset's pink", c.x, c.y, c.z);
    zox_tst_check(n.x > 0.85f && n.y > 0.85f && n.z > 0.8f, "noon isn't full daylight [%.2f %.2f %.2f]", n.x, n.y, n.z);
    zox_tst_check(zox_tst_day_night_noon_sun.y > 0.8f, "the noon sun isn't high [%.2f]", zox_tst_day_night_noon_sun.y);
    zox_tst_check(d.x + d.y + d.z < n.x + n.y + n.z, "dawn [%.2f %.2f %.2f] isn't dimmer than noon", d.x, d.y, d.z);
    zox_tst_check(zox_tst_day_night_dawn_sun.x > 0.9f && zox_tst_day_night_dawn_sun.y > 0 && zox_tst_day_night_dawn_sun.y < 0.3f,
        "the dawn sun isn't low in the east [%.2f %.2f]", zox_tst_day_night_dawn_sun.x, zox_tst_day_night_dawn_sun.y);
    zox_tst_check(fabsf(m.x - sky_night_tint.x) < 0.01f && fabsf(m.y - sky_night_tint.y) < 0.01f && fabsf(m.z - sky_night_tint.z) < 0.01f,
        "midnight isn't the night's light [%.2f %.2f %.2f]", m.x, m.y, m.z);
    zox_tst_check(zox_tst_day_night_night_sun.y < -0.8f, "the midnight sun isn't under the ground [%.2f]", zox_tst_day_night_night_sun.y);
    zox_tst_check(zox_tst_day_night_held == zox_tst_day_night_later, "paused, the realm's time moved [%.2f to %.2f]",
        zox_tst_day_night_held, zox_tst_day_night_later);
}

void zox_tsts_day_night(ecs* world) {
    zox_tst_add("day_night", zox_tst_day_night_ready, zox_tst_day_night_check, 60);
}
