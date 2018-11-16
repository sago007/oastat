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

#include "Accuracy2Db.hpp"
#include <boost/format.hpp>


std::string Accuracy2Db::getCommand() const
{
	return "Accuracy";
}

bool Accuracy2Db::canProcess(const OaStatStruct &oss) const
{
	if (oss.command != getCommand()) {
		return false;
	}
	return true;
}

void Accuracy2Db::process(const OaStatStruct &oss)
{
	if (!canProcess(oss)) {
		return;    //Invalid oss
	}

	if (oss.parameters.at(0)==-1) {
		return;    //Not valid player
	}

	const std::string& player = clientIdMap.at(oss.parameters.at(0));

	std::map<std::string,std::string> arguments = oss.GetInfostring();

	boost::format hformat("h%1%");
	boost::format fformat("f%1%");

	for (int i=0; i<100; ++i) {
		unsigned int hits = atoi(arguments[boost::str(hformat % i)].c_str());
		unsigned int fired = atoi(arguments[boost::str(fformat % i)].c_str());
		if (hits || fired) {
			dp->addAccuracy(oss.second,player,i,fired,hits);
		}
	}

}

