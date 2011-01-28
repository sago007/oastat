/* 
 * File:   CtfElimination2Db.hpp
 * Author: poul
 *
 * Created on 21. januar 2011, 21:12
 */

#ifndef _CTFELIMINATION2DB_HPP
#define	_CTFELIMINATION2DB_HPP


#include "struct2db.h"
#include "../local.h"

class CtfElimination2Db : public  Struct2Db {
public:
    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);

private:

};

#endif	/* _CTFELIMINATION2DB_HPP */

