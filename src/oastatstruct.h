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

#ifndef _OASTATSTRUCT_H
#define	_OASTATSTRUCT_H

#include <vector>
#include <string>
#include <map>
#include <time.h>

using namespace std;

/*
 * This is the most important class in oastat. This takes a line and stores all
 * information from that line in easy to process parts.
 */
class OaStatStruct {
public:
    OaStatStruct();
    OaStatStruct(const OaStatStruct& orig);
    virtual ~OaStatStruct();

    //There are no reason to make any variables private.
    int second; //number of seconds
    string command; //Like 'ClientConnect', 'Award' and 'Kill'
    vector<int> parameters; //The parameters for the command.
    string restOfLine;
    tm getDateTime();

    void parseLine(string line);
    void clear();

    /*
     *Parses the rest of the line to an info string
     */
    map<string,string> GetInfostring(string restOfLine);

    /*
     *Same but uses this.restOfLine
     */
    map<string,string> GetInfostring();

    /*
     Returns a string with the time in the format:
     * TIMESTAMP 'YYYY-MM-
     */
    string getTimeStamp();

    /*
     Sets _datetime. Requires input in the format: "YYYY-MM-DD HH:MM:SS"
     */
    void setTimeStamp(string timestring);
private:
    static tm _datetime;
};

#endif	/* _OASTATSTRUCT_H */

