/* 
 * File:   db2stdout.h
 * Author: poul
 *
 * Created on 6. marts 2010, 22:08
 */

#ifndef _DB2STDOUT_H
#define	_DB2STDOUT_H

#include "database.hpp"

using namespace std;

class DB2stdout : public Database {
public:
    DB2stdout();
    ~DB2stdout();
    void createTables();
    void startGame(int gametype, string mapname, string basegame);
    void endGame(int second);
    int getGameNumber();
    void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill);
    void addKill(int second, string attackerID, string targetID, int type);
    void addCapture(int second, string player, int team);
    void addAward(int second, string player, int award);
private:
    int gamenumber;
};

#endif	/* _DB2STDOUT_H */

