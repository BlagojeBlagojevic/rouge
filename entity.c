#include "entity.h"

Entitiy* create_entity(char ch, const char* name, i32 radius, i32 health, Position startPos) {
	Entitiy* entity = calloc(1, sizeof(Entitiy));
	entity->health = health;
	entity->maxHealth = health;
	if(ch != '@') {
		for(Monster_Types m = 0; m < NUM_MONSTER; m++) {
			if(ch == monsterChar[m]) {
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
	entity->isAlive = SDL_TRUE;
	memset(&entity->inventory, 0, sizeof(Item_DA));
	//DROP(health);
	//i32 len = strlen(name);
	//entity->name = calloc(name, )
	//strcpy(entity->name, name);
	if(name != NULL) {
		i32 len = strlen(name);
		CLAMP(len, 1, (MAX_NAME-1));
		entity->name = calloc(NUM_MONSTER, sizeof(char*));
		memcpy(entity->name, name, len);
		}

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
	i32 err = snprintf(attackText, len, "You are attacked by %s %s (-%d)", entity->name, damageStr[type], damage);
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
void player_attack(Entitiy *player, Entitiy* entity, Item_DA *items, Tile* map) {
	DROP(map);
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
		entity->isAlive = SDL_FALSE;
		//entity->ch = 'S';
		//DROP ITEMS
		for(u64 i = 0; i < entity->inventory.count; i++) {
			SDL_bool drop = SDL_FALSE;
			i32 mod = 3;
			i32 neg = -1;
			while(drop == SDL_FALSE) {

				drop = SDL_TRUE;
				i32 x = rand()%mod + neg;
				i32 y = rand()%mod + neg;
				//KINDA WE DO NOT SEE THEM IF IT DROP IN A WALL LIKE IF IT IS LOST
				//ITS STUPID
				Item item = entity->inventory.items[i];
				x+=entity->pos.x;
				y+=entity->pos.y;
				for(u64 j = 0; j < items->count; j++) {
					if(x == items->items[j].pos.x && y == items->items[j].pos.y) {

						drop = SDL_FALSE;
						mod++;
						neg--;
						break;
						}
					}
				if(drop == SDL_TRUE) {
					item.pos.x = x;
					item.pos.y = y;
					//LOG("Droped(%d %d)\n\n", x, y);
					da_append(items, item);
					}
				}

			}
		}
	}


//TBD attack type(Monsters prob always use a higest stats for attack
//TBD range attack type aka fire attack
void monster_attack(Entitiy *player, Entitiy* entity, f64 distance) {
	//DROP(entity);
	i32 damage = 0;
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

	else {
		if(entity->attack[DAMAGE_BASIC] > entity->attack[DAMAGE_POISON]) {
			damage = roll_the_dice(entity->attack[DAMAGE_BASIC], player->defence[DAMAGE_BASIC]);
			message_attacked_by_monster(player, entity, damage, DAMAGE_BASIC);
			}
		else {
			damage = roll_the_dice(entity->attack[DAMAGE_POISON], player->defence[DAMAGE_POISON]);
			message_attacked_by_monster(player, entity, damage, DAMAGE_POISON);
			}
		}

	player->health-=damage;
	CLAMP(player->health, 0, INF);
	//APPLAY LIFE STEAL TO ENEMY
	if(rand_f64() < entity->lifeStealChance && entity->lifeStealValue != 0 && entity->health < entity->maxHealth){
		entity->health+=(i32)entity->lifeStealValue;
		CLAMP(entity->health, 0, entity->maxHealth);
		char* msg = calloc(50, sizeof(char));
		snprintf(msg, 50, "%s lifestealed %u", entity->name, entity->lifeStealValue);
		da_append(&MESSAGES, msg);
	}
	//i32 startX =  player->pos.x;

	//system("pause");
	//Text_Renderer_C(RENDERER, FONT, )
	if(player->health == 0) {

		//(void)system("cls");
		LOG("You loose");
		exit(-1);
		}
	}
/*


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

	monsters[BASIC_MONSTER].isRunning = SDL_FALSE;
	monsters[BASIC_MONSTER].runWoundedPercent = 0.6f;
	monsters[BASIC_MONSTER].state = STATE_WANDERING;

	monsters[BASIC_MONSTER].stateChance[STATE_RUNING] = 0.6f;
	monsters[BASIC_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.01f;
	monsters[BASIC_MONSTER].stateChance[STATE_HUNTING] = 0.05f;
	monsters[BASIC_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[BASIC_MONSTER].stateChance[STATE_RESTING] = 0.3f;
	monsters[BASIC_MONSTER].stateChance[STATE_BESERK] = 0.01f;
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

	monsters[ZOMBIE_MONSTER].isRunning = SDL_FALSE;
	monsters[ZOMBIE_MONSTER].runWoundedPercent = 0.0f;
	monsters[ZOMBIE_MONSTER].state = STATE_WANDERING;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_RUNING] = 0.01f;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.01f;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_HUNTING] = 0.6f;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_WANDERING] = 0.05f;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_RESTING] = 0.05f;
	monsters[ZOMBIE_MONSTER].stateChance[STATE_BESERK] = 0.6f;
	//WIZARD MONSTER
	//ATT
	monsters[WIZARD_MONSTER].radius = 20;
	monsters[WIZARD_MONSTER].ch = 'W';
	monsters[WIZARD_MONSTER].attack[DAMAGE_BASIC]  = 2;
	monsters[WIZARD_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[WIZARD_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[WIZARD_MONSTER].attack[DAMAGE_SPELL]  = 4;
	//DEF
	monsters[WIZARD_MONSTER].defence[DAMAGE_BASIC]  = 1;
	monsters[WIZARD_MONSTER].defence[DAMAGE_POISON] = 3;
	monsters[WIZARD_MONSTER].defence[DAMAGE_RANGE]  = 3;
	monsters[WIZARD_MONSTER].defence[DAMAGE_SPELL]  = 5;

	monsters[WIZARD_MONSTER].isRunning = SDL_FALSE;
	monsters[WIZARD_MONSTER].runWoundedPercent = 0.40f;
	monsters[WIZARD_MONSTER].state = STATE_WANDERING;
	
	monsters[WIZARD_MONSTER].stateChance[STATE_RUNING] = 0.05f;
	monsters[WIZARD_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.3f;
	monsters[WIZARD_MONSTER].stateChance[STATE_HUNTING] = 0.5f;
	monsters[WIZARD_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[WIZARD_MONSTER].stateChance[STATE_RESTING] = 0.1f;
	monsters[WIZARD_MONSTER].stateChance[STATE_BESERK] = 0.05f;
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

	monsters[BEAR_MONSTER].isRunning = SDL_FALSE;
	monsters[BEAR_MONSTER].runWoundedPercent = 0.20f;
	monsters[BEAR_MONSTER].state = STATE_WANDERING;
	monsters[BEAR_MONSTER].stateChance[STATE_RUNING] = 0.5f;
	monsters[BEAR_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.03f;
	monsters[BEAR_MONSTER].stateChance[STATE_HUNTING] = 0.5f;
	monsters[BEAR_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[BEAR_MONSTER].stateChance[STATE_RESTING] = 0.7f;
	monsters[BEAR_MONSTER].stateChance[STATE_BESERK] = 0.7f;
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

	monsters[CROW_MONSTER].isRunning = SDL_FALSE;
	monsters[CROW_MONSTER].runWoundedPercent = 0.30f;
	monsters[CROW_MONSTER].state = STATE_WANDERING;
	
	monsters[CROW_MONSTER].stateChance[STATE_RUNING] = 0.05f;
	monsters[CROW_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.5f;
	monsters[CROW_MONSTER].stateChance[STATE_HUNTING] = 0.5f;
	monsters[CROW_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[CROW_MONSTER].stateChance[STATE_RESTING] = 0.1f;
	monsters[CROW_MONSTER].stateChance[STATE_BESERK] = 0.05f;
	//DEMON MONSTER
	//ATT
	monsters[DEMON_MONSTER].radius = 40;
	monsters[DEMON_MONSTER].ch = 'D';
	monsters[DEMON_MONSTER].attack[DAMAGE_BASIC]  = 3;
	monsters[DEMON_MONSTER].attack[DAMAGE_POISON] = 3;
	monsters[DEMON_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[DEMON_MONSTER].attack[DAMAGE_SPELL]  = 5;
	//DEF
	monsters[DEMON_MONSTER].defence[DAMAGE_BASIC]  = 4;
	monsters[DEMON_MONSTER].defence[DAMAGE_POISON] = 6;
	monsters[DEMON_MONSTER].defence[DAMAGE_RANGE]  = 0;
	monsters[DEMON_MONSTER].defence[DAMAGE_SPELL]  = 3;

	monsters[DEMON_MONSTER].isRunning = SDL_FALSE;
	monsters[DEMON_MONSTER].runWoundedPercent = 0.0f;
	monsters[DEMON_MONSTER].state = STATE_WANDERING;
	
	monsters[DEMON_MONSTER].stateChance[STATE_RUNING] = 0.05f;
	monsters[DEMON_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.05f;
	monsters[DEMON_MONSTER].stateChance[STATE_HUNTING] = 0.9f;
	monsters[DEMON_MONSTER].stateChance[STATE_WANDERING] = 0.1f;
	monsters[DEMON_MONSTER].stateChance[STATE_RESTING] = 0.01f;
	monsters[DEMON_MONSTER].stateChance[STATE_BESERK] = 0.8f;
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

	monsters[GHOST_MONSTER].isRunning = SDL_FALSE;
	monsters[GHOST_MONSTER].runWoundedPercent = 0.0f;
	monsters[GHOST_MONSTER].state = STATE_WANDERING;
	for(i32 i = 0; i < (i32)STATE_NUM; i++){
		monsters[GHOST_MONSTER].stateChance[i] = rand_f64();
	}
*/
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
	monsters[BASIC_MONSTER].health = 3;

	monsters[BASIC_MONSTER].isRunning = SDL_FALSE;
	monsters[BASIC_MONSTER].runWoundedPercent = 0.6f;
	monsters[BASIC_MONSTER].state = STATE_WANDERING;

	monsters[BASIC_MONSTER].stateChance[STATE_RUNING] = 0.6f;
	monsters[BASIC_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.01f;
	monsters[BASIC_MONSTER].stateChance[STATE_HUNTING] = 0.05f;
	monsters[BASIC_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[BASIC_MONSTER].stateChance[STATE_RESTING] = 0.3f;
	monsters[BASIC_MONSTER].stateChance[STATE_BESERK] = 0.01f;

	//ACOLAYT 
	monsters[ACOLAYT_MONSTER].radius = 20;
	monsters[ACOLAYT_MONSTER].ch = 'A';
	monsters[ACOLAYT_MONSTER].attack[DAMAGE_BASIC]  = 1;
	monsters[ACOLAYT_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[ACOLAYT_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[ACOLAYT_MONSTER].attack[DAMAGE_SPELL]  = 1;
	//DEF  unarmord
	monsters[ACOLAYT_MONSTER].defence[DAMAGE_BASIC]  = 2;
	monsters[ACOLAYT_MONSTER].defence[DAMAGE_POISON] = 2;
	monsters[ACOLAYT_MONSTER].defence[DAMAGE_RANGE]  = 2;
	monsters[ACOLAYT_MONSTER].defence[DAMAGE_SPELL]  = 2;
	monsters[ACOLAYT_MONSTER].health = 5;
	monsters[ACOLAYT_MONSTER].maxHealth = 5;

	monsters[ACOLAYT_MONSTER].isRunning = SDL_FALSE;
	monsters[ACOLAYT_MONSTER].runWoundedPercent = 0.6f;
	monsters[ACOLAYT_MONSTER].state = STATE_WANDERING;

	monsters[ACOLAYT_MONSTER].stateChance[STATE_RUNING] = 0.05f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.05f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_HUNTING] = 0.6f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_WANDERING] = 0.5f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_RESTING] = 0.3f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_BESERK] = 0.01f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_RESURECT] = 0.30f;
	monsters[ACOLAYT_MONSTER].stateChance[STATE_SUMMON] = 0.0f;
	
	monsters[ACOLAYT_MONSTER].lifeStealChance = 0.0f;
	monsters[ACOLAYT_MONSTER].lifeStealValue = 0.0f;
	//GHOUL 
	monsters[GHOUL_MONSTER].radius = 30;
	monsters[GHOUL_MONSTER].ch = 'G';
	monsters[GHOUL_MONSTER].attack[DAMAGE_BASIC]  = 2;
	monsters[GHOUL_MONSTER].attack[DAMAGE_POISON] = 0;
	monsters[GHOUL_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[GHOUL_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF  unarmord
	monsters[GHOUL_MONSTER].defence[DAMAGE_BASIC]  = 2;
	monsters[GHOUL_MONSTER].defence[DAMAGE_POISON] = 2;
	monsters[GHOUL_MONSTER].defence[DAMAGE_RANGE]  = 2;
	monsters[GHOUL_MONSTER].defence[DAMAGE_SPELL]  = 2;
	monsters[GHOUL_MONSTER].health = 7;
	monsters[GHOUL_MONSTER].maxHealth = 7;

	monsters[GHOUL_MONSTER].isRunning = SDL_FALSE;
	monsters[GHOUL_MONSTER].runWoundedPercent = 0.6f;
	monsters[GHOUL_MONSTER].state = STATE_WANDERING;

	monsters[GHOUL_MONSTER].stateChance[STATE_RUNING] = 0.01f;
	monsters[GHOUL_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.01f;
	monsters[GHOUL_MONSTER].stateChance[STATE_HUNTING] = 0.7f;
	monsters[GHOUL_MONSTER].stateChance[STATE_WANDERING] = 0.4f;
	monsters[GHOUL_MONSTER].stateChance[STATE_RESTING] = 0.2f;
	monsters[GHOUL_MONSTER].stateChance[STATE_BESERK] = 0.2f;
	monsters[GHOUL_MONSTER].stateChance[STATE_RESURECT] = 0.00f;
	monsters[GHOUL_MONSTER].stateChance[STATE_SUMMON] = 0.0f;

	monsters[GHOUL_MONSTER].lifeStealChance = 0.1f;
	monsters[GHOUL_MONSTER].lifeStealValue  = 1.0f;
	
	
	//GHOUL 
	monsters[NECROMANCER_MONSTER].radius = 5;
	monsters[NECROMANCER_MONSTER].ch = 'N';
	monsters[NECROMANCER_MONSTER].attack[DAMAGE_BASIC]  = 0;
	monsters[NECROMANCER_MONSTER].attack[DAMAGE_POISON] = 1;
	monsters[NECROMANCER_MONSTER].attack[DAMAGE_RANGE]  = 0;
	monsters[NECROMANCER_MONSTER].attack[DAMAGE_SPELL]  = 0;
	//DEF  light
	monsters[NECROMANCER_MONSTER].defence[DAMAGE_BASIC]  = 3;
	monsters[NECROMANCER_MONSTER].defence[DAMAGE_POISON] = 1;
	monsters[NECROMANCER_MONSTER].defence[DAMAGE_RANGE]  = 1;
	monsters[NECROMANCER_MONSTER].defence[DAMAGE_SPELL]  = 3;
	monsters[NECROMANCER_MONSTER].health = 2;
	monsters[NECROMANCER_MONSTER].maxHealth = 2;

	monsters[NECROMANCER_MONSTER].isRunning = SDL_FALSE;
	monsters[NECROMANCER_MONSTER].runWoundedPercent = 0.9f;
	monsters[NECROMANCER_MONSTER].state = STATE_WANDERING;

	monsters[NECROMANCER_MONSTER].stateChance[STATE_RUNING] = 0.3f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_MOVING_AWAY_RANGE] = 0.001f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_HUNTING] = 0.01f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_WANDERING] = 0.4f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_RESTING] = 0.2f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_BESERK] = 0.00f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_RESURECT] = 0.00f;
	monsters[NECROMANCER_MONSTER].stateChance[STATE_SUMMON] = 0.6f;

	monsters[NECROMANCER_MONSTER].lifeStealChance = 0.0f;
	monsters[NECROMANCER_MONSTER].lifeStealValue  = 0.0f;

	monsters[NECROMANCER_MONSTER].typesToSummon = GHOUL_MONSTER;
	monsters[NECROMANCER_MONSTER].turnsToSummon = 0;
	monsters[NECROMANCER_MONSTER].cooldown = 20;
	

	//return monsters;
	}



