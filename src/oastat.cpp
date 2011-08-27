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

#define VERSION "0.2 BETA"

#include <iostream>
#include <fstream>
#include <vector>
#include <gcrypt.h>
#include <stdio.h>
#include <list>


using namespace std;

#include "db/database.hpp"
#ifdef USESTDOUT
#include "db/db2stdout.h"
#endif
#ifdef USEPOSTGRESQL
#include "db/Db2PostgreSQL.h"
#endif
#ifdef USEDBIXX
#include "db/Db2DbiXX.hpp"
#endif
#include "oastatstruct.h"
#include "oss2db/struct2db.h"
#include "local.h"
#include "oss2db/kill2db.h"
#include "oss2db/init2db.h"
#include "oss2db/shutdown2db.h"
#include "oss2db/userinfo2db.h"
#include "oss2db/Disconnect2Db.h"
#include "oss2db/Award2Db.h"
#include "oss2db/Ctf2Db.hpp"
#include "oss2db/Ctf1f2Db.hpp"
#include "oss2db/Point2Db.hpp"
#include "oss2db/Elimination2Db.hpp"
#include "oss2db/CtfElimination2Db.hpp"
#include "oss2db/Harvester2Db.hpp"
#include "oss2db/Challenge2Db.hpp"
#include "oss2db/Warmup2Db.hpp"

string clientIdMap[MAX_ID];

static int processStdIn(istream* in_p);

Database *db;

vector<Struct2Db*> commands;

/**
 * This function adds objects that are inherited from the Struct2Db class
 * to the vector commands.
 */
void addCommands()
{
    //Add new commands here
    commands.push_back(new Kill2Db());
    commands.push_back(new Init2Db());
    commands.push_back(new Shutdown2Db());
    commands.push_back(new Userinfo2Db());
    commands.push_back(new Disconnect2Db());
    commands.push_back(new Award2Db());
    commands.push_back(new Ctf2Db());
    commands.push_back(new Point2Db());
    commands.push_back(new Ctf1f2Db());
    commands.push_back(new Elimination2Db());
    commands.push_back(new CtfElimination2Db());
    commands.push_back(new Harvester2Db());
    commands.push_back(new Challenge2Db());
    commands.push_back(new Warmup2Db());
    //Add more commands just above here

    for(int i=0;i<commands.size();i++) {
        commands.at(i)->setDb(db);
    }
}

int main (int argc, const char* argv[])
{
    string dbargs = "";
    string filename = "";
    /////////////
    dbargs = "mysql dbname oastat username poul";
    filename = "/home/poul/.openarena/elimination/games.log";
    ////////////
    for(int i=1;i<argc;i++) {
        bool onemore = i+1<argc;
        if(string(argv[i]) == "-dbarg" && onemore ) {
            i++;
            dbargs = string(argv[i]);
        }
        if(string(argv[i]) == "-f" && onemore) {
            i++;
            filename = string(argv[i]);
        }
    }
    try{
        #if USESTDOUT
	cout << "INFO: oastat " << VERSION << " ready to process data" << endl;
        db = new DB2stdout();
        #endif

        /*#if USEPOSTGRESQL
        cout << "Using postgreSQL" << endl;
        if(dbargs.length()<1)
            db = new Db2PostgreSQL();
        else
            db = new Db2PostgreSQL(dbargs);
        #endif*/

        #if USEDBIXX
        cout << "Using DBI" << endl;
        if(dbargs.length()<1)
            db = new Db2DbiXX();
        else
            db = new Db2DbiXX(dbargs);
        #endif

        addCommands();

        if(filename.length()>0) {
            ifstream in(filename.c_str(),ifstream::in);
            processStdIn(&in);
        } else
            processStdIn(&cin);

    }catch (const char *s) {
        cout << "Crashed: " << s << endl;
        return -1;
    }

        return 0;
}

static int processStdIn(istream* in_p) {
    bool done = true;
    do
    {
        string line = "";
        OaStatStruct oss;
        list<OaStatStruct> osslist;
        try{
            while( getline(*in_p,line) )
            {
                oss.clear();
                oss.parseLine(line);
                osslist.push_back(oss);
                if(oss.command=="ShutdownGame")
                {
                    while(!osslist.empty())
                    {
                        //cout << "next: " << oss.command << endl;
                        oss = osslist.front();
                        //cout << "gotten, now popping" << endl;
                        osslist.pop_front();
                        //cout << "popping complete" << endl;
                        for(int i=0;i<commands.size();i++)
                        {
                            //try {
                            //cout << "checking " << commands.at(i)->getCommand() << endl;
                            if(commands.at(i)->canProcess(oss)) {
                                //cout << "Exectured by " << commands.at(i)->getCommand();
                                commands.at(i)->process(oss);
                            }
                            /*} catch (exception &e)
                            {
                                cerr << "oastat: Sql_error at line: \"" << line << "\"" << endl <<
                                        "oastat:   Error is: " << e.what() <<
                                        "oastat:   Last error will be ignored" << endl;
                            }*/
                        }
                        //cout << "returned" << endl;
                    }
                }
            }
        } catch (exception &e2) {
            /*
             If there is an error write it in the log and try again continue
             */
            osslist.clear(); 
            cerr << "oastat: Crashed (NEAR FATAL EXCEPTION) at line: \"" << line << "\"" << endl <<
                    "oastat:   Error is: " << e2.what() << endl;
            done = false;
        }
    } while (!done);
    return 0;
}

/**
 * Hashes the user id so they cannot be recovered from the db.
 *
 * @param unhashedID - The unhashed id
 * @return the hashed id
 */
string getHashedId(string unhashedID) {
    int msg_len = unhashedID.length();
    int hash_len = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );
    unsigned char hash_binary[ hash_len ];
    char hash_hex[ hash_len*2+1 ]; //surpriseingly this works
    char *out = hash_hex; //(char *) malloc( sizeof(char) * ((hash_len*2)+1) );
    char *p = out;

    gcry_md_hash_buffer( GCRY_MD_SHA1, hash_binary, unhashedID.c_str(), msg_len );
    for ( int i = 0; i < hash_len; i++, p += 2 ) {
        snprintf ( p, 3, "%02x", hash_binary[i] );
    }

    unhashedID = hash_hex;

    //free(out);
    return  unhashedID; //Replace with md5 at some point
}
