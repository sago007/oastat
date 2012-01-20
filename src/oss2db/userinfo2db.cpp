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
#include <boost/format/format_fwd.hpp> 
#include <boost/format.hpp>


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
    bool isBot = false;
    map<string,string> arguments = oss.GetInfostring();

	//grow clientIdMap until oss.parameters.at(0)+1
	while(clientIdMap.size() < oss.parameters.at(0)+2)
		clientIdMap.push_back("");
	
    if(arguments["id"].length()>0 || arguments["hashedid"].length() > 0) { //Not bot
		if(arguments["id"].length()>0)
			clientIdMap[oss.parameters.at(0)] = getHashedId(arguments["id"]);
		else
			clientIdMap[oss.parameters.at(0)] = arguments["hashedid"];
	}
    else //bot
    {
        clientIdMap[oss.parameters.at(0)] = (boost::format("%1%_client%2%") % arguments["n"] % oss.parameters.at(0) ).str();
	isBot = true;
    }
    string player = clientIdMap[oss.parameters.at(0)];

    dp->setPlayerInfo(player,arguments["n"],isBot,oss.second,atoi(arguments["t"].c_str()), arguments["model"],arguments["hmodel"],-1,&oss);
}