void genereate_monsters(Entitiy_DA *monsters, Tile *map) {
	for(i32 y = 0; y < MAP_Y; y++) {
		for(i32 x = 0; x < MAP_X; x++) {
			if(MAP_CH(map, x, y) != '#') {
				if(rand_f64() < PERCENTAGE_MONSTER_GENERATED) {
					i32 type = rand()%(NUM_MONSTER - 1) + 1;
					//type = NECROMANCER_MONSTER;
					i32 vison = rand()%40+1;
					//i32 health = monsters->items[type].health;
					Entitiy *temp = create_entity(monsterChar[type], monsterName[type], vison, 3, (Position) {
						.x = x, .y = y
						});
					if(rand_f64() < 0.5f) {
						u64 count = monsters->count;
						if(count > 0) {
							Item *item  = create_item(0, 0, SWORD_CREATE());
							da_append(&monsters->items[count-1].inventory, (*item));
							}
						}
					da_append(monsters, *temp);
					}
				}
			}
		}
	LOG("\nGenerated monsters %d\n", (i32)monsters->count);
	}

SDL_bool Is_Monster(char c) {
	for(Monster_Types t = BASIC_MONSTER; t < NUM_MONSTER; t++) {
		if(monsterChar[t] == c) {
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
//check if 2 rec colide or if fieled of vison colide
SDL_bool check_colison_entitiy(Entitiy* player, Entitiy* ent, Tile* map) {

	if(MAP_ISV(map, ent->pos.x, ent->pos.y) == SDL_FALSE) {
		return SDL_FALSE;
		}

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



	
void cast_ray(Entitiy *entity, Tile* map, f64 x, f64 y) {

	f64 ox,oy;
	ox = (f64)entity->pos.x;
	oy = (f64)entity->pos.y;

	for(i32 i = 0; i < RADIUS; i++) {
		CLAMP(ox, 0.00f, (f64)(MAP_X - 1));
		CLAMP(ox, 0.00f, (f64)(MAP_Y - 1));
		//LOG("%d %d\n", (u32)ox, (u32)oy);
		MAP_VISITED(map, (i32)ox, (i32)oy) = SDL_TRUE;
		MAP_ISV(map, (i32)ox, (i32)oy) = SDL_TRUE;//MAP_ISW(map, (i32)ox, (i32)oy) == SDL_FALSE ||
		if(MAP_ISW(map, (u32)ox, (u32)oy) == SDL_FALSE) {
			return;
			}
		ox+=x;
		oy+=y;
		};
	}
void field_of_vison(Entitiy *entity, Tile* map) {
	f64 x,y;
	CLEAR_VISON_FIELD(map);
	i32 xE = entity->pos.x;
	i32 yE = entity->pos.y;
	CLAMP(xE, 1, (MAP_X-2));
	CLAMP(yE, 1, (MAP_X-2));
	MAP_ISW(map, xE, yE) = SDL_TRUE;
	for(u64 i = 1; i < 359; i++) {
		x = cos((f64)i*0.01745329251f); //to rad
		y = sin((f64)i*0.01745329251f);
		cast_ray(entity, map, x, y);
		};
	MAP_ISW(map, xE, yE)   = SDL_FALSE;

	//MAP_ISW(map, xE+1, yE) = SDL_FALSE;
	//MAP_ISW(map, xE-1, yE) = SDL_FALSE;
	//MAP_ISW(map, xE, yE+1) = SDL_FALSE;
	//MAP_ISW(map, xE, yE-1) = SDL_FALSE;
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
	//-1y -1x KEY_Q
	distance = distnace_move(x1, y1, (x2 - 1), (y2  - 1), map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 4;
		}
	//-1y +1x KEY_E_
	distance = distnace_move(x1, y1, (x2 + 1), (y2  - 1), map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 5;
		}
	//1y -1x KEY_Y
	distance = distnace_move(x1, y1, (x2 - 1), (y2  + 1), map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 6;
		}
		//1y 1x KEY_C
	distance = distnace_move(x1, y1, (x2 + 1), (y2  + 1), map);
	if(distance > distancesMax  && distance != INF) {
		distancesMax = distance;
		index = 7;
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
			case 4:{
					if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
						//LOG("Y--\n");
						ent->pos.x--;
						ent->pos.y--;
						}
					break;
					}
				
			case 5:{
					if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
						//LOG("Y--\n");
						ent->pos.x++;
						ent->pos.y--;
						}
					break;
						}
			case 6:{
					if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
						//LOG("Y--\n");
						ent->pos.x--;
						ent->pos.y++;
						}
					break;
				}
			case 7:{
					if(distancesMax >= DISTANCE_RANGE_ATTACK_MIN  && distancesMax != INF) {
						//LOG("Y--\n");
						ent->pos.x++;
						ent->pos.y++;
						}
					break;
				}		
				
		default: {
				ASSERT("Unreachable");
				break;
				}
		}

	}
	void make_best_move(Entitiy* player, Entitiy*  ent, Tile *map) {
		SDL_bool isMonsterVisible = is_monster_visible(map, ent);
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
		//-1y -1x KEY_Q
		distance = distnace_move(x1, y1, (x2 - 1), (y2  - 1), map);
		if(distance < distancesMin) {
			distancesMin = distance;
			index = 4;
			}
		//-1y +1x KEY_E_
		distance = distnace_move(x1, y1, (x2 + 1), (y2  - 1), map);
		if(distance < distancesMin) {
			distancesMin = distance;
			index = 5;
			}
		//1y -1x KEY_Y
		distance = distnace_move(x1, y1, (x2 - 1), (y2  + 1), map);
		if(distance < distancesMin) {
			distancesMin = distance;
			index = 6;
			}
			//1y 1x KEY_C
		distance = distnace_move(x1, y1, (x2 + 1), (y2  + 1), map);
		if(distance < distancesMin) {
			distancesMin = distance;
			index = 7;
			}	
		u8 isRangeAttack = SDL_FALSE;
		switch(ent->ch) {
			case 'A': {
					if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
						isRangeAttack = SDL_TRUE;
						break;
						}
					}
			case 'C': {
					if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
						isRangeAttack = SDL_TRUE;
						break;
						}
					}
			case 'D': {
					if(distancesMin >= DISTANCE_RANGE_ATTACK_MIN && distancesMin <= DISTANCE_RANGE_ATTACK_MAX ) {
						isRangeAttack = SDL_TRUE;
						break;
						}
				default: {
						break;
						}
					}
			}
		if (isRangeAttack == SDL_TRUE){
			return;
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
			case 4:{
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y--\n");
					ent->pos.x--;
					ent->pos.y--;
					}
				break;
				}
			
			case 5:{
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y--\n");
					ent->pos.x++;
					ent->pos.y--;
					}
				break;
					}
			case 6:{
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y--\n");
					ent->pos.x--;
					ent->pos.y++;
					}
				break;
			}
			case 7:{
				if(distancesMin < INF && distancesMin != 0.0f) {
					//LOG("Y--\n");
					ent->pos.x++;
					ent->pos.y++;
					}
				break;
			}		
			default: {
					ASSERT("Unreachable");
					break;
					}
			}
			CLAMP(ent->pos.x, 1 , MAP_X - 1);
			CLAMP(ent->pos.y, 1 , MAP_Y - 1);
		}	
