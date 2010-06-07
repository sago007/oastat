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

#include "Db2PostgreSQL.h"
#include <stdlib.h>


#define STARTGAME "INSERT INTO oastat_games(gametype, mapname, basegame) VALUES (%i,'%s','%s')"
#define PLAYERSINSERT "INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES ('%s','%s',now(),%s,'%s','%s')"
#define PLAYERSUPDATE "UPDATE oastat_players SET nickname = '%s',lastseen = now(),isBot = %s, model = '%s', headmodel = '%s' WHERE guid = '%s'"
#define USERINFOINSERT "INSERT INTO oastat_userinfo(gamenumber,second,guid,team,model,skill) VALUES (%i,%i,'%s',%i,'%s',%i)"
#define ENDGAME "UPDATE oastat_games SET second=%i,time = now() where gamenumber = %i"
#define KILL "INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES(%i,%i,'%s','%s',%i)"
#define CAPTURE "INSERT INTO oastat_captures(gamenumber,second,player,team) VALUES (%i,%i,'%s',%i)"
#define AWARD "INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES (%i,%i,'%s',%i)"

#define PREP1 "PREPARE addkill(int,int,text,text,int) AS INSERT INTO oastat_kills(gamenumber,second,attacker,target,modtype) VALUES($1,$2,$3,$4,$5)"
#define PREP2 "PREPARE addgame(int,text,text) AS INSERT INTO oastat_games(gametype, mapname, basegame) VALUES ($1,$2,$3)"
#define PREP3 "PREPARE endgame(int,int) AS UPDATE oastat_games SET second=$2,time = now() where gamenumber = $1"
#define PREP4 "PREPARE addplayer(text,text,boolean,text,text) AS INSERT INTO oastat_players(guid,nickname,lastseen,isBot, model, headmodel) VALUES ($1,$2,now(),$3,$4,$5)"
#define PREP5 "PREPARE updateplayer(text,text,boolean,text,text) AS UPDATE oastat_players SET nickname = $2,lastseen = now(),isBot = $3, model = $4, headmodel = $5 WHERE guid = $6"
#define PREP6 "PREPARE adduserinfo(int,int,text,int,text,int) AS INSERT INTO oastat_userinfo(gamenumber,second,guid,team,model,skill) VALUES ($1,$2,$3,$4,$5,$6)"
#define PREP7 "PREPARE addcapture(int,int,text,int) AS INSERT INTO oastat_captures(gamenumber,second,player,team) VALUES ($1,$2,$3,$4)"
#define PREP8 "PREPARE addaward(int,int,text,int) AS INSERT INTO oastat_awards(gamenumber,second,player,award) VALUES ($1,$2,$3,$4)"

static char* booltext[2] = {"false","true"};

/**
 * Escapes special chaecters. The best solution should be to use PQescapeStringConn
 * but unfortunatly I had limited success likely because the C++ string class
 * already handles encoding. The only char postgreSql can be cheated by is the
 * single quete, so that is the only one we removes. 
 *
 * @param unescaped string
 * @return escaped string
 */
string Db2PostgreSQL::sqlescape(string sql) {
    #if 1
    string output = "";
    string::iterator itr;
    for(itr = sql.begin();itr<sql.end();itr++) {
        if(*itr == '\'') {
            output+="\'\'";
        } else
        {
            output += *itr;
        }
    }
    return output;
    #else
    char* tmp = (char*)malloc(sql.size()*2);
    PQescapeStringConn(conn,tmp,sql.c_str(),sql.size()*2,NULL);
    string res = tmp;
    free(tmp);
    return tmp;
    #endif
}

/**
 * Connects to the database and executes the query.
 *
 * @param query to execute
 * @return <0 if fail, >=0 if succes and/or >0 if returned tubles
 */
int Db2PostgreSQL::simpleQuery(const char* query) {
    res = PQexec(conn, query_string);
    int ret;
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        if (PQresultStatus(res) == PGRES_TUPLES_OK ) {
            //If a SELECT STATEMENT count results
            ret = PQntuples(res);
            PQclear(res);
            return ret;
        }
        PQclear(res);
        cout << "FAIL: " << query << endl;
        return -1;
    }
    PQclear(res); //We don't relly care about the result
    cout << "SUCCES: " << query << endl;
    return 0;
}

