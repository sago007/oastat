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
https://github.com/sago007/oastat/
===========================================================================
*/

#define VERSION "0.2 BETA"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <deque>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "db/database.hpp"
#include "db/Db2CppDb.hpp"
#include "db/Db2Xml.hpp"

#include "common/oastatstruct.h"
#include "oss2db/struct2db.h"
#include "common/local.h"
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





/**
 * This function adds objects that are inherited from the Struct2Db class
 * to the vector commands.
 *
 * @param[in] db The database object. May not be freed once given as an argument to this function
 */
static void addCommands(std::shared_ptr<Database> &db,std::vector<std::shared_ptr<Struct2Db> > &commands)
{
	if (!db) {
		throw std::runtime_error("db was uninizialized in addCommands");
	}

	//Add new commands here
	commands.push_back(std::shared_ptr<Struct2Db>(new Kill2Db() ) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Init2Db() ) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Shutdown2Db() ) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Userinfo2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Disconnect2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Award2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Ctf2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Point2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Ctf1f2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Elimination2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new CtfElimination2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Harvester2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Challenge2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Warmup2Db()) );
	commands.push_back(std::shared_ptr<Struct2Db>(new Accuracy2Db()) );
	//Add more commands just above here

	for (size_t i=0; i<commands.size(); i++) {
		commands.at(i)->setDb(db);
	}
}


class OastatStreamProcessor {
	std::deque<OaStatStruct> osslist;

public:

	int processStdIn(std::istream &in_p, std::vector<std::shared_ptr<Struct2Db> > &commands)
	{
		bool done = false;
		OaStatStruct *startstruct = nullptr;
		while(!done) {
			done = true;
			OaStatStruct oss;
			std::string line = "";
			try {
				while ( std::getline(in_p,line) ) {
					oss.clear();
					oss.parseLine(line);
					osslist.push_back(oss);
					if (oss.command=="InitGame") {
						startstruct = &osslist.back();
					}
					if (oss.command=="Warmup" && startstruct) {
						//Workaround to stop warmup
						//If we spot a warmup command we add a cvar to the start struct
						//Warmup is a attribute that affect he whole game
						startstruct->restOfLine += "\\isWarmup\\1";
					}
					if (oss.command=="ShutdownGame") {
						while(!osslist.empty()) {
							oss = osslist.front();
							osslist.pop_front();
							for(size_t i=0; i<commands.size(); ++i) {
								if(commands.at(i)->canProcess(oss)) {
									commands.at(i)->process(oss);
								}
							}
						}
						startstruct = NULL;
					}
				}
			} catch (std::exception &e2) {
				/*
				If there is an error write it in the log and try again continue
				*/
				startstruct = nullptr;
				osslist.clear();
				std::cerr << "oastat: Crashed (NEAR FATAL EXCEPTION) at line: \"" << line << "\"\n"<<
					"oastat:   Error is: " << e2.what() << "\n";
				done = false;
			}
		}
		return 0;
	}
};


