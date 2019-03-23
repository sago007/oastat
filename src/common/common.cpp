/*
===========================================================================
oastat - a program for parsing log files and write the result to a database
Copyright (C) 2012 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/oastat/
===========================================================================
*/

#include <string>
#include <vector>
#include <gcrypt.h>

std::vector<std::string> clientIdMap;

/**
 * Hashes the user id so they cannot be recovered from the db.
 *
 * @param unhashedID - The unhashed id
 * @return the hashed id
 */
std::string getHashedId(const std::string& unhashedID)
{
	int msg_len = unhashedID.length();
	int hash_len = gcry_md_get_algo_dlen( GCRY_MD_SHA1 );
	std::vector<unsigned char> hash_binary(hash_len);
	std::vector<char> hash_hex(hash_len*2+1);
	char *out = &hash_hex.at(0);
	char *p = out;

	gcry_md_hash_buffer( GCRY_MD_SHA1, &hash_binary[0], unhashedID.c_str(), msg_len );
	for ( int i = 0; i < hash_len; i++, p += 2 ) {
		snprintf ( p, 3, "%02x", hash_binary[i] );
	}

	std::string hashedID = &hash_hex.at(0);

	return  hashedID; //Replace with md5 at some point
}