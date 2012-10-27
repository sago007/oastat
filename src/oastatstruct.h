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

#ifndef _OASTATSTRUCT_H
#define	_OASTATSTRUCT_H

#include <vector>
#include <string>
#include <map>
#include <time.h>


/*
 * This is the most important class in oastat. This takes a line and stores all
 * information from that line in easy to process parts.
 */
class OaStatStruct
{
public:
	OaStatStruct();
	OaStatStruct(const OaStatStruct& orig);
	virtual ~OaStatStruct();

	//There are no reason to make any variables private.
	int second; //number of seconds
	std::string command; //Like 'ClientConnect', 'Award' and 'Kill'
	std::vector<int> parameters; //The parameters for the command.
	std::string restOfLine;
	const tm getDateTime() const;

	void parseLine(std::string line);
	void clear();

	/*
	 *Parses the rest of the line to an info string
	 */
	std::map<std::string,std::string> GetInfostring(std::string restOfLine) const;

	/*
	 *Same but uses this.restOfLine
	 */
	std::map<std::string,std::string> GetInfostring() const;

	/*
	 Returns a string with the time in the format:
	 * TIMESTAMP 'YYYY-MM-
	 */
	std::string getTimeStamp() const;

	/*
	 Sets _datetime. Requires input in the format: "YYYY-MM-DD HH:MM:SS"
	 */
	void setTimeStamp(const std::string &timestring);
private:
	tm _datetime; 
};

#endif	/* _OASTATSTRUCT_H */

