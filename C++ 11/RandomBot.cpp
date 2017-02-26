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

int enemy_neighbor(unsigned char b, unsigned char a, unsigned char owner, hlt::GameMap m) {
    int i = 0;
    short r;

    if((m.getSite({ b, (unsigned short)((a + 1) % m.height) })).owner != owner &&
        (m.getSite({ b, (unsigned short)((a + 1) % m.height) })).owner)
        i++;

    r = a - 1;
    if(!a)
        r += m.height;  

    if((m.getSite({ b, (unsigned short)r })).owner != owner &&
        (m.getSite({ b, (unsigned short)r })).owner)
        i++;

    if((m.getSite({ (unsigned short)((b + 1) % m.width), a })).owner != owner &&
        (m.getSite({ (unsigned short)((b + 1) % m.width), a })).owner)
        i++;

    r = b - 1;
    if(!b)
        r += m.width;

    if((m.getSite({ (unsigned short)r, a })).owner != owner &&
        (m.getSite({ (unsigned short)r, a })).owner != owner)
        i++;

    return i;
}

unsigned char get_direction(unsigned short b, unsigned short a, hlt::GameMap m) {

    int i = 0, v[] = {0, 0, 0, 0};
    double val[] = {0, 0, 0, 0};
    short r;
    unsigned char owner = (m.getSite({ b, a })).owner, strength = (m.getSite({ b, a })).strength;
    hlt::Site s = m.getSite({ b, (unsigned short)((a + 1) % m.height)});


    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        v[2] = enemy_neighbor( b, (unsigned short)((a + 1) % m.height), owner, m);
        if(s.strength)
            val[2] = (double)s.production / (double)s.strength + 1;
        else
            val[2] = 10;
    }

    r = a - 1;
    if(!a)
        r += m.height;  

    s = m.getSite({ b, (unsigned short)r });

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        v[0] = enemy_neighbor( b, (unsigned short)r, owner, m);
        if(s.strength)
            val[0] = (double)s.production / (double)s.strength + 1;
        else
            val[0] = 10;
    }

    s = m.getSite({(unsigned short)((b + 1) % m.width), a});

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        v[1] = enemy_neighbor((unsigned short)((b + 1) % m.width), a , owner, m);
        if(s.strength)
            val[1] = (double)s.production / (double)s.strength + 1;
        else
            val[1] = 10;
    }

    r = b - 1;
    if(!b)
        r += m.width;

    s = m.getSite({ (unsigned short)r, a });

    if(s.owner == owner) {
        i++;
    } else if(s.strength < strength) {
        v[3] = enemy_neighbor((unsigned short)r, a, owner, m);
        if(s.strength)
            val[3] = (double)s.production / (double)s.strength + 1;
        else
            val[3] = 10;
    }

    unsigned char prod = (m.getSite({ b, a })).production;
    
    if(i == 4) {
        if(strength <= 5 * prod)
            return 0;
        return out_direction(b, a, m);
    }

    if(strength <= 3 * prod)
        return 0;

    int ind = 0, nr = 0;

    for(i = 0; i < 4; i++) {
        if(v[i] > nr) {
            nr = v[i];
            ind = i;
        }
    }

    if(nr) {
        return ind+1;
    }

    double nr2 = 0;

    for(i = 0; i < 4; i++) {
        if(val[i] > nr2) {
            nr2 = val[i];
            ind = i;
        }   
    }
    if(nr2 != 0) {
        return ind + 1;
    }
    
    return 0;
}

int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    unsigned char myID;
    hlt::GameMap presentMap;
    hlt::Site s;
    getInit(myID, presentMap);

    sendInit("Pelecinta");

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
