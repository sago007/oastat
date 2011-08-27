/* 
 * File:   Warmup2Db.cpp
 * Author: poul
 * 
 * Created on 26. august 2011, 20:28
 */

#include "Warmup2Db.hpp"

string Warmup2Db::getCommand() {
    return "Warmup";
}

bool Warmup2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand())
        return false;
    return true;
}

void Warmup2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    dp->doNotCommit();
}