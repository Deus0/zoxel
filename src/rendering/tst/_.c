#include "gpu_restore.c"
#include "gpu_links.c"
#include "gpu_restore_game.c"
#include "chunk_order.c"

void zox_tsts_rendering() {
    zox_tst_add("gpu_restore", zox_tst_gpu_restore_ready, zox_tst_gpu_restore_check, 120);
    zox_tst_set_setup("gpu_restore", NULL, 1);
    zox_tst_add("gpu_links", zox_tst_gpu_links_ready, zox_tst_gpu_links_check, 30);
    zox_tst_set_setup("gpu_links", zox_tst_level_setup, 0);
    zox_tst_add("gpu_restore_game", zox_tst_gpu_restore_game_ready, zox_tst_gpu_restore_game_check, 120);
    zox_tst_add("chunk_order", zox_tst_chunk_order_ready, zox_tst_chunk_order_check, 30);
}
