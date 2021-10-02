#include "game.h"

static void
init_game() {
    g_game = {};
    g_game.entity_list = (Entity_List*)malloc(sizeof(Entity_List));
    init_entity_list(g_game.entity_list);
    
    g_game.sprites = LoadTexture("sprites.png");
    g_game.monster_count = 0;
    g_game.uns_desire = get_rand(0, ITEM_TYPE_COUNT-1);

    Entity_ID player_id = add_entity(g_game.entity_list);
    {
        Entity* player = get_entity(g_game.entity_list, player_id);
        player->type = Player;
        player->pos_x = 1;
        player->pos_y = 1;
        player->hp = 100;
        g_game.player = player;
        g_game.player_inventory = None;
        g_game.player_gold = 0;
    }
    
    Entity_ID uns_id = add_entity(g_game.entity_list);
    {
        Entity* uns = get_entity(g_game.entity_list, uns_id);
        uns->type = Uns;
        uns->pos_x = 2;
        uns->pos_y = 9;
    }

    Entity_ID legendary_id = add_entity(g_game.entity_list);
    {
        Entity* sword = get_entity(g_game.entity_list, legendary_id);
        sword->type = Legendary_Sword;
        sword->pos_x = 1;
        sword->pos_y = 8;
    }

    g_game.sounds[Snd_Hit] = LoadSound("sound/hit.wav");
    g_game.sounds[Snd_Level_Up] = LoadSound("sound/levelup.wav");
    g_game.sounds[Snd_Pickup] = LoadSound("sound/pickup.wav");
    g_game.sounds[Snd_Success] = LoadSound("sound/success.wav");
    g_game.sounds[Snd_Fail] = LoadSound("sound/fail.wav");
    g_game.sounds[Snd_Game_Over] = LoadSound("sound/gameover.wav");
    g_game.sounds[Snd_Win] = LoadSound("sound/win.wav");

    g_game.music = LoadMusicStream("sound/music.mp3");
    SetMusicVolume(g_game.music, 1.f);
    PlayMusicStream(g_game.music);

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
    if(tile_id == 0 || (tile_id > 2 && tile_id < 10)) return true;
    return false;
}

static bool
can_spawn(s32 x, s32 y) {
    if(is_tile_blocked(x, y)) return false;
    if(get_entity_at(g_game.entity_list, x, y)) return false;

    // Don't spawn in the shop!
    if(x > 0 && x <= 6 && 
       y > 6 && y < 11) return false;

    return true;
}

