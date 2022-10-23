// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043


#ifndef INC_281P2A_ZOMBIEPQ_H
#include <string>
#include <vector>
#include "getopt.h"
#include <deque>
#include <queue>
#include <cinttypes>
#include "median.h"
using std::vector;
using std::priority_queue;
using std::string;
using std::deque;
using std::cin;
struct zombie_t{
    string name = "\0";
    uint32_t dist = 0;
    uint32_t speed = 0;
    uint32_t health = 0;
    uint32_t roundsActive = 1;
};

void printZombieState(zombie_t* zb, const string& mode){
    printf("%s: %s (distance: %" PRIu32 ", speed: %" PRIu32 ", health: %" PRIu32 ")\n",mode.c_str(),
            zb->name.c_str(),zb->dist,zb->speed,zb->health);
}

struct archer{
    uint32_t curCapacity;
    uint32_t MAX_CAP;
};


/// shooting sequence
struct shootCmp{
    // zl is supposed of lower priority, zh is suppose of higher priority
    bool operator()(zombie_t* zl, zombie_t* zh){
        // ETA
        uint32_t ETA_zl = zl->dist/zl->speed;
        uint32_t ETA_zh = zh->dist/zh->speed;
        if (ETA_zl > ETA_zh){
            return true;
        }
        else if (ETA_zl < ETA_zh){
            return false;
        }
        else{ // tie in ETA
            if (zl->health > zh->health){
                return true;
            }
            else if (zl->health < zh->health){
                return false;
            }
            else{ // tie in health
                if (zl->name > zh->name){
                    return true;
                }
                else if (zl->name < zh->name){
                    return false;
                }
                else{
                    printf("error in priority queue comparison!\n");
                    exit(1);
                }
            }
        }
    }
};

struct maxActiveCmp{
    bool operator()(zombie_t* zbL,zombie_t* zbH){
        if(zbL->roundsActive < zbH->roundsActive){
            return true;
        }
        else if (zbL->roundsActive > zbH->roundsActive){
            return false;
        }
        else{
            return zbL->name > zbH->name;
        }
    }
};

struct minActiveCmp{
    bool operator()(zombie_t* zbL,zombie_t* zbH) {
        if (zbL->roundsActive > zbH->roundsActive) {
            return true;
        } else if (zbL->roundsActive < zbH->roundsActive) {
            return false;
        } else {
            return zbL->name > zbH->name;
        }
    }
};

// utilities
/// TOP is the FIRST spawned zombie !!!
void createNewNamedVerbose(vector<zombie_t*>* spawnOrder, priority_queue<zombie_t*,vector<zombie_t*>, shootCmp>* livingZombiesToShoot){
    string temp;
    zombie_t *nmZombie = new zombie_t;
    cin >> temp;
    nmZombie->name = temp;
    cin >> temp >> temp;
    nmZombie->dist = static_cast<uint32_t>(stoi(temp)); // distance
    cin >> temp >> temp;
    nmZombie->speed = static_cast<uint32_t>(stoi(temp)); // speed
    cin >> temp >> temp;
    nmZombie->health = static_cast<uint32_t>(stoi(temp)); // health
    printZombieState(nmZombie,"Created"); // print Created info
    spawnOrder->push_back(nmZombie);
    livingZombiesToShoot->push(nmZombie); // push to PQ
}

