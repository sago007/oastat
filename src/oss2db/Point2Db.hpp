/* 
 * File:   Point2Db.hpp
 * Author: poul
 *
 * Created on 10. januar 2011, 21:32
 */

#ifndef _POINT2DB_HPP
#define	_POINT2DB_HPP

#include "struct2db.h"
#include "../local.h"

class Point2Db : public Struct2Db {
public:
    Point2Db();
    Point2Db(const Point2Db& orig);
    virtual ~Point2Db();

    string getCommand();
    bool canProcess(OaStatStruct oss);

    void process(OaStatStruct oss);
private:

};

#endif	/* _POINT2DB_HPP */

