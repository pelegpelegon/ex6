/*
Name: Peleg Sacher
ID: 333135440
Assignment: ex6
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "utils.h"

#define TRUE 1
#define FALSE 0

#define FIRST_ARG_BIGGER 1
#define SECOND_ARG_BIGGER -1
#define equal 0

#define QUITING_INDICATOR -1


/* printing functions */
static void displayMap(GameState* g);
static void displayLegend(GameState* g);
static void printRoom(GameState* g, Room* room);

/* freeing functions */
static void freeRoom(Room *room);
static void freePlayer(Player* player);

/* object creators*/
static Item* addItem();
static Monster* addMonster();


/* logic helpers */
static char getMonsterTypeName(int type);
static char getItemTypeName(int type);
static void linkNewRoom(GameState* g, Room* newRoom);
static Room* findRoom(Room* room, int ID);
static Room* findRoomByCords(Room* room, int x, int y);
static int getNewRoomID(GameState *g);
static int hasPlayerWon(GameState* g);
static void playRoom(GameState* g, Room* room);


/* player action functions */
static void moveAction(GameState* g, Room* room);
static void fightAction(GameState* g, Room* room);
static void pickupAction(GameState* g, Room* room);
static void defeatedAction(GameState* g, Room* room);
static void bagAction(GameState* g, Room* room);
static void quitAction(GameState* g, Room* room);



typedef enum {UP, DOWN, LEFT, RIGHT} Directions;
typedef enum {NONTHING, MOVE, FIGHT, PICKUP, BAG, DEFEATED, QUIT} Moves;
typedef enum {NOTHING, PREORDER, INORDER, POSTORDER} BTSPrintOrder;


static void displayMap(GameState* g) {
    if (!g->rooms) return;
    
    // Find bounds
    int minX = 0, maxX = 0, minY = 0, maxY = 0;
    for (Room* r = g->rooms; r; r = r->next) {
        if (r->x < minX) minX = r->x;
        if (r->x > maxX) maxX = r->x;
        if (r->y < minY) minY = r->y;
        if (r->y > maxY) maxY = r->y;
    }
    
    int width = maxX - minX + 1;
    int height = maxY - minY + 1;
    
    // Create grid
    int** grid = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        grid[i] = malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) grid[i][j] = -1;
    }
    
    for (Room* r = g->rooms; r; r = r->next)
        grid[r->y - minY][r->x - minX] = r->id;
    
    printf("=== SPATIAL MAP ===\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i][j] != -1) printf("[%2d]", grid[i][j]);
            else printf("    ");
        }
        printf("\n");
    }
    
    for (int i = 0; i < height; i++) free(grid[i]);
    free(grid);
}


static void displayLegend(GameState* g){
    if (!g->rooms) return;

    printf("=== ROOM LEGEND ===\n");
    LegendRecursion(g -> rooms);
}


static void LegendRecursion(Room* r){
    if (!r -> next) return;

    LegendRecursion(r -> next);

    printf("ID %d: ", r -> id);

    if(r -> monster)
        printf("[M:V] ");
    else
        printf("[M:X] ");


    if(r -> item)
        printf("[I:V]");
    else
        printf("[I:X]");
}


static Room* findRoom(Room* room, int ID){
    if (room -> id == ID)
        return room;

    return findRoom(room -> next, ID);
}


static Room* findRoomByCords(Room* room, int x, int y){
    if (room == NULL)
        return NULL;

    if ((room -> x == x) && (room -> y == y))
        return room;

    return findRoomByCords(room -> next, x, y);
}


static void freeRoom(Room *room){
    if(room == NULL)
        return;

    freeMonster(room -> monster);
    freeItem(room -> item);
    free(room);
}


static Item* addItem(){
    Item *newItem;
    newItem = malloc(sizeof(Item));
    
    if(newItem == NULL)
        exit(1);

    newItem -> name = getString("Item name: ");
    newItem -> type = getInt("Type (0=Armor, 1=Sword): ");
    newItem -> value = getInt("Value: ");
    return newItem;
}


static Monster* addMonster(){
    Monster *newMonster;
    newMonster = malloc(sizeof(Monster));

    /* reseting everything */
    newMonster -> name = getString("Monster name: ");
    newMonster -> type = getInt("Type (0-4): ");
    newMonster -> maxHp = getInt("HP: ");
    newMonster -> attack = getInt("Attack: ");
    newMonster -> hp = newMonster -> maxHp;
    return newMonster;
}


static int getNewRoomID(GameState *g){
    int newID = 0;
    for (Room* r = g->rooms; r; r = r->next)
        newID = (newID < r -> id)? (r-> id):(newID);

    return newID + 1;
}