//WE WILL SEE IF A* or Diakstra or This CRAP
void make_move_diakstra(Entitiy* player, Entitiy*  ent, Tile *map) {
	SDL_bool isMonsterVisible = is_monster_visible(map, ent);
	i32 x1 = player->pos.x;
	i32 y1 = player->pos.y;
	i32 x2 = ent->pos.x;
	i32 y2 = ent->pos.y;
	f64 distance  = DISTANCE(x1, y1, x2, y2);
	if(distance <= DISTANCE_RANGE_ATTACK_MIN) {
		monster_attack(player, ent, distance);
		//LOG("Range\n");
		da_append(&MESSAGES, "Mele");
		return;
		}
	//MOVES WILL DEPEND OF WHAT MONSTER IS!!!

	//+1x
	f64 distancesMin = INF;
	f64 trueDistance = INF;
	i32 index = 0;
	//-1x
	distance = MAP_DIJKSTRA(map, (x2 - 1), y2) ;
	
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 - 1), y2, map);
		index = 1;
		}

	//+1y
	distance = MAP_DIJKSTRA(map, (x2 + 1), y2) ;

	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 + 1), y2, map);
		index = 2;
		}
	//-1y
	distance = MAP_DIJKSTRA(map, (x2), (y2 - 1));
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2), (y2-1), map);
		index = 3;
		}
	//-1y -1x KEY_Q
	distance = MAP_DIJKSTRA(map, (x2 - 1), (y2 - 1));
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 - 1), (y2 - 1), map);
		index = 4;
		}
	//-1y +1x KEY_E_
	distance = MAP_DIJKSTRA(map, (x2 + 1), (y2 - 1));
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 + 1), (y2 - 1), map);
		index = 5;
		}
	//1y -1x KEY_Y
	distance = MAP_DIJKSTRA(map, (x2 - 1), (y2 + 1));
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 - 1), (y2 + 1), map);
		index = 6;
		}
		//1y 1x KEY_C
	distance = MAP_DIJKSTRA(map, (x2 - 1), (y2 - 1));
	if(distance < distancesMin) {
		distancesMin = distance;
		trueDistance = distnace_move(x1, y1, (x2 - 1), (y2 - 1), map);
		index = 7;
		}
		
	//RANGE ATTACK
	u8 isRangeAttack = SDL_FALSE;
	switch(ent->ch) {
		case 'A': {
				if(trueDistance >= DISTANCE_RANGE_ATTACK_MIN && trueDistance <= DISTANCE_RANGE_ATTACK_MAX ) {
					isRangeAttack = SDL_TRUE;
					monster_attack(player, ent, trueDistance);
					break;
					}
				}

		}
	if (isRangeAttack == SDL_TRUE){
		return;
	}
	u8 isMoved = 0;
	switch(index) {
		case 0: {
				if(MAP_ISW(map, ent->pos.x + 1, ent->pos.y)) {
					//LOG("X++\n");
					ent->pos.x = ent->pos.x + 1;
					isMoved = 1;
					}

				break;
				}
				

		case 1: {
				if(MAP_ISW(map, ent->pos.x - 1, ent->pos.y)) {
					//LOG("X--\n");
					ent->pos.x--;
					isMoved = 1;
					}
				break;
				}
		case 2: {
				if(MAP_ISW(map, ent->pos.x, ent->pos.y + 1)) {
					//LOG("Y++\n");
					ent->pos.y++;
					isMoved = 1;
					}
				break;
				}
		case 3: {
				if(MAP_ISW(map, ent->pos.x, ent->pos.y - 1)) {
					//LOG("Y--\n");
					ent->pos.y--;
					isMoved = 1;
					}
				break;
				}
		case 4:{
			if(MAP_ISW(map, ent->pos.x - 1, ent->pos.y - 1)) {
				//LOG("Y--\n");
				ent->pos.x--;
				ent->pos.y--;
				isMoved = 1;
				}
			break;
			}
		
		case 5:{
			if(MAP_ISW(map, ent->pos.x + 1, ent->pos.y - 1)) {
				//LOG("Y--\n");
				ent->pos.x++;
				ent->pos.y--;
				isMoved = 1;
				}
			break;
				}
		case 6:{
			if(MAP_ISW(map, ent->pos.x - 1, ent->pos.y + 1)) {
				//LOG("Y--\n");
				ent->pos.x--;
				ent->pos.y++;
				isMoved = 1;
				}
			break;
		}
		case 7:{
			if(MAP_ISW(map, ent->pos.x + 1, ent->pos.y + 1)) {
				//LOG("Y--\n");
				ent->pos.x++;
				ent->pos.y++;
				isMoved = 1;
				}
			break;
		}		
		default: {
				ASSERT("Unreachable");
				break;
				}
		}
		if(isMoved == 0){
			make_best_move(player,ent, map);
		}
		CLAMP(ent->pos.x, 1 , MAP_X - 1);
		CLAMP(ent->pos.y, 1 , MAP_Y - 1);

	}

