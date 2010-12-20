/* 
 * File:   Ctf2Db.cpp
 * Author: poul
 * 
 * Created on 19. november 2010, 23:08
 */

#include "Ctf2Db.hpp"

Ctf2Db::Ctf2Db() {
}

Ctf2Db::Ctf2Db(const Ctf2Db& orig) {
}

Ctf2Db::~Ctf2Db() {
}


string Ctf2Db::getCommand() {
    return "CTF";
}

void Ctf2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3 || oss.parameters.at(0)<0)
        return; //Invalid oss

    string player = "";
    if(oss.parameters.at(0)!=-1)
        player= clientIdMap[oss.parameters.at(0)];

    dp->addCtf(oss.second,player,oss.parameters.at(1) /*team*/, oss.parameters.at(2) /*event*/);
}
