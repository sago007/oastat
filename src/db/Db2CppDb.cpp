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

#include "Db2CppDb.hpp"
#include <iostream>
#include <sstream>
#include <set>


void Db2CppDb::InitStrings(const std::string &backend)
{
	last_value = true;
	sql_backend = backend;
}

void Db2CppDb::ReadConfigFromDb()
{
	std::string value;
	cppdb::result res = *sql<<"SELECT thekey FROM oastat_config_uservars2save";
	while (res.next()) {
		res >> value;
		uservars2save.insert(value);
	}
	res = *sql<< "SELECT cvar FROM oastat_config_gamevars2save";
	while (res.next()) {
		res >> value;
		cvars2save.insert(value);
	}
}

bool Db2CppDb::IsDuplicate(const std::string &servername,const tm &thetime)
{
	cppdb::result res = *sql << "SELECT 'X' FROM oastat_games WHERE servername = ? AND time = ?"<<servername<<thetime;
	if (res.next()) {
		return true;
	} else {
		return false;
	}
}

static void DoDbDataInit(cppdb::session& sql)
{
	std::string checkWorldSql = "SELECT 'X' FROM oastat_players WHERE guid = 'WORLD'";
	std::string insertWorldSql = "INSERT INTO oastat_players (guid,lastseen, isbot, model,headmodel,nickname) VALUES ('WORLD',date('now'),'n','-','-','-')";
	std::string updateWorldSql = "UPDATE oastat_players SET playerid = 0 WHERE guid = 'WORLD'";
	cppdb::result res = sql << checkWorldSql;
	if (res.next()) {
		//The WORLD record already exist
		return;
	}
	sql << insertWorldSql << cppdb::exec;
	sql << updateWorldSql << cppdb::exec;
}

Db2CppDb::Db2CppDb() : connectstring("pgsql:database=oastat")
{
	sql = std::shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
	InitStrings("pgsql");
	ReadConfigFromDb();
	DoDbDataInit(*sql);
	commitlock->commit();
	commitlock = nullptr;
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
}

Db2CppDb::Db2CppDb(const std::string &dbargs)
{
	std::stringstream stream(std::stringstream::in | std::stringstream::out);
	stream << dbargs;
	std::string holder;
	getline(stream,holder,':');
	connectstring = dbargs;
	sql = std::shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
	InitStrings(holder);
	ReadConfigFromDb();
	DoDbDataInit(*sql);
	debug = false;
	ignoreMissingTimestamp = std::getenv(OASTAT_CPPDB_IGNORE_MISSING_TIMESTAMP);
	commitlock->commit();
	commitlock = nullptr;
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
}


Db2CppDb::~Db2CppDb()
{
}

void Db2CppDb::createTables()
{

}

void Db2CppDb::startGame(int gametype, const std::string &mapname, const std::string &basegame, const std::string &servername, const OaStatStruct &oss)
{
	commitlock = nullptr;
	sql = std::shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
	Rollback(); //in case there was some garbage that could be comitted (like warmup or an unfinished game)
	SetOk(false);
	timestamp = oss.getDateTime();
	if (!ignoreMissingTimestamp && oss.restOfLine.find("\\g_timestamp\\") == std::string::npos) {
		std::cout << "g_timestamp not set! Skipping!\n";
		return;
	}
	if (oss.restOfLine.find("\\isWarmup\\1") != std::string::npos) {
		std::cout << "Warmup: " << servername << ", " << oss.getTimeStamp() << "\n";
		return;
	}
	if (IsDuplicate(servername,timestamp)) {
		std::cout << "Duplicate:" << servername << ", " << oss.getTimeStamp() << "\n";
		return;
	}
	if (oss.getDateTime().tm_year+1900 == 0) {
		std::cout << "No timestamp: "<< servername << "\n";
		return;
	}
	if (last_value) {
		cppdb::statement st = *sql << "INSERT INTO oastat_games(gametype, mapname, basegame,servername,time) VALUES (?,LOWER(?),?,?,?)"; //<<gametype<<mapname<<basegame<<servername<<timestamp<<exec;
		st.bind(gametype);
		st.bind(mapname);
		st.bind(basegame);
		st.bind(servername);
		st.bind(timestamp);
		st.exec();
		gamenumber = st.last_insert_id();
		if (gamenumber < 1) {
			std::cout << "Must FAIL!\n";
		}
		else {
			SetOk(true);
		}
	} else {
		gamenumber = getNextGameNumber();
		*sql << "INSERT INTO oastat_games(gamenumber,gametype, mapname, basegame,servername,time) VALUES (?,?,LOWER(?),?,?,?)"<<
			 gamenumber << gametype << mapname << basegame << servername << timestamp << cppdb::exec;
		SetOk(true);
	}
	DebugMessage("startGame");
}

