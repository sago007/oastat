/* 
 * File:   Db2DbiXX.cpp
 * Author: poul
 * 
 * Created on 11. oktober 2010, 20:59
 */

#include "Db2DbiXX.hpp"

//PostgreSQL:
#define GETNEXTGAMENUMBER "SELECT nextval('oastat_games_gamenumber_seq')"
#define GETLASTGAMENUMBER "SELECT currval('oastat_games_gamenumber_seq')"
//MySQL:
#define GETLASTGAMENUMBER_MYSQL "SELECT LAST_INSERT_ID() FROM DUAL"

#define STARTGAME "INSERT INTO oastat_games(gamenumber,gametype, mapname, basegame,servername,time) VALUES (?,?,LOWER(?),?,?,?)"
#define ADDCVAR "INSERT INTO oastat_gamecvars(gamenumber,cvar,value,numericvalue) VALUES (?,LOWER(?),?,?)"
#define STARTGAME_LASTVALUE "INSERT INTO oastat_games(gametype, mapname, basegame,servername,time) VALUES (?,LOWER(?),?,?,?)"
#define PLAYERSINSERT "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES (?,?,?,?,?,?)"
#define PLAYERSUPDATE "UPDATE oastat_players SET nickname = ?,lastseen = ?,isBot = ?, model = ?, headmodel = ? WHERE guid = ? AND lastseen < ?"
#define USERINFOINSERT "INSERT INTO oastat_userinfo(gamenumber,second,player,team,model,skill) VALUES (?,?,COALESCE(SELECT playerid FROM oastat_players WHERE guid = ?),0),?,?,?)"
#define USERINFOUPDATE "UPDATE oastat_userinfo SET team = ?, model = ?, skill = ? WHERE gamenumber = ? AND second = ? AND player = COALESCE((SELECT playerid FROM oastat_players WHERE GUID = ?),0)"
#define ENDGAME "UPDATE oastat_games SET second=? WHERE gamenumber = ?"
#define KILL "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?)"
#define AWARD "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?)"
#define POINT "INSERT INTO oastat_points(gamenumber,second,player,score) VALUES (?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?)"
#define CTF "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),'ctf',?)"
#define CTF1F "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),'1fctf',?)"
#define ELIMINATION "INSERT INTO oastat_team_events(gamenumber,second,team,eventtype,generic1,gametype) VALUES (?,?,?,?,?,'elimination')"
#define CTF_ELIM "INSERT INTO oastat_team_events(gamenumber,second,team,player,eventtype,generic1,gametype) VALUES (?,?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?,?,'ctfelim')"
#define HARVESTER "INSERT INTO oastat_team_events(gamenumber,second,team,player,player2,eventtype,amount,gametype) VALUES (?,?,?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?,?,'harvester')"
#define CHALLENGES "INSERT INTO oastat_challenges(gamenumber,player,challenge,amount) VALUES (?,COALESCE((SELECT playerid FROM oastat_players WHERE guid = ?),0),?,?)"

#define SELECT_USERVARS2SAVE "SELECT thekey FROM oastat_config_uservars2save"
#define SELECT_CVARS2SAVE "SELECT cvar FROM oastat_config_gamevars2save"

#define SELECT_DUPLICATE "SELECT 'X' FROM oastat_games WHERE servername = ? AND time = ?"

static string S_GETLASTGAMENUMBER = GETLASTGAMENUMBER;
static string S_STARTGAME = STARTGAME;
static string S_ADDCVAR = ADDCVAR;
static string S_STARTGAME_LASTVALUE = STARTGAME_LASTVALUE;
static string S_PLAYERSINSERT = PLAYERSINSERT;
static string S_PLAYERSUPDATE = PLAYERSUPDATE;
static string S_USERINFOINSERT = USERINFOINSERT;
static string S_USERINFOUPDATE = USERINFOUPDATE;
static string S_ENDGAME = ENDGAME;
static string S_KILL = KILL;
static string S_AWARD = AWARD;
static string S_POINT = POINT;
static string S_CTF = CTF;
static string S_CTF1F = CTF1F;
static string S_ELIMINATION = ELIMINATION;
static string S_CTF_ELIM = CTF_ELIM;
static string S_HARVESTER = HARVESTER;
static string S_CHALLENGES = CHALLENGES;

static string booltext[2] = {"n","y"};

