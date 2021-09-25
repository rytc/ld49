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
    return 0;
}