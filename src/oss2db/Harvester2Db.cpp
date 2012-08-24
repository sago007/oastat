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

#include "Harvester2Db.hpp"

string Harvester2Db::getCommand()
{
	return "HARVESTER";
}

bool Harvester2Db::canProcess(const OaStatStruct &oss)
{
	if(oss.command != getCommand() || oss.parameters.size()<5 )
		return false;
	return true;
}

void Harvester2Db::process(const OaStatStruct &oss)
{
	if(!canProcess(oss))
		return; //Invalid oss

	string player1 = ""; //Parameter 0
	string player2 = ""; //Parameter 3

	if(oss.parameters.at(0)>-1 && oss.parameters.at(0)<clientIdMap.size() && oss.parameters.at(0)!=1022)
	{
		player1 = clientIdMap.at(oss.parameters.at(0));
	}
	else
	{
		player1 = "WORLD";
	}

	if(oss.parameters.at(3)>-1 && oss.parameters.at(3)<clientIdMap.size() && oss.parameters.at(3)!=1022)
	{
		player2 = clientIdMap.at(oss.parameters.at(3));
	}
	else
	{
		player2 = "WORLD";
	}

	dp->addHarvester(oss.second,player1,player2,oss.parameters.at(1) /*team*/,oss.parameters.at(2) /*event*/, oss.parameters.at(4) /*score*/ );
}

