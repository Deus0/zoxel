#include "gpu_restore.c"
#include "chunk_order.c"

void zox_tsts_rendering() {
    zox_tst_add("gpu_restore", zox_tst_gpu_restore_ready, zox_tst_gpu_restore_check, 120);
    zox_tst_set_setup("gpu_restore", NULL, 1);
    zox_tst_add("chunk_order", zox_tst_chunk_order_ready, zox_tst_chunk_order_check, 30);
}
