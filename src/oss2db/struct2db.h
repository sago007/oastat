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

#ifndef _STRUCT2DB_H
#define	_STRUCT2DB_H

#include "../oastatstruct.h"
#include "../db/database.hpp"
#include <string>

using namespace std;

/*
 This is the struct to database class that all classes that interpret a log line must inherit
 */
class Struct2Db {
	public:
            /*
             * The object will need a database interface to cummunicate to. This is given here
             */
            void setDb(Database *d) {
                dp = d;
            }

            /*
             * The object need a way to tell what command it is capreble to preocess.
             *
             * @returns The command the object can process
             */
            virtual string getCommand() = 0;

            /*
             * Parses s OaStatStruct to the object to process
             */
            virtual void process(OaStatStruct oss) = 0;

        protected:
            Database *dp; //Pointer to the used db
};


#endif	/* _STRUCT2DB_H */

