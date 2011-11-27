#This is the makefile
USEPOSTGRESQL=0
USEDBIXX=1
USESTDOUT=0

BASE_LIBS=-lgcrypt
BASE_CFLAGS=-c -g -O0

O_FILES=build/oastat.o build/oastatstruct.o build/kill2db.o build/init2db.o build/shutdown2db.o build/userinfo2db.o build/disconnect2db.o build/award2db.o build/ctf2db.o build/point2db.o build/ctf1f2db.o build/elimination2db.o build/ctfelim2db.o build/harvester2db.o build/challenge2db.o build/warmup2db.o

H_FILES=local.h oastatstruct.h oss2db/struct2db.h

ifeq ($(USESTDOUT),1)
	O_FILES+= build/db2stdout.o
	BASE_CFLAGS+= -DUSESTDOUT=1
endif

ifeq ($(USEPOSTGRESQL),1)
	BASE_LIBS+= -lpq
	O_FILES+= build/db2postgresql.o
	BASE_CFLAGS+= -DUSEPOSTGRESQL=1
endif

ifeq ($(USEDBIXX),1)
        BASE_LIBS+= -ldbixx
        O_FILES+= build/db2dbixx.o
        BASE_CFLAGS+= -DUSEDBIXX=1
endif

total: build/oastat

clean: 
	rm -f build/*.o

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

ifeq ($(USEDBIXX),1)
build/db2dbixx.o: db/Db2DbiXX.cpp db/Db2DbiXX.hpp db/database.hpp
	g++ $(BASE_CFLAGS) db/Db2DbiXX.cpp -o build/db2dbixx.o
endif

build/oastatstruct.o: oastatstruct.cpp oastatstruct.h $(H_FILES)
	g++ $(BASE_CFLAGS) oastatstruct.cpp -o build/oastatstruct.o

build/kill2db.o: oss2db/kill2db.cpp oss2db/kill2db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/kill2db.cpp -o build/kill2db.o

build/init2db.o: oss2db/init2db.cpp oss2db/init2db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/init2db.cpp -o build/init2db.o

build/shutdown2db.o: oss2db/shutdown2db.cpp oss2db/shutdown2db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/shutdown2db.cpp -o build/shutdown2db.o

build/userinfo2db.o: oss2db/userinfo2db.cpp oss2db/userinfo2db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/userinfo2db.cpp -o build/userinfo2db.o

build/disconnect2db.o: oss2db/Disconnect2Db.cpp oss2db/Disconnect2Db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Disconnect2Db.cpp -o build/disconnect2db.o

build/award2db.o: oss2db/Award2Db.cpp oss2db/Award2Db.h $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Award2Db.cpp -o build/award2db.o

build/ctf2db.o: oss2db/Ctf2Db.cpp  $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Ctf2Db.cpp -o build/ctf2db.o

build/ctf1f2db.o: oss2db/Ctf1f2Db.cpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Ctf1f2Db.cpp -o build/ctf1f2db.o

build/elimination2db.o: oss2db/Elimination2Db.cpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Elimination2Db.cpp -o build/elimination2db.o

build/ctfelim2db.o: oss2db/CtfElimination2Db.cpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/CtfElimination2Db.cpp -o build/ctfelim2db.o

build/point2db.o: oss2db/Point2Db.cpp oss2db/Point2Db.hpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Point2Db.cpp -o build/point2db.o

build/harvester2db.o: oss2db/Harvester2Db.cpp oss2db/Harvester2Db.hpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Harvester2Db.cpp -o build/harvester2db.o

build/challenge2db.o: oss2db/Challenge2Db.cpp oss2db/Challenge2Db.hpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Challenge2Db.cpp -o build/challenge2db.o

build/warmup2db.o: oss2db/Warmup2Db.cpp oss2db/Warmup2Db.hpp $(H_FILES)
	g++ $(BASE_CFLAGS) oss2db/Warmup2Db.cpp -o build/warmup2db.o

