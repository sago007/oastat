/* 
 * File:   Elimination2Db.cpp
 * Author: poul
 * 
 * Created on 15. januar 2011, 16:21
 */

#include "Elimination2Db.hpp"


string Elimination2Db::getCommand() {
    return "ELIMINATION";
}

bool Elimination2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3 || oss.parameters.at(0)<0 )
        return false;
    return true;
}

void Elimination2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    dp->addElimination(oss.second,oss.parameters.at(0) /*roundnumber*/,oss.parameters.at(1) /*team*/, oss.parameters.at(2) /*event*/);
}