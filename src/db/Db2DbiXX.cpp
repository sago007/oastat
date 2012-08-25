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

#include "Db2DbiXX.hpp"

static const string booltext[2] = {"n","y"};

void Db2DbiXX::InitStrings(string backend)
{
	last_value = false;
	if(backend == "pgsql")
	{
		last_value = true; //Now also use last for PostgreSQL
	}
	if(backend == "mysql")
	{
		last_value = true;
	}
	sql_backend = backend;
}

void Db2DbiXX::ReadConfigFromDb()
{
	result res;
	row r;
	string value;
	*sql<<"SELECT thekey FROM oastat_config_uservars2save",res;
	while(res.next(r))
	{
		r >> value;
		uservars2save.insert(value);
	}
	*sql<< "SELECT cvar FROM oastat_config_gamevars2save",res;
	while(res.next(r))
	{
		r >> value;
		cvars2save.insert(value);
	}
}

bool Db2DbiXX::IsDuplicate(string servername, tm thetime)
{
	result res;
	*sql << "SELECT 'X' FROM oastat_games WHERE servername = ? AND time = ?",servername,thetime,res;
	if(res.rows())
		return true;
	else
		return false;
}

Db2DbiXX::Db2DbiXX()
{
	sql = new session("pgsql");
	InitStrings("pgsql");
	sql->param("dbname","oastat");
	sql->connect();
	ReadConfigFromDb();
	commitlock = new transaction(*sql);
	debug = false;
}

Db2DbiXX::Db2DbiXX(string dbargs)
{
	stringstream stream(stringstream::in | stringstream::out);
	stream << dbargs;
	string holder;
	stream >> holder;
	sql = new session(holder);
	InitStrings(holder);
	while(!stream.eof())
	{
		stream >> holder;
		if(!stream.eof())
		{
			string param;
			stream >> param;
			sql->param(holder,param);
		}
	}
	sql->connect();
	ReadConfigFromDb();
	commitlock = new transaction(*sql);
	debug = true;
	//sql(dbargs);
}

Db2DbiXX::Db2DbiXX(const Db2DbiXX& orig)
{
	throw "May not make copy of Db2DbiXX";
}

Db2DbiXX::~Db2DbiXX()
{
	delete commitlock;
	delete sql;
}

void Db2DbiXX::createTables()
{

}

void Db2DbiXX::startGame(int gametype, string mapname, string basegame, string servername, const OaStatStruct *oss)
{
	sql->reconnect();
	Rollback(); //in case there was some garbage that could be comitted (like warmup or an unfinished game)
	SetOk(true);
	if(oss->restOfLine.find("\\isWarmup\\1") != string::npos)
	{
		SetOk(false);
		cout << "Warmup: " << servername << ", " << oss->getTimeStamp() << endl;
		return;
	}
	if(IsDuplicate(servername,oss->getDateTime()))
	{
		SetOk(false);
		cout << "Duplicate:" << servername << ", " << oss->getTimeStamp() << endl;
		return;
	}
	if(last_value)
	{
		*sql << "INSERT INTO oastat_games(gametype, mapname, basegame,servername,time) VALUES (?,LOWER(?),?,?,?)",gametype,mapname,basegame,servername,oss->getDateTime(),exec();
		gamenumber = getLastGameNumber();
		if(gamenumber < 1)
		{
			SetOk(false);
			cout << "Must FAIL!" << endl;
		}
	}
	else
	{
		gamenumber = getNextGameNumber();
		*sql << "INSERT INTO oastat_games(gamenumber,gametype, mapname, basegame,servername,time) VALUES (?,?,LOWER(?),?,?,?)",
				gamenumber,gametype,mapname,basegame,servername,oss->getDateTime(),exec();
	}
	DebugMessage("startGame");
}

void Db2DbiXX::addGameCvar(string cvar, string value)
{
	if(!isok)
		return;
	if(cvars2save.find(cvar) == cvars2save.end())
		return; //not to be saved
	*sql << "INSERT INTO oastat_gamecvars(gamenumber,cvar,value,numericvalue) VALUES (?,LOWER(?),?,?)",gamenumber,cvar,value,0,exec();
	DebugMessage("addCvar");
}

