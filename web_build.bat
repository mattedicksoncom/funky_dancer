@echo off
emcc -O3 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s SAFE_HEAP=1 -s DETERMINISTIC=1 funky_dancer.c -o funky_web.html
echo Compilation complete.