SDL_bool is_monster_visible(Tile* map, Entitiy* ent){
	if(MAP_ISV(map, ent->pos.x, ent->pos.y) == SDL_FALSE) {
		return SDL_FALSE;
		}
	return SDL_TRUE;
}	

//#define LOG_AL
void state_entity(Entitiy* player, Entitiy_DA *entitys, Tile *map){
	DROP(player);
	for(u64 i = 0; i < entitys->count; i++){
		Entitiy entity = entitys->items[i];
		
		//IF HEALTH IS A LOW RUNING MONSTER
		//ELSE GO BESERK
		if(Is_Monster(entity.ch)){
			entity.turnsToSummon++;
			if(entity.health == 0){
			if(rand_f64() < entity.stateChance[STATE_RESURECT]){
				i32 health = rand()%2 + 1;
				entity.attack[DAMAGE_BASIC]++;
				entity.color = RED;
				char* msg = calloc(50, sizeof(char));
				snprintf(msg, 50, "%s is resurected", entity.name);
				da_append(&MESSAGES, msg);
				
			}
			else{
				char* msg = calloc(50, sizeof(char));
				snprintf(msg, 50, "You slay the %s", entity.name);
				da_append(&MESSAGES, msg);
				char* msg1 = "His spirit is wandering in this world";
				da_append(&MESSAGES, msg1);
				entity.ch = 'S';
				entity.color = GREEN;
			}
		}	
		else if(entity.health == 1){
			if(rand_f64() < entity.stateChance[STATE_RUNING]){
				entity.state = STATE_RUNING;
#ifdef LOG_AL
				LOG("Entity(%s):  ", entity.name);
				LOG("Channged to a running state\n");
#endif				
			}
			else if(rand_f64() < CHANCE_INCREMENT_HEALTH){
				entity.health++;
				entity.state = STATE_HUNTING;
			}
			else{
#ifdef LOG_AL
				LOG("Entity(%s):  ", entity.name);
				LOG("Channged to a beserk state\n");
#endif
				entity.state = STATE_BESERK;
			}			
		}
	else if(rand_f64() < entity.stateChance[STATE_SUMMON]  && entity.cooldown < entity.turnsToSummon){
			entity.state = STATE_SUMMON;
			entity.turnsToSummon = 0; //RESET
#ifdef LOG_AL
				LOG("Entity(%s):  ", entity.name);
				LOG("Channged to a summon state\n");
				da_append(&MESSAGES, "Summon Ghoul");
#endif
		}

		else if(is_monster_visible(map, &entity) == SDL_TRUE){
			if(rand_f64() < entity.stateChance[STATE_HUNTING]){
				entity.state = STATE_HUNTING;
#ifdef LOG_AL
			    LOG("Entity(%s):  ", entity.name);
			    LOG("Channged to a hunting state\n");
#endif
			}
			else if(rand_f64() < entity.stateChance[STATE_MOVING_AWAY_RANGE]){
				entity.state = STATE_MOVING_AWAY_RANGE;
#ifdef LOG_AL
			    LOG("Entity(%s):  ", entity.name);
			    LOG("Channged to a moving away state\n");
#endif
			}
			else if(rand_f64() < entity.stateChance[STATE_RUNING]){
				entity.state = STATE_RUNING;
			}
		}
		else if(entity.state == STATE_WANDERING){
			if(is_monster_visible(map, &entity) == SDL_TRUE){
				if(rand_f64() < entity.stateChance[STATE_HUNTING]){
					entity.state  = STATE_HUNTING;
#ifdef LOG_AL
					LOG("Entity(%s):  ", entity.name);
					LOG("Channged to a hunting state from wandering\n");
#endif
				}
			}
		else if(rand_f64() < entity.stateChance[STATE_RESTING]){
				entity.state  = STATE_RESTING;
#ifdef LOG_AL
				LOG("Entity(%s):  ", entity.name);
				LOG("Channged to a resting state\n");
#endif
			}
		}
		else if(entity.state == STATE_HUNTING){
			if(is_monster_visible(map, &entity) == SDL_FALSE){
				if(rand_f64() > entity.stateChance[STATE_WANDERING]){
					entity.state  = STATE_WANDERING;
#ifdef LOG_AL
					LOG("Entity(%s):  ", entity.name);
					LOG("Channged to a wandering state\n");
#endif
				}
			}
		}
		
		else if(entity.state == STATE_RESTING){
			if(rand_f64() < entity.stateChance[STATE_WANDERING]){
				entity.state  = STATE_WANDERING;
#ifdef LOG_AL
				LOG("Entity(%s):  ", entity.name); 
				LOG("Channged to a wandering state\n");
#endif
			}
		}
	}
	else if(entity.ch == 'S'){
		entity.state = STATE_WANDERING;
	}	
	entitys->items[i] = entity;
	}
}


