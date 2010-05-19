/* 
 * File:   Disconnect2Db.h
 * Author: poul
 *
 * Created on 17. maj 2010, 21:13
 */

#ifndef _DISCONNECT2DB_H
#define	_DISCONNECT2DB_H

#include "struct2db.h"
#include "../local.h"

class Disconnect2Db : public Struct2Db {
public:
    Disconnect2Db();
    Disconnect2Db(const Disconnect2Db& orig);
    virtual ~Disconnect2Db();

    string getCommand();

    void process(OaStatStruct oss);
private:

};

#endif	/* _DISCONNECT2DB_H */

