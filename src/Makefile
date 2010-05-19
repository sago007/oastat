#This is the makefile


build/oastat:	build/oastat.o build/db2stdout.o build/oastatstruct.o build/kill2db.o build/init2db.o build/shutdown2db.o build/userinfo2db.o build/disconnect2db.o
	g++ -O -o build/oastat build/oastat.o build/db2stdout.o build/oastatstruct.o build/kill2db.o build/init2db.o build/shutdown2db.o build/userinfo2db.o build/disconnect2db.o  -lgcrypt

build/oastat.o:	oastat.cpp
	g++ -c -g oastat.cpp -o build/oastat.o

build/db2stdout.o: db/db2stdout.cpp db/db2stdout.h db/database.hpp
	g++ -c -g db/db2stdout.cpp  -o build/db2stdout.o

build/oastatstruct.o: oastatstruct.cpp oastatstruct.h
	g++ -c -g oastatstruct.cpp -o build/oastatstruct.o

build/kill2db.o: oss2db/kill2db.cpp oss2db/kill2db.h
	g++ -c -g oss2db/kill2db.cpp -o build/kill2db.o

build/init2db.o: oss2db/init2db.cpp oss2db/init2db.h
	g++ -c -g oss2db/init2db.cpp -o build/init2db.o

build/shutdown2db.o: oss2db/shutdown2db.cpp oss2db/shutdown2db.h
	g++ -c -g oss2db/shutdown2db.cpp -o build/shutdown2db.o

build/userinfo2db.o: oss2db/userinfo2db.cpp oss2db/userinfo2db.h
	g++ -c -g oss2db/userinfo2db.cpp -o build/userinfo2db.o

build/disconnect2db.o: oss2db/Disconnect2Db.cpp oss2db/Disconnect2Db.h
	g++ -c -g oss2db/Disconnect2Db.cpp -o build/disconnect2db.o