void Db2DbiXX::InitStrings(string backend) {
    last_value = false;
    if(backend == "pgsql") {
        last_value = true; //Now also use last for PostgreSQL
    }
    if(backend == "mysql") {
        last_value = true;
        S_GETLASTGAMENUMBER = GETLASTGAMENUMBER_MYSQL;
    }
    sql_backend = backend;
}

void Db2DbiXX::ReadConfigFromDb() {
    result res;  
    row r;
    string value;
    *sql<<SELECT_USERVARS2SAVE,res;
    while(res.next(r)) {
        r >> value;
        uservars2save.insert(value);
    }
    *sql<< SELECT_CVARS2SAVE,res;
    while(res.next(r)) {
        r >> value;
        cvars2save.insert(value);
    }
}

bool Db2DbiXX::IsDuplicate(string servername, tm thetime) {
    result res;
    *sql << SELECT_DUPLICATE,servername,thetime,res;
    if(res.rows())
        return true;
    else
        return false;
}

Db2DbiXX::Db2DbiXX() {
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
    while(!stream.eof()) {
        stream >> holder;
        if(!stream.eof()) {
            string param;
            stream >> param;
            sql->param(holder,param);
        }
    }
    sql->connect();
    ReadConfigFromDb();
    commitlock = new transaction(*sql);
    debug = false;
    //sql(dbargs);
}

Db2DbiXX::Db2DbiXX(const Db2DbiXX& orig) {
    throw "May not make copy of Db2DbiXX";
}

Db2DbiXX::~Db2DbiXX() {
    delete commitlock;
    delete sql;
}

void Db2DbiXX::createTables() {
    
}

void Db2DbiXX::startGame(int gametype, string mapname, string basegame, string servername, OaStatStruct *oss) {
    sql->reconnect();
    Rollback(); //in case there was some garbage that could be comitted (like warmup or an unfinished game)
    SetOk(true);
    if(oss->restOfLine.find("\\isWarmup\\1") != string::npos)
    {
	SetOk(false);
	cout << "Warmup: " << servername << ", " << oss->getTimeStamp() << endl;
	return;
    }
    if(IsDuplicate(servername,oss->getDateTime())) {
        SetOk(false);
        cout << "Duplicate:" << servername << ", " << oss->getTimeStamp() << endl;
        return;
    }  
    if(last_value) {
        *sql << S_STARTGAME_LASTVALUE,gametype,mapname,basegame,servername,oss->getDateTime(),exec();
        gamenumber = getLastGameNumber();
        if(gamenumber < 1) {
            SetOk(false);
            cout << "Must FAIL!" << endl;
        }
    } else {
        gamenumber = getNextGameNumber();
        *sql << S_STARTGAME,gamenumber,gametype,mapname,basegame,servername,oss->getDateTime(),exec();
    }
    DebugMessage("startGame");
}

void Db2DbiXX::addGameCvar(string cvar, string value) {
    if(!isok)
        return;
    if(cvars2save.find(cvar) == cvars2save.end())
        return; //not to be saved
    *sql << S_ADDCVAR,gamenumber,cvar,value,0,exec();
    DebugMessage("addCvar");
}

/*
 endGame is called then we finnish the game. This also means that it is safe to commit
 */
void Db2DbiXX::endGame(int second) {
    if(!isok)
        return;
    *sql << S_ENDGAME,second,gamenumber,exec();
    Commit(); //Game have ended, transaction is in a stable state
    DebugMessage("endgame");
}

int Db2DbiXX::getGameNumber() {
    return gamenumber;
}

void Db2DbiXX::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, OaStatStruct *oss) {
    if(!isok)
        return;
    if(team>-1)
    {
        try{
            *sql << "SAVEPOINT SETPLAYER",exec();
            *sql << S_PLAYERSINSERT,guid,nickname,oss->getDateTime(),booltext[isBot],model,headmodel,exec();
            *sql << "RELEASE SAVEPOINT SETPLAYER",exec(); //Needed by postgresql
        }catch (dbixx_error &e) {
            DebugMessage("Already inserted? "+(string)e.what());
            *sql << "ROLLBACK TO SAVEPOINT SETPLAYER",exec();
        }
        *sql << S_PLAYERSUPDATE,nickname,oss->getDateTime(),booltext[isBot],model,headmodel,guid,oss->getDateTime(),exec();
    }
    try{
        *sql << "SAVEPOINT SETUSERINFO",exec();
        *sql << S_USERINFOINSERT,gamenumber,second,guid,team,model,skill,exec();
        *sql << "RELEASE SAVEPOINT SETUSERINFO",exec(); //Needed by postgresql
    }catch (dbixx_error &e) {
        *sql << "ROLLBACK TO SAVEPOINT SETUSERINFO",exec();
        *sql << S_USERINFOUPDATE,team,model,skill,gamenumber,second,guid,exec();
    }
    DebugMessage("setPlayerInfo for "+nickname+" with GUID: "+guid);
}

