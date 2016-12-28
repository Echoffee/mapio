#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct s_map_case
{
	int x;
	int y;
	int type;

	struct s_map_case* next;
};

struct s_object
{
	int lenght_path;
	char* path;
	int frames;
	int solidity;
	int destructible;
	int collectible;
	int generator;
	int active;
	struct s_object* next;
};

struct s_map_info
{
	char* filename;
	int width;
	int height;
	int nb_element;
	int nb_object;

	struct s_map_case* first_case;
	struct s_object* first_object;
};

typedef struct s_map_case* map_case;
typedef struct s_object* object;
typedef struct s_map_info* map;

void skip_line(int file_id)
{
	char c = 'a';
	while (c != '\n')
		read(file_id, &c, sizeof(char));
}

void skip_word(int file_id)
{
	char c = 'a';
	while (c != ' ')
		read(file_id, &c, sizeof(char));
}

void read_word(int file_id, char label[])
{
	printf("%s", label);
	char c = 62;
	while (c !=' ')
	{
		read(file_id, &c, sizeof(char));
		printf("%c", c);
	}
}

int read_digit(int file_id)
{
	char c = '0';
	read(file_id, &c, sizeof(char));
	return c - '0';
}

map convertFile(char* filename){
	//Initialisation des trois structures
	map_case first_case = (map_case)malloc(sizeof(struct s_map_case));
	object first_object = (object)malloc(sizeof(struct s_object));
	map loading_map = (map)malloc(sizeof(struct s_map_info));
	//On recopie le nom du fichier dans la map entrain de charger
	loading_map->filename = (char*)malloc(sizeof(char) * strlen(filename));
	strcpy(loading_map->filename, filename);

	//On ouvre le fichier de sauvegarde en lecture
	FILE* file_save = fopen(filename, "r");

	int buff_size = 256;
	char buffer[buff_size];
	char* tokken;
	char delim[2] = " ";

	//On recupere la largeur et la hauteur
	fgets(buffer, buff_size, file_save);
	tokken = strtok(buffer, delim);
	loading_map->width = atoi(tokken);
	tokken = strtok(NULL, delim);
	loading_map->height = atoi(tokken);

	//On recupere le nombre d'élement, et on initialise la struture du premier element à NULL
	fgets(buffer, buff_size, file_save);
	tokken = strtok(buffer, delim);
	loading_map->nb_element = atoi(tokken);
	loading_map->first_case	= NULL;

	//Si il y a au moins un element, alors on rempli les structures map_case
	if(loading_map->nb_element > 0){

		fgets(buffer, buff_size, file_save);
		tokken = strtok(buffer, delim);
		first_case->x = atoi(tokken);
		tokken = strtok(NULL, delim);
		first_case->y = atoi(tokken);
		tokken = strtok(NULL, delim);
		first_case->type = atoi(tokken);

		first_case->next = NULL;

		loading_map->first_case = first_case;

		for(int i = 0; i < loading_map->nb_element - 1; i++){
			fgets(buffer, buff_size, file_save);

			map_case actual_case = (map_case)malloc(sizeof(struct s_map_case));
			first_case->next = actual_case;

			tokken = strtok(buffer, delim);
			actual_case->x = atoi(tokken);
			tokken = strtok(NULL, delim);
			actual_case->y = atoi(tokken);
			tokken = strtok(NULL, delim);
			actual_case->type = atoi(tokken);
			actual_case->next = NULL;
			first_case = actual_case;

		}

	}

	//On recupere le nombre d'objet
	fgets(buffer, buff_size, file_save);
	tokken = strtok(buffer, delim);
	loading_map->nb_object = atoi(tokken);

	//Si il n'y a pas d'objet chargé, on fixe la structure first_object à NULL, et on retourne la structure de la map
	if(loading_map->nb_object <= 0)
	{
		loading_map->first_object = NULL;
		return loading_map;
	}

	//On recupere les informations du premier objet
	fgets(buffer, buff_size, file_save);
	tokken = strtok(buffer, delim);
	first_object->lenght_path = atoi(tokken);

	tokken = strtok(NULL, delim);
	first_object->path = (char*) malloc(first_object->lenght_path * sizeof(char));
	strcpy(first_object->path, tokken);
	tokken = strtok(NULL, delim);
	first_object->frames = atoi(tokken);

	tokken = strtok(NULL, delim);
	first_object->solidity = atoi(tokken);

	tokken = strtok(NULL, delim);
	first_object->destructible = atoi(tokken);

	tokken = strtok(NULL, delim);
	first_object->collectible = atoi(tokken);

	tokken = strtok(NULL, delim);
	first_object->generator = atoi(tokken);

	first_object->active = 1;


	first_object->next = NULL;

	loading_map->first_object = first_object;

	//On recupere les informations des objets suivant
	for(int i = 0; i < loading_map->nb_object - 1; i++){

		fgets(buffer, buff_size, file_save);
		object actual_object = (object)malloc(sizeof(struct s_object));

		tokken = strtok(buffer, delim);
		actual_object->lenght_path = atoi(tokken);

		tokken = strtok(NULL, delim);
		actual_object->path = (char*) malloc(actual_object->lenght_path * sizeof(char));
		strcpy(actual_object->path, tokken);

		tokken = strtok(NULL, delim);
		actual_object->frames = atoi(tokken);

		tokken = strtok(NULL, delim);
		actual_object->solidity = atoi(tokken);

		tokken = strtok(NULL, delim);
		actual_object->destructible = atoi(tokken);

		tokken = strtok(NULL, delim);
		actual_object->collectible = atoi(tokken);

		tokken = strtok(NULL, delim);
		actual_object->generator = atoi(tokken);

		actual_object->active = 1;

		actual_object->next = NULL;
		first_object->next = actual_object;

		first_object = actual_object;
	}

	return loading_map;
}

