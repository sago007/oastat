/*
===========================================================================
oastat - a program for parsing log files and write the result to a database
Copyright (C) 2014 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/oastat/
===========================================================================
*/

#include "DbExtra.hpp"

void deleteGame(cppdb::session* connection, int gamenumber) {
	const char* deleteAccuracy = "delete from oastat_accuracy where gamenumber = ?";
	const char* deleteAwards = "delete from oastat_awards where gamenumber = ?";
	const char* deleteChallenges = "delete from oastat_challenges where gamenumber = ?";
	const char* deleteGameCvars = "delete from oastat_gamecvars where gamenumber = ?";
	const char* deleteKills = "delete from oastat_kills where gamenumber = ?";
	const char* deletePoints = "delete from oastat_points where gamenumber = ?";
	const char* deleteTeamEvents = "delete from oastat_team_events where gamenumber = ?";

	const char* deleteUserInfoVars = "delete from oastat_uservars where userinfoevent in (select eventNumber from oastat_userinfo where gamenumber = ?)";
	const char* deleteUserInfo = "delete from oastat_userinfo where gamenumber = ?";

	const char* deleteGame = "delete from oastat_games where gamenumber = ?";

	*connection << deleteAccuracy << gamenumber << cppdb::exec;
	*connection << deleteAwards << gamenumber << cppdb::exec;
	*connection << deleteChallenges << gamenumber << cppdb::exec;
	*connection << deleteGameCvars << gamenumber << cppdb::exec;
	*connection << deleteKills << gamenumber << cppdb::exec;
	*connection << deletePoints << gamenumber << cppdb::exec;
	*connection << deleteTeamEvents << gamenumber << cppdb::exec;
	*connection << deleteUserInfoVars << gamenumber << cppdb::exec;
	*connection << deleteUserInfo << gamenumber << cppdb::exec;
	*connection << deleteGame << gamenumber << cppdb::exec;
}
