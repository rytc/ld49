#include "common.h"
#include <cmath>
#include <cstring> // memset
#include <raylib.h>

#ifdef PLATFORM_LINUX
#include "sys_linux.cpp"
#else
#include "sys_windows.cpp"
#endif

#include "utils.cpp"

static Rand_State g_rand;


#include "dialog.cpp"
#include "entity.cpp"
#include "game.cpp"

//int main(int argc, char** argv) {
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    InitWindow(800, 600, "Uns' Table");
    InitAudioDevice();
    SetTargetFPS(60);

    init_game();
    bool running = true;

    while(running) {

        if(WindowShouldClose() || g_game.state == Game_Over) {
            running = false;
            continue;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        draw_world(); 
        
        DrawRectangleRec({0,0, 800, 32}, {0,0,0,128});

        char buff[128];
        snprintf(buff, 196, "Gold: %i", g_game.player_gold);
        DrawText(buff, 196,8, 20, YELLOW);
        snprintf(buff, 128, "HP: %i", g_game.player->hp);
        DrawText(buff, 96, 8, 20, RED);
        snprintf(buff, 128, "LVL: %i", g_game.player_level);
        DrawText(buff, 10, 8, 20, BLUE);
        if(g_game.state == Intro) {
            DrawText("Talk to Uns", 400, 8, 20, WHITE);
        }
        if(g_game.player_inventory == None && g_game.state == Gameplay) {
            DrawText("Find loot", 432, 8, 20, WHITE);
        } else if(g_game.state == Gameplay){
            DrawText(g_item_names[g_game.player_inventory], 600, 8, 20, WHITE);
            DrawTexturePro(g_game.sprites, Rectangle{(f32)g_game.player_inventory*8, 8,8,8}, Rectangle{550, 4, 32, 32}, Vector2{0, 0}, 0, WHITE);
            Vector2 player_pos = get_entity_world_pos(g_game.player);
            DrawTexturePro(g_game.sprites, Rectangle{(f32)g_game.player_inventory*8, 8,8,8}, Rectangle{player_pos.x, player_pos.y-32, 32, 32}, Vector2{0, 0}, 0, WHITE);
        }
        
        if(g_game.state == Dialog) {
            draw_dialog();

            if(IsKeyPressed(KEY_SPACE)) { 
                if(continue_dialog() == false) {
                    if(g_game.dialog_seq == DIALOG_SEQUENCE_INTRO) {
                        g_game.dialog_seq = DIALOG_SEQUENCE_SEARCH_0 + g_game.uns_desire;
                        g_game.dialog_line = 0;
                        g_game.state = Dialog;
                        spawn_monsters();
                        // Spawn entities
                    } else if(g_game.dialog_seq == DIALOG_SEQUENCE_UNS_SUCCESS) {
                        g_game.dialog_seq = DIALOG_SEQUENCE_SEARCH_0 + g_game.uns_desire;
                        g_game.dialog_line = 0;
                        g_game.state = Dialog;

                    } else if(g_game.dialog_seq >= DIALOG_SEQUENCE_SEARCH_0 && g_game.dialog_seq <= DIALOG_SEQUENCE_SEARCH_15) {
                        if(g_game.player_level >= 15 && g_game.player_gold >= 100) {
                            g_game.dialog_seq = DIALOG_SEQUENCE_QUALIFIED;
                            g_game.dialog_line = 0;
                            g_game.state = Dialog;
                        } else {
                            g_game.state = Gameplay;
                        }

                    } else if(g_game.dialog_seq == DIALOG_SEQUENCE_LOSE) {
                        g_game.state = Game_Over;
                        PlaySound(g_game.sounds[Snd_Game_Over]);
                        running = false;
                    } else if(g_game.dialog_seq == DIALOG_SEQUENCE_GAME_WIN) {
                        g_game.state = Game_Over;
                        running = false;
                    } else {
                        g_game.state = Gameplay;
                    }
                }
            }
            
        } else {
            update_game();
        }
        
        EndDrawing();
    }

    destroy_game();
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}