//IF IN VISON FIELD MOVE TOWARDS PLAYER
//IF NOT RAND MOV DEPENDING ON TYPE OR PROB
void move_entity(Entitiy* player, Entitiy_DA *entitys, Tile *map) {
	i32 co = 0;
	DROP(co);
	for(u64 count = 0; count < entitys->count; count++) {
		Entitiy entity = entitys->items[count];
		if(Is_Monster(entity.ch)) 
		{
			if(entity.state == STATE_RESTING){
				//NOTHING 
				//LOG("Resting");
			}
			else if(entity.state == STATE_WANDERING){
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
				i32 chance = rand()%2;
				if(chance)  //{make_move_diakstra(player, &entity, map);}
					{make_run_move(player, &entity, map);}
				else {make_move_diakstra(player, &entity, map);}
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
			}
			else if(entity.state == STATE_HUNTING){
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
				//make_run_move(player, &entity, map);
				make_move_diakstra(player, &entity, map);
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
			}
			else if(entity.state == STATE_BESERK){
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
				make_move_diakstra(player, &entity, map);
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
			}
			else if(entity.state == STATE_RUNING){
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
				make_run_move(player, &entity, map);
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
			}
			else if(entity.state == STATE_MOVING_AWAY_RANGE){
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
				make_run_move(player, &entity, map);
				MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
				entity.state = STATE_HUNTING;
			}
			else if(entity.state == STATE_SUMMON){
				Entitiy* summon = create_entity(monsterChar[GHOUL_MONSTER], monsterName[GHOUL_MONSTER], 30, 3, (Position) {
						.x = entity.pos.x, .y = entity.pos.y
						});
				summon->health = 2;
				make_run_move(player, &entity, map);
				entity.state = STATE_RUNING;		
				entity.cooldown *=3; 	
				da_append(entitys, *summon);			
			}


					
		}
		/*else{
			MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_TRUE;
			f64 chance = CHANCE_SPIRIT_ATTACK * (f64)COUNTMOVES;
			CLAMP(chance, 0.0f, 0.10f);
			if(rand_f64() < chance){
				make_best_move(player, &entity, map);	
			}
			else{
				make_run_move(player, &entity, map);
			}
			
			MAP_ISW(map, entity.pos.x, entity.pos.y) = SDL_FALSE;
		}  */
		
		entitys->items[count] = entity;
	//LOG("Colided entitys %d\n", co);
	}
}
void increment_player_health(Entitiy* player){
	if(player->maxHealth != player->health){
		//LOG("Difrent\n");
		f64 chance = rand_f64();
		if(chance < CHANCE_INCREMENT_HEALTH){
			player->health++;
			da_append(&MESSAGES, "Good luck strike you health +1\n");
		}
	}
}
void player_destroy_boolder(Entitiy* player, Tile* map){
	//SDL_bool isDoor = SDL_FALSE;
	i32 startX = player->pos.x - 1;
	i32 stopX  = player->pos.x + 1;
	i32 startY = player->pos.y - 1;
	i32 stopY  = player->pos.y + 1;
	CLAMP(startX, 0 , MAP_X-1);
	CLAMP(stopX, 0  , MAP_X-1);
	CLAMP(startY, 0 , MAP_Y-1);
	CLAMP(stopY, 0  , MAP_Y-1);
	for (i32 y = startY; y <= stopY; y++){
		for (i32 x = startX; x <= stopX; x++){
			if(MAP_CH(map, x, y) == '+'){
				if(rand_f64() < CHANCE_NON_CLEAR_RUINS){
					char* msg = "This ruins are tuff";
					da_append(&MESSAGES, msg);

				}
				else{
					char* msg = "You clear the ruins";
					da_append(&MESSAGES, msg);
					MAP_CH(map, x, y) = '.';
					MAP_ISW(map, x, y) = SDL_TRUE;
					if(rand_f64() < CHANCE_DMG_CLEAR_RUINS){
						i32 ran = rand()%4;
						player->health-=ran;
						char* msg = "This ruin is tuff";
						da_append(&MESSAGES, msg);
						char* msg1 = "Your health decresed";
						da_append(&MESSAGES, msg1);
				}
				}
				
			}
			
		}	
	}
	OPENDOOR = SDL_FALSE;
	
}

