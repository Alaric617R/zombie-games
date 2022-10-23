// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#include <iostream>
#include "zombiePQ.h"
#include "P2random.h"
#include "median.h"
#include <algorithm>
using std::vector;
using std::priority_queue;
using namespace std;

void readHeader(uint32_t* seed, uint32_t* max_rand_distance, uint32_t* max_rand_speed, uint32_t* max_rand_health, archer* player);
// zombies move (LOOP)
zombie_t* zombiesMoveVerbose(vector<zombie_t *> *zombiesSpawnOrder, bool* playerLiving);
// player shoots zombies (LOOP)
zombie_t * shootsZombiesVerbose(priority_queue<zombie_t *, vector<zombie_t *>, shootCmp> *livingZombiesToShoot, archer &player,
                                vector<zombie_t *> *deadZombies, MedianContainer &median);
zombie_t* zombiesMoveNotV(vector<zombie_t *> *zombiesSpawnOrder, bool* playerLiving);
zombie_t * shootsZombiesNotV(priority_queue<zombie_t *, vector<zombie_t *>, shootCmp> *livingZombiesToShoot, archer &player,
                             vector<zombie_t *> *deadZombies, MedianContainer &median);
int main(int argc, char* argv[]) {
    int gotopt;
    int optionIdx = 0;
    int stat = 0; // statistics
    bool isVerbose = false,
         isStatistics = false,
         isMedian = false;
    option long_opts[] = {
            {"verbose", no_argument,nullptr, 'v'},
            {"statistics", required_argument,nullptr, 's'},
            {"median", no_argument, nullptr, 'm'},
            {"help",   no_argument, nullptr, 'h'},
            {nullptr, 0,nullptr, '\0'}
    };
    while ((gotopt = getopt_long(argc, argv, "vs:mh", long_opts, &optionIdx)) != -1) {
        switch (gotopt) {
            case 'v': {
                isVerbose = true;
                break;
            }
            case 's': {
                stat = atoi(optarg);
                isStatistics = true;
                break;
            }
            case 'm': {
                isMedian = true;
                break;
            }
            case 'h': {
                printf("Watch the Zombie Game!\n[--verbose | -v]\n[--statistics | -s (N)]\n"
                       "[--median | -m]\n[--help | -h]\n");
                return 0;
            }
            default: {
                printf("Unknown argument !\n");
                exit(1);
            }
        }
    }
    /// READ HEADER
    archer player;
    uint32_t seed, max_rand_distance, max_rand_speed, max_rand_health;
    readHeader(&seed,&max_rand_distance,&max_rand_speed,&max_rand_health,&player);
    P2random::initialize(seed,max_rand_distance,max_rand_speed,max_rand_health);

    // important variables
    string temp;
    priority_queue<zombie_t*,vector<zombie_t*>, shootCmp> livingZombiesToShoot;
    vector<zombie_t*> zombieSpawnOrder;
    vector<zombie_t*> deadZombies;
    bool playerLiving = true;
    bool noZombie2Read = false;
    bool jumpRound = false;
    zombie_t* zombieAteUrBrain = nullptr;
    zombie_t* zombieLastKilled = nullptr;
    // median flag
    MedianContainer medianLife;
    int     curRound = 1,
            prevRound = 1,
            random_zombies = 0,
            named_zombies = 0;
    // Verbose
    if (isVerbose){ // go to verbose mode
        // TODO: get first round if it's not starting at 1
        cin >> temp >> temp >> temp;
        curRound = stoi(temp);
        if (curRound != 1){
            for (int i = 1; i < curRound; ++i) {
                printf("Round: %d\n",i);
            }
            prevRound = curRound;
        }
        jumpRound = true;
        while (true){
            if (curRound - prevRound == 0 && !jumpRound){
                if (! ( cin >> temp >> temp >> temp )){ // no zombie files
                    noZombie2Read = true;
                    if (livingZombiesToShoot.empty())
                        break;
                }
                else
                    curRound = stoi(temp); // update curRound, leave prev unchanged
            }
            if ( noZombie2Read || curRound - prevRound > 1){ // TODO: some rounds are skipped, careful...
                prevRound ++;
                jumpRound = true;
                printf("Round: %d\n",prevRound);
                // TODO: Refill quiver
                player.curCapacity = player.MAX_CAP;
                // TODO: Living zombies moving forward
                zombieAteUrBrain = zombiesMoveVerbose(&zombieSpawnOrder,&playerLiving);
                // TODO: Check mortality
                if (!playerLiving){
                    printf("DEFEAT IN ROUND %d! %s ate your brains!\n",prevRound,zombieAteUrBrain->name.c_str());
                    break;
                }
                // TODO: NO NEW ZOMBIES !!! do nothing ~

                // TODO: Player shoots zombies
                zombieLastKilled = shootsZombiesVerbose(&livingZombiesToShoot, player, &deadZombies, medianLife);

                // TODO:get rid of dead zombies in zombieSpawn
                auto pd = std::remove_if(zombieSpawnOrder.begin(),zombieSpawnOrder.end(),[](zombie_t* zb){return zb->health == 0;});
                zombieSpawnOrder = vector<zombie_t*>(zombieSpawnOrder.begin(),pd);
                // TODO: median flag
                if (isMedian && medianLife.size() != 0){
                    printf("At the end of round %d, the median zombie lifetime is %d\n",prevRound,medianLife.inspect());
                }
                // TODO: player wins ? ( In case zombie last killed is NULL !!
                if (noZombie2Read && livingZombiesToShoot.empty())
                    break;
            }
            else{ // TODO: Contiguous round, just read the following as normal ONLY FROM READING
                jumpRound = false;
                printf("Round: %d\n",curRound);
                // TODO: Refill quiver
                player.curCapacity = player.MAX_CAP;
                // TODO: Living zombies moving forward
                zombieAteUrBrain = zombiesMoveVerbose(&zombieSpawnOrder,&playerLiving);
                // TODO: Check mortality
                if (!playerLiving){
                    printf("DEFEAT IN ROUND %d! %s ate your brains!\n",curRound,zombieAteUrBrain->name.c_str());
                    break;
                }
                // TODO: New zombies appears OR NO zombies !!
                if (!noZombie2Read) {
                    cin >> temp >> temp;
                    random_zombies = stoi(temp);
                    cin >> temp >> temp;
                    named_zombies = stoi(temp);
                    /// random zombies
                    for (int i = 0; i < random_zombies; ++i) {
                        zombie_t *rdZombie = new zombie_t;
                        rdZombie->name = P2random::getNextZombieName();
                        rdZombie->dist = P2random::getNextZombieDistance();
                        rdZombie->speed = P2random::getNextZombieSpeed();
                        rdZombie->health = P2random::getNextZombieHealth();
                        printZombieState(rdZombie, "Created"); // print Created info
                        zombieSpawnOrder.push_back(rdZombie);
                        livingZombiesToShoot.push(rdZombie); // push to PQ
                    }
                    /// named zombies
                    for (int i = 0; i < named_zombies; ++i) {
                        createNewNamedVerbose(&zombieSpawnOrder, &livingZombiesToShoot);
                    }
                }
                // TODO: Player shoots zombies
                zombieLastKilled = shootsZombiesVerbose(&livingZombiesToShoot, player, &deadZombies, medianLife);


                // TODO:get rid of dead zombies in zombieSpawn
                auto pd = std::remove_if(zombieSpawnOrder.begin(),zombieSpawnOrder.end(),[](zombie_t* zb){return zb->health == 0;});
                zombieSpawnOrder = vector<zombie_t*>(zombieSpawnOrder.begin(),pd);

                // TODO: median
                if (isMedian && medianLife.size() != 0){
                    printf("At the end of round %d, the median zombie lifetime is %d\n",curRound,medianLife.inspect());
                }
                // TODO: current round is finished, update prevRound
                prevRound = curRound ;
                // TODO: player wins ? ( In case zombie last killed is NULL !!
                if (noZombie2Read && livingZombiesToShoot.empty())
                    break;
            }

        }
    }
    else{ // not verbose
        // TODO: get first round if it's not starting at 1
        cin >> temp >> temp >> temp;
        curRound = stoi(temp);
        if (curRound != 1){
            prevRound = curRound;
        }
        jumpRound = true;
        while (true){
            if (curRound - prevRound == 0 && !jumpRound){
                if (! ( cin >> temp >> temp >> temp )){ // no zombie files
                    noZombie2Read = true;
                    if (livingZombiesToShoot.empty())
                        break;
                }
                else
                    curRound = stoi(temp); // update curRound, leave prev unchanged
            }
            if ( noZombie2Read || curRound - prevRound > 1){ // TODO: some rounds are skipped, careful...
                prevRound ++;
                jumpRound = true;
                // TODO: Refill quiver
                player.curCapacity = player.MAX_CAP;
                // TODO: Living zombies moving forward
                zombieAteUrBrain = zombiesMoveNotV(&zombieSpawnOrder,&playerLiving);
                // TODO: Check mortality
                if (!playerLiving){
                    printf("DEFEAT IN ROUND %d! %s ate your brains!\n",prevRound,zombieAteUrBrain->name.c_str());
                    break;
                }
                // TODO: NO NEW ZOMBIES !!! do nothing ~

                // TODO: Player shoots zombies
                zombieLastKilled = shootsZombiesNotV(&livingZombiesToShoot, player, &deadZombies, medianLife);

                // TODO:get rid of dead zombies in zombieSpawn
                auto pd = std::remove_if(zombieSpawnOrder.begin(),zombieSpawnOrder.end(),[](zombie_t* zb){return zb->health == 0;});
                zombieSpawnOrder = vector<zombie_t*>(zombieSpawnOrder.begin(),pd);
                // TODO: median flag
                if (isMedian && medianLife.size() != 0){
                    printf("At the end of round %d, the median zombie lifetime is %d\n",prevRound,medianLife.inspect());
                }
                // TODO: player wins ? ( In case zombie last killed is NULL !!
                if (noZombie2Read && livingZombiesToShoot.empty())
                    break;
            }
            else{ // TODO: Contiguous round, just read the following as normal ONLY FROM READING
                jumpRound = false;
                // TODO: Refill quiver
                player.curCapacity = player.MAX_CAP;
                // TODO: Living zombies moving forward
                zombieAteUrBrain = zombiesMoveNotV(&zombieSpawnOrder,&playerLiving);
                // TODO: Check mortality
                if (!playerLiving){
                    printf("DEFEAT IN ROUND %d! %s ate your brains!\n",curRound,zombieAteUrBrain->name.c_str());
                    break;
                }
                // TODO: New zombies appears OR NO zombies !!
                if (!noZombie2Read) {
                    cin >> temp >> temp;
                    random_zombies = stoi(temp);
                    cin >> temp >> temp;
                    named_zombies = stoi(temp);
                    /// random zombies
                    for (int i = 0; i < random_zombies; ++i) {
                        zombie_t *rdZombie = new zombie_t;
                        rdZombie->name = P2random::getNextZombieName();
                        rdZombie->dist = P2random::getNextZombieDistance();
                        rdZombie->speed = P2random::getNextZombieSpeed();
                        rdZombie->health = P2random::getNextZombieHealth();
                        zombieSpawnOrder.push_back(rdZombie);
                        livingZombiesToShoot.push(rdZombie); // push to PQ
                    }
                    /// named zombies
                    for (int i = 0; i < named_zombies; ++i) {
                        createNewNamedNotV(&zombieSpawnOrder, &livingZombiesToShoot);
                    }
                }
                // TODO: Player shoots zombies
                zombieLastKilled = shootsZombiesNotV(&livingZombiesToShoot, player, &deadZombies, medianLife);


                // TODO:get rid of dead zombies in zombieSpawn
                auto pd = std::remove_if(zombieSpawnOrder.begin(),zombieSpawnOrder.end(),[](zombie_t* zb){return zb->health == 0;});
                zombieSpawnOrder = vector<zombie_t*>(zombieSpawnOrder.begin(),pd);

                // TODO: median
                if (isMedian && medianLife.size() != 0){
                    printf("At the end of round %d, the median zombie lifetime is %d\n",curRound,medianLife.inspect());
                }
                // TODO: current round is finished, update prevRound
                prevRound = curRound ;
                // TODO: player wins ? ( In case zombie last killed is NULL !!
                if (noZombie2Read && livingZombiesToShoot.empty())
                    break;
            }

        }

    }
    // TODO: player wins or not ?
    if(playerLiving){
        printf("VICTORY IN ROUND %d! %s was the last zombie.\n",jumpRound ? prevRound : curRound,zombieLastKilled->name.c_str());
    }
    // TODO: statistics
    if (isStatistics){
        statisticsGen(stat,zombieSpawnOrder,deadZombies);
    }
    // TODO: delete heap pointers when statistic is not on
    if (isStatistics) {
        for (auto &e: zombieSpawnOrder) {
            delete e;
        }
    }
    else{
        for (auto &e: zombieSpawnOrder){
            delete e;
        }
        for (auto &e: deadZombies){
            delete e;
        }
    }

    return 0;
}



