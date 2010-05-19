/* 
 * File:   shutdown2db.h
 * Author: poul
 *
 * Created on 16. april 2010, 22:54
 */

#ifndef _SHUTDOWN2DB_H
#define	_SHUTDOWN2DB_H

#include "struct2db.h"
#include "../local.h"

class Shutdown2Db : public Struct2Db{
public:
    Shutdown2Db();
    Shutdown2Db(const Shutdown2Db& orig);
    virtual ~Shutdown2Db();

    string getCommand();

    void process(OaStatStruct oss);
private:

};

#endif	/* _SHUTDOWN2DB_H */