void createNewNamedNotV(vector<zombie_t*>* spawnOrder, priority_queue<zombie_t*,vector<zombie_t*>, shootCmp>* livingZombiesToShoot){
    string temp;
    zombie_t *nmZombie = new zombie_t;
    cin >> temp;
    nmZombie->name = temp;
    cin >> temp >> temp;
    nmZombie->dist = static_cast<uint32_t>(stoi(temp)); // distance
    cin >> temp >> temp;
    nmZombie->speed = static_cast<uint32_t>(stoi(temp)); // speed
    cin >> temp >> temp;
    nmZombie->health = static_cast<uint32_t>(stoi(temp)); // health
    spawnOrder->push_back(nmZombie);
    livingZombiesToShoot->push(nmZombie); // push to PQ
}
// read header (ONCE)
void readHeader(uint32_t* seed, uint32_t* max_rand_distance, uint32_t* max_rand_speed, uint32_t* max_rand_health, archer* player){
    string temp;
    getline(cin,temp); // skip comment
    cin >> temp >> temp; // quiver..
    player->MAX_CAP =player->curCapacity = stoi(temp); // update MAX_CAP and curCap at the same time
    cin >> temp >> temp; // random-seed..
    *seed = stoi(temp);
    cin >> temp >> temp; // dist
    *max_rand_distance = stoi(temp);
    cin >> temp >> temp; // speed
    *max_rand_speed = stoi(temp);
    cin >> temp >> temp; // health
    *max_rand_health = stoi(temp);
}

// TODO: VERBOSE
// zombies move (LOOP)
// update roundsActive here !!!    TODO: @debug here
zombie_t* zombiesMoveVerbose(vector<zombie_t *> *zombiesSpawnOrder, bool* playerLiving){
    zombie_t* zombieAteUrBrain = nullptr;
    bool firstAteFound = false;
    for (auto & iter : *zombiesSpawnOrder){
        // TODO: update health
        iter->dist = iter->dist > iter->speed ? iter->dist - iter->speed : 0;
        // TODO:update zombies roundsActive
        iter->roundsActive ++;
        if (iter->dist == 0 && !firstAteFound){
            firstAteFound = true; // find first zombie updated
            *playerLiving = false;
            zombieAteUrBrain = iter;
        }
        printZombieState(iter,"Moved");
    }
    return zombieAteUrBrain;
}

// player shoots zombies (LOOP)
zombie_t * shootsZombiesVerbose(priority_queue<zombie_t *, vector<zombie_t *>, shootCmp> *livingZombiesToShoot, archer &player,
                                vector<zombie_t *> *deadZombies, MedianContainer &median) {
    while(!livingZombiesToShoot->empty()){
        uint32_t cur = player.curCapacity;
        auto zbTop = livingZombiesToShoot->top();
        player.curCapacity = cur >= zbTop->health ? cur - zbTop->health : 0;
        // the quiver becomes empty
        if (player.curCapacity  == 0){
            zbTop->health -= cur;
            if (zbTop->health == 0){
                printZombieState(zbTop,"Destroyed");
                // TODO: update to dead zombies
                deadZombies->push_back(zbTop);
                // TODO: update median life PQ
                median.push(zbTop->roundsActive);
                livingZombiesToShoot->pop();
                // TODO: is the zombie the last zombie?
                if (livingZombiesToShoot->empty()){
                    return zbTop;
                }
                break;
            }
            break;
        }
            // zombie died
        else{
            zbTop->health = 0;
            printZombieState(zbTop,"Destroyed");
            livingZombiesToShoot->pop();
            deadZombies->push_back(zbTop);
            median.push(zbTop->roundsActive);
            // TODO: is the zombie the last zombie?
            if (livingZombiesToShoot->empty()){
                return zbTop;
            }
        }
    }
    return nullptr;
}

// TODO: NOT VERBOSE
zombie_t* zombiesMoveNotV(vector<zombie_t *> *zombiesSpawnOrder, bool* playerLiving){
    zombie_t* zombieAteUrBrain = nullptr;
    bool firstAteFound = false;
    for (auto & iter : *zombiesSpawnOrder){
        // TODO: update health
        iter->dist = iter->dist > iter->speed ? iter->dist - iter->speed : 0;
        // TODO:update zombies roundsActive
        iter->roundsActive ++;
        if (iter->dist == 0 && !firstAteFound){
            firstAteFound = true; // find first zombie updated
            *playerLiving = false;
            zombieAteUrBrain = iter;
        }
    }
    return zombieAteUrBrain;
}

