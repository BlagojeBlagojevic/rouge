//#include"utils.h"
#include "map.h"
#include "entity.h"
#include "app.h"
#define SEED 12344
#include<time.h>

int main() {

	DROP(damageStr);
	DROP(monsterName);
	DROP(monsters);
	SDL_ERR(SDL_Init(SDL_INIT_VIDEO));
	SDL_ERR(TTF_Init());
	monster_definitions_export();
	srand(time(0));
	WINDOW   = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1400, 800,  SDL_WINDOW_OPENGL);
	(void*)P_SDL_ERR(WINDOW);
	RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetWindowResizable(WINDOW, SDL_TRUE);
	FONT = TTF_OpenFont(fontLoc, 128);
	(void*)P_SDL_ERR(FONT);
	(void*)P_SDL_ERR(RENDERER);
	QUIT = 0;
	MOVMENT = SDL_TRUE;
	ITEMSREND = SDL_FALSE;
	Entitiy* player = create_entity('@', "Some Name", 5, 100, (Position) {
		10, 10
		});
	player->attack[0]  = 5;
	player->defence[0] = 5;
	player->attack[1]  = 9;
	player->defence[1] = 5;
	player->attack[2]  = 9;
	player->defence[2] = 5;
	player->attack[3]  = 9;
	player->defence[3] = 5;

	//player->invertory = {0};
	Tile *map = init_map();
	Entitiy_DA monsters = {0};
	Item_DA items = {0};
	Item* sword = create_item(12, 12, SWORD_CREATE());
	da_append(&items, (*sword));
	Item* dart = create_item(14, 14, DART_CREATE());
	da_append(&items, (*dart));
	Item* potion = create_item(16, 16, POTION_CREATE());
	da_append(&items, (*potion));
	//da_append(&items, (*item));
	//da_append(&items, (*item));
	//da_append(&items, (*item));
	//da_append(&items, (*item));
	//da_append(&items, (*item));
	//LOG("char %c\n", items.items[0].ch);
	for(i32 i = 0; i < NUM_RENDER_MSG; i++) {
		da_append(&MESSAGES, "   ");
		}
	genereate_monsters(&monsters, map);
	MOVMENT = 0;
	main_renderer(player,  &monsters, &items, map);
	//MAP_STDOUT();
	while(!QUIT) {
		update_entity(player, &monsters, map, &items);
		main_renderer(player,  &monsters, &items, map);
		event_user(player, &monsters, &items, map);
		
		//for(u64 count = 0; count < MESSAGES.count; count++) {
		//	LOG("%s", MESSAGES.items[count]);
		//	}

		//system("pause");
		//SDL_Delay(1000);
		}

	return 0;
	ASSERT("UNREACHABLE");
	}
