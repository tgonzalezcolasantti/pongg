COMPILER = g++
LINKER = g++
STUFF = sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-tff-dev libsdl2-mixer-dev
COMPILERFLAGS = -pthread -Wall -pedantic -o3 `pkg-config --cflags sdl2` -Isrc/include -Wno-write-strings
LDFLAGS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer`