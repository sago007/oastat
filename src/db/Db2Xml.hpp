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
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>

class Db2Xml : public Database
{
public:
	Db2Xml();
	Db2Xml(string dbargs);
	Db2Xml(const Db2Xml& orig);
	virtual ~Db2Xml();
	void createTables();
	void startGame(int gametype, string mapname, string basegame, string servername, const OaStatStruct *oss);
	void addGameCvar(string cvar, string value);
	void endGame(int second);
	int getGameNumber();
	void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, const OaStatStruct &oss);
	void addKill(int second, string attackerID, string targetID, int type);
	//void addCapture(int second, string player, int team);
	void addAward(int second, string player, int award);
	void addScoreInfo(int second, string player, int score);
	void addCtf(int second, string player, int team, int event);
	void addCtf1f(int second, string player, int team, int event);
	void addElimination(int second, int roundnumber, int team, int event);
	void addCtfElimination(int second, int roundnumber, string player, int team, int event);
	void addHarvester(int second, string player1, string player2, int team, int event, int score);
	void addChallenge(int second, string player, int challenge, int amount);
	void addAccuracy(int secind, string player, int type, int shotsFired, int shotsHit);
	void doNotCommit();
private:
	string p_output_dir;
	string p_servername; //Used for filename
	tm p_gametime; //Used for filename
	string p_xmlcontent;
	string p_postscript;
	bool isOk;

	/*
	 * To make it easier to parse the XML file oastat will generate a unique id for each game
	 * This is used then converting back to a log file
	 */
	map<string,int> psoudo_playerids;
	int nextId;

	int getPsoudoId(string guid);
	string getXmlEscaped(string org);
};

#endif	/* DB2XML_HPP */

