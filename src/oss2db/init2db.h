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

#ifndef _INIT2DB_H
#define	_INIT2DB_H

#include "struct2db.h"
#include "../local.h"

class Init2Db : public Struct2Db
{
public:

	std::string getCommand() const;
	bool canProcess(const OaStatStruct &oss) const;

	void process(const OaStatStruct &oss);
private:

};

#endif	/* _INIT2DB_H */