static bool
can_move(Entity* ent, s32 dst_x, s32 dst_y) {

    if(ent->type == Player && ent->hp > 0) {
        Entity *entity = get_entity_at(g_game.entity_list, dst_x, dst_y);
        if(entity) {
            if(entity->type == Uns) {
                if(g_game.state == Intro) {
                    g_game.state = Dialog;
                } else if(g_game.state == Gameplay) {
                    if(g_game.player_inventory != None) {
                        if(g_game.player_inventory != g_game.uns_desire) {
                            g_game.dialog_seq = DIALOG_SEQUENCE_WRONG;
                            g_game.dialog_line = 0;
                            g_game.state = Dialog;
                            g_game.player_inventory = None;
                            PlaySound(g_game.sounds[Snd_Fail]);
                            if(g_game.player_gold > 5) {
                                g_game.player_gold -= 5;
                            }

                            if(g_game.player->hp > 0) {
                                g_game.player->hp -= 10;
                            }
                        } else {
                            g_game.dialog_seq = DIALOG_SEQUENCE_UNS_SUCCESS;
                            g_game.dialog_line = 0;
                            g_game.state = Dialog;
                            PlaySound(g_game.sounds[Snd_Success]);
                            g_game.player_inventory = None;
                            g_game.player_gold += 10;
                            g_game.player_exp += 5;
                            g_game.uns_desire = get_rand(0, ITEM_TYPE_COUNT-1);
                        }
                    } else {
                        g_game.dialog_seq = DIALOG_SEQUENCE_SEARCH_0 + g_game.uns_desire;
                        g_game.dialog_line = 0;
                        g_game.state = Dialog;
                    }
                                    }
            } else if(entity->type == Monster) {
                entity->hp -= 1;

                if(entity->subtype == Dragon) {
                    ent->hp -= 2;
                }

                if(entity->hp <= 0) {
                    if(entity->subtype == Dragon) {
                        g_game.dialog_seq = DIALOG_SEQUENCE_GAME_WIN;
                        g_game.dialog_line = 0;
                        g_game.state = Dialog;
                        PlaySound(g_game.sounds[Snd_Success]);
                    } 
                    s32 loot = get_rand(0, DROP_TABLE_SIZE-1);
                    add_item(g_game.entity_list, entity->pos_x, entity->pos_y, entity->drop_table[loot]);
                    remove_entity(g_game.entity_list, entity->id);
                    g_game.player_exp += 1;
                    g_game.monster_count -= 1;
                    
                }
                PlaySound(g_game.sounds[Snd_Hit]);
            } else if(entity->type == Item) {
                g_game.player_inventory = entity->subtype;
                remove_entity(g_game.entity_list, entity->id);
                PlaySound(g_game.sounds[Snd_Pickup]);
            } else if(entity->type == Legendary_Sword && g_game.state == Gameplay) {
                if(g_game.player_level < 15 && g_game.player_gold < 100) {
                    g_game.dialog_seq = DIALOG_SEQUENCE_SWORD_ASK;
                    g_game.dialog_line = 0;
                    g_game.state = Dialog;
                } else {
                    g_game.dialog_seq = DIALOG_SEQUENCE_SWORD_GET;
                    g_game.dialog_line = 0;
                    g_game.state = Dialog;
                    remove_entity(g_game.entity_list, entity->id);
                    g_game.player_gold -= 100;
                    PlaySound(g_game.sounds[Snd_Win]);
                    add_monster(g_game.entity_list, 14, 8, Dragon);
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

static bool 
continue_dialog() {
    auto seq_def = d_sequences[g_game.dialog_seq];
    if(g_game.dialog_line < seq_def.dialog_count - 1) {
        g_game.dialog_line += 1;
        return true;
    } else {
        return false;
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
    draw_entities(g_game.entity_list, g_game.sprites);
}

static void
spawn_monsters() {
    while(g_game.monster_count < 13) {
        s32 x = get_rand(1, 23);
        s32 y = get_rand(1, 17);
        if(!can_spawn(x, y)) continue;
        Entity_Subtype type = (Entity_Subtype)get_rand(0, 4);
        add_monster(g_game.entity_list, x, y, type);
        g_game.monster_count += 1;
    } 
}

static void
draw_dialog() {
    auto seq_def = d_sequences[g_game.dialog_seq];
    DrawRectangleRec({0,400, 800, 300}, {0,0,0,128});
    if(seq_def.author == 0) {
        DrawText("Uns, the Unscrupulous Merchant", 24, 424, 20, YELLOW); 
    } else if(seq_def.author == 1) {
        DrawText("Legendary Sword", 24, 424, 20, DARKPURPLE); 
    }
    DrawText(seq_def.lines[g_game.dialog_line], 24, 460, 20, WHITE); 
    DrawText("Press SPACE to continue...", 24, 550, 20, GRAY); 

}

static void
update_game() {
    UpdateMusicStream(g_game.music);

    if(g_game.player->hp <= 0 && g_game.state == Gameplay) {

        g_game.dialog_seq = DIALOG_SEQUENCE_LOSE;
        g_game.dialog_line = 0;
        g_game.state = Dialog;

        return;
    }

    if(g_game.player_level >= 99) g_game.player_level = 99;
    if(g_game.player_gold >= 500) g_game.player_gold = 500;

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

    if(g_game.state == Gameplay) {
        g_game.spawn_timer += GetFrameTime();
    }

    if(g_game.spawn_timer >= 5.f) {
        g_game.spawn_timer = 0;
        spawn_monsters();
    }

    if(g_game.player_exp >= 10) {
        g_game.player_level += 1;
        g_game.player_exp = 0;
        PlaySound(g_game.sounds[Snd_Level_Up]);
    }
    for(s32 entity_index = 0; entity_index < g_game.entity_list->entity_count; entity_index++) {
        Entity *entity = &g_game.entity_list->entities[entity_index];

        if(!update_entity(entity)) {
            remove_entity(g_game.entity_list, entity->id);
        } else {
            if(entity->type == Monster) {
                entity->timer += GetFrameTime();
                if(entity->timer > 2.f) {
                    entity->timer = 0;
                    s32 dir = get_rand(0,12);
                    if(dir < 3 && can_spawn(entity->pos_x, entity->pos_y - 1)) {
                        move_entity(entity, DIR_NORTH);
                    } else if(dir < 6 && can_spawn(entity->pos_x, entity->pos_y + 1)) {
                        move_entity(entity, DIR_SOUTH);
                    } else if(dir < 9 && can_spawn(entity->pos_x + 1, entity->pos_y)) {
                        move_entity(entity, DIR_EAST);
                    } else if(dir <= 12 && can_spawn(entity->pos_x - 1, entity->pos_y)) {
                        move_entity(entity, DIR_WEST);
                    }
                }
            }
        }
    }

}


