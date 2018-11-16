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

/*
 * This file generates a file
 *
 */

#ifndef DB2XML_HPP
#define	DB2XML_HPP

#include "database.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

class Db2Xml : public Database
{
public:
	Db2Xml();
	explicit Db2Xml(std::string dbargs);
	Db2Xml(const Db2Xml& orig) = delete;
	Db2Xml& operator=(const Db2Xml&) = delete;
	virtual ~Db2Xml();
	void createTables() override;
	void startGame(int gametype, const std::string &mapname, const std::string &basegame, const std::string &servername, const OaStatStruct &oss) override;
	void addGameCvar(const std::string &cvar, const std::string &value) override;
	void endGame(int second) override;
	int getGameNumber() override;
	void setPlayerInfo(const std::string &guid, const std::string &nickname, bool isBot, 
		int second, int team, const std::string &model, const std::string &headmodel, int skill) override;
	void addKill(int second, const std::string &attackerID, const std::string &targetID, int type) override;
	//void addCapture(int second, string player, int team);
	void addAward(int second, const std::string &player, int award) override;
	void addScoreInfo(int second, const std::string &player, int score) override;
	void addCtf(int second, const std::string &player, int team, int event) override;
	void addCtf1f(int second, const std::string &player, int team, int event) override;
	void addElimination(int second, int roundnumber, int team, int event) override;
	void addCtfElimination(int second, int roundnumber, const std::string &player, int team, int event) override;
	void addHarvester(int second, const std::string &player1, const std::string &player2, int team, int event, int score) override;
	void addChallenge(int second, const std::string &player, int challenge, int amount) override;
	void addAccuracy(int second, const std::string &player, int type, int shotsFired, int shotsHit) override;
	void addGenericTeamEvent(int second, int team, int amount, const std::string &gametype, const std::string &player1, const std::string &player2, int event, int generic1) override;
	void doNotCommit() override;
private:
	std::string p_output_dir;
	std::string p_servername; //Used for filename
	tm p_gametime; //Used for filename
	std::string p_xmlcontent;
	std::string p_postscript;
	bool isOk;

	/*
	 * To make it easier to parse the XML file oastat will generate a unique id for each game
	 * This is used then converting back to a log file
	 */
	std::map<std::string,int> psoudo_playerids;
	int nextId;

	int getPsoudoId(const std::string &guid);
	std::string getXmlEscaped(const std::string &org);
};

#endif	/* DB2XML_HPP */

