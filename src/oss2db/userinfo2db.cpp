/* 
 * File:   userinfo2db.cpp
 * Author: poul
 * 
 * Created on 16. april 2010, 23:08
 */

#include "userinfo2db.h"
#include <stdlib.h> 

Userinfo2Db::Userinfo2Db() {
}

Userinfo2Db::Userinfo2Db(const Userinfo2Db& orig) {
}

Userinfo2Db::~Userinfo2Db() {
}

string Userinfo2Db::getCommand() {
    return "ClientUserinfoChanged";
}

void Userinfo2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<1)
        return; //Invalid oss
    map<string,string> arguments = oss.GetInfostring();

    if(arguments["id"].length()>0) //Not bot
        clientIdMap[oss.parameters.at(0)] = getHashedId(arguments["id"]);
    else //bot
        clientIdMap[oss.parameters.at(0)] = arguments["n"];

    dp->setPlayerInfo(clientIdMap[oss.parameters.at(0)],arguments["n"],arguments["id"]=="",oss.second,atoi(arguments["t"].c_str()), arguments["model"],arguments["hmodel"]);
}
