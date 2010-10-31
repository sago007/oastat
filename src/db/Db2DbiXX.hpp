/* 
 * File:   Db2DbiXX.hpp
 * Author: poul
 *
 * Created on 11. oktober 2010, 20:59
 */

#ifndef _DB2DBIXX_HPP
#define	_DB2DBIXX_HPP

#include "database.hpp"

#include <dbixx/dbixx.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace dbixx;

class Db2DbiXX  : public Database {
public:
    Db2DbiXX();
    Db2DbiXX(string dbargs);
    Db2DbiXX(const Db2DbiXX& orig);
    virtual ~Db2DbiXX();
    void createTables();
    void startGame(int gametype, string mapname, string basegame);
    void endGame(int second);
    int getGameNumber();
    void setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill);
    void addKill(int second, string attackerID, string targetID, int type);
    void addCapture(int second, string player, int team);
    void addAward(int second, string player, int award);
private:
    session *sql;
    transaction *commitlock;
    bool isok;
    bool debug;
    int gamenumber;
    int getNextGameNumber();
    void Commit();
    void Rollback();
    bool Ok();
    void SetOk(bool ok);
    void DebugMessage(string msg);
};

#endif	/* _DB2DBIXX_HPP */

