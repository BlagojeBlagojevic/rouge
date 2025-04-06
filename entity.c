#include "entity.h"

Entitiy* create_entity(char ch, i32 radius, i32 health, Position startPos) {
	Entitiy* entity = calloc(1, sizeof(Entitiy));

	if(ch != '@') {
		for(Monster_Types m = 0; m < NUM_MONSTER; m++) {
			if(ch == monsterName[m]) {
				memcpy(entity, &monsters[m], sizeof(Entitiy));
				}
			}
		}
	else {
		entity->ch = ch;
		entity->radius = radius;
		}

	entity->pos.x = startPos.x;
	entity->pos.y = startPos.y;
	entity->health = health;
	return entity;
	}



i32 roll_the_dice(i32 attack, i32 defence) {
	f64 maxAttack = 0.0f, maxDefence = 0.0f, num;
	//ATTACK
	for(i32 i = 0; i < attack; i++) {
		num = rand_f64();

		if(num > maxAttack) {
			maxAttack = num;
			}
		}
	//DEFENCE
	for(i32 i = 0; i < defence; i++) {
		num = rand_f64();

		if(num > maxDefence) {
			maxDefence = num;
			}
		}

	if(maxDefence > maxAttack) {
		return 0;
		}

	i32 diff = (i32)((maxAttack - maxDefence) * attack);
	CLAMP(diff, 1, INF);
	return diff;
	}

///TBD reusable
void message_attacked_by_monster(Entitiy* player, Entitiy* entity, i32 damage, Damage_Types type) {
	DROP(player);
	u64 len = 100;
	char* attackText = malloc(len*sizeof(char*));
	memset(attackText, '\0', len);
	if(attackText == NULL) {
		ASSERT("CALLOC FAILED\n");
		}
	i32 err = snprintf(attackText, len, "You are attacked by %c %s (-%d)", entity->ch, damageStr[type], damage);
	if(err < -1) {
		ASSERT("snprintf failed");
		}
	//LOG("%s\n", attackText);
	da_append(&MESSAGES, attackText);
	//add_to_str(attackText, &MESSAGES);
	//Text_Renderer_C(RENDERER, FONT, WIDTH - 100, HEIGHT - 100, 100, 20, "Da li ovo radi", WHITE);
	}

void message_attacked_by_player(Entitiy* player, Entitiy* entity, i32 damage) {
	DROP(player);
	DROP(damageStr);
	u64 len = strlen("x attacked by you damage (xxxx)") + 10;
	char* attackText = malloc(len*sizeof(char*));
	memset(attackText, '\0', len);
	if(attackText == NULL) {
		ASSERT("CALLOC FAILED\n");
		}
	i32 err = snprintf(attackText, len, "%c attacked by you damage (%d)", entity->ch, damage);
	if(err < -1) {
		ASSERT("snprintf failed");
		}
	//LOG("%s\n", attackText);
	da_append(&MESSAGES, attackText);
	//add_to_str(attackText, &MESSAGES);
	//Text_Renderer_C(RENDERER, FONT, WIDTH - 100, HEIGHT - 100, 100, 20, "Da li ovo radi", WHITE);
	}

//TBD attack type
void player_attack(Entitiy *player, Entitiy* entity) {

	i32 damage = roll_the_dice(player->attack[0], entity->defence[0]);
	entity->health-=damage;
	CLAMP(entity->health, 0, 100);
	message_attacked_by_player(player, entity, damage);
	//DROP(player);
	//i32 x1 = player->pos.x;
	//i32 y1 = player->pos.y;

	//i32 x2 = entity->pos.x;
	//i32 y2 = entity->pos.y;
	//f64 distance = DISTANCE(x1, y1, x2, y2);
	//if(distance == 0.0f) {
	//	}
	if(entity->health == 0) {
		entity->ch = 'S';
		}
	}


