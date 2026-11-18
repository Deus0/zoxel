void on_module_dispose_rendering(ecs *world, void *ctx) {
    dispose_render_loop();
    dispose_hook_gpu_dispose();
    dispose_hook_gpu_restore();
    dispose_hook_load_shader();
}