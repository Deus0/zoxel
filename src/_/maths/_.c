/* *
 *  Zox Maths
 *
 *      - This just adds our data types together in mathematical ways
 *      - pure util functions
 *      - todo: put data and fun into util sub folder
 *
 *      - NOTE: Zox uses Column Major Matrix (as opposed to Row Major)
 *
 * */
#ifndef zoxm_maths
#define zoxm_maths

#include "dat/constants.c"
#include "dat/_.c"
#include "fun/_.c"
#include "tst/test_quadrants.c"
#include "bounds/_.c"
#include "planes/_.c"

// hashmaps
zox_hashmap(byte3_hashmap, entity, 0, byte3, uint, get_byte3_hash)
zox_hashmap(int3_hashmap, entity, 0, int3, uint, get_int3_hash)
zox_hashmap(string_hashmap, entity, 0, string_data, uint, get_string_hash)
zox_hashmap(int_hashmap, entity, 0, int, uint,  get_int_hash)
zox_hashmap(int2_hashmap, entity, 0, int2, uint, get_int2_hash)


#endif
