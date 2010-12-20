/* 
 * File:   Db2DbiXX.cpp
 * Author: poul
 * 
 * Created on 11. oktober 2010, 20:59
 */

#include "Db2DbiXX.hpp"

#define GETNEXTGAMENUMBER "SELECT nextval('oastat_games_gamenumber_seq')"

#define STARTGAME "INSERT INTO oastat_games(gamenumber,gametype, mapname, basegame) VALUES (?,?,?,?)"
#define PLAYERSINSERT "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES (?,?,?,?,?,?)"
#define PLAYERSUPDATE "UPDATE oastat_players SET nickname = ?,lastseen = ?,isBot = ?, model = ?, headmodel = ? WHERE guid = ?"
#define USERINFOINSERT "INSERT INTO oastat_userinfo(gamenumber,second,guid,team,model,skill) VALUES (?,?,?,?,?,?)"
#define ENDGAME "UPDATE oastat_games SET second=?,time = now() where gamenumber = ?"
#define KILL "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(?,?,?,?,?)"
#define CAPTURE "INSERT INTO oastat_captures(gamenumber,second,player,team) VALUES (?,?,?,?)"
#define AWARD "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (?,?,?,?)"
#define CTF "INSERT INTO oastat_team_events(gamenumber,second,team,player,gametype,eventtype) VALUES (?,?,?,?,'ctf',?)"

static char* booltext[2] = {"false","true"};

static long long int MakeTimestamp(tm timestamp) {
    return mktime(&timestamp);
}

Db2DbiXX::Db2DbiXX() {
    sql = new session("pgsql");
    sql->param("dbname","oastat");
    sql->connect();
    commitlock = new transaction(*sql);
    debug = true;
}

Db2DbiXX::Db2DbiXX(string dbargs)
{
    stringstream stream(stringstream::in | stringstream::out);
    stream << dbargs;
    string holder;
    stream >> holder;
    sql = new session(holder);
    while(!stream.eof()) {
        stream >> holder;
        if(!stream.eof()) {
            string param;
            stream >> param;
            sql->param(holder,param);
        }
    }
    sql->connect();
    commitlock = new transaction(*sql);
    debug = true;
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

void Db2DbiXX::startGame(int gametype, string mapname, string basegame) {
    sql->reconnect();
    Rollback(); //in case there was some garbage that could be comitted (like warmup or an unfinished game)
    SetOk(true);
    gamenumber = getNextGameNumber();
    *sql << STARTGAME,gamenumber,gametype,mapname,basegame,exec();
    DebugMessage("startGame");
}

/*
 endGame is called then we finnish the game. This also means that it is safe to commit
 */
void Db2DbiXX::endGame(int second) {
    *sql << ENDGAME,second,gamenumber,exec();
    Commit(); //Game have ended, transaction is in a stable state
    DebugMessage("endgame");
}

int Db2DbiXX::getGameNumber() {
    return gamenumber;
}

void Db2DbiXX::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, OaStatStruct *oss) {
    if(team>-1)
    {
        try{
            *sql << "SAVEPOINT SETPLAYER",exec();
            *sql << PLAYERSINSERT,guid,nickname,oss->getDateTime(),booltext[isBot],model,headmodel,exec();
            *sql << "RELEASE SAVEPOINT SETPLAYER",exec(); //Needed by postgresql
        }catch (dbixx_error &e) {
            DebugMessage("Already inserted? "+(string)e.what());
            *sql << "ROLLBACK TO SAVEPOINT SETPLAYER",exec();
        }
        *sql << PLAYERSUPDATE,nickname,oss->getDateTime(),booltext[isBot],model,headmodel,guid,exec();
    }
    *sql << USERINFOINSERT,gamenumber,second,guid,team,model,skill,exec();
    DebugMessage("setPlayerInfo for "+nickname+" with GUID: "+guid);
}

void Db2DbiXX::addKill(int second, string attackerID, string targetID, int type) {
    *sql<< KILL,gamenumber,second,attackerID,targetID,type,exec();
    DebugMessage("addKill");
}

void Db2DbiXX::addCapture(int second, string player, int team) {
    *sql<< CAPTURE,gamenumber,second,player,team,exec();
    DebugMessage("addCapture");
}

void Db2DbiXX::addAward(int second, string player, int award) {
    *sql << AWARD,gamenumber,second,player,award,exec();
    DebugMessage("addAward");
}

void Db2DbiXX::addCtf(int second, string player, int team, int event) {
    *sql << CTF,gamenumber,second,team,player,event,exec();
    DebugMessage("addCtf");
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

/*
 This is a helper function so we never forget to start a new transaction after a commit.
 */
void Db2DbiXX::Commit() {
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

void Db2DbiXX::DebugMessage(string msg) {
    if(debug) {
        cout << "oastat: " << msg << endl;
    }
}
