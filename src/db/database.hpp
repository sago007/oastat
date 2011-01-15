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

#ifndef _DATABASE_H
#define	_DATABASE_H

#include <string>
#include <iostream>
#include "../oastatstruct.h"

using namespace std;

/*
 This is the class that all database connections need to inherit to be able to recieve data from the program
 */
class Database {
	public:
		virtual void createTables() = 0;

                /**
                 * Starts a new game in the database might also start a transaction
                 *
                 * @param gametype - integer of the gametype DM=0,CTF=4, Elim=8 and so on
                 * @param mapname - the mapname used for this game
                 * @param basegame - the mod... normally baseoa
                 */
		virtual void startGame(int gametype, string mapname, string basegame, string servername, OaStatStruct *oss) = 0;

                /**
                 * Called then a game ends.
                 *
                 * @param second - relative time since game start
                 */
                virtual void endGame(int second) = 0;

                /**
                 * Document getGameNumber() here...
                 *
                 * @return the gamenumber that we are working on. Retrieved from the db.
                 */
                virtual int getGameNumber() = 0;

                /**
                 * Document setPlayerInfo(string,string,bool,int,int,string,string,int) here...
                 *
                 * @param guid - the hashed GUID of the player
                 * @param nickname - The players nickname
                 * @param isBot - true if a bot, false otherwise
                 * @param second - relative time for the event
                 * @param team - team number to join, -1 if leaving
                 * @param model - model used
                 * @param headmodel - headmodel used
                 * @param skill - bot skill, bots only
                 */
		virtual void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, OaStatStruct *oss) = 0;

                /**
                 * Represents a kill
                 *
                 * @param second time of the kill
                 * @param attackerID hashed guid of the attacker
                 * @param targetID hashed guid of the victim
                 * @param type weapon number
                 */
		virtual void addKill(int second, string attackerID, string targetID, int type) = 0;

                /**
                 * A flag/skull capture or obelisk destruction
                 *
                 * @param second time of the event
                 * @param player guid of the player if any, empty if none
                 * @param team
                 */
                //virtual void addCapture(int second, string player, int team) = 0;

                /**
                 * Add an award: Like Defence, Excelent, Gauntlet and so on
                 *
                 * @param second time of the
                 * @param player hashed guid of the player
                 * @param award the award, numbers must be found in code
                 */
                virtual void addAward(int second, string player, int award) = 0;

                /**
                 * Adds information about a players score that is changing. 
                 * Notice that the score is the total amount of points the 
                 * player has for the entire match
                 *
                 * @param second time of the event
                 * @param player hashed guid of the player
                 * @param score the Total 
                 */
                virtual void addScoreInfo(int second, string player, int score) = 0;

                /**
                 * Adds an event from a CTF-game.
                 *
                 * Note that event:
                 * 0 | Flag is taken
                 * 1 | Flag is captured
                 * 2 | Flag is returned
                 * 3 | Flagcarrier got killed
                 *
                 * @param second
                 * @param player
                 * @param team
                 * @param event
                 */
                virtual void addCtf(int second, string player, int team, int event) = 0;

                /**
                 * Adds an event from a 1FCTF-game.
                 *
                 * Note that event:
                 * 0 | Flag is taken
                 * 1 | Flag is captured
                 * 2 | Flag is returned (like timeout)
                 * 3 | Flagcarrier got killed
                 *
                 * @param second
                 * @param player
                 * @param team
                 * @param event
                 */
                virtual void addCtf1f(int second, string player, int team, int event) = 0;

                virtual void addElimination(int second, int roundnumber, int team, int event) = 0;
};

#endif //_DATABASE_H
