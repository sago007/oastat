/* 
 * File:   Point2Db.cpp
 * Author: poul
 * 
 * Created on 10. januar 2011, 21:32
 */

#include "Point2Db.hpp"

Point2Db::Point2Db() {
}

Point2Db::Point2Db(const Point2Db& orig) {
}

Point2Db::~Point2Db() {
}

string Point2Db::getCommand() {
    return "PlayerScore";
}

bool Point2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<2 || oss.parameters.at(0)<0)
        return false;
    return true;
}

void Point2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    string player = clientIdMap[oss.parameters.at(0)];

    dp->addScoreInfo(oss.second,player,oss.parameters.at(1));
}