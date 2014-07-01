cd sql/create_tables/
./create_specific.sh
cd ../../src
./mxe-build.sh
cd ..
mkdir -p oastat-win
cd oastat-win
cp ../sql/create_tables/oastat_create_tables_sqlite.sql ./
cp ../src/oastat.exe ./
