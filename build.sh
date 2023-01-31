#!/usr/bin/env bash
C_FLAGS="-g"
INCLUDE_FLAGS="$(pkg-config freetype2 --cflags) -Iglad/include"
LIB_FLAGS="$(pkg-config freetype2 --libs) -ldl -lutil"



if [[ $(uname) == "Darwin" ]]; then
    GL_LIB="-L${HOME}/.local/lib -framework OpenGL -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo"
else
    GL_LIB="-lGL -lglfw"
fi

LIB_FLAGS="${LIB_FLAGS} ${GL_LIB}"



mkdir object_files 2> /dev/null

for file in src/*.c; do
    gcc -c ${C_FLAGS} ${INCLUDE_FLAGS} -Wall -Werror -o object_files/$(basename $file .c).o $file &
done;

wait

gcc ${C_FLAGS} ${INCLUDE_FLAGS} -Wall -Werror -o SIG object_files/*.o ${LIB_FLAGS}
