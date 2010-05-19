/* 
 * File:   kill2db.h
 * Author: poul
 *
 * Created on 21. marts 2010, 10:50
 */

#ifndef _KILL2DB_H
#define	_KILL2DB_H

#include "struct2db.h"
#include "../local.h"

class Kill2Db : public Struct2Db {

    string getCommand();

    void process(OaStatStruct oss);
    
};



#endif	/* _KILL2DB_H */

