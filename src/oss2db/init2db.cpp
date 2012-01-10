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

#include "init2db.h"
#include <stdlib.h>


string Init2Db::getCommand() {
    return "InitGame";
}

bool Init2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand())
        return false;
    return true;
}

void Init2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss
    map<string,string> arguments = oss.GetInfostring();
    int gametype = atoi(arguments["g_gametype"].c_str());
    string basegame = arguments["gamename"];
    string mapname = arguments["mapname"];
    string servername = arguments["sv_hostname"];
    oss.setTimeStamp(arguments["g_timestamp"]);

    dp->startGame(gametype,mapname,basegame,servername,&oss);
    map<string,string>::iterator it;
    for(it = arguments.begin(); it != arguments.end(); it++) {
        if(
                it->first == "g_gametype" ||
                it->first == "gamename" ||
                it->first == "mapname" ||
                it->first == "sv_hostname" /*||
                it->first == "g_timestamp" do not skip this one*/
          )
               continue; //Skip the ones on the games-table
        dp->addGameCvar(it->first,it->second);
    }
}
