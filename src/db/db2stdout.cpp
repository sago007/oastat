/* 
 * File:   db2stdout.cpp
 * Author: poul
 * 
 * Created on 6. marts 2010, 22:08
 */

#include "db2stdout.h"
#include <stdio.h>

static int gamecount = 1; //Only to standard out, normally this should be read from db!!!

/*Note: I use printf instead of cout because that is closest to the format that most databases expect.
 Most databases want :1,:2 instead of %s,%s and no quetes but still*/
#define STARTGAME "INSERT INTO oastat_games(gametype, mapname, basegame) VALUES (%i,'%s','%s');"
#define PLAYERSINSERT "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES ('%s','%s',now(),%s,'%s','%s');"
#define PLAYERSUPDATE "UPDATE oastat_players SET nickname = '%s',lastseen = now(),isBot = %s, model = '%s', headmodel = '%s' WHERE guid = '%s';"
#define USERINFOINSERT "INSERT INTO oastat_userinfo(gamenumber,second,guid,team,model,skill) VALUES (%i,%i,'%s',%i,'%s',%i);"
#define ENDGAME "UPDATE oastat_games SET second=%i,time = now() where gamenumber = %i;"
#define KILL "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(%i,%i,'%s','%s',%i);"
#define CAPTURE "INSERT INTO oastat_captures(gamenumber,second,player,team) VALUES (%i,%i,'%s',%i);"
#define AWARD "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (%i,%i,'%s',%i);"

static char* booltext[2] = {"false","true"};

DB2stdout::DB2stdout() {
}

DB2stdout::~DB2stdout() {
}

void DB2stdout::createTables()
{
    //Nothing to do
    cout << "CREATE TABLE(S) ..." << endl;
}

void DB2stdout::startGame(int gametype, string mapname, string basegame)
{
    printf(STARTGAME,gametype,mapname.c_str(),basegame.c_str());
    cout << endl;
    gamecount++; //Only to standard out, normally this should be read from db!!!
}

int DB2stdout::getGameNumber()
{
    return gamecount; //Only to standard out, normally this should be read from db!!!
}

void DB2stdout::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill)
{
    //If team < 0 then it is a disconnect event that need not to be in
    if(team>-1)
    {
        printf(PLAYERSINSERT,guid.c_str(),nickname.c_str(),booltext[isBot],model.c_str(),headmodel.c_str());
        cout << endl;
        printf(PLAYERSUPDATE,nickname.c_str(),booltext[isBot],model.c_str(),headmodel.c_str(),guid.c_str());
        cout << endl;
    }
    printf(USERINFOINSERT,getGameNumber(),second,guid.c_str(),team,model.c_str(),skill);
    cout << endl;
}

void DB2stdout::addKill(int second, string attackerID, string targetID, int type)
{
    printf(KILL,gamenumber,second,attackerID.c_str(),targetID.c_str(),type);
    cout << endl;
}

void DB2stdout::endGame(int second)
{
    printf(ENDGAME,second,gamenumber);
    cout << endl;
}

void DB2stdout::addCapture(int second, string player, int team)
{
    printf(CAPTURE,gamenumber,second,player.c_str(),team);
    cout << endl;
}

void DB2stdout::addAward(int second, string player, int award)
{
    printf(AWARD,gamenumber,second,player.c_str(),award);
    cout << endl;
}