/* 
 * File:   Ctf2Db.cpp
 * Author: poul
 * 
 * Created on 19. november 2010, 23:08
 */

#include "Ctf2Db.hpp"


string Ctf2Db::getCommand() {
    return "CTF";
}

bool Ctf2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3)
        return false;
    return true;
}

void Ctf2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    string player = "";
    if(oss.parameters.at(0)!=-1)
        player= clientIdMap.at(oss.parameters.at(0));

    dp->addCtf(oss.second,player,oss.parameters.at(1) /*team*/, oss.parameters.at(2) /*event*/);
}