void update_entity(Entitiy* player, Entitiy_DA *entitys, Tile *map, Item_DA *items) {
	
	
	state_entity(player, entitys, map);
	move_entity(player, entitys, map);
	block_movement(entitys, map);
	field_of_vison(player, map);
	increment_player_health(player);
	
	COUNTMOVES++;
	if(OPENDOOR == SDL_TRUE){
		//player_open_door(player, map);
		player_destroy_boolder(player, map);
	}
	

	if(PICKITEM == SDL_TRUE) {
		picking_item_from_list(player, items);
		PICKITEM = SDL_FALSE;
		}
	calculate_diakstra_map(player, map, entitys);	
	}


Entitiy* create_inventory(i32 size) {
	if(size < 0) {
		ASSERT("Size for inventory is zero");
		}
	Entitiy* inventory = calloc(size, sizeof(Entitiy*));
	if(inventory == NULL) {
		ASSERT("Calloc is null");
		}
	return inventory;
	}

SDL_bool check_if_item_and_player_colide(Entitiy* player, Item* item) {
	if(player->pos.x == item->pos.x && player->pos.y == item->pos.y) {
		return SDL_TRUE;
		}
	return SDL_FALSE;
	}

void picking_item_from_list(Entitiy* entity, Item_DA *items) {
	for(u64 i = 0; i < items->count; i++) {
		Item item = items->items[i];
		if(check_if_item_and_player_colide(entity, &item) == SDL_TRUE) {
			pick_item_from_ground(&items->items[i], &entity->inventory);
			char* msg = calloc(MAX_NAME, sizeof(char*));
			snprintf(msg, MAX_NAME, "You picked %s", item.name);
			da_append(&MESSAGES, msg);
			}
		}
	}

	void calculate_diakstra_map(Entitiy* player, Tile* map, Entitiy_DA* entitys) {
		DROP(entitys);
	
		for (i32 i = 0; i < MAP_X * MAP_Y; i++) {
			map[i].distance = INF;
		}
	
		MAP_DIJKSTRA(map, player->pos.x, player->pos.y) = 0.0f;
		Position* queue = calloc(MAP_X * MAP_Y, sizeof(Position));
		int front = 0, back = 0;
	
		queue[back++] = (Position){player->pos.x, player->pos.y};
	
		// 8 directions: N, NE, E, SE, S, SW, W, NW
		const int dx[8] = {  0,  1, 1,  1,  0, -1, -1, -1 };
		const int dy[8] = { -1, -1, 0,  1,  1,  1,  0, -1 };
	
		while (front < back) {
			Position p = queue[front++];
			float current_dist = MAP_DIJKSTRA(map, p.x, p.y);
	
			for (int dir = 0; dir < 8; dir++) {
				int nx = p.x + dx[dir];
				int ny = p.y + dy[dir];
	
				if (nx < 0 || ny < 0 || nx >= MAP_X || ny >= MAP_Y) continue;
				if (MAP_ISW(map, nx, ny) == SDL_FALSE) continue;
	
				if (MAP_DIJKSTRA(map, nx, ny) > current_dist + 1.0f) {
					MAP_DIJKSTRA(map, nx, ny) = current_dist + 1.0f;
					queue[back++] = (Position){nx, ny};
				}
			}
		}
	
		free(queue);
	}
	
	
