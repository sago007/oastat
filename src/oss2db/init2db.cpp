/* 
 * File:   init2db.cpp
 * Author: poul
 * 
 * Created on 16. april 2010, 22:19
 */

#include "init2db.h"
#include <stdlib.h>

Init2Db::Init2Db() {
}

Init2Db::Init2Db(const Init2Db& orig) {
}

Init2Db::~Init2Db() {
}

string Init2Db::getCommand() {
    return "InitGame";
}

void Init2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand())
        return; //Invalid oss
    map<string,string> arguments = oss.GetInfostring();
    int gametype = atoi(arguments["g_gametype"].c_str());
    string basegame = arguments["gamename"];
    string mapname = arguments["mapname"];

    dp->startGame(gametype,mapname,basegame);
}