//TBD attack type(Monsters prob always use a higest stats for attack
//TBD range attack type aka fire attack
void monster_attack(Entitiy *player, Entitiy* entity, f64 distance) {
	//DROP(entity);
	i32 damage = 0;
	if(distance != 0) {
		if(distance >= DISTANCE_RANGE_ATTACK_MIN && distance <= DISTANCE_RANGE_ATTACK_MAX) {
			if(entity->attack[DAMAGE_RANGE] > entity->attack[DAMAGE_SPELL]) {
				damage = roll_the_dice(entity->attack[DAMAGE_RANGE], player->defence[DAMAGE_RANGE]);
				if(entity->attack[DAMAGE_RANGE] != 0) {
					message_attacked_by_monster(player, entity, damage, DAMAGE_RANGE);
					}

				}
			else {
				damage = roll_the_dice(entity->attack[DAMAGE_SPELL], player->defence[DAMAGE_SPELL]);
				if(entity->attack[DAMAGE_SPELL] != 0) {
					message_attacked_by_monster(player, entity, damage, DAMAGE_SPELL);
					}

				}
			}
		}
	else {
		if(entity->attack[DAMAGE_BASIC] > entity->attack[DAMAGE_POISON]) {
			damage = roll_the_dice(entity->attack[DAMAGE_BASIC], player->defence[DAMAGE_BASIC]);
			message_attacked_by_monster(player, entity, damage, DAMAGE_BASIC);
			}
		else {
			damage = roll_the_dice(entity->attack[DAMAGE_POISON], player->defence[DAMAGE_POISON]);
			if(damage != 0) {
				message_attacked_by_monster(player, entity, damage, DAMAGE_POISON);
				}

			}
		}



	player->health-=damage;
	CLAMP(player->health, 0, INF);
	//i32 startX =  player->pos.x;

	//system("pause");
	//Text_Renderer_C(RENDERER, FONT, )
	if(player->health == 0) {

		system("cls");
		LOG("You loose");
		exit(-1);
		}
	}

