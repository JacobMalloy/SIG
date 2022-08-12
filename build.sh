#!/usr/bin/env bash
INCLUDE_FLAGS="$(pkg-config freetype2 --cflags) -Iglad/include"
LIB_FLAGS="$(pkg-config freetype2 --libs) -ldl"



if [[ $(uname) == "Darwin" ]]; then
    GL_LIB="-L${HOME}/.local/lib -framework OpenGL -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo"
else
    GL_LIB="-lGL -lglfw"
fi

LIB_FLAGS="${LIB_FLAGS} ${GL_LIB}"


gcc -g ${INCLUDE_FLAGS} -o SIG src/main.c src/glad.c ${LIB_FLAGS}
