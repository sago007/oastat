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

#ifndef _DB2STDOUT_H
#define	_DB2STDOUT_H

#include "database.hpp"

using namespace std;

class DB2stdout : public Database {
public:
    DB2stdout();
    ~DB2stdout();
    void createTables();
    void startGame(int gametype, string mapname, string basegame);
    void endGame(int second);
    int getGameNumber();
    void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill);
    void addKill(int second, string attackerID, string targetID, int type);
    void addCapture(int second, string player, int team);
    void addAward(int second, string player, int award);
private:
    int gamenumber;
};

#endif	/* _DB2STDOUT_H */

