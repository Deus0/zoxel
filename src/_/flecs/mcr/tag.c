#define zox_tag(name)\
    ECS_DECLARE(name)

#define zoxd_tag(name)\
    ECS_TAG_DEFINE(world, name);

// obsolete
#define zoxd_tag(name)\
    ECS_TAG_DEFINE(world, name);