zombie_t * shootsZombiesNotV(priority_queue<zombie_t *, vector<zombie_t *>, shootCmp> *livingZombiesToShoot, archer &player,
                             vector<zombie_t *> *deadZombies, MedianContainer &median) {
    while(!livingZombiesToShoot->empty()){
        uint32_t cur = player.curCapacity;
        auto zbTop = livingZombiesToShoot->top();
        player.curCapacity = cur >= zbTop->health ? cur - zbTop->health : 0;
        if (player.curCapacity  == 0){
            zbTop->health -= cur;
            if (zbTop->health == 0){
                // TODO: update to dead zombies
                deadZombies->push_back(zbTop);
                // TODO: update median life PQ
                median.push(zbTop->roundsActive);
                livingZombiesToShoot->pop();
                // TODO: is the zombie the last zombie?
                if (livingZombiesToShoot->empty()){
                    return zbTop;
                }
                break;
            }
            break;
        }
        else{ // zombie died
            zbTop->health = 0;
            livingZombiesToShoot->pop();
            deadZombies->push_back(zbTop);
            median.push(zbTop->roundsActive);
            // TODO: is the zombie the last zombie?
            if (livingZombiesToShoot->empty()){
                return zbTop;
            }
        }
    }
    return nullptr;
}

// statistics mode
// TODO: free dynamic memory
void statisticsGen(const int &N, vector<zombie_t*>& zombiesSpawn, vector<zombie_t *> &deadZombies){
    printf("Zombies still active: %d\n",(int)zombiesSpawn.size());
    // TODO: names of first N zombies killed
    // front of deque are those killed earlier
    int deadZombieNum = static_cast<int>(deadZombies.size());
    printf("First zombies killed:\n");
    int upperB = deadZombieNum >= N ? N : deadZombieNum;
    for (int i = 0; i < upperB; ++i) {
        printf("%s %d\n",deadZombies[i]->name.c_str(),i+1);
    }
    // TODO: names of last N zombies killed
    printf("Last zombies killed:\n");
    for (int i = deadZombieNum - 1, j = deadZombieNum >= N ? N : deadZombieNum ; j > 0 ; --i, --j) {
        printf("%s %d\n",deadZombies[i]->name.c_str(),j);
    }
    // TODO: names of most active zombies
    // TODO: merge two vectors
    zombiesSpawn.reserve(zombiesSpawn.size() + static_cast<uint32_t>(deadZombieNum));
    zombiesSpawn.insert(zombiesSpawn.end(),deadZombies.begin(),deadZombies.end());
    priority_queue<zombie_t*,vector<zombie_t*>,maxActiveCmp> maxHeap(zombiesSpawn.begin(),zombiesSpawn.end());
    priority_queue<zombie_t*,vector<zombie_t*>,minActiveCmp> minHeap(zombiesSpawn.begin(),zombiesSpawn.end());
    int heapSize = static_cast<int>(maxHeap.size());
    printf("Most active zombies:\n");
    if (heapSize >= N){
        int count = N;
        while(count > 0){
            auto zb = maxHeap.top();
            maxHeap.pop();
            printf("%s %d\n",zb->name.c_str(),zb->roundsActive);
            count --;
        }
    }
    else{
        while(!maxHeap.empty()){
            auto zb = maxHeap.top();
            maxHeap.pop();
            printf("%s %d\n",zb->name.c_str(),zb->roundsActive);
        }
    }
    // TODO: names of least active zombies
    printf("Least active zombies:\n");
    if (heapSize >= N){
        int count = N;
        while(count > 0){
            auto zb = minHeap.top();
            minHeap.pop();
            printf("%s %d\n",zb->name.c_str(),zb->roundsActive);
            count --;
        }
    }
    else{
        while(!minHeap.empty()){
            auto zb = minHeap.top();
            minHeap.pop();
            printf("%s %d\n",zb->name.c_str(),zb->roundsActive);
        }
    }
}

#define INC_281P2A_ZOMBIEPQ_H

#endif //INC_281P2A_ZOMBIEPQ_H
