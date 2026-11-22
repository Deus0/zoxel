#include "all.c"
#ifdef zox_tests
#include "furniture.c"
#include "furniture_fillers.c"
#include "ground_removed.c"
#include "item_sizes.c"

#include "block_data_queue.c"
#include "bite_hitch.c"
#include "all_given.c"

void zox_tsts_items(ecs* world) {
    zox_tsts_furniture(world);
    zox_tsts_furniture_fillers(world);
    zox_tsts_ground_removed(world);
    zox_tsts_item_sizes(world);
    zox_tsts_block_data_queue(world);
    zox_tsts_bite_hitch(world);
    zox_tsts_all_items(world);
}
#endif