int main (int argc, const char* argv[])
{
	try {
		std::ios_base::sync_with_stdio(false);
		std::string dbargs = "";
		std::string backend = "Xml";
		bool useTail = false;
		bool doIntegrationTest = false;
		std::vector<std::shared_ptr<Struct2Db> > commands;
		/////////////
		boost::format f("%1%/.openarena/baseoa/games.log");
		f % getenv("HOME");
		std::string filename = f.str();
		////////////
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
		("help,h", "Print basic usage information to stdout and quits")
		("backend", boost::program_options::value<std::string>(), "The DB backend to use")
		("dbargs", boost::program_options::value<std::string>(), "Arguments passed to the DB backend")
		("filename,f", boost::program_options::value<std::string>(), "Filename to read. Providing a blank string will read from stdin")
		("config,c", boost::program_options::value<std::vector<std::string> >(), "Read a config file with the values. Can be given multiple times")
		("tail", "Continue to minitor the file given by the filename")
		("integration-test", "Perform integration test")
		;
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
		if (vm.count("config")) {
			std::vector<std::string> config_filenames = vm["config"].as<std::vector<std::string> >();
			for ( const std::string& s : config_filenames) {
				std::ifstream config_file(s);
				store(parse_config_file(config_file, desc), vm);
				notify(vm);
			}
		}
		if (vm.count("help")) {
			std::cout << desc << "\n";
			std::cout << "Examples: \n";
			std::cout << argv[0] << " -f \""<< f << "/.openarena/baseoa/games.log\" --backend \"CppDb\" --dbarg \"mysql:database=oastat;user=openarena\"\n";
			std::cout << argv[0] << " -f \"" << f << "/.openarena/baseoa/games.log\" --backend \"CppDb\" --dbarg \"pgsql:database=oastat;username=openarena\"\n";
			std::cout << "cat \"~/.openarena/baseoa/games.log\" |" << argv[0] << " -f \"\" --backend \"CppDb\" --dbarg \"pgsql:database=oastat;username=openarena\"\n";
			std::cout << argv[0] << " -f %APPDATA%/OpenArena/baseoa/games.log --backend \"CppDb\" --dbarg \"sqlite3:db=defoastat.db3\"\n";
			std::cout << "\n";
			std::cout << "CppDb backend options:\n";
			std::cout << "If the environment variable " << OASTAT_CPPDB_IGNORE_MISSING_TIMESTAMP << " is set and g_gametime is missing then oastat will use current time as timestamp\n";
			std::cout << "Deplicate elimination will not work with " << OASTAT_CPPDB_IGNORE_MISSING_TIMESTAMP << " set\n";
			std::cout << "\nLook at https://github.com/sago007/oastat for more help and more details\n";
			return 1;
		}
		if (vm.count("backend")) {
			backend = vm["backend"].as<std::string>();
		}
		if (vm.count("dbargs")) {
			dbargs = vm["dbargs"].as<std::string>();
		}
		if (vm.count("filename")) {
			filename = vm["filename"].as<std::string>();
		}
		if (vm.count("tail")) {
			useTail = true;
		}
		if (vm.count("integration-test")) {
			doIntegrationTest = true;
		}
		std::shared_ptr<Database> db;

		if (backend == "CppDb") {
			std::cout << "Using CppDb\n";
			if (dbargs.length()<1) {
				db = std::shared_ptr<Database>(new Db2CppDb() );
			} else {
				db = std::shared_ptr<Database>(new Db2CppDb(dbargs) );
			}
		}
		if (backend == "Xml") {
			std::cout << "Using XML\n";
			if (dbargs.length()<1) {
				db = std::shared_ptr<Database>(new Db2Xml() );
			} else {
				db = std::shared_ptr<Database>(new Db2Xml(dbargs) );
			}
		}

		if (!db) {
			std::string error("Failed to find backend: ");
			error += backend;
			throw std::runtime_error(error);
		}

		addCommands(db,commands);

		if (doIntegrationTest) {
			OaStatStruct oss_test;
			oss_test.setTimeStamp("2013-12-08 20:42:30");
			db->startGame(1,"oasago2","baseoa-mod","testserver",oss_test);
			db->addGenericTeamEvent(5,2,0,"sometype","","",3,4);
			std::cerr << "Test called\n";
			return 0;
		}

		OastatStreamProcessor stream_processor;
		if (filename.length()>0) {
			if (useTail) {
				size_t last_position=0;
				while (true) {
					std::ifstream in(filename.c_str(), std::ifstream::in);
					in.seekg(0, std::ios::end);
					size_t filesize = in.tellg();
					if (filesize < last_position) {
						last_position = 0;
					}
					in.seekg( last_position, std::ios::beg);
					stream_processor.processStdIn(in, commands);
					in.clear();  // processStdIn leves in a bad state. tellg will not work unless we clear it
					last_position = in.tellg();
					sleep(2);
				}
			}
			else {
				std::ifstream in(filename.c_str(), std::ifstream::in);
				stream_processor.processStdIn(in,commands);
			}
		} else {
			stream_processor.processStdIn(std::cin, commands);
		}

	} catch (std::exception &e) {
		std::cerr << "Crashed: " << e.what() << "\n";
		return 2;
	}

	return 0;
}