static void linkNewRoom(GameState* g, Room* newRoom){
    Room* r = g->rooms;

    if (r == NULL)
        g -> rooms = newRoom;

    /* gets the last room in linked list */
    for (; r -> next; r = r->next);
    r -> next = newRoom;
}


static void playRoom(GameState* g, Room* room){
    Monster* monster;
    Item* item;
    Player* player = g -> player;
    int choice;

    if(room == NULL)
        return;

    room -> visited = 1;
    /* incase the player has already won */
    if(hasPlayerWon(g))
        return;

    monster = room -> monster;
    item = room -> item;

    /* all the printing stuff*/
    printRoom(g, room);

    choice = getInt("1.Move 2.Fight 3.Pickup 4.Bag 5.Defeated 6.Quit\n");
    switch (choice){
    case MOVE:
        moveAction(g, room);
        return;
        break;

    case FIGHT:
        fightAction(g, room);
        return;
        break;

    case PICKUP:
        pickupAction(g, room);
        return;
        break;

    case BAG:
        bagAction(g, room);
        return;
        break;

    case DEFEATED:
        defeatedAction(g, room);
        return;
        break;

    case QUIT:
        quitAction(g);
        return;
        break;
    
    default: break;
    }
}


static void printRoom(GameState* g, Room* room){
    Monster* monster;
    Item* item;
    Player* player = g -> player;

    monster = room -> monster;
    item = room -> item;

    displayMap(g);
    displayLegend(g);
    printf("===================\n");


    printf("--- Room %d ---\n", room -> id);
    if ((monster) && (monster -> hp))
        printf("Monster: %s (HP:%d)\n", monster -> name, monster ->hp);
    if(item)
        printf("Item: %s\n", item -> name);
    printf("HP: %d/%d\n", player -> hp, player -> maxHp);

}


static void moveAction(GameState* g, Room* room){
    int direction;
    int newX, newY;
    Room* moveToRoom;

    /* checks if there are alive monsters */
    if(room -> monster -> hp != 0){
        printf("Kill monster first\n");
        return;
    }

    newX = room -> x;
    newY = room -> y;

    direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");
    /* gets the new room cordinates */
    switch (direction){
        case UP:
            newX = room -> x;
            newY = room -> y - 1;
            break;

        case DOWN:
            newX = room -> x;
            newY = room -> y + 1;
            break;

        case LEFT:
            newX = room -> x - 1;
            newY = room -> y;
            break;

        case RIGHT:
            newX = room -> x + 1;
            newY = room -> y;
            break;
            
        default: break;
    }

    moveToRoom = findRoomByCords(g -> rooms ,newX, newY);
    if(!moveToRoom){
        printf("No room there\n");
        return;
    }

    /* moving the player to the room */
    g -> player -> currentRoom = moveToRoom;
    moveToRoom -> visited = 1;
}


static void fightAction(GameState* g, Room* room){
    Monster* monster;
    Player* player;
    int playerAttack, monsterAttack;

    if(room == NULL)
        return;

    player = g -> player;
    monster = room -> monster;

    /* checking if there is a monster */
    if(!monster){
        printf("No monster\n");
        return;
    }

    playerAttack = player -> baseAttack;
    monsterAttack = monster -> attack;

    /* the turns */
    for (int i = 0; !(player -> hp) || !(monster -> hp); i++){
        if (!(i % 2)){
            /* even turn number: player's turn */
            monster -> hp = (monster -> hp - playerAttack < 0)? 0:(monster -> hp - playerAttack);
            printf("You deal %d damage. Monster HP: %d\n", playerAttack, monster -> hp);
        }
        else{
            /* odd turn number: player's turn */
            player -> hp = (player -> hp - monsterAttack < 0)? 0:(player -> hp - monsterAttack);
            printf("Monster deals %d damage. Your HP: %d\n", monsterAttack, player -> hp);
        }
    }
    
    if(player -> hp){
        /* player has won */
        printf("Monster defeated!\n");
        bstInsert(player -> defeatedMonsters -> root, monster, player -> defeatedMonsters -> compare);
        room -> monster = NULL;
        return;
    }
}


static void pickupAction(GameState* g, Room* room){
    Player* player = g -> player;
    Item* item = room -> item;

    /* checks if monster is still alive */
    if(room -> monster -> hp){
        printf("Kill monster first\n");
        return;
    }

    /* checks if there is an item */
    if(!item){
        printf("No item here\n");
        return;
    }

    /* checks if item is duplicate */
    if(bstFind(player -> bag -> root, item, player -> bag -> compare)){
        printf("Duplicate item.\n");
        return;
    }

    bstInsert(player -> bag -> root, item, player -> bag -> compare);
    printf("Picked up %s\n", item -> name);
    room -> item = NULL;
}


