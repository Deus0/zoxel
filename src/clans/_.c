/*
 * +------------------------------------------------------+
 * | Zox Module: Clans                                    |
 * |                                                      |
 * |  Groups - Members - Ranks - Alliances - Clan State   |
 * |                                                      |
 * +------------------------------------------------------+
 */
#include "com/_.c"

void import_clans(ecs* world) {
    zox_module(clans);
    zox_components_clans(world);
}

