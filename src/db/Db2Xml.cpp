/* 
 * File:   Db2Xml.cpp
 * Author: poul
 * 
 * Created on 3. december 2011, 17:30
 */

#include "Db2Xml.hpp"


Db2Xml::Db2Xml() 
{
	p_output_dir = "~/oastat";
	p_postscript = "";
	psoudo_playerids.clear();
	nextId = 1;
	//mkdir(p_output_dir.c_str());
}
Db2Xml::Db2Xml(string dbargs)
{
	p_output_dir = "~/oastat";
	p_postscript = "";
	psoudo_playerids.clear();
	nextId = 1;
	//mkdir(p_output_dir.c_str());
	
	stringstream stream(stringstream::in | stringstream::out);
	stream << dbargs;
	string holder;
	//stream >> holder;
	while(!stream.eof()) {
		stream >> holder;
		if(!stream.eof()) {
			string param;
			stream >> param;
			if(holder == "outputdir")
				p_output_dir = param;
			if(holder == "postscript")
				p_postscript = param;
		}
	}	
}
Db2Xml::Db2Xml(const Db2Xml& orig)
{

}
Db2Xml::~Db2Xml()
{
    
}
void Db2Xml::startGame(int gametype, string mapname, string basegame, string servername, OaStatStruct *oss)
{
	isOk = true;
	p_xmlcontent = "";
	p_gametime = oss->getDateTime();
	p_servername = servername;
	p_xmlcontent += "<OaStatGame>\n";
	p_xmlcontent += (boost::format("    <servername>%1%</servername>\n") % getXmlEscaped(p_servername) ).str();
	p_xmlcontent += (boost::format("    <gametype>%1%</gametype>\n") % gametype).str();
	p_xmlcontent += (boost::format("    <mapname>%1%</mapname>\n") % getXmlEscaped(mapname) ).str();
	p_xmlcontent += (boost::format("    <basegame>%1%</basegame>\n") % getXmlEscaped(basegame) ).str();
	p_xmlcontent += (boost::format("    <gametime>%04i-%02i-%02iT%02i:%02i:%02i</gametime>\n") 
			% (p_gametime.tm_year+1900) % (p_gametime.tm_mon+1) % (p_gametime.tm_mday)
			% (p_gametime.tm_hour) % (p_gametime.tm_min) % (p_gametime.tm_sec) ).str();
}
void Db2Xml::addGameCvar(string cvar, string value)
{
	if(!isOk)
		return;
	p_xmlcontent += "    <cvar>\n";
	p_xmlcontent += (boost::format("        <key>%1%</key>\n") % getXmlEscaped(cvar) ).str();
	p_xmlcontent += (boost::format("        <value>%1%</value>\n") % getXmlEscaped(value) ).str();
	p_xmlcontent += "    </cvar>\n";
}
void Db2Xml::endGame(int second)
{
	if(isOk) {
		p_xmlcontent += (boost::format("    <second>%1%</second>\n") % second).str();
		p_xmlcontent += "</OaStatGame>\n";
		string filename = (boost::format("%1%/%2%_%3%-%4%-%5%_%6%-%7%-%8%.xml") % p_output_dir % p_servername
			% (p_gametime.tm_year+1900) % (p_gametime.tm_mon+1) % (p_gametime.tm_mday)
			% (p_gametime.tm_hour) % (p_gametime.tm_min) % (p_gametime.tm_sec) ).str();
		ofstream outfile;
		outfile.open (filename.c_str());
		outfile << p_xmlcontent;
		outfile.close();
		cout << "End game at " << second << "with size " << p_xmlcontent.length() << " written to " << filename << endl;
	}
}
int Db2Xml::getGameNumber()
{
	return 1;
}
void Db2Xml::setPlayerInfo(string guid, string nickname, bool isBot, int second, int team, string model, string headmodel, int skill, OaStatStruct *oss)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <playerinfo>\n";
	p_xmlcontent += (boost::format("        <hashedguid>%1%</hashedguid>\n") % guid).str();
	p_xmlcontent += (boost::format("        <playerid>%1%</playerid>\n") % getPsoudoId(guid)).str();
	p_xmlcontent += (boost::format("        <name>%1%</name>\n") % getXmlEscaped(nickname) ).str();
	p_xmlcontent += (boost::format("        <isbot>%1%</isbot>\n") % (isBot? 1:0)).str();
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <model>%1%</model>\n") % getXmlEscaped(model) ).str();
	p_xmlcontent += (boost::format("        <headmodel>%1%</headmodel>\n") % getXmlEscaped(headmodel) ).str();
	p_xmlcontent += (boost::format("        <skill>%1%</skill>\n") % skill).str();
	p_xmlcontent += "    </playerinfo>\n";
}
void Db2Xml::addKill(int second, string attackerID, string targetID, int type)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <kill>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <attacker>%1%</attacker>\n") % attackerID).str();
	p_xmlcontent += (boost::format("        <target>%1%</target>\n") % targetID).str();
	p_xmlcontent += (boost::format("        <mod_type>%1%</mod_type>\n") % type).str();
	p_xmlcontent += "    </kill>\n";
}
//void addCapture(int second, string player, int team);
void Db2Xml::addAward(int second, string player, int award)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <award>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <award>%1%</award>\n") % second).str();
	p_xmlcontent += "    </award>\n";
}
void Db2Xml::addScoreInfo(int second, string player, int score)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <score>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <score>%1%</score>\n") % score).str();
	p_xmlcontent += "    </score>\n";
}
void Db2Xml::addCtf(int second, string player, int team, int event)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <ctf>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <event>%1%</event>\n") % event).str();
	p_xmlcontent += "    </ctf>\n";
}
void Db2Xml::addCtf1f(int second, string player, int team, int event)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <ctf1flag>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <event>%1%</event>\n") % event).str();
	p_xmlcontent += "    </ctf1flag>\n";
}
void Db2Xml::addElimination(int second, int roundnumber, int team, int event)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <elimination>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <roundnumber>%1%</roundnumber>\n") % roundnumber).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <event>%1%</event>\n") % event).str();
	p_xmlcontent += "    </elimination>\n";
}
void Db2Xml::addCtfElimination(int second, int roundnumber, string player, int team, int event)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <ctfelimination>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <roundnumber>%1%</roundnumber>\n") % roundnumber).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <event>%1%</event>\n") % event).str();
	p_xmlcontent += "    </ctfelimination>\n";    
}
void Db2Xml::addHarvester(int second, string player1, string player2, int team, int event, int score)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <harvester>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player1>%1%</player1>\n") % player1).str();
	p_xmlcontent += (boost::format("        <player2>%1%</player2>\n") % player2).str();
	p_xmlcontent += (boost::format("        <team>%1%</team>\n") % team).str();
	p_xmlcontent += (boost::format("        <event>%1%</event>\n") % event).str();
	p_xmlcontent += (boost::format("        <score>%1%</score>\n") % score).str();
	p_xmlcontent += "    </harvester>\n";   
}
void Db2Xml::addChallenge(int second, string player, int challenge, int amount)
{
    if(!isOk)
		return;
	p_xmlcontent += "    <challenge>\n";
	p_xmlcontent += (boost::format("        <second>%1%</second>\n") % second).str();
	p_xmlcontent += (boost::format("        <player>%1%</player>\n") % player).str();
	p_xmlcontent += (boost::format("        <challenge>%1%</challenge>\n") % challenge).str();
	p_xmlcontent += (boost::format("        <amount>%1%</amount>\n") % amount).str();
	p_xmlcontent += "    </challenge>\n";
}
void Db2Xml::doNotCommit()
{
	isOk = false;
}

int Db2Xml::getPsoudoId(string guid) 
{
	int ret = psoudo_playerids[guid];
	if(ret > 0)
		return ret;
	ret = nextId++;
	if(ret == 1022) //world
		ret = nextId++;
	psoudo_playerids[guid] = ret;
	return ret;
}

string Db2Xml::getXmlEscaped(string org) 
{
	string result = org;
	boost::algorithm::replace_all(result,(string)"&",(string)"&amp;");
	boost::algorithm::replace_all(result,(string)"<",(string)"&lt;");
	boost::algorithm::replace_all(result,(string)">",(string)"&gt;");
	boost::algorithm::replace_all(result,(string)"\"",(string)"&quot;");
	boost::algorithm::replace_all(result,(string)"'",(string)"&apos;");
	
	return result;
}

void Db2Xml::createTables() {
	
}