void Db2CppDb::addGameCvar(const std::string &cvar, const std::string &value)
{
	if (!isok) {
		return;
	}
	if (cvars2save.find(cvar) == cvars2save.end()) {
		return;    //not to be saved
	}
	*sql << "INSERT INTO oastat_gamecvars(gamenumber,cvar,value,numericvalue) VALUES (?,LOWER(?),?,?)" << gamenumber << cvar << value << 0 << cppdb::exec;
	DebugMessage("addCvar");
}

/*
 endGame is called then we finnish the game. This also means that it is safe to commit
 */
void Db2CppDb::endGame(int second)
{
	if (true) {
		//Not ok if no human kills
		cppdb::result res = *sql << "select k.gamenumber"
			" from oastat_kills k, oastat_players p"
			" where p.playerid = k.attacker and p.isbot = 'n' and k.target <> k.attacker and p.guid <> 'WORLD' and k.gamenumber = ?" << gamenumber;
		if (!res.next()) {
			isok = false;
		}
	}
	if (!isok) {
		Rollback();
		return;
	}
	*sql << "UPDATE oastat_games SET second=? WHERE gamenumber = ?" << second << gamenumber << cppdb::exec;
	Commit(); //Game have ended, transaction is in a stable state
	DebugMessage("endgame");
}

int Db2CppDb::getGameNumber()
{
	return gamenumber;
}

void Db2CppDb::setPlayerInfo(const std::string &guid, const std::string &nickname, bool isBot, int second, int team, const std::string &model, const std::string &headmodel, int skill)
{
	if (!isok) {
		return;
	}
	if (team>-1) {
		try {
			cppdb::statement st = sql->create_statement("SAVEPOINT SETPLAYER");
			st.exec();
			*sql << "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES (?,?,?,?,?,?)" << 
					guid << nickname << timestamp << (isBot? "y":"n") << model << headmodel << cppdb::exec;
			cppdb::statement str = sql->create_statement("RELEASE SAVEPOINT SETPLAYER");
			str.exec();
		} catch (cppdb::cppdb_error &e) {
			DebugMessage("Already inserted? "+(std::string)e.what());
			cppdb::statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETPLAYER");
			str.exec();
		}
		*sql << "UPDATE oastat_players SET nickname = ?,lastseen = ?,isBot = ?, model = ?, headmodel = ? WHERE guid = ? AND lastseen < ?" << 
				nickname << timestamp << (isBot? "y":"n") << model << headmodel << guid << timestamp << cppdb::exec;
	}
	try {
		cppdb::statement st = sql->create_statement("SAVEPOINT SETUSERINFO");
		st.exec();
		*sql << "INSERT INTO oastat_userinfo(gamenumber,second,player,team,model,skill) VALUES (?,?,?,?,?,?)" << gamenumber << second << getPlayerId(guid) << team << model << skill << cppdb::exec;
		cppdb::statement str = sql->create_statement("RELEASE SAVEPOINT SETUSERINFO");
		str.exec();
	} catch (cppdb::cppdb_error &e) {
		cppdb::statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETUSERINFO");
		str.exec();
		*sql << "UPDATE oastat_userinfo SET team = ?, model = ?, skill = ? WHERE gamenumber = ? AND second = ? AND player = ?" << 
				team << model << skill << gamenumber << second << getPlayerId(guid) << cppdb::exec;
	}
	DebugMessage("setPlayerInfo for "+nickname+" with GUID: "+guid);
}

void Db2CppDb::addKill(int second, const std::string &attackerID, const std::string &targetID, int type)
{
	if (!isok) {
		return;
	}
	*sql<< "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(?,?,?,?,?)" << gamenumber << second << getPlayerId(attackerID) << getPlayerId(targetID) << type << cppdb::exec;
	DebugMessage("addKill");
}

void Db2CppDb::addAward(int second, const std::string &player, int award)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (?,?,?,?)" << gamenumber << second << getPlayerId(player) << award << cppdb::exec;
	DebugMessage("addAward");
}

void Db2CppDb::addScoreInfo(int second, const std::string &player, int score)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_points(gamenumber,second,player,score) VALUES (?,?,?,?)" << gamenumber << second << getPlayerId(player) << score << cppdb::exec;
	DebugMessage("addScoreInfo");
}

void Db2CppDb::addCtf(int second, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'ctf',?)" << gamenumber << second << team << getPlayerId(player) << event << cppdb::exec;
	DebugMessage("addCtf");
}

void Db2CppDb::addCtf1f(int second, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'1fctf',?)" << gamenumber << second << team << getPlayerId(player) << event << cppdb::exec;
	DebugMessage("addCtf1f");
}

