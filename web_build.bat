@echo off
emcc -O3 -s USE_SDL=2 -s DETERMINISTIC=1 funky_dancer.c --preload-file resources --shell-file em_template.html -o ./builds/web/funky_web.html
echo Compilation complete.