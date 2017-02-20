ifndef USEDBIXX
USEDBIXX=0
endif
ifndef USECPPDB
USECPPDB=1
endif
ifndef PROGRAMNAME
PROGRAMNAME=oastat
endif
ifdef CROSS
CXX=$(CROSS)g++
endif

BASE_LIBS=-lgcrypt -lboost_program_options -lgpg-error
BASE_CFLAGS=-c -g -O1 -Wall -std=c++11

-include Makefile.local

O_FILES=oastat.o oastatstruct.o oss2db/kill2db.o oss2db/init2db.o oss2db/shutdown2db.o oss2db/userinfo2db.o oss2db/Disconnect2Db.o oss2db/Award2Db.o oss2db/Ctf2Db.o oss2db/Point2Db.o oss2db/Ctf1f2Db.o oss2db/Elimination2Db.o oss2db/CtfElimination2Db.o oss2db/Harvester2Db.o oss2db/Challenge2Db.o oss2db/Warmup2Db.o oss2db/Accuracy2Db.o

ifeq ($(USEDBIXX),1)
ifeq ($(wildcard /usr/local/lib/libdbixx.so),)
	BASE_LIBS+= -ldbixx
else 
	BASE_LIBS+= /usr/local/lib/libdbixx.so # -ldbixx
endif
	O_FILES+= db/Db2DbiXX.o
	BASE_CFLAGS+= -DUSEDBIXX=1
endif

ifeq ($(USECPPDB),1)
	BASE_LIBS+= -lcppdb
	O_FILES+= db/Db2CppDb.o
	BASE_CFLAGS+= -DUSECPPDB=1
endif

O_FILES+= db/Db2Xml.o
BASE_CFLAGS+= -DUSEXML=1

total: ${PROGRAMNAME}

clean: 
	rm -f */*.o *.o *.P */*.P */*/*.P ${PROGRAMNAME}

${PROGRAMNAME}: $(O_FILES)
	$(CXX) -O -o ${PROGRAMNAME} $(O_FILES) $(BASE_LIBS)

%.o : %.cpp
	$(CXX) -MD ${BASE_CFLAGS} -o $@ $<
	@mkdir -p .$(CROSS)deps/$(basename $*); cp $*.d .$(CROSS)deps/$*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> .$(CROSS)deps/$*.P; \
             rm -f $*.d

-include .$(CROSS)deps/*.P
-include .$(CROSS)deps/*/*.P
