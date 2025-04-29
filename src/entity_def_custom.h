
#define ENTITY_TYPES(TYPE) TYPE(ENTITY_TYPE_OTHER, other)

// All entity types share the same struct. Calling ENTITY_DEFINE() defines that
// struct with the fields required by high_impact and the additional fields
// specified here.

ENTITY_DEFINE(
    // Entity private data
    union {
      struct {
        entity_list_t targets;
        float delay;
        float delay_time;
        bool can_fire;
      } trigger;
    };);

// The entity_message_t is used with the entity_message() function. You can
// extend this as you wish.

typedef enum {
  EM_INVALID,
  EM_ACTIVATE,
} entity_message_t;