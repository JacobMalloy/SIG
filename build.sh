INCLUDE_FLAGS="$(freetype-config --cflags) -Iglad/include"
LIB_FLAGS="$(freetype-config --libs) -ldl"



if [[ $(uname)=="Darwin" ]]; then
    GL_LIB="-L${HOME}/.local/lib -framework OpenGL -lglfw3 -framework Cocoa -framework IOKit -framework CoreVideo"
else
    GL_LIB="-lGL -lglfw"
fi

LIB_FLAGS="${LIB_FLAGS} ${GL_LIB}"


gcc -g ${INCLUDE_FLAGS} ${LIB_FLAGS} -o SIG src/main.c src/glad.c
