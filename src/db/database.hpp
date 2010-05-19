#ifndef _DATABASE_H
#define	_DATABASE_H

#include <string>
#include <iostream>

using namespace std;

/*
 This is the class that all database connections need to inherit to be able to recieve data from the program
 */
class Database {
	public:
		virtual void createTables() = 0;

		virtual void startGame(int gametype, string mapname, string basegame) = 0;
                
                virtual void endGame(int second) = 0;

                virtual int getGameNumber() = 0;

		virtual void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill = -1) = 0;

		virtual void addKill(int second, string attackerID, string targetID, int type) = 0;

                virtual void addCapture(int second, string player, int team) = 0;

                virtual void addAward(int second, string player, int award) = 0;
};

#endif //_DATABASE_H
