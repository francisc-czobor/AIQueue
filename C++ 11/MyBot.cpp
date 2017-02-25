#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <queue>
#include <vector>

#include "hlt.hpp"
#include "networking.hpp"

#define STILL 0
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4


class Urs {

    public:
        hlt::GameMap map;
        unsigned char myID;
        std::vector<hlt::Location> alarms, orders;
        std::queue<hlt::Location> to_move;
        std::set<hlt::Move> moves;
        bool war = false;
        float ray, size;

    void clear_alarms() {
        alarms.clear();
    }

    void shout(unsigned short b, unsigned short a) {

        if(!war)
            alarms.push_back({ b, a });
    }

    void call_to_arms(unsigned short b, unsigned short a) {

        if(!war){
            clear_alarms();
            war = true;
        }
        
        alarms.push_back({ b, a });
    }

    unsigned char find_inner_path(hlt::Location a, hlt::Location b) {

        unsigned char IDK = 5;

        hlt::Location l = map.getLocation(a, NORTH);
        float d = map.width, aux;

        if(map.getSite(l).owner == myID) {
            d = map.getDistance(l, b);
            IDK = NORTH;
        }

        l = map.getLocation(a, EAST);
        if(map.getSite(l).owner == myID && d > (aux = map.getDistance(l, b))) {
            d = aux;
            IDK = EAST;
        }

        l = map.getLocation(a, SOUTH);
        if(map.getSite(l).owner == myID && d > (aux = map.getDistance(l, b))) {
            d = aux;
            IDK = SOUTH;
        }

        l = map.getLocation(a, WEST);
        if(map.getSite(l).owner == myID && d > (aux = map.getDistance(l, b))) {
            d = aux;
            IDK = WEST;
        }

        return IDK;
    }

    unsigned char find_short_path(hlt::Location a, hlt::Location b) {

        unsigned char IDK = 0;

        return IDK;
    }

    void assign() {

        hlt::Location l, alarm;
        unsigned char c;
        int k;
        
        while(!(to_move.empty())) {

            l = to_move.front();
            k = 1;

            for(int i = 0; i < alarms.size(); i++) {

                alarm = alarms[i];
                if((c = find_inner_path(l, alarm)) <= 4) {

                    moves.insert({ l, c });
                    k = 0;
                    break;
                }
            }

            if(k)
                moves.insert({ l, STILL });

            to_move.pop();
        }
    }

    unsigned char out_direction(unsigned short b, unsigned short a) {

        unsigned char owner = (map.getSite({ b, a })).owner, d = 3;
        short r;
        
        for(unsigned short i = 0; i < (map).width || i < (map).height; i++) {

            if(((map).getSite({ b, (unsigned short)((a + i) % (map).height) })).owner != myID)
                return 3;

            r = a - i;
            if(r < 0)
                r += map.height;  

            if(((map).getSite({ b, (unsigned short)r })).owner != myID)
                return 1;

            if(((map).getSite({ (unsigned short)((b + i) % (map).width), a })).owner != myID)
                return 2;

            r = b - i;
            if(r < 0)
                r += map.width;

            if(((map).getSite({ (unsigned short)r, a })).owner != myID)
                return 4;
        }

        return 0;
    }

    int enemy_neighbor(unsigned char b, unsigned char a) {

        int i = 0;

        if(((map).getSite({ b, a }, SOUTH)).owner != (myID) &&
            ((map).getSite({ b, a }, SOUTH)).owner)
            i++;

        if(((map).getSite({ b, a }, NORTH)).owner != (myID) &&
            ((map).getSite({ b, a }, NORTH)).owner)
            i++;

        if(((map).getSite({ b, a }, EAST)).owner != (myID) &&
            ((map).getSite({ b, a }, EAST)).owner)
            i++;

        if(((map).getSite({ b, a }, WEST).owner != (myID) &&
            ((map).getSite({ b, a }, WEST)).owner))
            i++;

        return i;
    }

    void do_move(unsigned short b, unsigned short a) {

        int i = 0, v[] = {0, 0, 0, 0};
        double val[] = {0, 0, 0, 0};
        short r;
        unsigned char strength = (map.getSite({ b, a })).strength,
            prod = ((map).getSite({ b, a })).production;
        hlt::Site s = (map).getSite({ b, a }, SOUTH);


        if(s.owner == myID) {
            i++;
        } else if(s.strength < strength) {

            v[2] = enemy_neighbor( b, (unsigned short)((a + 1) % (map).height));

            if(s.strength)
                val[2] = (double)s.production / (double)s.strength + 1;
            else
                val[2] = 10;
        }

        s = (map).getSite({ b, a }, NORTH);

        if(s.owner == myID) {
            i++;
        } else if(s.strength < strength) {

            r = a - 1;
            if(r < 0)
                r += map.height;

            v[0] = enemy_neighbor( b, r);

            if(s.strength)
                val[0] = (double)s.production / (double)s.strength + 1;
            else
                val[0] = 10;
        }

        s = (map).getSite({ b, a }, EAST);

        if(s.owner == myID) {
            i++;
        } else if(s.strength < strength) {

            v[1] = enemy_neighbor((unsigned short)((b + 1) % (map).width), a);

            if(s.strength)
                val[1] = (double)s.production / (double)s.strength + 1;
            else
                val[1] = 10;
        }

        s = (map).getSite({ b, a }, WEST);

        if(s.owner == myID) {
            i++;
        } else if(s.strength < strength) {

            r = b - 1;
            if(r < 0)
                r += map.width;

            v[3] = enemy_neighbor(b - 1, a);

            if(s.strength)
                val[3] = (double)s.production / (double)s.strength + 1;
            else
                val[3] = 10;
        }

        if(i == 4) {
            if(strength <= 2 * prod) {
                moves.insert({ { b, a }, 0 });
                return;
            }

            to_move.push({ b, a });
            
            return;
        }

        int ind = 0, nr = 0;

        for(i = 0; i < 4; i++) {
            if(v[i] > nr) {
                nr = v[i];
                ind = i;
            }
        }

        if(nr) {
            moves.insert({ { b, a }, (unsigned char)(ind + 1) });
            call_to_arms(b, a);
            return;
        }

        double nr2 = 0;

        for(i = 0; i < 4; i++) {
            if(val[i] > nr2) {
                nr2 = val[i];
                ind = i;
            }   
        }
        if(nr2) {
            moves.insert({ { b, a }, (unsigned char)(ind + 1) });
            shout(b, a);
            return;
        }
        
        to_move.push({ b, a });
    }

};

int main(void) {

    srand(time(NULL));
    std::cout.sync_with_stdio(0);

    Urs urs;
    hlt::Site s;

    getInit(urs.myID, urs.map);

    sendInit("Mda");

    //std::cout << "pl";
    
    while(true) {

        urs.moves.clear();
        getFrame(urs.map);

        for(unsigned short a = 0; a < (urs.map).height; a++) {
            for(unsigned short b = 0; b < (urs.map).width; b++) {

                if((s = (urs.map).getSite({ b, a })).owner == urs.myID) {
                    
                    if(s.strength == 0) {
                        urs.moves.insert({ { b, a }, STILL });

                    } else {

                        urs.do_move(b, a);
                    }
                }
            }
        }

        urs.assign();
        urs.clear_alarms();

        sendFrame(urs.moves);
    }

    return 0;
}