Db2PostgreSQL::Db2PostgreSQL() {
    conn = PQconnectdb("dbname=oastat"); //Connect to the database
    if (PQstatus(conn) == CONNECTION_BAD)
        throw "Failed to open connection to DB!";
    cout << "CONNECTED" << endl;
}

Db2PostgreSQL::Db2PostgreSQL(string args) {
    conn = PQconnectdb(args.c_str()); //Connect to the database
    if (PQstatus(conn) == CONNECTION_BAD)
        throw "Failed to open connection to DB!";
    cout << "CONNECTED" << endl;
}

Db2PostgreSQL::Db2PostgreSQL(const Db2PostgreSQL& orig) {
    throw "May not make copy of Db2PostgreSQL";
}

Db2PostgreSQL::~Db2PostgreSQL() {
    PQfinish(conn);
}

void Db2PostgreSQL::createTables()
{
    //Nothing to do
}

void Db2PostgreSQL::startGame(int gametype, string mapname, string basegame)
{
    simpleQuery("BEGIN");
    //simpleQuery("LOCK TABLE OASTAT_GAMES IN SHARE ROW EXCLUSIVE"); //Hold a lock until we have selected the just inserted line
    gamenumber = -1; //Prevent any following sql commands from completing until this is set!
    sprintf(query_string,STARTGAME,gametype,mapname.c_str(),basegame.c_str());
    res = PQexec(conn, query_string);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        cout << "FAIL: " << query_string << endl;
        simpleQuery("ROLLBACK");
        return;
    }
    PQclear(res);
    //res = PQexec(conn, "SELECT MAX(gamenumber) FROM OASTAT_GAMES");
    res = PQexec(conn, "SELECT currval('oastat_games_gamenumber_seq')");
    if (PQresultStatus(res) != PGRES_TUPLES_OK ) {
        PQclear(res);
        simpleQuery("ROLLBACK");
        return;
    }
    gamenumber = atoi(PQgetvalue(res,0,0));
    cout << "GAME: " << PQgetvalue(res,0,0) << ":" << gamenumber << endl;
    PQclear(res);
    //simpleQuery("COMMIT"); //release lock
    //simpleQuery("BEGIN"); //This is just for performace...
}

int Db2PostgreSQL::getGameNumber()
{
    return gamenumber; //Only to standard out, normally this should be read from db!!!
}

void Db2PostgreSQL::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill)
{
    //If team < 0 then it is a disconnect event that need not to be in
    if(team>-1)
    {
        /*
         Note: There is a possible race condition here!
         * One might consider ending the transaction and using autocommit for this part.
         */
        //simpleQuery("COMMIT");
        sprintf(query_string,"SELECT * FROM oastat_players WHERE GUID = '%s'",sqlescape(guid).c_str());
        if(simpleQuery(query_string)<1) {
            sprintf(query_string,PLAYERSINSERT,sqlescape(guid).c_str(),sqlescape(nickname).c_str(),booltext[isBot],sqlescape(model).c_str(),sqlescape(headmodel).c_str());
            simpleQuery(query_string);
        }
        sprintf(query_string,PLAYERSUPDATE,sqlescape(nickname).c_str(),booltext[isBot],sqlescape(model).c_str(),sqlescape(headmodel).c_str(),guid.c_str());
        simpleQuery(query_string);
        //simpleQuery("BEGIN");
    }
    sprintf(query_string,USERINFOINSERT,getGameNumber(),second,sqlescape(guid).c_str(),team,sqlescape(model).c_str(),skill);
    simpleQuery(query_string);
}

void Db2PostgreSQL::addKill(int second, string attackerID, string targetID, int type)
{
    sprintf(query_string,KILL,gamenumber,second,sqlescape(attackerID).c_str(),sqlescape(targetID).c_str(),type);
    simpleQuery(query_string);
}

void Db2PostgreSQL::endGame(int second)
{
    sprintf(query_string,ENDGAME,second,gamenumber);
    simpleQuery(query_string);
    simpleQuery("COMMIT"); //This is just for performace...
}

void Db2PostgreSQL::addCapture(int second, string player, int team)
{
    sprintf(query_string,CAPTURE,gamenumber,second,sqlescape(player).c_str(),team);
    simpleQuery(query_string);
}

void Db2PostgreSQL::addAward(int second, string player, int award)
{
    sprintf(query_string,AWARD,gamenumber,second,sqlescape(player).c_str(),award);
    simpleQuery(query_string);
}