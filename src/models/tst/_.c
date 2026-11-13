#include "clone.c"
#include "model_views.c"
#include "nodes.c"
#include "shapes.c"

void zox_tsts_models() {
    zox_tst_add("model_shapes", zox_tst_model_shapes_ready, zox_tst_model_shapes_check, 60);
    zox_tst_set_setup("model_shapes", NULL, 1);
    zox_tst_add("model_nodes", zox_tst_model_nodes_ready, zox_tst_model_nodes_check, 30);
    zox_tst_set_setup("model_nodes", NULL, 1);
    zox_tst_add("model_views", zox_tst_model_views_ready, zox_tst_model_views_check, 60);
    zox_tst_set_setup("model_views", NULL, 1);
}
