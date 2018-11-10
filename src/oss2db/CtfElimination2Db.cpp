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

#include "CtfElimination2Db.hpp"

std::string CtfElimination2Db::getCommand() const
{
	return "ELIMINATION";
}

bool CtfElimination2Db::canProcess(const OaStatStruct &oss) const
{
	if (oss.command != getCommand() || oss.parameters.size()<3 ) {
		return false;
	}
	return true;
}

void CtfElimination2Db::process(const OaStatStruct &oss)
{
	if (!canProcess(oss)) {
		return;    //Invalid oss
	}

	std::string player = "";
	if (oss.parameters.at(1)!=-1) {
		player= clientIdMap.at(oss.parameters.at(1));
	}

	dp->addCtfElimination(oss.second,oss.parameters.at(0) /*roundnumber*/,
						  player,oss.parameters.at(2) /*team*/, oss.parameters.at(3) /*event*/);
}
