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

#ifndef _DATABASE_H
#define	_DATABASE_H

#include <string>
#include <iostream>

using namespace std;

/*
 This is the class that all database connections need to inherit to be able to recieve data from the program
 */
class Database {
	public:
		virtual void createTables() = 0;

		virtual void startGame(int gametype, string mapname, string basegame) = 0;
                
                virtual void endGame(int second) = 0;

                virtual int getGameNumber() = 0;

		virtual void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill = -1) = 0;

		virtual void addKill(int second, string attackerID, string targetID, int type) = 0;

                virtual void addCapture(int second, string player, int team) = 0;

                virtual void addAward(int second, string player, int award) = 0;
};

#endif //_DATABASE_H
