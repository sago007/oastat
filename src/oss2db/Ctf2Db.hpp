/* 
 * File:   Ctf2Db.hpp
 * Author: poul
 *
 * Created on 19. november 2010, 23:08
 */

#ifndef _CTF2DB_HPP
#define	_CTF2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Ctf2Db : public Struct2Db {
public:

    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* _CTF2DB_HPP */

