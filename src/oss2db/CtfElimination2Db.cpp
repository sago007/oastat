/* 
 * File:   CtfElimination2Db.cpp
 * Author: poul
 * 
 * Created on 21. januar 2011, 21:12
 */

#include "CtfElimination2Db.hpp"

string CtfElimination2Db::getCommand() {
    return "ELIMINATION";
}

bool CtfElimination2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3 )
        return false;
    return true;
}

void CtfElimination2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    string player = "";
    if(oss.parameters.at(1)!=-1)
        player= clientIdMap[oss.parameters.at(1)];

    dp->addCtfElimination(oss.second,oss.parameters.at(0) /*roundnumber*/,
            player,oss.parameters.at(2) /*team*/, oss.parameters.at(3) /*event*/);
}