/*
 endGame is called then we finnish the game. This also means that it is safe to commit
 */
void Db2DbiXX::endGame(int second)
{
	if(!isok)
		return;
	*sql << "UPDATE oastat_games SET second=? WHERE gamenumber = ?",second,gamenumber,exec();
	Commit(); //Game have ended, transaction is in a stable state
	DebugMessage("endgame");
}

int Db2DbiXX::getGameNumber()
{
	return gamenumber;
}

void Db2DbiXX::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, const OaStatStruct *oss)
{
	if(!isok)
		return;
	if(team>-1)
	{
		try
		{
			*sql << "SAVEPOINT SETPLAYER",exec();
			*sql << "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES (?,?,?,?,?,?)",guid,nickname,oss->getDateTime(),booltext[isBot],model,headmodel,exec();
			*sql << "RELEASE SAVEPOINT SETPLAYER",exec(); //Needed by postgresql
		}
		catch (dbixx_error &e)
		{
			DebugMessage("Already inserted? "+(string)e.what());
			*sql << "ROLLBACK TO SAVEPOINT SETPLAYER",exec();
		}
		*sql << "UPDATE oastat_players SET nickname = ?,lastseen = ?,isBot = ?, model = ?, headmodel = ? WHERE guid = ? AND lastseen < ?",nickname,oss->getDateTime(),booltext[isBot],model,headmodel,guid,oss->getDateTime(),exec();
	}
	try
	{
		*sql << "SAVEPOINT SETUSERINFO",exec();
		*sql << "INSERT INTO oastat_userinfo(gamenumber,second,player,team,model,skill) VALUES (?,?,?,?,?,?)",gamenumber,second,getPlayerId(guid),team,model,skill,exec();
		*sql << "RELEASE SAVEPOINT SETUSERINFO",exec(); //Needed by postgresql
	}
	catch (dbixx_error &e)
	{
		*sql << "ROLLBACK TO SAVEPOINT SETUSERINFO",exec();
		*sql << "UPDATE oastat_userinfo SET team = ?, model = ?, skill = ? WHERE gamenumber = ? AND second = ? AND player = ?",team,model,skill,gamenumber,second,getPlayerId(guid),exec();
	}
	DebugMessage("setPlayerInfo for "+nickname+" with GUID: "+guid);
}

void Db2DbiXX::addKill(int second, string attackerID, string targetID, int type)
{
	if(!isok)
		return;
	*sql<< "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(?,?,?,?,?)",gamenumber,second,getPlayerId(attackerID),getPlayerId(targetID),type,exec();
	DebugMessage("addKill");
}

void Db2DbiXX::addAward(int second, string player, int award)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (?,?,?,?)",gamenumber,second,getPlayerId(player),award,exec();
	DebugMessage("addAward");
}

void Db2DbiXX::addScoreInfo(int second, string player, int score)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_points(gamenumber,second,player,score) VALUES (?,?,?,?)",gamenumber,second,getPlayerId(player),score,exec();
	DebugMessage("addScoreInfo");
}

void Db2DbiXX::addCtf(int second, string player, int team, int event)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'ctf',?)",gamenumber,second,team,getPlayerId(player),event,exec();
	DebugMessage("addCtf");
}

void Db2DbiXX::addCtf1f(int second, string player, int team, int event)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'1fctf',?)",gamenumber,second,team,getPlayerId(player),event,exec();
	DebugMessage("addCtf1f");
}

void Db2DbiXX::addElimination(int second, int roundnumber, int team, int event)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,eventtype,generic1,gametype) VALUES (?,?,?,?,?,'elimination')",
			gamenumber,second,team,event,roundnumber,exec();
	DebugMessage("addElimination");
}

