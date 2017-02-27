#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <queue>
#include <vector>
#include <cmath>

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
        int ray, empire_size = 1, i = 0;

    void clear_alarms() {
        i++;
        if(i == 5) {
            alarms.clear();
            i = 0;
        }
    }

    void update_ray() {

        ray = empire_size / 5 + 6;
    }

    void shout(hlt::Location l) {

        if(!war) {

            int k = 1;

            for(int i = 0; i < alarms.size(); i++) {

                if(map.getDistance(l, alarms[i]) < 2) {

                    k = 0;
                    alarms[i] = l;
                    break;
                }

                if(map.getDistance(l, alarms[i]) < 4) {

                    k = 0;
                    break;
                }
            }
            if(k && alarms.size() < 2)
                alarms.push_back(l);
        }
    }

    void call_to_arms(hlt::Location l) {

        if(!war){
            alarms.clear();
            war = true;
        }

        int k = 1;

        for(int i = 0; i < alarms.size(); i++) {

            if(map.getDistance(l, alarms[i]) < 3) {

                k = 0;
                break;
            }
        }

        if(k && alarms.size() < 3)
            alarms.push_back(l);

    }

    void alarm_normalize() {

        std::vector<hlt::Location> new_alarms;
        bool bun;

        for(int  i = 0; i < alarms.size(); i++) {

            bun = true;
            for(int j = 0; new_alarms.size(); j++) {
                if(map.getDistance(alarms[i], new_alarms[j]) < 4) {
                    bun = false;
                    break;
                }
            }

            if(bun && !(inner_tile(alarms[i].x, alarms[i].y)))

                new_alarms.push_back(alarms[i]);
        }

        alarms = new_alarms;
    }

    unsigned char find_inner_path(hlt::Location a, hlt::Location b) {

        unsigned char IDK = 5;

        if(map.getDistance(a, b) == 0)
            return 0;

        if(war && (IDK = enemy_direction(a.x, a.y))) {
            return IDK;
        }
        IDK = 5;

        hlt::Location l = map.getLocation(a, NORTH);
        float d = map.width + map.height, aux;

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

        if(map.getDistance(a, b) < d)
            return 5;

        return IDK;
    }

    unsigned char find_short_path(hlt::Location a, hlt::Location b) {

        unsigned char IDK = 0;

        return IDK;
    }

    void assign() {

        hlt::Location l, alarm;
        unsigned char c;
        int k, d, ind;
        
        while(!(to_move.empty())) {

            l = to_move.front();
            k = 1;
            d = map.width + map.height;
            ind;

            for(int i = 0; i < alarms.size(); i++) {

                alarm = alarms[i];

                if(map.getDistance(l, alarm) < d) {

                    d = map.getDistance(l, alarm);
                    k = 0;
                    ind = i;
                }
            }

            if(k) {
                if(inner_tile(l.x, l.y)) {
                    moves.insert({ l, out_direction(l.x, l.y) });
                } else {
                    moves.insert({ l, STILL });
                }
            } else {
                unsigned char rez = find_inner_path(l, alarms[ind]);
                if(rez <= 4)
                    moves.insert({ l, rez });
                    else
                        moves.insert({ l, STILL });
            }

            to_move.pop();
        }
    }

    unsigned char out_direction(unsigned short b, unsigned short a) {

        unsigned char d = 3;
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

    unsigned char enemy_direction(unsigned short b, unsigned short a) {

        bool v[] = { true, true, true, true };
        hlt::Location l = { b, a };
        hlt::Site s;
        short r;
        
        for(unsigned short i = 0; i < (map).width || i < (map).height; i++) {

            if(v[2]) {

                s = map.getSite({ b, (unsigned short)((a + i) % map.height) });

                if(s.owner != myID && s.owner)
                    return SOUTH;

                if(!s.owner && s.strength) {
                    v[2] = false;
                }
            }
           
            if(v[0]) {

                r = a - i;
                if(r < 0)
                    r += map.height;

                s = map.getSite({ b, (unsigned short)r });

                if(s.owner != myID && s.owner)
                    return NORTH;

                if(!s.owner && s.strength) {
                    v[0] = false;
                }

            }
            
            if(v[1]) {

                s = map.getSite({ (unsigned short)((b + i) % map.width), a });

                if(s.owner != myID && s.owner)
                    return EAST;

                if(!s.owner && s.strength) {
                    v[1] = false;
                }
            }

            if(v[3]) {

                r = b - i;
                if(r < 0)
                    r += map.width;

                s = map.getSite({ (unsigned short)r, a });

                if(s.owner != myID && s.owner)
                    return WEST;

                if(!s.owner && s.strength) {
                    v[3] = false;
                }
            }
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

    bool inner_tile(unsigned char b, unsigned char a) {

        if(((map).getSite({ b, a }, SOUTH)).owner != (myID))
            return false;

        if(((map).getSite({ b, a }, NORTH)).owner != (myID))
            return false;

        if(((map).getSite({ b, a }, EAST)).owner != (myID))
            return false;

        if(((map).getSite({ b, a }, WEST)).owner != (myID))
            return false;

        return true;
    }

    bool alarmable(unsigned short b, unsigned short a) {

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
                val[2] = (double)s.production / (double)s.strength;
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
                val[0] = (double)s.production / (double)s.strength;
            else
                val[0] = 10;
        }

        s = (map).getSite({ b, a }, EAST);

        if(s.owner == myID) {
            i++;
        } else if(s.strength < strength) {

            v[1] = enemy_neighbor((unsigned short)((b + 1) % (map).width), a);

            if(s.strength)
                val[1] = (double)s.production / (double)s.strength;
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
                val[3] = (double)s.production / (double)s.strength;
            else
                val[3] = 10;
        }

        if(strength <= 3 * prod) {
            moves.insert({ { b, a }, 0 });
            return;
        }

        if(i == 4) {
            if(strength <= 5 * prod) {
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
            call_to_arms(map.getLocation({ b, a }, (unsigned char)(ind + 1)));
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
            hlt::Location l = map.getLocation({ b, a }, (unsigned char)(ind + 1));

            if(!(inner_tile(l.x, l.y)))
                shout(map.getLocation({ b, a }, (unsigned char)(ind + 1)));

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

    int new_size;

    getInit(urs.myID, urs.map);

    sendInit("Mda");

    //std::cout << "pl";
    
    while(true) {

        urs.moves.clear();
        getFrame(urs.map);

        new_size = 0;

        for(unsigned short a = 0; a < (urs.map).height; a++) {
            for(unsigned short b = 0; b < (urs.map).width; b++) {


                if((s = (urs.map).getSite({ b, a })).owner == urs.myID) {
                    
                    new_size++;

                    if(s.strength == 0) {
                        urs.moves.insert({ { b, a }, STILL });

                    } else {

                        urs.do_move(b, a);
                    }
                }
            }
        }
        urs.empire_size = new_size;

        urs.update_ray();
        urs.alarm_normalize();
        urs.assign();
        sendFrame(urs.moves);
    }

    return 0;
}
