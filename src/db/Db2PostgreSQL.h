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

#ifndef _DB2POSTGRESQL_H
#define	_DB2POSTGRESQL_H

#include "database.hpp"
#include "postgresql/libpq-fe.h"

using namespace std;

class Db2PostgreSQL : public Database
{
public:
	Db2PostgreSQL();
	Db2PostgreSQL(string args);
	Db2PostgreSQL(const Db2PostgreSQL& orig);
	virtual ~Db2PostgreSQL();
	void createTables();
	void startGame(int gametype, string mapname, string basegame);
	void endGame(int second);
	int getGameNumber();
	void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill);
	void addKill(int second, string attackerID, string targetID, int type);
	void addCapture(int second, string player, int team);
	void addAward(int second, string player, int award);
private:
	int simpleQuery(const char *);
	string sqlescape(string sql);
	PGconn *conn;
	PGresult   *res;
	int gamenumber;
	char query_string[4096];
};

#endif	/* _DB2POSTGRESQL_H */

