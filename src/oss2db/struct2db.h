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

#ifndef _STRUCT2DB_H
#define	_STRUCT2DB_H

#include "../oastatstruct.h"
#include "../db/database.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

/*
 This is the struct to database class that all classes that interpret a log line must inherit
 */
class Struct2Db
{
public:
	/*
	 * The object will need a database interface to cummunicate to. This is given here
	 */
	void setDb(boost::shared_ptr<Database> &d)
	{
		dp = d;
	}

	/*
	 * The object need a way to tell what command it is capreble to preocess.
	 *
	 * @returns The command the object can process
	 */
	virtual std::string getCommand() const = 0;

	/*
	 *  The object needs a way to tell if it can process a command.
	 *
	 * This is better than the last getCommand, because it can check for multiple commands at once, and can
	 * discard subevents for supported commands.
	 */
	virtual bool canProcess(const OaStatStruct &oss) const = 0;

	/*
	 * Parses s OaStatStruct to the object to process
	 */
	virtual void process(const OaStatStruct &oss) = 0;

	virtual bool shouldCommit()
	{
		return false;
	};

protected:
	boost::shared_ptr<Database> dp; //Pointer to the used db
};


#endif	/* _STRUCT2DB_H */

