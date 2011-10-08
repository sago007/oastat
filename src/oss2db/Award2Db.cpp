/* 
 * File:   Award2Db.cpp
 * Author: poul
 * 
 * Created on 7. juni 2010, 19:41
 */

#include "Award2Db.h"



string Award2Db::getCommand() {
    return "Award";
}

bool Award2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<2 || oss.parameters.at(0)<0)
        return false;
    return true;
}

void Award2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    if(oss.parameters.at(0)>-1 && oss.parameters.at(0)<MAX_ID)
        return; //Not valid player
    
    string player = clientIdMap[oss.parameters.at(0)];

    dp->addAward(oss.second,player,oss.parameters.at(1));
}