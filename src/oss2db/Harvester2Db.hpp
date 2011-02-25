/* 
 * File:   Harvester2Db.hpp
 * Author: poul
 *
 * Created on 25. februar 2011, 22:01
 */

#ifndef _HARVESTER2DB_HPP
#define	_HARVESTER2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Harvester2Db : public Struct2Db {
public:
    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* _HARVESTER2DB_HPP */

