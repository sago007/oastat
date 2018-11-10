/*
===========================================================================
oastat - a program for parsing log files and write the result to a database
Copyright (C) 2012 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/oastat/
===========================================================================
*/

#include "userinfo2db.h"
#include <stdlib.h>
#include <boost/format/format_fwd.hpp>
#include <boost/format.hpp>


std::string Userinfo2Db::getCommand() const
{
	return "ClientUserinfoChanged";
}

bool Userinfo2Db::canProcess(const OaStatStruct &oss) const
{
	if (oss.command != getCommand() || oss.parameters.size()<1) {
		return false;
	}
	return true;
}

void Userinfo2Db::process(const OaStatStruct &oss)
{
	if (!canProcess(oss)) {
		return; //Invalid oss
	}
	bool isBot = false;
	std::map<std::string,std::string> arguments = oss.GetInfostring();

	//grow clientIdMap until oss.parameters.at(0)+1
	while ((int)clientIdMap.size() < oss.parameters.at(0)+2) {
		clientIdMap.push_back("");
	}

	if (arguments["id"].length()>0 || arguments["hashedid"].length() > 0) {
		//Not bot
		if(arguments["id"].length()>0) {
			clientIdMap[oss.parameters.at(0)] = getHashedId(arguments["id"]);
		} else {
			clientIdMap[oss.parameters.at(0)] = arguments["hashedid"];
		}
	} else {
		//bot
		clientIdMap[oss.parameters.at(0)] = (boost::format("%1%_client%2%") % arguments["n"] % oss.parameters.at(0) ).str();
		isBot = true;
	}
	std::string player = clientIdMap[oss.parameters.at(0)];

	dp->setPlayerInfo(player,arguments["n"],isBot,oss.second,atoi(arguments["t"].c_str()), arguments["model"],arguments["hmodel"],-1);
}
