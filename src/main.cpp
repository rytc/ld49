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
#include "dialog.cpp"
#include "entity.cpp"
#include "game.cpp"


int main(int argc, char** argv) {
    InitWindow(800, 600, "LD49");
    SetTargetFPS(60);

    init_game();

    while(!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        draw_world(); 
        
        
        char buff[128];
        snprintf(buff, 128, "Gold: %i", g_game.player_gold);
        DrawText(buff, 128,8, 20, YELLOW);
        snprintf(buff, 128, "HP: %i", g_game.player->hp);
        DrawText(buff, 30, 8, 20, RED);


        if(g_game.state == Dialog) {
            draw_dialog();

            if(IsKeyPressed(KEY_SPACE)) continue_dialog();
            
        } else {
            update_game();
        }
        
        EndDrawing();
    }

    destroy_game();
    CloseWindow();
    
    return 0;
}