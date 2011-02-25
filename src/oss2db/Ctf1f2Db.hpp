/* 
 * File:   Ctf1f2Db.hpp
 * Author: poul
 *
 * Created on 11. januar 2011, 20:48
 */

#ifndef _CTF1F2DB_HPP
#define	_CTF1F2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Ctf1f2Db : public Struct2Db {
public:

    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* _CTF1F2DB_HPP */
