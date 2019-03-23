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

#ifndef _LOCAL_H
#define	_LOCAL_H

#define MAX_ID 128

#include "oastatstruct.h"

/* This array contains all the guid hashes of the players in a game */
extern std::vector<std::string> clientIdMap;

/* returns the SHA-1 code of the parameter */
std::string getHashedId(const std::string& unhashedID);

#endif	/* _LOCAL_H */

