/* 
 * File:   Award2Db.cpp
 * Author: poul
 * 
 * Created on 7. juni 2010, 19:41
 */

#include "Award2Db.h"

Award2Db::Award2Db() {
}

Award2Db::Award2Db(const Award2Db& orig) {
}

Award2Db::~Award2Db() {
}



string Award2Db::getCommand() {
    return "Award";
}

bool Award2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<1)
        return false;
    return true;
}

void Award2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<2 || oss.parameters.at(0)<0)
        return; //Invalid oss

    string player = clientIdMap[oss.parameters.at(0)];

    dp->addAward(oss.second,player,oss.parameters.at(1));
}