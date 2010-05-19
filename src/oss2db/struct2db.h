/* 
 * File:   struct2db.h
 * Author: poul
 *
 * Created on 20. marts 2010, 18:04
 */

#ifndef _STRUCT2DB_H
#define	_STRUCT2DB_H

#include "../oastatstruct.h"
#include "../db/database.hpp"
#include <string>

using namespace std;

/*
 This is the struct to database class that all classes that interpret a log line must inherit
 */
class Struct2Db {
	public:
            /*
             * The object will need a database interface to cummunicate to. This is given here
             */
            void setDb(Database *d) {
                dp = d;
            }

            /*
             * The object need a way to tell what command it is capreble to preocess.
             *
             * @returns The command the object can process
             */
            virtual string getCommand() = 0;

            /*
             * Parses s OaStatStruct to the object to process
             */
            virtual void process(OaStatStruct oss) = 0;

        protected:
            Database *dp; //Pointer to the used db
};


#endif	/* _STRUCT2DB_H */

