#include "clone.c"
#include "model_views.c"

void zox_tsts_models() {
    zox_tst_add("model_views", zox_tst_model_views_ready, zox_tst_model_views_check, 60);
    zox_tst_set_setup("model_views", NULL, 1);
}
