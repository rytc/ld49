typedef u32 Entity_ID; 

typedef enum Entity_Subtype_E {
    Soldier,
    Archer,
    Ghost,
    Chicken,
    Zombie,
    Dragon
} Entity_Subtype;

typedef enum Item_Type_E {
    None = -1,

    Sword,
    Bow,
    Axe,

    Leather_Armor,
    Iron_Armor,
    Steel_Armor,

    Feathers,
    Chicken_Meat,
    Bones,

    Ghost_Essence,
    Candy_Corn,
    Garlic,

    Zombie_Flesh,
    Decaying_Bones,
    Brains,

    Pumpkin,

    ITEM_TYPE_COUNT
} Item_Type;

const char* g_item_names[] {
    "Sword", "Bow", "Axe", "Leather Armor", "Iron Armor", "Steel Armor", "Feathers", "Chicken Meat", "Bones", "Ghost Essence",
    "Candy Corn", "Garlic", "Zombie Flesh", "Decaying Bones", "Brains", "Pumpkin"
};

typedef enum Entity_State_E {
    Idle = 0,
    Moving,
    Bump,
    Attacking,
} Entity_State;

typedef enum Entity_Type_E {
    Nothing,
    Player,
    Monster,
    Item,
    Uns,
    Legendary_Sword,

} Entity_Type;

constexpr s32 DROP_TABLE_SIZE = 5;
s32 g_soldier_drop_table[DROP_TABLE_SIZE] = {
    Sword,
    Iron_Armor,
    Steel_Armor,
    Axe,
    Bones,
};

s32 g_archer_drop_table[DROP_TABLE_SIZE] = {
    Bow,
    Leather_Armor,
    Bones,
    Pumpkin,
    Sword
};

s32 g_ghost_drop_table[DROP_TABLE_SIZE] = {
    Ghost_Essence,
    Candy_Corn,
    Garlic,
    Pumpkin,
    Iron_Armor
};

s32 g_chicken_drop_table[DROP_TABLE_SIZE] = {
    Feathers,
    Chicken_Meat,
    Bones,
    Zombie_Flesh,
    Leather_Armor
};

s32 g_zombie_drop_table[DROP_TABLE_SIZE] = {
    Zombie_Flesh,
    Decaying_Bones,
    Brains,
    Candy_Corn,
    Steel_Armor
};


struct Entity {
    Entity_ID id;
    Entity_Type type;
    s32 subtype;
    s32 pos_x;
    s32 pos_y;
    s32 move_delta_x;
    s32 move_delta_y;
    f32 timer;
    Entity_State state;
    s32 hp;
    s32 *drop_table;
};

static constexpr s32 MAX_ENTITY_COUNT = 64*1024;
#define INDEX_MASK 0xfff
#define NEW_ENTITY_ID_ADD 0x1000

struct Entity_Index {
    u32 id;
    u16 index;
    u16 next;
};

struct Entity_List {
   u32 entity_count;
   Entity entities[MAX_ENTITY_COUNT];
   Entity_Index indices[MAX_ENTITY_COUNT];
   u16 freelist_enqueue;
   u16 freelist_dequeue;
};

static void
init_entity_list(Entity_List *entity_list) {
    entity_list->entity_count = 0;
    for(u32 i = 0; i < MAX_ENTITY_COUNT; i++) {
        entity_list->indices[i].id = i;
        entity_list->indices[i].next = i + 1;
    }
    memset(entity_list->entities, 0, sizeof(Entity)*MAX_ENTITY_COUNT);
    entity_list->freelist_dequeue = 0;
    entity_list->freelist_enqueue = MAX_ENTITY_COUNT - 1;
}

inline static bool
has_entity(Entity_List *entity_list, Entity_ID id) {
    Entity_Index in = entity_list->indices[id & INDEX_MASK];
    return (in.id == id && in.index != UINT16_MAX);
}

inline static Entity*
get_entity(Entity_List *entity_list, Entity_ID id) {
    return &entity_list->entities[entity_list->indices[id & INDEX_MASK].index];    
}

inline static Entity_ID
add_entity(Entity_List *entity_list) {
    Entity_Index *in = &entity_list->indices[entity_list->freelist_dequeue];
    entity_list->freelist_dequeue = in->next;
    in->id += NEW_ENTITY_ID_ADD;
    in->index = entity_list->entity_count++;

    Entity *entity = &entity_list->entities[in->index];
    entity->id = in->id;
    return entity->id;
}

