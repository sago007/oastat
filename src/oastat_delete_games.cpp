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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "db/DbExtra.hpp"

int main (int argc, const char* argv[])
{
	std::string connectstring;
	std::vector<int> games_to_delete;
	std::shared_ptr<cppdb::session> session;
	try {
		boost::program_options::options_description desc("Allowed options");
		desc.add_options()
		("help,h", "Print basic usage information to stdout and quits")
		("dbargs", boost::program_options::value<std::string>(), "Arguments passed to the DB backend")
		("delete-game,d", boost::program_options::value<std::vector<int>>(), "Delete game with a given number from the database")
		("config,c", boost::program_options::value<std::vector<std::string> >(), "Read a config file with the values. Can be given multiple times")
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
			return 1;
		}
		if (vm.count("dbargs")) {
			connectstring = vm["dbargs"].as<std::string>();
		}
		if (vm.count("delete-game")) {
			games_to_delete = vm["delete-game"].as<std::vector<int>>();
		}

		session = std::make_shared<cppdb::session>(connectstring);
		cppdb::transaction(*session.get());
		for (int game : games_to_delete) {
			std::cout << "Deleting " << game << "..." << std::flush;
			deleteGame(session.get(), game);
			std::cout << "done!\n";
		}
		session->commit();

	} catch (std::exception& e) {
		std::cerr << "Failed!\n" << e.what() << "\n";
		return 1;
	}
	return 0;
}
