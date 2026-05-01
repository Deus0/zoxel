// Materials

static inline void zox_dispose_material(uint value) { }

static inline uint zox_gpu_get_material_property(uint id, const char* name) { return 0; }
static inline uint zox_gpu_get_material_attribute(uint id, const char* name) { return 0; }
static inline uint zox_gpu_get_material_uniform(uint id, const char* name) { return 0; }

static inline void zox_gpu_material(uint material) { }
static inline void zox_disable_material() { }
static inline void zox_gpu_reset_mesh() { }

static inline uint2 spawn_gpu_mesh_buffers() { return uint2_zero; }
