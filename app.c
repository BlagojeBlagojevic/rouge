#include "app.h"
void SDL_ERR(int code) {

	if((code) < 0) {
		ERROR_BREAK("SDL_ERROR: %s\n", SDL_GetError());
		}
	return;
	}

void *P_SDL_ERR(void *ptr) {
	if(ptr == NULL) {
		ERROR_BREAK("SDL_ERROR: %s\n", SDL_GetError());
		}
	return ptr;
	}



void Text_Renderer_C(SDL_Renderer *renderer, TTF_Font *font, i32 startX, i32 startY, i32 w_c, i32 h_c, char *c, SDL_Color textColor) {
	if(renderer == NULL) {
		ASSERT("renderer is null!!!");
		}
	if(font == NULL) {
		ASSERT("font is null!!!");
		}
	SDL_Surface *textSurface = P_SDL_ERR(TTF_RenderText_Solid(font, c, textColor));
	SDL_Texture *textTexture = P_SDL_ERR(SDL_CreateTextureFromSurface(renderer, textSurface));
	SDL_Rect textRect = {startX, startY, w_c, h_c}; // rectangle where the text is drawn
	SDL_ERR(SDL_RenderCopy(renderer, textTexture, NULL, &textRect));
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
	}







void render_player(Entitiy *player) {
	i32 startX = player->pos.x * FONT_W;
	i32 startY = player->pos.y * FONT_H;
	SDL_Color color;
	if(player->health >= 3) {
		color = (SDL_Color) {
			255, 255, 255, 0
			};
		}
	else if(player->health == 2) {
		color = (SDL_Color) {
			255, 125, 125, 0
			};
		}
	else if(player->health == 1) {
		color = (SDL_Color) {
			255, 0, 0, 0
			};
		}
	else {
		color = (SDL_Color) {
			0, 255, 0, 0
			};
		}
	Text_Renderer_C(RENDERER, FONT, startX, startY, 10, 15, &player->ch, color);
	}



void player_input(SDL_Event *event, Entitiy* player, Entitiy_DA *entitis, Tile* map) {
	const u32 key = event->key.keysym.sym;
	MOVMENT = SDL_FALSE;  //NOT PROB
	if(key == UP_ARROW) {
		if(player->pos.y > 0 && MAP_ISW(map, player->pos.x, player->pos.y-1) == SDL_TRUE) {
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			player->pos.y--;
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_FALSE;
			MOVMENT = SDL_TRUE;
			}
		else {
			i32 witchIsMonster = is_monster_on_entity(player->pos.x,  player->pos.y-1, entitis);
			if(witchIsMonster != -1) {
				player_attack(player, &entitis->items[witchIsMonster]);
				MOVMENT = SDL_TRUE;
				}
			}
		}
	else if(key == DOWN_ARROW) {
		if(player->pos.y < MAP_Y && MAP_ISW(map, player->pos.x, player->pos.y+1) == SDL_TRUE) {
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			player->pos.y++;
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_FALSE;
			MOVMENT = SDL_TRUE;
			}
		else {
			i32 witchIsMonster = is_monster_on_entity(player->pos.x,  player->pos.y + 1, entitis);
			if(witchIsMonster != -1) {
				player_attack(player, &entitis->items[witchIsMonster]);
				MOVMENT = SDL_TRUE;
				}
			}

		}
	else if(key == LEFT_ARROW) {
		if(player->pos.x > 0 && MAP_ISW(map, player->pos.x-1, player->pos.y) == SDL_TRUE) {
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			player->pos.x--;
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			MOVMENT = SDL_TRUE;
			}
		else {
			i32 witchIsMonster = is_monster_on_entity(player->pos.x - 1,  player->pos.y, entitis);
			if(witchIsMonster != -1) {
				player_attack(player, &entitis->items[witchIsMonster]);
				MOVMENT = SDL_TRUE;
				}
			}

		}
	else if(key == RIGHT_ARROW) {
		if(player->pos.x < MAP_X && MAP_ISW(map, player->pos.x + 1, player->pos.y) == SDL_TRUE) {
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			player->pos.x++;
			MAP_ISW(map, player->pos.x, player->pos.y) = SDL_TRUE;
			MOVMENT = SDL_TRUE;
			}
		else {
			i32 witchIsMonster = is_monster_on_entity(player->pos.x + 1,  player->pos.y, entitis);
			if(witchIsMonster != -1) {
				player_attack(player, &entitis->items[witchIsMonster]);
				MOVMENT = SDL_TRUE;
				}
			}
		}
	else if(key == SPACE) {
		//DO NOTHING
		MOVMENT = SDL_TRUE;
		}
	}


void render_stats(Entitiy *player) {
	char stats[1024];
	snprintf(stats, 1024, "STATS: Health %d", player->health);
	Text_Renderer_C(RENDERER, FONT, 15, HEIGHT - 100, strlen(stats) * FONT_W, 20, stats, WHITE);
	snprintf(stats, 1024, "STATS: Health %d", player->health);
	Text_Renderer_C(RENDERER, FONT, 15, HEIGHT - 100, strlen(stats) * FONT_W, FONT_W_MESSAGES, stats, WHITE);
	for(Damage_Types i = 0; i < DAMAGE_NUM; i++) {
		stats[0] = '\0';
		snprintf(stats, 1024, "STATS: %s att: %d def: %d", damageStr[i], player->attack[i], player->defence[i]);
		Text_Renderer_C(RENDERER, FONT, 15, HEIGHT - 100 + FONT_W_MESSAGES*(i+1), strlen(stats) * FONT_W,
		                FONT_W_MESSAGES, stats, WHITE);
		}

	}

