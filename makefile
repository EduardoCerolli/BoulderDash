CC = gcc
CFLAGS = -Wall -std=c99
ARQS = boulder_dash.c lib_boulder.c 
ALLEGRO = -lallegro -lallegro_font -lallegro_primitives -lallegro_audio -lallegro_acodec -lallegro_image

all:
	$(CC) $(CFLAGS) -c $(ARQS)
	$(CC) $(CFLAGS) -o boulder_dash *.o $(ALLEGRO)

clean:
	rm -f *.o

purge: clean
	rm -f boulder_dash

