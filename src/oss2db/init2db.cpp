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
    string servername = arguments["sv_hostname"];
    oss.setTimeStamp(arguments["g_timestamp"]);

    dp->startGame(gametype,mapname,basegame,servername,&oss);
}
