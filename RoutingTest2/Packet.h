#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>

#include "INETDefs.h"

using namespace std;


class Packet : public cPacket
{
private:
    int nodeIdSrc;
    int nodeIdTrg;

public:
    Packet(int nodeIdSrc, int nodeIdTrg){
        this->nodeIdSrc = nodeIdSrc;
        this->nodeIdTrg = nodeIdTrg;
    }

    int getNodeIdSrc() {return nodeIdSrc;}
    int getNodeIdTrg() {return nodeIdTrg;}
};

#endif
