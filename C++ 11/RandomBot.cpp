#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>

#include "hlt.hpp"
#include "networking.hpp"

/* 
#define STILL 0
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4 
*/

unsigned char out_direction(unsigned short b, unsigned short a, hlt::GameMap m) {

    unsigned char owner = (m.getSite({ b, a })).owner, d = 3;
    short r;
    
    for(unsigned short i = 0; i < m.width || i < m.height; i++) {

        if((m.getSite({ b, (unsigned short)((a + i) % m.height) })).owner != owner)
            return 3;

        r = a - i;
        if(r < 0)
            r += 30;  

        if((m.getSite({ b, (unsigned short)r })).owner != owner)
            return 1;

        if((m.getSite({ (unsigned short)((b + i) % m.width), a })).owner != owner)
            return 2;

         r = b - i;
        if(r < 0)
            r += 30;

        if((m.getSite({ (unsigned short)r, a })).owner != owner)
            return 4;
    }

    return 0;
}

unsigned char get_direction(unsigned short b, unsigned short a, hlt::GameMap m) {

    int i = 0;
    unsigned char owner = (m.getSite({ b, a })).owner, strength = (m.getSite({ b, a })).strength;
    hlt::Site s = m.getSite({ b, (unsigned short)((a + 1) % m.height)});

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        return 3;
    }

    if(a)
        s = m.getSite({ b, (unsigned short)((a - 1) % m.height)});
    else 
        s  = m.getSite({ b, 29 });

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        return 1;
    }

    s = m.getSite({(unsigned short)((b + 1) % m.width), a});


    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        return 2;
    }

    if(b)
        s = m.getSite({(unsigned short)((b - 1) % m.width), a});
    else 
        s = m.getSite({29, a});

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        return 4;
    }

    unsigned char prod = (m.getSite({ b, a })).production;
    
    if(prod && strength/prod <= 5)
        return 0;

    if(i == 4)
        return out_direction(b, a, m);

    return 0;
}

int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    hlt::Site s;
    getInit(myID, presentMap);



    sendInit("MyC++Bot2");

    

    std::set<hlt::Move> moves;
    while(true) {
        moves.clear();
        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                if((s = presentMap.getSite({ b, a })).owner == myID) {
                    
                    if(s.strength == 0) {
                        moves.insert({ { b, a }, 0 });

                    } else {

                        moves.insert({ { b, a }, get_direction(b, a, presentMap) });
                    }
                }
            }
        }

        sendFrame(moves);
    }

    return 0;
}
