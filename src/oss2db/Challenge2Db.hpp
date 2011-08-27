/* 
 * File:   Challenge2Db.hpp
 * Author: poul
 *
 * Created on 19. august 2011, 22:45
 */

#ifndef CHALLENGE2DB_HPP
#define	CHALLENGE2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Challenge2Db : public Struct2Db {
public:
    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* CHALLENGE2DB_HPP */