//Permet de determiner les nouvelles ID des elements sur la map (pour pruneobjects)
int* findNewId(int* checkTab, int nb_object){
	int* tab2 = (int*)malloc(sizeof(int) * nb_object);

	int debug = 0;

	for(int i = 0; i < nb_object; i++)
		tab2[i] = 0;
	
	for(int i = 0; i < nb_object; i++){
		int nb_zero = 0;
		for(int j = 0; j < i; j++){
			if(checkTab[j] == 0){
				nb_zero += 1;
			}
		}
		if(checkTab[i] != 0)
			tab2[i] = i - nb_zero;
	}

	return tab2;
}

//Enleve les objets non utilisés
void pruneobjects(map saved_map){
	int nb_element = saved_map->nb_element;
	int nb_object = saved_map->nb_object;
	int checkObject[nb_object];
	for(int i = 0; i < nb_object; i++)
		checkObject[i] = 0;

	map_case actual_case = saved_map->first_case;
	while(actual_case != NULL){
		checkObject[actual_case->type] = 1;
		actual_case = actual_case->next;
	}

	int* newIDs = findNewId(checkObject, nb_object);

	actual_case = saved_map->first_case;

	while(actual_case != NULL){
		actual_case->type = newIDs[actual_case->type];
		actual_case = actual_case->next;
	}

	object actual_object = saved_map->first_object;

	int i = 0;
	int new_nb_object = 0;
	while(actual_object != NULL){
		if(checkObject[i] == 0)
			actual_object->active = 0;

		new_nb_object += checkObject[i];
		i++;
		actual_object = actual_object->next;
	}
	saved_map->nb_object = new_nb_object;

	free(newIDs);
}

//Convertit la structure map en un fichier de sauvegarde 
void translateMapInFile(map new_map, char* filename){
	remove(filename);
	FILE* new_file = fopen(filename, "w");
	char buffer[256];
	sprintf(buffer, "%d %d\n", new_map->width, new_map->height);
	fputs(buffer, new_file);

	sprintf(buffer, "%d\n", new_map->nb_element);
	fputs(buffer, new_file);

	map_case actual_case = new_map->first_case;
	while(actual_case != NULL){
		sprintf(buffer, "%d %d %d\n", actual_case->x, actual_case->y, actual_case->type);
		fputs(buffer, new_file);
		actual_case = actual_case->next;
	}

	sprintf(buffer, "%d\n", new_map->nb_object);
	fputs(buffer, new_file);

	object actual_object = new_map->first_object;
	while(actual_object != NULL){
		if(actual_object->active == 1){
			sprintf(buffer, "%d %s %d %d %d %d %d\n", 
				actual_object->lenght_path, actual_object->path, actual_object->frames, actual_object->solidity,
					actual_object->destructible, actual_object->collectible, actual_object->generator);
			fputs(buffer, new_file);
		}
		actual_object = actual_object->next;
	}

	fclose(new_file);
}

