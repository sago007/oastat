/* 
 * File:   Disconnect2Db.cpp
 * Author: poul
 * 
 * Created on 17. maj 2010, 21:13
 */

#include "Disconnect2Db.h"

Disconnect2Db::Disconnect2Db() {
}

Disconnect2Db::Disconnect2Db(const Disconnect2Db& orig) {
}

Disconnect2Db::~Disconnect2Db() {
}

string Disconnect2Db::getCommand() {
    return "ClientDisconnect";
}

void Disconnect2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<1)
        return; //Invalid oss

    string player = clientIdMap[oss.parameters.at(0)];

    dp->setPlayerInfo(player,"",false,oss.second,-1,"","",-1);
}
