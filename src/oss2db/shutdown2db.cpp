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

#include "shutdown2db.h"



string Shutdown2Db::getCommand()
{
	return "ShutdownGame";
}

bool Shutdown2Db::canProcess(const OaStatStruct &oss)
{
	if(oss.command != getCommand())
		return false;
	return true;
}

void Shutdown2Db::process(const OaStatStruct &oss)
{
	if(!canProcess(oss))
		return;
	dp->endGame(oss.second);
}

bool Shutdown2Db::shouldCommit()
{
	return true;
}