inline static void
remove_entity(Entity_List *entity_list, Entity_ID id) {
    Entity_Index *in = &entity_list->indices[id & INDEX_MASK];

    Entity &entity = entity_list->entities[in->index];
    entity = entity_list->entities[--entity_list->entity_count];
    entity_list->indices[entity.id & INDEX_MASK].index = in->index;

    in->index = UINT16_MAX;
    entity_list->indices[entity_list->freelist_enqueue].next = id & INDEX_MASK;
    entity_list->freelist_enqueue = id & INDEX_MASK;
}

static Entity*
get_entity_at(Entity_List* entity_list, s32 x, s32 y) {
    for(s32 entity_index = 0; entity_index < entity_list->entity_count; entity_index++) {
        Entity *entity = &entity_list->entities[entity_index];

        if(entity->pos_x == x && entity->pos_y == y) {
            return entity;
        }
    }

    return nullptr;
}

static Vector2
get_entity_world_pos(Entity* ent) {
    return Vector2{
        (f32)((ent->pos_x + 1) * 32) - (32 - ent->move_delta_x),
        (f32)((ent->pos_y + 1) * 32) - (32 - ent->move_delta_y)
    };
}

static bool
update_entity(Entity* ent) {
    if(ent->state == Moving || ent->state == Bump) {
        s32 dir_x = sign(ent->move_delta_x);
        s32 dir_y = sign(ent->move_delta_y);

        ent->move_delta_x -= 16 * dir_x;
        ent->move_delta_y -= 16 * dir_y;

        if(abs(ent->move_delta_x) == 0 && abs(ent->move_delta_y) == 0) {
            if(ent->state != Bump) {
                ent->pos_x += 1 * dir_x;
                ent->pos_y += 1 * dir_y;
            }

            ent->state = Idle;
        }
    }

    if(ent->type == Item) {
        ent->timer += GetFrameTime();
        if(ent->timer >= 2.f) {
            return false;
        }
    }

    

    return true;
}

static void
draw_entities(Entity_List *entity_list, Texture2D sprites) {
    for(s32 entity_index = 0; entity_index < entity_list->entity_count; entity_index++) {
        Entity *entity = &entity_list->entities[entity_index];
        Vector2 world_pos = get_entity_world_pos(entity);

        if(entity->type == Player) {
            DrawTexturePro(sprites, Rectangle{(f32)80, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
        } else if(entity->type == Monster) {
            switch(entity->subtype) {
                case Soldier: {
                    DrawTexturePro(sprites, Rectangle{(f32)88, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                } break;
                case Archer: {
                    DrawTexturePro(sprites, Rectangle{(f32)96, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                } break;
                case Ghost: {
                    DrawTexturePro(sprites, Rectangle{(f32)104, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                } break;
                case Chicken: {
                    DrawTexturePro(sprites, Rectangle{(f32)112, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                } break;
                case Zombie: {
                    DrawTexturePro(sprites, Rectangle{(f32)120, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                } break;
                case Dragon: {
                    DrawTexturePro(sprites, Rectangle{(f32)0, 16,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
                }
            }
        } else if(entity->type == Item) {
            DrawTexturePro(sprites, Rectangle{(f32)entity->subtype*8, 8,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
        } else if(entity->type == Legendary_Sword) {
            DrawTexturePro(sprites, Rectangle{(f32)72, 0,8,8}, Rectangle{world_pos.x, world_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
        }
    }
}

static void
add_monster(Entity_List *entity_list, s32 x, s32 y, Entity_Subtype type) {
    Entity_ID id = add_entity(entity_list);
    Entity* ent = get_entity(entity_list, id);

    ent->type = Monster;
    ent->subtype = type;
    if(type == Dragon) {
        ent->hp = 25;
    } else {
        ent->hp = 1;
    }
    ent->pos_x = x;
    ent->pos_y = y;
    ent->state = Idle;
    ent->timer = 0;

    switch(type) {
        case Soldier: {
            ent->drop_table = g_soldier_drop_table;
        } break;
        case Archer: {
            ent->drop_table = g_archer_drop_table;
        } break;
        case Ghost: {
            ent->drop_table = g_ghost_drop_table;
        } break;
        case Chicken: {
            ent->drop_table = g_chicken_drop_table;
        } break;
        case Zombie: {
            ent->drop_table = g_zombie_drop_table;
        }
    }
}

static void
add_item(Entity_List *entity_list, s32 x, s32 y, s32 type) {
    Entity_ID id = add_entity(entity_list);
    Entity* ent = get_entity(entity_list, id);

    ent->type = Item;
    ent->subtype = type;
    ent->pos_x = x;
    ent->pos_y = y;
    ent->state = Idle;
    ent->timer = 0;
}