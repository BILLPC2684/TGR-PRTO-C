[[ "$#" -gt 0 ]] && reset
rm --force ./TGR-C
gcc main.c -o ./TGR-C -std=c99 -lm `pkg-config --cflags --libs sdl2` -Wall
[[ "$#" -gt 0 ]] && ./TGR-C $1 $2 $3 $4 $5 $6 $7 $8 $9
