rm -rf build && mkdir build

# clang ./src/*.c -O3 -flto=thin -o ./build/main -F /Library/Frameworks -framework SDL2 -framework SDL2_ttf
clang -O3 -flto=thin \
-I/usr/local/include/SDL2 -D_THREAD_SAFE -L/usr/local/lib -lSDL2 -lSDL2_ttf \
./src/*.c \
 -o ./build/main

./build/main