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

#ifndef _CTFELIMINATION2DB_HPP
#define	_CTFELIMINATION2DB_HPP


#include "struct2db.h"
#include "../common/local.h"

class CtfElimination2Db : public  Struct2Db
{
public:
	std::string getCommand() const override;
	bool canProcess(const OaStatStruct &oss) const override;

	void process(const OaStatStruct &oss) override;

private:

};

#endif	/* _CTFELIMINATION2DB_HPP */