void Db2DbiXX::addKill(int second, string attackerID, string targetID, int type) {
    if(!isok)
        return;
    *sql<< S_KILL,gamenumber,second,attackerID,targetID,type,exec();
    DebugMessage("addKill");
}

/*void Db2DbiXX::addCapture(int second, string player, int team) {
    *sql<< CAPTURE,gamenumber,second,player,team,exec();
    DebugMessage("addCapture");
}*/

void Db2DbiXX::addAward(int second, string player, int award) {
    if(!isok)
        return;
    *sql << S_AWARD,gamenumber,second,player,award,exec();
    DebugMessage("addAward");
}

void Db2DbiXX::addScoreInfo(int second, string player, int score) {
    if(!isok)
        return;
    *sql << S_POINT,gamenumber,second,player,score,exec();
    DebugMessage("addScoreInfo");
}

void Db2DbiXX::addCtf(int second, string player, int team, int event) {
    if(!isok)
        return;
    *sql << S_CTF,gamenumber,second,team,player,event,exec();
    DebugMessage("addCtf");
}

void Db2DbiXX::addCtf1f(int second, string player, int team, int event) {
    if(!isok)
        return;
    *sql << S_CTF1F,gamenumber,second,team,player,event,exec();
    DebugMessage("addCtf1f");
}

void Db2DbiXX::addElimination(int second, int roundnumber, int team, int event) {
    if(!isok)
        return;
    *sql << S_ELIMINATION,gamenumber,second,team,event,roundnumber,exec();
    DebugMessage("addElimination");
}

void Db2DbiXX::addCtfElimination(int second, int roundnumber, string player, int team, int event) {
    if(!isok)
        return;
    *sql << S_CTF_ELIM,gamenumber,second,team,player,event,roundnumber,exec();
    DebugMessage("addCtfElimination");
}

void Db2DbiXX::addHarvester(int second, string player1, string player2, int team, int event, int score) {
    if(!isok)
        return;
    *sql << S_HARVESTER,gamenumber,second,team,player1,player2,event,score,exec();
    DebugMessage("addHarvester");
}

void Db2DbiXX::addChallenge(int second, string player, int challenge, int amount) {
    if(!isok)
        return;
    *sql << S_CHALLENGES,gamenumber,player,challenge,amount,exec();
    DebugMessage("addChallenge");
}

int Db2DbiXX::getNextGameNumber() {
    int result = -1;
    row r;
    *sql<< GETNEXTGAMENUMBER;
    DebugMessage("Gettings next game number");
    if(sql->single(r)) {
        r>> result;
        return result;
    }
    else {
        throw "Could not get next gamenumber";
    }
}

int Db2DbiXX::getLastGameNumber() {
    int result = -1;
    row r;
    *sql<< S_GETLASTGAMENUMBER;
    DebugMessage("Gettings last game number");
    if(sql->single(r)) {
        r>> result;
        cout << "Game number: " << result << endl;
        return result;
    }
    else {
        throw "Could not get last gamenumber";
    }
}


/*
 This is a helper function so we never forget to start a new transaction after a commit.
 */
void Db2DbiXX::Commit() {
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

void Db2DbiXX::Rollback() {
    commitlock->rollback();
    delete commitlock;
    commitlock = new transaction(*sql);
    DebugMessage("Rollback");
}

bool Db2DbiXX::Ok() {
    return isok;
}

void Db2DbiXX::SetOk(bool ok) {
    isok = ok;
}

void Db2DbiXX::doNotCommit() {
    SetOk(false);
}

void Db2DbiXX::DebugMessage(string msg) {
    if(debug) {
        cout << "oastat: " << msg << endl;
    }
}
