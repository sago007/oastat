/* 
 * File:   Challenge2Db.cpp
 * Author: poul
 * 
 * Created on 19. august 2011, 22:44
 */

#include "Challenge2Db.hpp"

string Challenge2Db::getCommand() {
    return "Challenge";
}


bool Challenge2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3)
        return false;
    return true;
}

void Challenge2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss
    
    if(oss.parameters.at(0)>-1 && oss.parameters.at(0)<MAX_ID)
        return; //Not valid player

    string player = clientIdMap.at(oss.parameters.at(0));

    dp->addChallenge(oss.second,player,oss.parameters.at(1),oss.parameters.at(2));
}