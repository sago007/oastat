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

#ifndef _DB2DBIXX_HPP
#define	_DB2DBIXX_HPP

#include "database.hpp"

#include <dbixx/dbixx.h>
#include <iostream>
#include <sstream>
#include <set>

using namespace std;
using namespace dbixx;

class Db2DbiXX  : public Database
{
public:
	Db2DbiXX();
	Db2DbiXX(string dbargs);
	Db2DbiXX(const Db2DbiXX& orig);
	virtual ~Db2DbiXX();
	void createTables();
	void startGame(int gametype, string mapname, string basegame, string servername, const OaStatStruct *oss);
	void addGameCvar(string cvar, string value);
	void endGame(int second);
	int getGameNumber();
	void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, const OaStatStruct *oss);
	void addKill(int second, string attackerID, string targetID, int type);
	//void addCapture(int second, string player, int team);
	void addAward(int second, string player, int award);
	void addScoreInfo(int second, string player, int score);
	void addCtf(int second, string player, int team, int event);
	void addCtf1f(int second, string player, int team, int event);
	void addElimination(int second, int roundnumber, int team, int event);
	void addCtfElimination(int second, int roundnumber, string player, int team, int event);
	void addHarvester(int second, string player1, string player2, int team, int event, int score);
	void addChallenge(int second, string player, int challenge, int amount);
	void addAccuracy(int secind, string player, int type, int shotsFired, int shotsHit);
	void doNotCommit();
private:
	session *sql;
	transaction *commitlock;
	bool isok;
	bool debug;
	int gamenumber;
	int getNextGameNumber();
	int getLastGameNumber();
	void Commit();
	void Rollback();
	bool Ok();
	void SetOk(bool ok);
	void DebugMessage(const string &msg);
	void InitStrings(string backend);
	void ReadConfigFromDb();
	bool IsDuplicate(string servername, tm thetime);
	int getPlayerId(const string &guid);
	std::map<std::string,int> playerids;
	string sql_backend;
	bool last_value;
	set<string> cvars2save;
	set<string> uservars2save;
};

#endif	/* _DB2DBIXX_HPP */

