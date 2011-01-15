/* 
 * File:   Award2Db.h
 * Author: poul
 *
 * Created on 7. juni 2010, 19:41
 */

#ifndef _AWARD2DB_H
#define	_AWARD2DB_H

#include "struct2db.h"
#include "../local.h"

class Award2Db : public Struct2Db {
public:

    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* _AWARD2DB_H */

