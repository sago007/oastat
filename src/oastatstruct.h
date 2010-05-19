/* 
 * File:   oastatstruct.h
 * Author: poul
 *
 * Created on 18. marts 2010, 19:10
 */

#ifndef _OASTATSTRUCT_H
#define	_OASTATSTRUCT_H

#include <vector>
#include <string>
#include <map>

using namespace std;

/*
 * This is the most important class in oastat. This takes a line and stores all
 * information from that line in easy to process parts.
 */
class OaStatStruct {
public:
    OaStatStruct();
    OaStatStruct(const OaStatStruct& orig);
    virtual ~OaStatStruct();

    //There are no reason to make any variables private.
    int second; //number of seconds
    string command; //Like 'ClientConnect', 'Award' and 'Kill'
    vector<int> parameters; //The parameters for the command.
    string restOfLine;

    void parseLine(string line);

    /*
     *Parses the rest of the line to an info string
     */
    map<string,string> GetInfostring(string restOfLine);

    /*
     *Same but uses this.restOfLine
     */
    map<string,string> GetInfostring();
private:

};

#endif	/* _OASTATSTRUCT_H */

