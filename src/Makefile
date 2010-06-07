#This is the makefile
USEPOSTGRESQL=1
USESTDOUT=0

BASE_LIBS=-lgcrypt
BASE_CFLAGS=-c -g

O_FILES=build/oastat.o build/oastatstruct.o build/kill2db.o build/init2db.o build/shutdown2db.o build/userinfo2db.o build/disconnect2db.o build/award2db.o

ifeq ($(USESTDOUT),1)
	O_FILES+= build/db2stdout.o
	BASE_CFLAGS+= -DUSESTDOUT=1
endif

ifeq ($(USEPOSTGRESQL),1)
	BASE_LIBS+= -lpq
	O_FILES+= build/db2postgresql.o
	BASE_CFLAGS+= -DUSEPOSTGRESQL=1
endif

build/oastat: $(O_FILES)
	g++ -O -o build/oastat $(O_FILES) $(BASE_LIBS)

build/oastat.o:	oastat.cpp
	g++ $(BASE_CFLAGS) oastat.cpp -o build/oastat.o

ifeq ($(USESTDOUT),1)
build/db2stdout.o: db/db2stdout.cpp db/db2stdout.h db/database.hpp
	g++ $(BASE_CFLAGS) db/db2stdout.cpp  -o build/db2stdout.o
endif

ifeq ($(USEPOSTGRESQL),1)
build/db2postgresql.o: db/Db2PostgreSQL.cpp db/Db2PostgreSQL.h db/database.hpp
	g++ $(BASE_CFLAGS) db/Db2PostgreSQL.cpp  -o build/db2postgresql.o
endif

build/oastatstruct.o: oastatstruct.cpp oastatstruct.h
	g++ $(BASE_CFLAGS) oastatstruct.cpp -o build/oastatstruct.o

build/kill2db.o: oss2db/kill2db.cpp oss2db/kill2db.h
	g++ $(BASE_CFLAGS) oss2db/kill2db.cpp -o build/kill2db.o

build/init2db.o: oss2db/init2db.cpp oss2db/init2db.h
	g++ $(BASE_CFLAGS) oss2db/init2db.cpp -o build/init2db.o

build/shutdown2db.o: oss2db/shutdown2db.cpp oss2db/shutdown2db.h
	g++ $(BASE_CFLAGS) oss2db/shutdown2db.cpp -o build/shutdown2db.o

build/userinfo2db.o: oss2db/userinfo2db.cpp oss2db/userinfo2db.h
	g++ $(BASE_CFLAGS) oss2db/userinfo2db.cpp -o build/userinfo2db.o

build/disconnect2db.o: oss2db/Disconnect2Db.cpp oss2db/Disconnect2Db.h
	g++ $(BASE_CFLAGS) oss2db/Disconnect2Db.cpp -o build/disconnect2db.o

build/award2db.o: oss2db/Award2Db.cpp oss2db/Award2Db.h
	g++ $(BASE_CFLAGS) oss2db/Award2Db.cpp -o build/award2db.o