int main(int argc, char* argv[])
{
	if (argc % 6 != 3 && argc % 6 != 4)
	{
		fprintf(stderr, "%s\n", "Usage : maputil <file> <command> [value] ");
		return 1;
	}

	int getwidth = strcmp(argv[2], "--getwidth");
	int getheight = strcmp(argv[2], "--getheight");
	int getobjects = strcmp(argv[2], "--getobjects");
	int getinfo = strcmp(argv[2], "--getinfo");
	int setwidth = strcmp(argv[2], "--setwidth");
	int setheight = strcmp(argv[2], "--setheight");
	int setobjects = strcmp(argv[2], "--setobjects");
	int pruneobject = strcmp(argv[2], "--pruneobjects");

	if (getwidth * getheight * getobjects * getinfo * setwidth * setheight * setobjects * pruneobject)
	{
		fprintf(stderr, "%s\n", "Aivalable commandes : --getwidth, --getheight, --getobjects, --getinfo, --setwidth, --setheight, --setobjects");
		return 1;
	}

	int f = open(argv[1], O_RDONLY);
	map actual_map = convertFile(argv[1]);
	// printf("name: %s\nwidth: %d\nheight: %d\nnb_elem: %d\nnb_object: %d\n", 
	// 	actual_map->filename, actual_map->width, actual_map->height, actual_map->nb_element, actual_map->nb_object);
	if (f == -1)
	{
		fprintf(stderr, "%s %s %s\n", "File", argv[1], "does not exist.");
		return 1;
	}

	if (!getinfo)
	{
		getwidth = 0;
		getheight = 0;
		getobjects = 0;
	}

	if(!pruneobject)
	{
		pruneobjects(actual_map);
		translateMapInFile(actual_map, argv[1]);
	}

	if (!getwidth || !getheight)
	{
		char c = 'a';
		int width = 0;
		int height = 0;

		read(f, &c, sizeof(char));

		while (c != ' ')
		{
			width = width * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			height = height * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		if (!getwidth)
			printf("Width : %d\n", width);
		
		if (!getheight)
			printf("Height : %d\n", height);	
	}

	if (!getobjects)
	{
		if (getinfo)
			skip_line(f);

		int num_object = 0;
		char c = '0';
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		for (int i = 0; i < num_object; i++)
			skip_line(f);

		num_object = 0;
		c = '0';
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		for(int i = 0; i < num_object; i++)
		{
			skip_word(f);
			read_word(f, "\nPath : ");
			read_word(f, ", frames : ");
			for (int i = 0; i < 4; i++)
			{
				int v = read_digit(f);
				read_digit(f);
				switch(i)
				{
					case 0:
					if (!v)
						printf(", not solid");
					else if (v == 1)
						printf(", semi-solid");
					else if (v == 2)
						printf(", solid");
					break;

					case 1:
					if (v)
						printf(", destructible");
					else
						printf(", not destructible");
					break;

					case 2:
					if (v)
						printf(", collectible");
					else
						printf(", not collectible");
					break;

					case 3:
					if (v)
						printf(", is generator.");
					else
						printf(", is not generator.");
					break;
				}
			}
		}

		printf("\n");
	}

	if (!setwidth || !setheight)
	{
		int value_width = atoi(argv[3]);
		int value_height = 12; //TODO
		if (argc < 4 || value_width < 1 || value_height < 1)
		{
			fprintf(stderr, "%s\n", "--setwidth and --setheight need a int value superior to zero.");
		}

		char c = 'a';
		int width = 0;
		int height = 0;
		int num_object = 0;
		read(f, &c, sizeof(char));

		while (c != ' ')
		{
			width = width * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			height = height * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		read(f, &c, sizeof(char));
		while (c != '\n')
		{
			num_object = num_object * 10 + c - '0';
			read(f, &c, sizeof(char));
		}

		map_case prev = NULL;
		map_case first = NULL;
		int nb_objects = 0;
		for (int i = 0; i < num_object; i++)
		{
			int pos_x = 0;
			int pos_y = 0;
			int type = 0;
			read(f, &c, sizeof(char));
			while (c != ' ')
			{
				pos_x = pos_x * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			read(f, &c, sizeof(char));
			while (c != ' ')
			{
				pos_y = pos_y * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			read(f, &c, sizeof(char));
			while (c != '\n')
			{
				type = type * 10 + c - '0';
				read(f, &c, sizeof(char));
			}

			if (pos_x < value_width && pos_y < value_height)
			{
				nb_objects++;

				map_case m_case = (map_case)malloc(sizeof(struct s_map_case));
				m_case->x = pos_x;
				m_case->y = pos_y + (value_height - height);
				m_case->type = type;
				m_case->next = NULL;

				if (first == NULL)
					first = m_case;
				else
					prev->next = m_case;
				prev = m_case;
			}
		}

		//int new_file = open("tmp", O_WRONLY);
		FILE* new_file = fopen("tmp", "w");
		fprintf(new_file, "%d %d\n%d\n", value_width, value_height, nb_objects);
		map_case m_case = first;
		while (m_case != NULL)
		{
			first = m_case;
			fprintf(new_file, "%d %d %d\n", first->x, first->y, first->type);

			m_case = first->next;
			free(first);
		}

		int remaining = read(f, &c, sizeof(char));
		while (remaining > 0)
		{
			fprintf(new_file, "%c", c);
			remaining = read(f, &c, sizeof(char));
		}

		fclose(new_file);
		close(f);
		remove(argv[2]);
		rename("tmp", argv[2]);
	}

	close(f);
	return 0;
}