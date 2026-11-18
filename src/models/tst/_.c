#include "model_views.c"
#include "nodes.c"
#include "shapes.c"
#include "gpu_mesh_views.c"
#include "furniture.c"
#include "vox_import.c"
#include "grass_tufts.c"

void zox_tsts_models() {
    zox_tst_add("model_shapes", zox_tst_model_shapes_ready, zox_tst_model_shapes_check, 60);
    zox_tst_set_setup("model_shapes", NULL, 1);
    zox_tst_add("model_nodes", zox_tst_model_nodes_ready, zox_tst_model_nodes_check, 30);
    zox_tst_set_setup("model_nodes", NULL, 1);
    zox_tst_add("model_views", zox_tst_model_views_ready, zox_tst_model_views_check, 60);
    zox_tst_set_setup("model_views", NULL, 1);
    zox_tst_add("furniture_models", zox_tst_furniture_models_ready, zox_tst_furniture_models_check, 60);
    zox_tst_set_setup("furniture_models", NULL, 1);
    zox_tst_add("vox_import", zox_tst_vox_import_ready, zox_tst_vox_import_check, 30);
    zox_tst_set_setup("vox_import", NULL, 1);
    zox_tst_add("grass_tufts", zox_tst_grass_tufts_ready, zox_tst_grass_tufts_check, 30);
}
