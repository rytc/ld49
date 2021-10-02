#include "game.h"

static void
init_game() {
    g_game = {};
    g_game.entity_list = (Entity_List*)malloc(sizeof(Entity_List));
    init_entity_list(g_game.entity_list);
    
    g_game.sprites = LoadTexture("sprites.png");

    Entity_ID player_id = add_entity(g_game.entity_list);
    {
        Entity* player = get_entity(g_game.entity_list, player_id);
        player->type = Player;
        player->pos_x = 1;
        player->pos_y = 1;
        player->hp = 100;
        g_game.player = player;
        g_game.player_gold = 0;
    }
    
    Entity_ID uns_id = add_entity(g_game.entity_list);
    {
        Entity* uns = get_entity(g_game.entity_list, uns_id);
        uns->type = Uns;
        uns->pos_x = 2;
        uns->pos_y = 9;
    }


}

static void
destroy_game() {
    free(g_game.entity_list);
}

static inline u32
get_tile_id_at(s32 x, s32 y) {
    return MAP[x + (y * 25)];
}

static inline bool 
is_tile_blocked(s32 x, s32 y) {
    u32 tile_id = get_tile_id_at(x, y);
    if(tile_id > 2 && tile_id < 11) return true;
    return false;
}

static bool
can_move(Entity* ent, s32 dst_x, s32 dst_y) {

    if(ent->type == Player) {
        Entity *entity = get_entity_at(g_game.entity_list, dst_x, dst_y);
        if(entity) {
            if(entity->type == Uns) {
                if(g_game.state == Intro) {
                    g_game.state = Dialog;
                }
            }

            return false;
        }
    }

    if(is_tile_blocked(dst_x, dst_y)) return false;

    return true;
}

static void
move_entity(Entity *ent, Direction dir) {
    if(ent->state == Idle) {
        ent->timer = 0;
        switch(dir) {
            case DIR_NORTH: {
                if(!can_move(ent, ent->pos_x, ent->pos_y - 1)) ent->state = Bump;
                ent->move_delta_y = -32;
            } break;
            case DIR_EAST: {
                if(!can_move(ent, ent->pos_x + 1, ent->pos_y)) ent->state = Bump;
                ent->move_delta_x = 32;
            } break;
            case DIR_SOUTH: {
                if(!can_move(ent, ent->pos_x, ent->pos_y + 1)) ent->state = Bump;
                ent->move_delta_y = 32;
            } break;
            case DIR_WEST: {
                if(!can_move(ent, ent->pos_x - 1, ent->pos_y)) ent->state = Bump;
                ent->move_delta_x = -32;
            } break;
            default: {d_assert(false)};
        }

        if(ent->state != Bump) ent->state = Moving;

    }
}



static void
continue_dialog() {
    auto seq_def = d_sequences[g_game.dialog_seq];
    if(g_game.dialog_line < seq_def.dialog_count - 1) {
        g_game.dialog_line += 1;
    } else {
        g_game.dialog_seq = -1;
        g_game.state = Gameplay;
    }
}

static void
draw_world() {
    for(s32 y = 0; y < 19; y++) {
        for(s32 x = 0; x < 26; x++) {
            u32 tile_id = get_tile_id_at(x, y);
            s32 uv_x = (tile_id - (tile_id / 128)) * 8;
            s32 uv_y = (tile_id / 128) * 8;
            DrawTexturePro(g_game.sprites, Rectangle{(f32)uv_x, (f32)uv_y,8,8}, Rectangle{(f32)x * 32, (f32)y * 32, 32, 32}, Vector2{0, 0}, 0, WHITE);

        }
    }

    Vector2 player_pos = get_entity_world_pos(g_game.player);
    DrawTexturePro(g_game.sprites, Rectangle{(f32)88, 0,8,8}, Rectangle{player_pos.x, player_pos.y, 32, 32}, Vector2{0, 0}, 0, WHITE);
}

static void
draw_dialog() {
    auto seq_def = d_sequences[g_game.dialog_seq];
    DrawRectangleRec({0,400, 800, 300}, {0,0,0,128});
    if(seq_def.author == 0) {
        DrawText("Uns, the Unscrupulous Merchant", 24, 424, 20, YELLOW); 
    }
    DrawText(seq_def.lines[g_game.dialog_line], 24, 460, 20, WHITE); 
    DrawText("Press SPACE to continue...", 24, 550, 20, GRAY); 

}

static void
update_game() {
    if(IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        move_entity(g_game.player, DIR_NORTH);
    } else if(IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        move_entity(g_game.player, DIR_SOUTH);
    }

    if(IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        move_entity(g_game.player, DIR_EAST);
    } else if(IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        move_entity(g_game.player, DIR_WEST);
    }

    update_entity(g_game.player);

}