static void bagAction(GameState* g, Room* room){
    BST* tree = g -> player -> bag;
    int choice;

    printf("=== INVENTORY ===\n");
    if (isEmpty(tree -> root)){
        printf("        None        \n");
        return;
    }
    choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");

    /* printing the bag accurding to the chiose*/
    switch (choice){
    case PREORDER:
        bstPreorder(tree -> root, tree -> print);
        break;

    case INORDER:
        bstInorder(tree -> root, tree -> print);
        break;

    case POSTORDER:
        bstPostorder(tree -> root, tree -> print);
        break;

    default: break;
    }
}


static void defeatedAction(GameState* g, Room* room){
    BST* tree = g -> player -> defeatedMonsters;
    int choice;
    printf("=== DEFEATED MONSTERS ===\n");
    if (isEmpty(tree -> root)){
        printf("        None        \n");
        return;
    }
    choice = getInt("1.Preorder 2.Inorder 3.Postorder\n");

    /* printing the defeated monsters accurding to the chiose*/
    switch (choice){
    case PREORDER:
        bstPreorder(tree -> root, tree -> print);
        break;

    case INORDER:
        bstInorder(tree -> root, tree -> print);
        break;

    case POSTORDER:
        bstPostorder(tree -> root, tree -> print);
        break;

    default: break;
    }
}


static void quitAction(GameState* g){
    g -> configMaxHp = QUITING_INDICATOR;
}


static int hasPlayerWon(GameState* g){
    if(!(g -> player -> hp))
        return FALSE;

    for (Room* r = g -> rooms; r ; r = r-> next){
        if((r -> monster) || !(r -> visited))
            return FALSE;
    }
    return TRUE;
}


static void freePlayer(Player* player){

    /* freeing the bag */
    if(player -> bag){
        bstFree(player -> bag -> root, player -> bag -> freeData);
        free(player -> bag);
    }
    /* freeing the defeated monster */
    if(player -> defeatedMonsters){
        bstFree(player -> defeatedMonsters -> root, player -> defeatedMonsters -> freeData);
        free(player -> defeatedMonsters);
    }

    free(player);
}


static char getMonsterTypeName(int type){
    switch (type){
    case PHANTOM:
        return "PHANTOM";
        break;

    case SPIDER:
        return "SPIDER";
        break;

    case DEMON:
        return "DEMON";
        break;

    case GOLEM:
        return "GOLEM";
        break;

    case COBRA:
        return "COBRA";
        break;
    
    default: break;
    }
}


static char getItemTypeName(int type){
    switch (type)
    {
    case SWORD:
        return "SWORD";
        break;

    case ARMOR:
        return "ARMOR";
        break;
    
    default: break;
    }
}


void addRoom(GameState* g){
    int adjRoomID, direction, newX, newY;
    Room *newRoom, *adjRoom;

    /* allocating space */
    newRoom = calloc(1, sizeof(Room));
    if(newRoom == NULL){
        printf("we have encountered an error when trying to allocate memory for the new room\n");
        exit(1);
    }

    /* print maps */
    displayMap(g);
    displayLegend(g);
    printf("===================\n");

    if(!(g -> rooms)){
        /* if its the first room */
        newRoom -> id = 0;
        newRoom -> x = 0;
        newRoom -> y = 0;
    }
    else{
        /*if its not the first room */
        adjRoomID = getInt("Attach to room ID: ");
        direction = getInt("Direction (0=Up,1=Down,2=Left,3=Right): ");

        adjRoom = findRoom(g -> rooms, adjRoomID);

        /* calculates the cordinates */
        switch (direction){
            case UP:
                newX = adjRoom -> x;
                newY = adjRoom -> y - 1;
                break;

            case DOWN:
                newX = adjRoom -> x;
                newY = adjRoom -> y + 1;
                break;

            case LEFT:
                newX = adjRoom -> x - 1;
                newY = adjRoom -> y;
                break;

            case RIGHT:
                newX = adjRoom -> x + 1;
                newY = adjRoom -> y;
                break;
            
            default:
                break;
        }

        if(findRoomByCords(g -> rooms, newX, newY)){
            printf("Room exists there\n");
            freeRoom(newRoom);
            return;
        }
        newRoom -> id = getNewRoomID(g);
        newRoom -> x = newX;
        newRoom -> y = newY;
    }

    /* handles monster adding and item adding*/
    if(getInt("Add monster? (1=Yes, 0=No): "))
        newRoom -> monster = addMonster();

    if(getInt("Add item? (1=Yes, 0=No):"))
        newRoom -> item = addItem();

    /* linking the room */
    linkNewRoom(g, newRoom);
    printf("Created room %d at (%d,%d)\n", newRoom -> id, newRoom -> x, newRoom -> y);
    g -> roomCount++;
}


