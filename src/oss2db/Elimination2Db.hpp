/* 
 * File:   Elimination2Db.hpp
 * Author: poul
 *
 * Created on 15. januar 2011, 16:21
 */

#ifndef _ELIMINATION2DB_HPP
#define	_ELIMINATION2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Elimination2Db : public Struct2Db {
public:
    Elimination2Db();
    Elimination2Db(const Elimination2Db& orig);
    virtual ~Elimination2Db();

    string getCommand();

    void process(OaStatStruct oss);
private:

};

#endif	/* _ELIMINATION2DB_HPP */

