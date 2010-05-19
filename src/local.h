/* 
 * File:   local.h
 * Author: poul
 *
 * Created on 21. marts 2010, 12:14
 */

#ifndef _LOCAL_H
#define	_LOCAL_H

#define MAX_ID 128

#include "oastatstruct.h"

/* This array contains all the guid hashes of the players in a game */
extern string clientIdMap[MAX_ID];

/* returns the SHA-1 code of the parameter */
extern string getHashedId(string unhashedID);

#endif	/* _LOCAL_H */

