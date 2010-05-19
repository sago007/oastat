
#include <vector>

#include "kill2db.h"

string Kill2Db::getCommand() {
    return "Kill";
}

void Kill2Db::process(OaStatStruct oss) {
    if(oss.command != "Kill" || oss.parameters.size()<=3)
        return; //Invalid oss
    string target = ""; //Parameter 1
    string killer = ""; //Parameter 0
    //mod is parameter 2
    if(oss.parameters.at(1)>-1 && oss.parameters.at(1)<MAX_ID) {
        target = clientIdMap[oss.parameters.at(1)];
    } else
        target ="WORLD"; //Should not be possible... except maybe for NPC-creatures

    if(oss.parameters.at(0)>-1 && oss.parameters.at(0)<MAX_ID) {
        killer = clientIdMap[oss.parameters.at(0)];
    } else
        killer ="WORLD";

    dp->addKill(oss.second,killer,target,oss.parameters.at(2));
}
