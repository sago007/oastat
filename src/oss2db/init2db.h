/* 
 * File:   init2db.h
 * Author: poul
 *
 * Created on 16. april 2010, 22:19
 */

#ifndef _INIT2DB_H
#define	_INIT2DB_H

#include "struct2db.h"
#include "../local.h"

class Init2Db : public Struct2Db {
public:
    Init2Db();
    Init2Db(const Init2Db& orig);
    virtual ~Init2Db();

    string getCommand();

    void process(OaStatStruct oss);
private:

};

#endif	/* _INIT2DB_H */

