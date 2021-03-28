rm -rf build && mkdir build

clang ./src/*.c -O3 -flto=thin -o ./build/main -F /Library/Frameworks -framework SDL2 -framework SDL2_ttf

./build/main