void Db2DbiXX::addCtfElimination(int second, int roundnumber, string player, int team, int event)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,eventtype,generic1,gametype) VALUES (?,?,?,?,?,?,'ctfelim')",gamenumber,second,team,getPlayerId(player),event,roundnumber,exec();
	DebugMessage("addCtfElimination");
}

void Db2DbiXX::addHarvester(int second, string player1, string player2, int team, int event, int score)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,gametype) VALUES (?,?,?,?,?,?,?,'harvester')",
			gamenumber,second,team,getPlayerId(player1),getPlayerId(player2),event,score,exec();
	DebugMessage("addHarvester");
}

void Db2DbiXX::addChallenge(int second, string player, int challenge, int amount)
{
	if(!isok)
		return;
	*sql << "INSERT INTO oastat_challenges(gamenumber,player,challenge,amount) VALUES (?,?,?,?)",gamenumber,getPlayerId(player),challenge,amount,exec();
	DebugMessage("addChallenge");
}

void Db2DbiXX::addAccuracy(int second, string player, int type, int shotsFired, int shotsHit)
{
	if(!isok)
		return;
	try
	{
		*sql << "SAVEPOINT SETACCURACY",exec();
		*sql << "INSERT INTO oastat_accuracy(gamenumber,player,shotsfired,shotshit,modtype) VALUES (?,?,?,?,?)",gamenumber,getPlayerId(player),shotsFired,shotsHit,type,exec();
		*sql << "RELEASE SAVEPOINT SETACCURACY",exec(); //Needed by postgresql
	}
	catch (dbixx_error &e)
	{
		*sql << "ROLLBACK TO SAVEPOINT SETACCURACY",exec();
		*sql << "UPDATE oastat_accuracy SET shotsfired = ?, shotshit = ?, modtype = ? WHERE player = ? AND gamenumber = ?",shotsFired,shotsHit,type,getPlayerId(player),gamenumber,exec();
	}
}

int Db2DbiXX::getNextGameNumber()
{
	int result = -1;
	row r;
	*sql<< "SELECT nextval('oastat_games_gamenumber_seq')";
	DebugMessage("Gettings next game number");
	if(sql->single(r))
	{
		r>> result;
		return result;
	}
	else
	{
		throw "Could not get next gamenumber";
	}
}

int Db2DbiXX::getLastGameNumber()
{
	int result = -1;
	row r;
	if(sql_backend == "mysql")
		*sql << "SELECT LAST_INSERT_ID() FROM DUAL";
	else
		*sql<< "SELECT currval('oastat_games_gamenumber_seq')";
	DebugMessage("Gettings last game number");
	if(sql->single(r))
	{
		r>> result;
		cout << "Game number: " << result << endl;
		return result;
	}
	else
	{
		throw "Could not get last gamenumber";
	}
}


/*
 This is a helper function so we never forget to start a new transaction after a commit.
 */
void Db2DbiXX::Commit()
{
	if(!isok)
	{
		Rollback();
		return;
	}
	commitlock->commit();
	delete commitlock;
	commitlock = new transaction(*sql);
	DebugMessage("Commited");
}

void Db2DbiXX::Rollback()
{
	commitlock->rollback();
	delete commitlock;
	commitlock = new transaction(*sql);
	DebugMessage("Rollback");
}

bool Db2DbiXX::Ok()
{
	return isok;
}

void Db2DbiXX::SetOk(bool ok)
{
	isok = ok;
}

void Db2DbiXX::doNotCommit()
{
	SetOk(false);
}

void Db2DbiXX::DebugMessage(const string &msg)
{
	if(debug)
	{
		cout << "oastat: " << msg << endl;
	}
}

int Db2DbiXX::getPlayerId(const string& guid)
{
	int ret = 0;
	if(playerids.count(guid))
	{
		ret = playerids[guid];
	}
	else
	{
		row r;
		*sql << "SELECT playerid FROM oastat_players WHERE guid = ?",guid;
		if(sql->single(r))
		{
			r >> ret;
			playerids[guid] = ret;
		}
	}
	return ret;
}
