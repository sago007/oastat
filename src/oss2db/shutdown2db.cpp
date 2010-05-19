/* 
 * File:   shutdown2db.cpp
 * Author: poul
 * 
 * Created on 16. april 2010, 22:54
 */

#include "shutdown2db.h"


Shutdown2Db::Shutdown2Db() {
}

Shutdown2Db::Shutdown2Db(const Shutdown2Db& orig) {
}

Shutdown2Db::~Shutdown2Db() {
}

string Shutdown2Db::getCommand() {
    return "ShutdownGame";
}

void Shutdown2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand())
        return;
    dp->endGame(oss.second);
}