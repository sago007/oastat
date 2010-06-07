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

#define VERSION "0.1 BETA"

#include <iostream>
#include <vector>
#include <gcrypt.h>
#include <stdio.h>


using namespace std;

#include "db/database.hpp"
#ifdef USESTDOUT
#include "db/db2stdout.h"
#endif
#ifdef USEPOSTGRESQL
#include "db/Db2PostgreSQL.h"
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

string clientIdMap[MAX_ID];

static void processStdIn();

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
    //Add more commands just above here

    for(int i=0;i<commands.size();i++) {
        commands.at(i)->setDb(db);
    }
}

int main (int argc, const char* argv[])
{
    string dbargs = "";
    for(int i=1;i<argc;i++) {
        bool onemore = i+1<argc;
        if(string(argv[i]) == "-dbarg" && onemore ) {
            i++;
            dbargs = string(argv[i]);
        }
    }
    try{
        #if USESTDOUT
	cout << "INFO: oastat " << VERSION << " ready to process data" << endl;
        db = new DB2stdout();
        #endif

        #if USEPOSTGRESQL
        if(dbargs.length()<1)
            db = new Db2PostgreSQL();
        else
            db = new Db2PostgreSQL(dbargs);
        #endif

        addCommands();

        processStdIn();

    }catch (const char *s) {
        cout << "Crashed: " << s << endl;
    }

        return 0;
        //Rest of file is for testing

        string line = "  3:33 Kill: 2 5 6: aaargh!!!!!!!!!!!!!! killed RALV by MOD_ROCKET";

        OaStatStruct oss;
        oss.parseLine(line);

        cout << "Time: '" << oss.second << "' command: '" << oss.command << "' restOfLine: '" << oss.restOfLine << "'" << endl;

        for (int i=0; i<oss.parameters.size(); i++) {
            cout << i << ": " << oss.parameters.at(i) << endl;
        }

        Kill2Db k2db;
        Struct2Db *s2dp = &k2db;

        s2dp->setDb(db);

        s2dp->process(oss);

        db->addKill(10, "DSAGOJSAOGFJWA55555555AGA", "AHFDSHDFBSDTRHSJYFDSGFA", 3);

	return 0;
}

static void processStdIn() {
    string line;
    while( getline(cin,line) ) {
        OaStatStruct oss;
        oss.parseLine(line);
        for(int i=0;i<commands.size();i++) {
            commands.at(i)->process(oss);
        }
    }
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

    //free(out);
    return  unhashedID; //Replace with md5 at some point
}
