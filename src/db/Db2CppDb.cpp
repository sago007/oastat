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
http://code.google.com/p/oastat/
===========================================================================
*/

#include "Db2CppDb.hpp"
#include <iostream>
#include <sstream>
#include <set>

using namespace std;
using namespace cppdb;

void Db2CppDb::InitStrings(const string &backend)
{
	last_value = true;
	if (backend == "pgsql") {
		last_value = true; //Now also use last for PostgreSQL
	}
	if (backend == "mysql") {
		last_value = true;
	}
	sql_backend = backend;
}

void Db2CppDb::ReadConfigFromDb()
{
	string value;
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

bool Db2CppDb::IsDuplicate(const string &servername,const tm &thetime)
{
	cppdb::result res = *sql << "SELECT 'X' FROM oastat_games WHERE servername = ? AND time = ?"<<servername<<thetime;
	if (res.next()) {
		return true;
	} else {
		return false;
	}
}

Db2CppDb::Db2CppDb()
{
	connectstring = "pgsql:database=oastat";
	sql = shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = shared_ptr<transaction>(new transaction(*sql));
	InitStrings("pgsql");
	ReadConfigFromDb();
	debug = false;
}

Db2CppDb::Db2CppDb(const string &dbargs)
{
	stringstream stream(stringstream::in | stringstream::out);
	stream << dbargs;
	string holder;
	getline(stream,holder,':');
	connectstring = dbargs;
	sql = shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = shared_ptr<transaction>(new transaction(*sql));
	InitStrings(holder);
	ReadConfigFromDb();
	debug = false;
}

Db2CppDb::Db2CppDb(const Db2CppDb& orig)
{
	throw runtime_error("May not make copy of Db2CppDb");
}

Db2CppDb::~Db2CppDb()
{
}

void Db2CppDb::createTables()
{

}

void Db2CppDb::startGame(int gametype, const string &mapname, const string &basegame, const string &servername, const OaStatStruct &oss)
{
	sql = shared_ptr<cppdb::session>(new cppdb::session(connectstring));
	commitlock = nullptr;
	commitlock = shared_ptr<transaction>(new transaction(*sql));
	Rollback(); //in case there was some garbage that could be comitted (like warmup or an unfinished game)
	SetOk(true);
	timestamp = oss.getDateTime();
	if (oss.restOfLine.find("\\isWarmup\\1") != string::npos) {
		SetOk(false);
		cout << "Warmup: " << servername << ", " << oss.getTimeStamp() << endl;
		return;
	}
	if (IsDuplicate(servername,timestamp)) {
		SetOk(false);
		cout << "Duplicate:" << servername << ", " << oss.getTimeStamp() << endl;
		return;
	}
	if (last_value) {
		statement st = *sql << "INSERT INTO oastat_games(gametype, mapname, basegame,servername,time) VALUES (?,LOWER(?),?,?,?)"; //<<gametype<<mapname<<basegame<<servername<<timestamp<<exec;
		st.bind(gametype);
		st.bind(mapname);
		st.bind(basegame);
		st.bind(servername);
		st.bind(timestamp);
		st.exec();
		gamenumber = st.last_insert_id();
		if (gamenumber < 1) {
			SetOk(false);
			cout << "Must FAIL!" << endl;
		}
	} else {
		gamenumber = getNextGameNumber();
		*sql << "INSERT INTO oastat_games(gamenumber,gametype, mapname, basegame,servername,time) VALUES (?,?,LOWER(?),?,?,?)"<<
			 gamenumber<<gametype<<mapname<<basegame<<servername<<timestamp<<exec;
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
	*sql << "INSERT INTO oastat_gamecvars(gamenumber,cvar,value,numericvalue) VALUES (?,LOWER(?),?,?)" << gamenumber << cvar << value << 0 << exec;
	DebugMessage("addCvar");
}

/*
 endGame is called then we finnish the game. This also means that it is safe to commit
 */
void Db2CppDb::endGame(int second)
{
	if (!isok) {
		return;
	}
	*sql << "UPDATE oastat_games SET second=? WHERE gamenumber = ?" << second << gamenumber << exec;
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
			statement st = sql->create_statement("SAVEPOINT SETPLAYER");
			st.exec();
			*sql << "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES (?,?,?,?,?,?)" << 
					guid << nickname << timestamp << (isBot? "y":"n") << model << headmodel << exec;
			statement str = sql->create_statement("RELEASE SAVEPOINT SETPLAYER");
			str.exec();
		} catch (cppdb::cppdb_error &e) {
			DebugMessage("Already inserted? "+(string)e.what());
			statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETPLAYER");
			str.exec();
		}
		*sql << "UPDATE oastat_players SET nickname = ?,lastseen = ?,isBot = ?, model = ?, headmodel = ? WHERE guid = ? AND lastseen < ?" << 
				nickname << timestamp << (isBot? "y":"n") << model << headmodel << guid << timestamp << exec;
	}
	try {
		statement st = sql->create_statement("SAVEPOINT SETUSERINFO");
		st.exec();
		*sql << "INSERT INTO oastat_userinfo(gamenumber,second,player,team,model,skill) VALUES (?,?,?,?,?,?)" << gamenumber << second << getPlayerId(guid) << team << model << skill << exec;
		statement str = sql->create_statement("RELEASE SAVEPOINT SETUSERINFO");
		str.exec();
	} catch (cppdb_error &e) {
		statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETUSERINFO");
		str.exec();
		*sql << "UPDATE oastat_userinfo SET team = ?, model = ?, skill = ? WHERE gamenumber = ? AND second = ? AND player = ?" << 
				team << model << skill << gamenumber << second << getPlayerId(guid) << exec;
	}
	DebugMessage("setPlayerInfo for "+nickname+" with GUID: "+guid);
}