void initPlayer(GameState* g){
    Player* player;
    if(g -> player != NULL){
        printf("Player exists\n");
        return;
    }


    if(g -> rooms == NULL){
        printf("Create rooms first\n");
        return;
    }

    /* creates and reset player */
    player = calloc(1, sizeof(Player));
    if(player == NULL){
        printf("we've encountered a memory allocation error when trying to create the player\n");
        exit(1);
    }

    player -> maxHp = g -> configMaxHp;
    player -> hp = g -> configMaxHp;
    player -> baseAttack = g -> configBaseAttack;
    player -> bag = createBST(compareItems, printItem, freeItem);
    player -> defeatedMonsters = createBST(compareMonsters, printMonster, freeMonster);
    player -> currentRoom = g -> rooms;
}


int compareMonsters(void* a, void* b){
    Monster *monsterA = (Monster*)a;
    Monster *monsterB = (Monster*)b;

    /* comparing the names */
    if(strcmp(monsterA -> name, monsterB -> name) > 0)
        return FIRST_ARG_BIGGER;

    if(strcmp(monsterA -> name, monsterB -> name) < 0)
        return SECOND_ARG_BIGGER;

    /* comparing the attack */
    if (monsterA -> attack > monsterB -> attack)
        return FIRST_ARG_BIGGER;

    if (monsterA -> attack < monsterB -> attack)
        return SECOND_ARG_BIGGER;

    /* comparing the max hp */
    if(monsterA -> maxHp > monsterB -> maxHp)
        return FIRST_ARG_BIGGER;

    if(monsterA -> maxHp < monsterB -> maxHp)
        return SECOND_ARG_BIGGER;

    /* comparing the type */
    if(monsterA -> type > monsterB -> type)
        return FIRST_ARG_BIGGER;

    if(monsterA -> type < monsterB -> type)
        return SECOND_ARG_BIGGER;
       
    /* 
        still gonna leave it here
        even though it said we can assume there will be no identical monsters
    */
    return equal;
}


int compareItems(void* a, void* b){
    Item *itemA = (Item*)a;
    Item *itemB = (Item*)b;

    if(strcmp(itemA -> name, itemB -> name) > 0)
        return FIRST_ARG_BIGGER;

    if(strcmp(itemA -> name, itemB -> name) < 0)
        return SECOND_ARG_BIGGER;

    if(itemA -> value > itemB -> value)
        return FIRST_ARG_BIGGER;

    if(itemA -> value < itemB -> value)
        return SECOND_ARG_BIGGER;

    if(itemA -> type > itemB -> type)
        return FIRST_ARG_BIGGER;

    if(itemA -> type < itemB -> type)
        return SECOND_ARG_BIGGER;

    return equal;
}


void freeItem(void *data){
    Item *item = (Item*)data;
    if(item == NULL)
        return;

    free(item -> name);
    free(item);
}


 void freeMonster(void *data){
    Monster* monster = (Monster*)data;
    if(monster == NULL)
        return;

    free(monster -> name);
    free(monster);
}


void playGame(GameState* g){
    Player* player = g -> player;
    if(player == NULL){
        printf("Init player first\n");
        return;
    }

    while (!(g -> configMaxHp == QUITING_INDICATOR) || ((!hasPlayerWon(g)) && (player -> hp))){
        playRoom(g, player -> currentRoom);
    }

    if(g -> configMaxHp == QUITING_INDICATOR){
        g -> configMaxHp = g -> player -> maxHp;
        return;
    }

    if(hasPlayerWon(g)){
        printf("***************************************\n");
        printf("             VICTORY!                  \nAll rooms explored. All monsters defeated.\n");
        printf("***************************************\n");
        freeGame(g);
        exit(0);
    }
    else{
        printf("--- YOU DIED ---\n");
        exit(0);
    }
}


void printMonster(void* data){
    Monster* monster = (Monster*)data;

    printf("[%s] Type: %s, Attack: %d, HP: %d\n",
            monster -> name,
            getMonsterTypeName(monster -> type),
            monster -> attack,
            monster -> maxHp);
}


void printItem(void* data){
    Item* item = (Item*)data;

    printf("        [type] %s - Value: %d",
            getItemTypeName(item -> type),
            item -> name,
            item -> value);
}


void freeGame(GameState* g){
    Player* player = g -> player;

    /* freeing the player */
    freePlayer(player);

    for (Room* r = g -> rooms; r ; r = r-> next){
        freeRoom(r);
    }
}
