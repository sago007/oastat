/* 
 * File:   Warmup2Db.hpp
 * Author: poul
 *
 * Created on 26. august 2011, 20:28
 */

#ifndef WARMUP2DB_HPP
#define	WARMUP2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Warmup2Db : public Struct2Db {
public:
    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* WARMUP2DB_HPP */