void monster_definitions_export() {


	//PROB THRU FILE OR SOMTHING
	//FOR NOW LET BE IN CODE
	//BASIC MONSTER
	//ATT
	monsters[BASIC_MONSTER].radius = 10;
	monsters[BASIC_MONSTER].ch = 'M';
	monsters[BASIC_MONSTER].attack[DAMAGE_BASIC]  = 3;
	monsters[BASIC_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[BASIC_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[BASIC_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF
	monsters[BASIC_MONSTER].defence[DAMAGE_BASIC]  = 3;
	monsters[BASIC_MONSTER].defence[DAMAGE_POISON] = 1;
	monsters[BASIC_MONSTER].defence[DAMAGE_RANGE]  = 2;
	monsters[BASIC_MONSTER].defence[DAMAGE_SPELL]  = 1;

	//ZOMBIE MONSTER
	//ATT

	monsters[ZOMBIE_MONSTER].radius = 50;
	monsters[ZOMBIE_MONSTER].ch = 'Z';
	monsters[ZOMBIE_MONSTER].attack[DAMAGE_BASIC]  = 1;
	monsters[ZOMBIE_MONSTER].attack[DAMAGE_POISON] = 4;
	monsters[ZOMBIE_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[ZOMBIE_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF
	monsters[ZOMBIE_MONSTER].defence[DAMAGE_BASIC]  = 3;
	monsters[ZOMBIE_MONSTER].defence[DAMAGE_POISON] = 3;
	monsters[ZOMBIE_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[ZOMBIE_MONSTER].defence[DAMAGE_SPELL]  = 3;

	//WIZARD MONSTER
	//ATT
	monsters[WIZARD_MONSTER].radius = 20;
	monsters[WIZARD_MONSTER].ch = 'W';
	monsters[WIZARD_MONSTER].attack[DAMAGE_BASIC]  = 1;
	monsters[WIZARD_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[WIZARD_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[WIZARD_MONSTER].attack[DAMAGE_SPELL]  = 4;
	//DEF
	monsters[WIZARD_MONSTER].defence[DAMAGE_BASIC]  = 1;
	monsters[WIZARD_MONSTER].defence[DAMAGE_POISON] = 3;
	monsters[WIZARD_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[WIZARD_MONSTER].defence[DAMAGE_SPELL]  = 5;

	//BEAR MONSTER
	//ATT

	monsters[BEAR_MONSTER].radius = 20;
	monsters[BEAR_MONSTER].ch = 'B';
	monsters[BEAR_MONSTER].attack[DAMAGE_BASIC]  = 4;
	monsters[BEAR_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[BEAR_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[BEAR_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF
	monsters[BEAR_MONSTER].defence[DAMAGE_BASIC]  = 4;
	monsters[BEAR_MONSTER].defence[DAMAGE_POISON] = 1;
	monsters[BEAR_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[BEAR_MONSTER].defence[DAMAGE_SPELL]  = 1;

	//CROW MONSTER
	//ATT
	monsters[CROW_MONSTER].radius = 20;
	monsters[CROW_MONSTER].ch = 'C';
	monsters[CROW_MONSTER].attack[DAMAGE_BASIC]  = 1;
	monsters[CROW_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[CROW_MONSTER].attack[DAMAGE_RANGE]  = 4;
	monsters[CROW_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF
	monsters[CROW_MONSTER].defence[DAMAGE_BASIC]  = 6;
	monsters[CROW_MONSTER].defence[DAMAGE_POISON] = 1;
	monsters[CROW_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[CROW_MONSTER].defence[DAMAGE_SPELL]  = 1;

	//DEMON MONSTER
	//ATT
	monsters[DEMON_MONSTER].radius = 20;
	monsters[DEMON_MONSTER].ch = 'D';
	monsters[DEMON_MONSTER].attack[DAMAGE_BASIC]  = 3;
	monsters[DEMON_MONSTER].attack[DAMAGE_POISON] = 3;
	monsters[DEMON_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[DEMON_MONSTER].attack[DAMAGE_SPELL]  = 5;
	//DEF
	monsters[DEMON_MONSTER].defence[DAMAGE_BASIC]  = 4;
	monsters[DEMON_MONSTER].defence[DAMAGE_POISON] = 6;
	monsters[DEMON_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[DEMON_MONSTER].defence[DAMAGE_SPELL]  = 3;

	//GHOST MONSTER
	//ATT
	monsters[GHOST_MONSTER].radius = 20;
	monsters[GHOST_MONSTER].ch = 'G';
	monsters[GHOST_MONSTER].attack[DAMAGE_BASIC]  = 3;
	monsters[GHOST_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[GHOST_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[GHOST_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF
	monsters[GHOST_MONSTER].defence[DAMAGE_BASIC]  = 4;
	monsters[GHOST_MONSTER].defence[DAMAGE_POISON] = 6;
	monsters[GHOST_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[GHOST_MONSTER].defence[DAMAGE_SPELL]  = 3;
	//return monsters;
	}


void genereate_monsters(Entitiy_DA *monsters, Tile *map) {
	for(i32 y = 0; y < MAP_Y; y++) {
		for(i32 x = 0; x < MAP_X; x++) {
			if(MAP_CH(map, x, y) != '#') {
				if(rand_f64() < PERCENTAGE_MONSTER_GENERATED) {
					i32 type = rand()%NUM_MONSTER;
					i32 vison = rand()%40+1;
					Entitiy *temp = create_entity(monsterName[type], vison, 3, (Position) {
						.x = x, .y = y
						});
					da_append(monsters, *temp);
					}
				}
			}
		}
	LOG("\nGenerated monsters %d\n", (i32)monsters->count);
	}

SDL_bool Is_Monster(char c) {
	for(Monster_Types t = BASIC_MONSTER; t < NUM_MONSTER; t++) {
		if(monsterName[t] == c) {
			//LOG("True\n\n");
			return SDL_TRUE;
			}
		}
	return SDL_FALSE;
	}


i32 is_monster_on_entity(i32 x, i32 y, Entitiy_DA* entities) {
	CLAMP(x, 0, (MAP_X - 1));
	CLAMP(y, 0, (MAP_Y - 1));
	for(u64 count = 0; count < entities->count; count++) {
		Entitiy entity = entities->items[count];
		if(Is_Monster(entity.ch) == SDL_TRUE && entity.pos.x == x && entity.pos.y == y) {

			entities->items[count] = entity;
			//LOG("health %d, count %d", entity.health, count);
			return count;
			}
		}


	return -1;
	}



//BLOCK MOVMENT DEPENDING ON TYPE OF MONSTERS
void block_movement(Entitiy_DA *entitys, Tile *map) {
	for(u64 count = 0; count < entitys->count; count++) {
		if(Is_Monster(entitys->items[count].ch) == SDL_TRUE) {
			i32 x = entitys->items[count].pos.x;
			i32 y = entitys->items[count].pos.y;
			MAP_ISW(map, x, y) = SDL_FALSE;
			}
		else if(entitys->items[count].ch == 'S') {
			i32 x = entitys->items[count].pos.x;
			i32 y = entitys->items[count].pos.y;
			MAP_ISW(map, x, y) = SDL_TRUE;
			}
		}
	}
//check if 2 fieled of vison colide
SDL_bool check_colison_entitiy(Entitiy* player, Entitiy*  ent) {
	SDL_Rect A = {.h = player->radius, .w = player->radius, .x = player->pos.x, .y = player->pos.y };
	SDL_Rect B = {.h = ent->radius, .w = ent->radius, .x = ent->pos.x, .y = ent->pos.y };

	i32 leftA, leftB;
	i32 rightA, rightB;
	i32 topA, topB;
	i32 bottomA, bottomB;

	leftA = A.x;
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;

	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	if( bottomA <= topB ) {
		return SDL_FALSE;
		}

	if( topA >= bottomB ) {
		return SDL_FALSE;
		}

	if( rightA <= leftB ) {
		return SDL_FALSE;
		}

	if( leftA >= rightB ) {
		return SDL_FALSE;
		}

	return SDL_TRUE;
	}


f64 distnace_move(i32 x1, i32 y1, i32 x2, i32 y2, Tile *map) {
	CLAMP(x2, 0, MAP_X-1);
	CLAMP(y2, 0, MAP_Y-1);
	if(MAP_ISW(map, x2, y2) == SDL_TRUE) {
		f64 distance = DISTANCE(x1, y1, x2, y2);
		//if(distance == 0.0f){
		//	return INF;
		//}
		return distance;
		}
	//LOG("dist INF\n");
	//system("pause");
	return INF;
	}
void make_run_move(Entitiy* player, Entitiy*  ent, Tile *map) {
	i32 x1 = player->pos.x;
	i32 y1 = player->pos.y;
	i32 x2 = ent->pos.x;
	i32 y2 = ent->pos.y;
	f64 distance  = DISTANCE(x1, y1, x2, y2);

	//MOVES WILL DEPEND OF WHAT MONSTER IS!!!

	//+1x
	f64 distancesMax = distnace_move(x1, y1, (x2 + 1), y2, map);
	if(distancesMax == INF) {
		distancesMax = 0.0f;
		}
	i32 index = 0;
	//-1x
	distance = distnace_move(x1, y1, (x2 - 1), y2, map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 1;
		}
	//+1y
	distance = distnace_move(x1, y1, x2, (y2 + 1), map);
	if(distance > distancesMax && distance != INF) {
		distancesMax = distance;
		index = 2;
		}
	//-1y
	distance = distnace_move(x1, y1, x2, (y2  - 1), map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 3;
		}

	switch(index) {
		case 0: {
				if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN && distancesMax != INF) {
					//LOG("X++\n");
					ent->pos.x = ent->pos.x + 1;
					}
				break;
				}

		case 1: {
				if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
					//LOG("X--\n");
					ent->pos.x--;
					}
				break;
				}
		case 2: {
				if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
					//LOG("Y++\n");
					ent->pos.y++;
					}
				break;
				}
		case 3: {
				if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
					//LOG("Y--\n");
					ent->pos.y--;
					}
				break;
				}
		default: {
				ASSERT("Unreachable");
				break;
				}
		}



	}
//WE WILL SEE IF A* or Diakstra or This CRAP
void make_best_move(Entitiy* player, Entitiy*  ent, Tile *map) {
	i32 x1 = player->pos.x;
	i32 y1 = player->pos.y;
	i32 x2 = ent->pos.x;
	i32 y2 = ent->pos.y;
	f64 distance  = DISTANCE(x1, y1, x2, y2);

	//MOVES WILL DEPEND OF WHAT MONSTER IS!!!

	//+1x
	f64 distancesMin = distnace_move(x1, y1, (x2 + 1), y2, map);
	i32 index = 0;
	//-1x
	distance = distnace_move(x1, y1, (x2 - 1), y2, map);
	if(distance < distancesMin) {
		distancesMin = distance;
		index = 1;
		}
	//+1y
	distance = distnace_move(x1, y1, x2, (y2 + 1), map);
	if(distance < distancesMin) {
		distancesMin = distance;
		index = 2;
		}
	//-1y
	distance = distnace_move(x1, y1, x2, (y2  - 1), map);
	if(distance < distancesMin) {
		distancesMin = distance;
		index = 3;
		}
	if(distancesMin >= 0 && distancesMin <= DISTANCE_RANGE_ATTACK_MAX) {
		monster_attack(player, ent, distancesMin);
		//return;
		}
	switch(ent->ch) {
		case 'W': {
				if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
					return;
					break;
					}
				}
		case 'C': {
				if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
					return;
					break;
					}
				}
		case 'D': {
				if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
					return;
					break;
					}
			default: {
					break;
					}
				}
		}
	switch(index) {
		case 0: {
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("X++\n");
					ent->pos.x = ent->pos.x + 1;
					}
				break;
				}

		case 1: {
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("X--\n");
					ent->pos.x--;
					}
				break;
				}
		case 2: {
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y++\n");
					ent->pos.y++;
					}
				break;
				}
		case 3: {
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y--\n");
					ent->pos.y--;
					}
				break;
				}
		default: {
				ASSERT("Unreachable");
				break;
				}
		}

	}

//IF IN VISON FIELD MOVE TOWARDS PLAYER
//IF NOT RAND MOV DEPENDING ON TYPE OR PROB
void move_entity(Entitiy* player, Entitiy_DA *entitys, Tile *map) {
	i32 co = 0;
	DROP(co);
	for(u64 count = 0; count < entitys->count; count++) {
		Entitiy entity = entitys->items[count];
		if(Is_Monster(entity.ch)) {
			if(check_colison_entitiy(player, &entity) == SDL_TRUE) {
				//co++;
				if(entity.health == 1 && rand_f64() <= PERCANTAGE_RUN_CHANCE) {
					MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
					make_run_move(player, &entity, map);
					MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
					entitys->items[count] = entity;
					}
				else if(entity.ch == 'C' && rand_f64() <= PERCANTAGE_CROW_RUN_CHANCE) {
					f64 distance = DISTANCE(player->pos.x, player->pos.y, entity.pos.x, entity.pos.y);
					if(distance >= DISTANCE_RANGE_ATTACK_MIN && distance <= DISTANCE_RANGE_ATTACK_MAX) {
						MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
						make_run_move(player, &entity, map);
						MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
						entitys->items[count] = entity;
						}
					}
				else {
					MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
					make_best_move(player, &entity, map);
					MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
					entitys->items[count] = entity;
					}
				}
			}
		}
	//LOG("Colided entitys %d\n", co);
	}

void update_entity(Entitiy* player, Entitiy_DA *entitys, Tile *map) {

	move_entity(player, entitys, map);
	block_movement(entitys, map);
	}



