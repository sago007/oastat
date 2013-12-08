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

using namespace std;

#include "oastatstruct.h"
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

namespace
{

void makeLower(string &x)
{
	for (unsigned int i=0; i<x.length(); i++) {
		if (x[i] >= 'A' && x[i] <= 'X') {
			x[i] = x[i]-'A'+'a';
		}
	}
}

}

//tm OaStatStruct::_datetime;

OaStatStruct::OaStatStruct()
{
	second = 0;
	command = "";
	parameters.clear();
	restOfLine = "";
}

OaStatStruct::OaStatStruct(const OaStatStruct& orig)
{
	second = orig.second;
	command = orig.command;
	parameters = orig.parameters;
	restOfLine = orig.restOfLine;
	_datetime = orig._datetime;
}

OaStatStruct::~OaStatStruct()
{
}

void OaStatStruct::clear()
{
	second=0;
	command = "";
	parameters.clear();
	restOfLine = "";
}

void OaStatStruct::parseLine(string line)
{
	//Parse time
	//cout << line << endl;
	//try{
	string tempTimeString = line.substr(0,7);
	int posColon = tempTimeString.find(":");
	int minute = atoi(tempTimeString.substr(0,posColon).c_str());
	/*time_t thetime = time(NULL);
	gmtime_r(&thetime,&_datetime);*/
	second = atoi(tempTimeString.substr(posColon+1,7).c_str());
	second+=minute*60;
	line = line.substr(7,line.length()); //Cut the first part of the string.

	//Parse command name:
	posColon = line.find(":");
	command = line.substr(0,posColon);
	try {
		line = line.substr(posColon+2,line.length()); //Also remove the colon and the first space after it (+2)

		posColon = line.find_first_not_of(" -0123456789"); //First non-number or space
		stringstream ss;
		ss << line.substr(0,posColon);
		int counter = 0; //to ensure that we don't go on forever
		while (ss && counter++<10) {
			int value;
			ss >> value;
			parameters.push_back(value);
		}


		if (line.at(posColon) == ':') {
			posColon++; //There is some inconsistensy about then there is a colon here
			if(line.at(posColon) == ' ') {
				posColon++;    //If there is a colon there is likely a space
			}
		}
		line = line.substr(posColon,line.length());
	} catch (exception &e) {
		//The last part does not always exist... ignore it
		restOfLine = "";
		return;
	}


	restOfLine = line;
}


map<string,string> OaStatStruct::GetInfostring(string restOfLine) const
{
	map<string,string> list;
	int curPos = 0;
	unsigned int lastPos = 0;
	bool iskey = true;
	string key, value;
	if (restOfLine[0] == '\\') {
		lastPos++;
	}
	while (lastPos<restOfLine.length()) {
		curPos = restOfLine.find_first_of("\\", lastPos);
		if (curPos<0) {
			curPos = restOfLine.length();
		}
		if (iskey) {
			key = restOfLine.substr(lastPos,curPos-lastPos);
		} else {
			value = restOfLine.substr(lastPos,curPos-lastPos);
			//list.insert(pair<string,string>(key,value));
			makeLower(key);
			if (key.length()>0) {
				list[key] = value;
			}
			//cout << key << ":" << value << endl;
		}
		iskey = !iskey;
		lastPos = curPos+1;
	}
	return list;
}

map<string,string> OaStatStruct::GetInfostring() const
{
	return GetInfostring(restOfLine);
}

const tm OaStatStruct::getDateTime() const
{
	return _datetime;
}

string ZeroPadNumber(int num, int size = 2)
{
	std::ostringstream ss;
	ss.clear();
	ss << setw( size ) << setfill( '0' ) << num;
	return ss.str();
}

string OaStatStruct::getTimeStamp() const
{
	string s;
	s = "TIMESTAMP \'" + ZeroPadNumber(_datetime.tm_year+1900,4) + "-" + ZeroPadNumber(_datetime.tm_mon+1) + "-"
		+ ZeroPadNumber(_datetime.tm_mday) + " " + ZeroPadNumber(_datetime.tm_hour) + ":"
		+ ZeroPadNumber(_datetime.tm_min) + ":" + ZeroPadNumber(_datetime.tm_sec) + "\'";
	return s;
}

void OaStatStruct::setTimeStamp(const string &timestring)
{
	std::stringstream ss(timestring);
	string tmp;
	getline(ss,tmp,'-');
	_datetime.tm_year = atoi(tmp.c_str())-1900;
	getline(ss,tmp,'-');
	_datetime.tm_mon = atoi(tmp.c_str())-1;
	getline(ss,tmp,' ');
	_datetime.tm_mday = atoi(tmp.c_str());
	getline(ss,tmp,':');
	_datetime.tm_hour = atoi(tmp.c_str());
	getline(ss,tmp,':');
	_datetime.tm_min = atoi(tmp.c_str());
	getline(ss,tmp);
	_datetime.tm_sec = atoi(tmp.c_str());
}
