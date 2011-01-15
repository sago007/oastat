/*
===========================================================================
oastat - OpenArena stat program
Copyright (C) 2010 Poul Sander (oastat@poulsander.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
===========================================================================
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

bool Userinfo2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<1)
        return false;
    return true;
}

void Userinfo2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss
    map<string,string> arguments = oss.GetInfostring();

    if(arguments["id"].length()>0) //Not bot
        clientIdMap[oss.parameters.at(0)] = getHashedId(arguments["id"]);
    else //bot
        clientIdMap[oss.parameters.at(0)] = arguments["n"];

    dp->setPlayerInfo(clientIdMap[oss.parameters.at(0)],arguments["n"],arguments["id"]=="",oss.second,atoi(arguments["t"].c_str()), arguments["model"],arguments["hmodel"],-1,&oss);
}
