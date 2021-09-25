#include "common.h"
#include <cmath>
#include <cstring> // memset
#include <raylib.h>

#ifdef PLATFORM_LINUX
#include "sys_linux.cpp"
#else
#include "sys_windows.cpp"
#endif

int main(int argc, char** argv) {
    printf("Hello world");

    InitWindow(800, 600, "LD49");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Ludum Dare 49!", 100, 100, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}