void Db2CppDb::addElimination(int second, int roundnumber, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,eventtype,generic1,gametype) VALUES (?,?,?,?,?,'elimination')" <<
		 gamenumber << second << team << event << roundnumber << cppdb::exec;
	DebugMessage("addElimination");
}

void Db2CppDb::addCtfElimination(int second, int roundnumber, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,eventtype,generic1,gametype) VALUES (?,?,?,?,?,?,'ctfelim')" << gamenumber << second <<
			team << getPlayerId(player) << event << roundnumber << cppdb::exec;
	DebugMessage("addCtfElimination");
}

void Db2CppDb::addHarvester(int second, const std::string &player1, const std::string &player2, int team, int event, int score)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,gametype) VALUES (?,?,?,?,?,?,?,'harvester')" << 
		 gamenumber << second << team << getPlayerId(player1) << getPlayerId(player2) << event << score << cppdb::exec;
	DebugMessage("addHarvester");
}

void Db2CppDb::addGenericTeamEvent(int second, int team, int amount, const std::string &gametype, const std::string &player1, const std::string &player2, int event, int generic1)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,generic1,gametype) VALUES (?,?,?,?,?,?,?,?,?)" << 
		 gamenumber << second << team << getPlayerId(player1) << getPlayerId(player2) << event << amount << generic1 << gametype << cppdb::exec;
	DebugMessage("addGenericTeamEvent");
}

void Db2CppDb::addChallenge(int, const std::string &player, int challenge, int amount)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_challenges(gamenumber,player,challenge,amount) VALUES (?,?,?,?)" << gamenumber << getPlayerId(player) << challenge << amount << cppdb::exec;
	DebugMessage("addChallenge");
}

void Db2CppDb::addAccuracy(int, const std::string &player, int type, int shotsFired, int shotsHit)
{
	if (!isok) {
		return;
	}
	try {
		cppdb::statement st = sql->create_statement("SAVEPOINT SETACCURACY");
		st.exec();
		*sql << "INSERT INTO oastat_accuracy(gamenumber,player,shotsfired,shotshit,modtype) VALUES (?,?,?,?,?)" << gamenumber << getPlayerId(player) << shotsFired << shotsHit << type << cppdb::exec;
		cppdb::statement str = sql->create_statement("RELEASE SAVEPOINT SETACCURACY");
		str.exec();
	} catch (cppdb::cppdb_error &e) {
		cppdb::statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETACCURACY");
		str.exec();
		*sql << "UPDATE oastat_accuracy SET shotsfired = ?, shotshit = ?, modtype = ? WHERE player = ? AND gamenumber = ?" << shotsFired << shotsHit << type << getPlayerId(player) << gamenumber << cppdb::exec;
	}
}

int Db2CppDb::getNextGameNumber()
{
	cppdb::result res = *sql<< "SELECT nextval('oastat_games_gamenumber_seq')";
	DebugMessage("Gettings next game number");
	if (res.next()) {
		int ret = -1;
		res >> ret;
		return ret;
	} 
	throw std::runtime_error("Could not get next gamenumber");
}

int Db2CppDb::getLastGameNumber()
{
	cppdb::result res;
	if (sql_backend == "mysql") {
		res = *sql << "SELECT LAST_INSERT_ID() FROM DUAL";
	} else {
		res = *sql<< "SELECT currval('oastat_games_gamenumber_seq')";
	}
	DebugMessage("Gettings last game number");
	if (res.next()) {
		int result = -1;
		res>> result;
		std::cout << "Game number: " << result << "\n";
		return result;
	} 
	throw std::runtime_error("Could not get last gamenumber");
}


/*
 This is a helper function so we never forget to start a new transaction after a commit.
 */
void Db2CppDb::Commit()
{
	if (!isok) {
		Rollback();
		return;
	}
	commitlock->commit();
	commitlock = nullptr;
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
	DebugMessage("Commited");
}

void Db2CppDb::Rollback()
{
	commitlock->rollback();
	commitlock = nullptr;
	commitlock = std::shared_ptr<cppdb::transaction>(new cppdb::transaction(*sql));
	DebugMessage("Rollback");
}

bool Db2CppDb::Ok()
{
	return isok;
}

void Db2CppDb::SetOk(bool ok)
{
	isok = ok;
}

void Db2CppDb::doNotCommit()
{
	SetOk(false);
}

void Db2CppDb::DebugMessage(const std::string &msg)
{
	if (debug) {
		std::cout << "oastat: " << msg << "\n";
	}
}

int Db2CppDb::getPlayerId(const std::string& guid)
{
	int ret = 0;
	if (playerids.count(guid)) {
		ret = playerids[guid];
	} else {
		cppdb::result res = *sql << "SELECT playerid FROM oastat_players WHERE guid = ?" << guid;
		if (res.next()) {
			res >> ret;
			playerids[guid] = ret;
		}
	}
	return ret;
}
