/* 
 * File:   Harvester2Db.cpp
 * Author: poul
 * 
 * Created on 25. februar 2011, 22:01
 */

#include "Harvester2Db.hpp"

string Harvester2Db::getCommand() {
    return "HARVESTER";
}

bool Harvester2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<5 )
        return false;
    return true;
}

void Harvester2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    string player1 = ""; //Parameter 0
    string player2 = ""; //Parameter 3

    if(oss.parameters.at(0)>-1 && oss.parameters.at(0)<clientIdMap.size() && oss.parameters.at(0)!=1022) {
        player1 = clientIdMap.at(oss.parameters.at(0));
    } else {
        player1 = "WORLD";
    }

    if(oss.parameters.at(3)>-1 && oss.parameters.at(3)<clientIdMap.size() && oss.parameters.at(3)!=1022) {
        player2 = clientIdMap.at(oss.parameters.at(3));
    } else {
        player2 = "WORLD";
    }

    dp->addHarvester(oss.second,player1,player2,oss.parameters.at(1) /*team*/,oss.parameters.at(2) /*event*/, oss.parameters.at(4) /*score*/ );
}

