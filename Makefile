
snake : hello.c
	gcc -o snake hello.c `pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 --libs --cflags`