void Db2CppDb::addKill(int second, const std::string &attackerID, const std::string &targetID, int type)
{
	if (!isok) {
		return;
	}
	*sql<< "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(?,?,?,?,?)" << gamenumber << second << getPlayerId(attackerID) << getPlayerId(targetID) << type << exec;
	DebugMessage("addKill");
}

void Db2CppDb::addAward(int second, const std::string &player, int award)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (?,?,?,?)" << gamenumber << second << getPlayerId(player) << award << exec;
	DebugMessage("addAward");
}

void Db2CppDb::addScoreInfo(int second, const std::string &player, int score)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_points(gamenumber,second,player,score) VALUES (?,?,?,?)" << gamenumber << second << getPlayerId(player) << score << exec;
	DebugMessage("addScoreInfo");
}

void Db2CppDb::addCtf(int second, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'ctf',?)" << gamenumber << second << team << getPlayerId(player) << event << exec;
	DebugMessage("addCtf");
}

void Db2CppDb::addCtf1f(int second, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'1fctf',?)" << gamenumber << second << team << getPlayerId(player) << event << exec;
	DebugMessage("addCtf1f");
}

void Db2CppDb::addElimination(int second, int roundnumber, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,eventtype,generic1,gametype) VALUES (?,?,?,?,?,'elimination')" <<
		 gamenumber << second << team << event << roundnumber << exec;
	DebugMessage("addElimination");
}

void Db2CppDb::addCtfElimination(int second, int roundnumber, const std::string &player, int team, int event)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,eventtype,generic1,gametype) VALUES (?,?,?,?,?,?,'ctfelim')" << gamenumber << second <<
			team << getPlayerId(player) << event << roundnumber << exec;
	DebugMessage("addCtfElimination");
}

void Db2CppDb::addHarvester(int second, const std::string &player1, const std::string &player2, int team, int event, int score)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,gametype) VALUES (?,?,?,?,?,?,?,'harvester')" << 
		 gamenumber << second << team << getPlayerId(player1) << getPlayerId(player2) << event << score << exec;
	DebugMessage("addHarvester");
}

void Db2CppDb::addGenericTeamEvent(int second, int team, int amount, const std::string &gametype, const std::string &player1, const std::string &player2, int event, int generic1)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,generic1,gametype) VALUES (?,?,?,?,?,?,?,?,?)" << 
		 gamenumber << second << team << getPlayerId(player1) << getPlayerId(player2) << event << amount << generic1 << gametype << exec;
	DebugMessage("addGenericTeamEvent");
}

void Db2CppDb::addChallenge(int second, const std::string &player, int challenge, int amount)
{
	if (!isok) {
		return;
	}
	*sql << "INSERT INTO oastat_challenges(gamenumber,player,challenge,amount) VALUES (?,?,?,?)" << gamenumber << getPlayerId(player) << challenge << amount << exec;
	DebugMessage("addChallenge");
}

void Db2CppDb::addAccuracy(int second, const std::string &player, int type, int shotsFired, int shotsHit)
{
	if (!isok) {
		return;
	}
	try {
		statement st = sql->create_statement("SAVEPOINT SETACCURACY");
		st.exec();
		*sql << "INSERT INTO oastat_accuracy(gamenumber,player,shotsfired,shotshit,modtype) VALUES (?,?,?,?,?)" << gamenumber << getPlayerId(player) << shotsFired << shotsHit << type << exec;
		statement str = sql->create_statement("RELEASE SAVEPOINT SETACCURACY");
		str.exec();
	} catch (cppdb_error &e) {
		statement str = sql->create_statement("ROLLBACK TO SAVEPOINT SETACCURACY");
		str.exec();
		*sql << "UPDATE oastat_accuracy SET shotsfired = ?, shotshit = ?, modtype = ? WHERE player = ? AND gamenumber = ?" << shotsFired << shotsHit << type << getPlayerId(player) << gamenumber << exec;
	}
}

int Db2CppDb::getNextGameNumber()
{
	int ret = -1;
	result res = *sql<< "SELECT nextval('oastat_games_gamenumber_seq')";
	DebugMessage("Gettings next game number");
	if (res.next()) {
		res >> ret;
		return ret;
	} else {
		throw runtime_error("Could not get next gamenumber");
	}
}

int Db2CppDb::getLastGameNumber()
{
	int result = -1;
	cppdb::result res;
	if (sql_backend == "mysql") {
		res = *sql << "SELECT LAST_INSERT_ID() FROM DUAL";
	} else {
		res = *sql<< "SELECT currval('oastat_games_gamenumber_seq')";
	}
	DebugMessage("Gettings last game number");
	if (res.next()) {
		res>> result;
		cout << "Game number: " << result << endl;
		return result;
	} else {
		throw runtime_error("Could not get last gamenumber");
	}
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
	commitlock = shared_ptr<transaction>(new transaction(*sql));
	DebugMessage("Commited");
}

void Db2CppDb::Rollback()
{
	commitlock->rollback();
	commitlock = nullptr;
	commitlock = shared_ptr<transaction>(new transaction(*sql));
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

void Db2CppDb::DebugMessage(const string &msg)
{
	if (debug) {
		cout << "oastat: " << msg << endl;
	}
}

int Db2CppDb::getPlayerId(const string& guid)
{
	int ret = 0;
	if (playerids.count(guid)) {
		ret = playerids[guid];
	} else {
		result res = *sql << "SELECT playerid FROM oastat_players WHERE guid = ?" << guid;
		if (res.next()) {
			res >> ret;
			playerids[guid] = ret;
		}
	}
	return ret;
}
