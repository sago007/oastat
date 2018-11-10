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

#include "Disconnect2Db.h"


std::string Disconnect2Db::getCommand() const
{
	return "ClientDisconnect";
}

bool Disconnect2Db::canProcess(const OaStatStruct &oss) const
{
	if (oss.command != getCommand() || oss.parameters.size()<1) {
		return false;
	}
	return true;
}

void Disconnect2Db::process(const OaStatStruct &oss)
{
	if (!canProcess(oss)) {
		return;    //Invalid oss
	}

	std::string player = clientIdMap.at(oss.parameters.at(0));

	dp->setPlayerInfo(player,"",false,oss.second,-1,"","",-1);
}
