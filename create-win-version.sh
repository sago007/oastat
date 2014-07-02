#!/bin/bash
set -e
cd sql/create_tables/
./create_specific.sh
cd ../../src
./mxe-build.sh
cd ..
mkdir -p oastat-win
cd oastat-win
cp ../sql/create_tables/oastat_create_tables_sqlite.sql ./
cp ../src/oastat.exe ./
cp ~/git/mxe/usr/i686-pc-mingw32/bin/libcppdb.dll ./
cp ~/git/mxe/usr/i686-pc-mingw32/bin/libcppdb_sqlite3.dll ./
cp ~/git/mxe/usr/i686-pc-mingw32/bin/sqlite3.exe ./
cp ../COPYING ./COPYING.txt
cp ../extra/windows/oastat_fill_db.bat ./
cp ../extra/windows/oastat_create_db.bat ./
DATE=$(date +"%Y-%m-%d")
cp ../README.md ./README-$DATE.txt
cd ..
zip -r oastat-win-$DATE oastat-win
