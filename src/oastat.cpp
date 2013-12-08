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

#define VERSION "0.2 BETA"

#include <iostream>
#include <fstream>
#include "pstream.h" //<pstreams/pstream.h>
#include <vector>
#include <gcrypt.h>
#include <stdio.h>
#include <list>
#include <boost/shared_ptr.hpp>


using namespace std;
using namespace boost;

#include "db/database.hpp"
#ifdef USEDBIXX
#include "db/Db2DbiXX.hpp"
#endif

#include "db/Db2Xml.hpp"

#include "oastatstruct.h"
#include "oss2db/struct2db.h"
#include "local.h"
#include "oss2db/kill2db.h"
#include "oss2db/init2db.h"
#include "oss2db/shutdown2db.h"
#include "oss2db/userinfo2db.h"
#include "oss2db/Disconnect2Db.h"
#include "oss2db/Accuracy2Db.hpp"
#include "oss2db/Award2Db.h"
#include "oss2db/Ctf2Db.hpp"
#include "oss2db/Ctf1f2Db.hpp"
#include "oss2db/Point2Db.hpp"
#include "oss2db/Elimination2Db.hpp"
#include "oss2db/CtfElimination2Db.hpp"
#include "oss2db/Harvester2Db.hpp"
#include "oss2db/Challenge2Db.hpp"
#include "oss2db/Warmup2Db.hpp"

vector<string> clientIdMap;

static int processStdIn(istream &in_p,vector<shared_ptr<Struct2Db> > &commands);



/**
 * This function adds objects that are inherited from the Struct2Db class
 * to the vector commands.
 *
 * @param[in] db The database object. May not be freed once given as an argument to this function
 */
static void addCommands(shared_ptr<Database> &db,vector<shared_ptr<Struct2Db> > &commands)
{
	if(!db) {
		throw runtime_error("db was uninizialized in addCommands");
	}

	//Add new commands here
	commands.push_back(shared_ptr<Struct2Db>(new Kill2Db() ) );
	commands.push_back(shared_ptr<Struct2Db>(new Init2Db() ) );
	commands.push_back(shared_ptr<Struct2Db>(new Shutdown2Db() ) );
	commands.push_back(shared_ptr<Struct2Db>(new Userinfo2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Disconnect2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Award2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Ctf2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Point2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Ctf1f2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Elimination2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new CtfElimination2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Harvester2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Challenge2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Warmup2Db()) );
	commands.push_back(shared_ptr<Struct2Db>(new Accuracy2Db()) );
	//Add more commands just above here

	for(unsigned int i=0; i<commands.size(); i++) {
		commands.at(i)->setDb(db);
	}
}



static int processStdIn(istream &in_p,vector<shared_ptr<Struct2Db> > &commands)
{
	bool done = true;
	OaStatStruct *startstruct;
	startstruct = NULL;
	do {
		string line = "";
		OaStatStruct oss;
		list<OaStatStruct> osslist;
		try {
			while( getline(in_p,line) ) {
				oss.clear();
				oss.parseLine(line);
				osslist.push_back(oss);
				if(oss.command=="InitGame") {
					startstruct = &osslist.back();
				}
				if(oss.command=="Warmup" && startstruct) {
					//Workaround to stop warmup
					//If we spot a warmup command we add a cvar to the start struct
					//Warmup is a attribute that affect he whole game
					startstruct->restOfLine += "\\isWarmup\\1";
				}
				if(oss.command=="ShutdownGame") {
					while(!osslist.empty()) {
						oss = osslist.front();
						osslist.pop_front();
						for(unsigned int i=0; i<commands.size(); i++) {
							//try {
							//cout << "checking " << commands.at(i)->getCommand() << endl;
							if(commands.at(i)->canProcess(oss)) {
								//cout << "Execturedg by " << commands.at(i)->getCommand();
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
					startstruct = NULL;
				}
			}
		} catch (std::exception &e2) {
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
string getHashedId(string unhashedID)
{
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

int main (int argc, const char* argv[])
{
	string dbargs = "";
	string filename = "";
	string backend = "Xml";
	bool useTail = false;
	bool doIntegrationTest = false;
	vector<shared_ptr<Struct2Db> > commands;
	/////////////
	//dbargs = "mysql dbname oastat";
	boost::format f("%1%/.openarena/baseoa/games.log");
	f % getenv("HOME");
	filename = f.str();
	////////////
	for(int i=1; i<argc; i++) {
		bool onemore = i+1<argc;
		if (string(argv[i]) == "-dbarg" && onemore ) {
			i++;
			dbargs = string(argv[i]);
		}
		if (string(argv[i]) == "-f" && onemore) {
			i++;
			filename = string(argv[i]);
		}
		if (string(argv[i]) == "-tail") {
			useTail = true;
		}
		if (string(argv[i]) == "-backend" && onemore) {
			i++;
			backend = string(argv[i]);
		}
		if (string(argv[i]) == "--integration-test") {
			doIntegrationTest = true;
		}
	}
	try {
		shared_ptr<Database> db;

#if USEDBIXX
		if(backend == "DbiXX") {
			cout << "Using DBI" << endl;
			if(dbargs.length()<1) {
				db = shared_ptr<Database>(new Db2DbiXX() );
			} else {
				db = shared_ptr<Database>(new Db2DbiXX(dbargs) );
			}
		}
#endif
		if(backend == "Xml") {
			cout << "Using XML" << endl;
			if(dbargs.length()<1) {
				db = shared_ptr<Database>(new Db2Xml() );
			} else {
				db = shared_ptr<Database>(new Db2Xml(dbargs) );
			}
		}

		if(!db) {
			string error("Failed to find backend: ");
			error += backend;
			throw runtime_error(error);
		}

		addCommands(db,commands);

		if (doIntegrationTest) {
			OaStatStruct oss_test;
			oss_test.setTimeStamp("2013-12-08 20:42:30");
			db->startGame(1,"oasago2","baseoa-mod","testserver",oss_test);
			db->addGenericTeamEvent(5,2,0,"sometype","","",3,4);
			cerr << "Test called" << endl;
			return 1;
		}

		if(filename.length()>0) {
			if(useTail) {
				redi::ipstream in("tail -s 1 -f "+filename);
				processStdIn(in,commands);
			} else {
				ifstream in(filename.c_str(),ifstream::in);
				processStdIn(in,commands);
			}
		} else {
			processStdIn(cin,commands);
		}

	} catch (std::exception &e) {
		cout << "Crashed: " << e.what() << endl;
		return 2;
	}

	return 0;
}
