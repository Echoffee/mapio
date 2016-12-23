#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

struct s_map_case
{
  int x;
  int y;
  int type;

  struct s_map_case* next;
};

typedef struct s_map_case* map_case;


void map_new (unsigned width, unsigned height)
{
	map_allocate (width, height);

	for (int x = 0; x < width; x++)
		map_set (x, height - 1, 0); // Ground

	for (int y = 0; y < height - 1; y++) {
		map_set (0, y, 1); // Wall
		map_set (width - 1, y, 1); // Wall
	}

	map_object_begin (6);

	// Texture pour le sol
	map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
	// Mur
	map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
	// Gazon
	map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
	// Marbre
	map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
	//Fleurs
	map_object_add("images/flower.png", 1, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);
	//Pieces
	map_object_add("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);
	map_object_end ();

}

void map_save (char *filename)
{
	// TODO
  FILE* f = fopen(filename, "w");
  int num_items = map_objects();
  int w = map_width();
  int h = map_height();
  map_case prev = NULL;
  map_case first = NULL;
  fprintf(f, "%d %d\n", w, h);
  for (int i = 0; i < w; i++)
    for (int j = 0; j < h; j++)
    {
      int o = map_get(i, j);
      if (o != MAP_OBJECT_NONE)
      {
        map_case m = (map_case)malloc(sizeof(struct s_map_case));
        m->x = i;
        m->y = j;
        m->type = o;
        m->next = NULL;

        if (first == NULL)
          first = m;
        else
          prev->next = m;
        prev = m;
      }
    }

  while (first->next != NULL)
    {
      fprintf(f, "%d %d %d\n", first->x, first->y, first->type);
      first = first->next;
    }

  fprintf(f, "%d\n", num_items);
  for (int i = 0; i < num_items; i++)
  {
    char* path = map_get_name(i);
    int len = strlen(path);
    int frames_num = map_get_frames(i);
    int solid = map_get_solidity(i);
    int dest = map_is_destructible(i);
    int coll = map_is_collectible(i);
    int gen = map_is_generator(i);
    fprintf(f, "%d %s %d %d %d %d %d\n", len, path, frames_num, solid, dest, coll, gen);
  }

  fclose(f);
	fprintf (stderr, "Map saved at %s\n", filename);
}

void map_load (char *filename)
{
	// TODO
	exit_with_error ("Map load is not yet implemented\n");
}

#endif
