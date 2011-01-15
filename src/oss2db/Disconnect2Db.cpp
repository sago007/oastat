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

#include "Disconnect2Db.h"


string Disconnect2Db::getCommand() {
    return "ClientDisconnect";
}

bool Disconnect2Db::canProcess(OaStatStruct oss) {
    if(oss.command != getCommand() || oss.parameters.size()<1)
        return false;
    return true;
}

void Disconnect2Db::process(OaStatStruct oss) {
    if(!canProcess(oss))
        return; //Invalid oss

    string player = clientIdMap[oss.parameters.at(0)];

    dp->setPlayerInfo(player,"",false,oss.second,-1,"","",-1,&oss);
}