void render_monsters(Entitiy_DA *monsters, Entitiy *player) {
	DROP(damageStr);
	DROP(monsterName);
	i32 radius = player->radius;
	i32 startX = player->pos.x - radius;
	i32 startY = player->pos.y - radius;
	i32 stopX  = player->pos.x + radius;
	i32 stopY  = player->pos.y + radius;
	CLAMP(startX, 0, MAP_X-1);
	CLAMP(stopX,  0, MAP_X-1);
	CLAMP(startY, 0, MAP_Y-1);
	CLAMP(stopY,  0, MAP_Y-1);//*/
	for(u64 count = 0; count < monsters->count; count++) {
		if(monsters->items[count].pos.x >= startX && monsters->items[count].pos.x <= stopX
		    && monsters->items[count].pos.y >= startY && monsters->items[count].pos.y <= stopY) {
			render_player(&monsters->items[count]);
			}

		}

	}

void render_messages(i32 startX, i32 startY, char* message) {
	if(message != NULL) {
		u64 w_c = strlen(message) * FONT_W;
		Text_Renderer_C(RENDERER, FONT, startX, startY, (i32)w_c, 20, message, WHITE);
		}

	}


void render_map(Tile *map, Entitiy *player) {
	Text_Renderer_C(RENDERER, FONT, WIDTH/2, 0, 10*10, 20, "ROUGE GAME", WHITE);


	i32 radius = player->radius;
	i32 startX = player->pos.x - radius;
	i32 startY = player->pos.y - radius;
	i32 stopX  = player->pos.x + radius;
	i32 stopY  = player->pos.y + radius;
	CLAMP(startX, 0, MAP_X-1);
	CLAMP(stopX,  0, MAP_X-1);
	CLAMP(startY, 0, MAP_Y-1);
	CLAMP(stopY,  0, MAP_Y-1);
	///*
#ifdef PLAYER_VISION
	for(i32 y  = startY; y < stopY; y++) {
		for(i32 x = startX; x < stopX; x++) {//*/
#endif
			///*
#ifndef PLAYER_VISION
			for(i32 y  = 0; y < MAP_Y; y++) {
				for(i32 x = 0; x < MAP_X; x++) {
#endif
					//*/
					i32 startX = x * FONT_W;
					i32 startY = y * FONT_H;
					char ch = MAP_CH(map, x, y);
					if(ch == '#') {
						SDL_Rect textRect = {.x=startX, .y = startY, .w = FONT_W, .h = FONT_H};
						SDL_SetRenderDrawColor(RENDERER, 100, 100, 100, 100);
						SDL_RenderFillRect(RENDERER, &textRect);
						}
					else if(ch == ',') {
						SDL_Rect textRect = {.x=startX, .y = startY, .w = FONT_W, .h = FONT_H};
						SDL_SetRenderDrawColor(RENDERER, 10, 10, 10, 100);
						DROP(textRect);
						//SDL_RenderFillRect(RENDERER, &textRect);
						}
					else if(ch != '.') {
						Text_Renderer_C(RENDERER, FONT, startX, startY, FONT_W, FONT_H, &ch, WHITE);
						}

					}
				}

			}

		void main_renderer(Entitiy* player, Entitiy_DA *monster, Tile *map) {
			SDL_ERR(SDL_RenderClear(RENDERER));
			render_map(map, player);
			render_player(player);
			//render_player(&monster->items[0]);
			render_monsters(monster, player);
			render_stats(player);
			i32 count = 1;
			for(i32 i = (i32)MESSAGES.count-1; i >= ((i32)MESSAGES.count - 5); i--) {
				render_messages((WIDTH - 600), (HEIGHT - 120 + FONT_W_MESSAGES*(count++)), MESSAGES.items[i]);
				}
			SDL_ERR(SDL_SetRenderDrawColor(RENDERER, 0X10, 0X10, 0X10, 0XFF));
			SDL_RenderPresent(RENDERER);
			}

		void event_user(Entitiy *player, Entitiy_DA *entitis, Tile* map) {
			MOVMENT = SDL_FALSE;
			while(MOVMENT == SDL_FALSE) {
				if(SDL_WaitEvent(&EVENT)) {
					if(EVENT.type == SDL_QUIT) {
						MOVMENT = SDL_TRUE;
						QUIT = 1;
						}
					else if(EVENT.type == SDL_WINDOWEVENT) {  //JUST FOR NOW
						MOVMENT = SDL_TRUE;
						SDL_GetWindowSize(WINDOW, &WIDTH, &HEIGHT);
						FONT_H = HEIGHT / MAP_Y - 1;
						FONT_W = WIDTH  / MAP_X;
						//FONT_H = 15;
						//FONT_W = 10;
						if(WIDTH > 4096 || HEIGHT > 2048) {
							ASSERT("Oversized window\n");
							}
						//LOG("width %d, height %d\n", WIDTH, HEIGHT);
						}
					else if(EVENT.type==SDL_KEYDOWN) {
						player_input(&EVENT, player, entitis, map);
						}
					}
				}
			}