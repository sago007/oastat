/*
===========================================================================
oastat - a program for parsing log files and write the result to a database
Copyright (C) 2014 Poul Sander

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

#ifndef DB2CPPDB_HPP
#define	DB2CPPDB_HPP

#include "database.hpp"
#include <memory>
#include <set>
#include <cppdb/frontend.h>

class Db2CppDb : public Database {
public:
	Db2CppDb();
	Db2CppDb(const std::string &dbargs);
	Db2CppDb(const Db2CppDb& orig);
	virtual ~Db2CppDb();
	void createTables() override;
	void startGame(int gametype, const std::string &mapname, const std::string &basegame, const std::string &servername, const OaStatStruct &oss) override;
	void addGameCvar(const std::string &cvar, const std::string &value) override;
	void endGame(int second) override;
	int getGameNumber() override;
	void setPlayerInfo(const std::string &guid, const std::string &nickname, bool isBot, int second, int team, const std::string &model, const std::string &headmodel, int skill) override;
	void addKill(int second, const std::string &attackerID, const std::string &targetID, int type) override;
	//void addCapture(int second, string player, int team);
	void addAward(int second, const std::string &player, int award) override;
	void addScoreInfo(int second, const std::string &player, int score) override;
	void addCtf(int second, const std::string &player, int team, int event) override;
	void addCtf1f(int second, const std::string &player, int team, int event) override;
	void addElimination(int second, int roundnumber, int team, int event) override;
	void addCtfElimination(int second, int roundnumber, const std::string &player, int team, int event) override;
	void addHarvester(int second, const std::string &player1, const std::string &player2, int team, int event, int score) override;
	void addChallenge(int second, const std::string &player, int challenge, int amount) override;
	void addAccuracy(int second, const std::string &player, int type, int shotsFired, int shotsHit) override;
	void addGenericTeamEvent(int second, int team, int amount, const std::string &gametype, const std::string &player1, const std::string &player2, int event, int generic1) override;
	void doNotCommit() override;
private:
	std::shared_ptr<cppdb::session> sql;
	std::shared_ptr<cppdb::transaction> commitlock;
	std::string connectstring;
	bool isok;
	bool debug;
	int gamenumber; /** Gamenumber we are currently working on */
	tm timestamp; /** Start time of the game we are currently working on */
	int getNextGameNumber();
	int getLastGameNumber();
	void Commit();
	void Rollback();
	bool Ok();
	void SetOk(bool ok);
	void DebugMessage(const std::string &msg);
	void InitStrings(const std::string &backend);
	void ReadConfigFromDb();
	bool IsDuplicate(const std::string &servername, const tm &thetime);
	int getPlayerId(const std::string &guid);
	std::map<std::string,int> playerids;
	std::string sql_backend;
	bool last_value;
	std::set<std::string> cvars2save;
	std::set<std::string> uservars2save;
};

#endif	/* DB2CPPDB_HPP */

