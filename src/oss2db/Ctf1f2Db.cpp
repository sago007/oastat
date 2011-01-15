/* 
 * File:   Ctf1f2Db.cpp
 * Author: poul
 * 
 * Created on 11. januar 2011, 20:48
 */

#include "Ctf1f2Db.hpp"

Ctf1f2Db::Ctf1f2Db() {
}

Ctf1f2Db::Ctf1f2Db(const Ctf1f2Db& orig) {
}

Ctf1f2Db::~Ctf1f2Db() {
}

string Ctf1f2Db::getCommand() {
    return "1FCTF";
}

bool Ctf1f2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3 || oss.parameters.at(0)<0)
        return false;
    return true;
}

void Ctf1f2Db::process(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<3 || oss.parameters.at(0)<0)
        return; //Invalid oss

    string player = "";
    if(oss.parameters.at(0)!=-1)
        player= clientIdMap[oss.parameters.at(0)];

    dp->addCtf1f(oss.second,player,oss.parameters.at(1) /*team*/, oss.parameters.at(2) /*event*/);
}
