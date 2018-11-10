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
http://code.google.com/p/oastat/
===========================================================================
*/

#include "init2db.h"
#include <stdlib.h>


std::string Init2Db::getCommand() const
{
	return "InitGame";
}

bool Init2Db::canProcess(const OaStatStruct &oss) const
{
	if (oss.command != getCommand()) {
		return false;
	}
	return true;
}

void Init2Db::process(const OaStatStruct &oss)
{
	if (!canProcess(oss)) {
		return;    //Invalid oss
	}
	OaStatStruct oss_copy(oss);
	std::map<std::string,std::string> arguments = oss_copy.GetInfostring();
	int gametype = atoi(arguments["g_gametype"].c_str());
	std::string basegame = arguments["gamename"];
	std::string mapname = arguments["mapname"];
	std::string servername = arguments["sv_hostname"];
	oss_copy.setTimeStamp(arguments["g_timestamp"]);

	dp->startGame(gametype,mapname,basegame,servername,oss_copy);
	std::map<std::string,std::string>::iterator it;
	for (it = arguments.begin(); it != arguments.end(); it++) {
		if(
			it->first == "g_gametype" ||
			it->first == "gamename" ||
			it->first == "mapname" ||
			it->first == "sv_hostname" /*||
                it->first == "g_timestamp" do not skip this one*/
		) {
			continue;    //Skip the ones on the games-table
		}
		dp->addGameCvar(it->first,it->second);
	}
}
