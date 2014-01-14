USEDBIXX=1

BASE_LIBS=-lgcrypt -lboost_program_options
BASE_CFLAGS=-c -g -O2 -Wall

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

O_FILES+= db/Db2Xml.o
BASE_CFLAGS+= -DUSEXML=1

total: oastat

clean: 
	rm -f */*.o *.o *.P */*.P oastat

oastat: $(O_FILES)
	g++ -O -o oastat $(O_FILES) $(BASE_LIBS)

%.o : %.cpp
	g++ -MD -c -o $@ $<
	@cp $*.d $*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
             rm -f $*.d

-include *.P
