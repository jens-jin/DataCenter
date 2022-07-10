MYSQLINCL=-I/usr/local/mysql/include

MYSQLLIB=-L/usr/local/mysql/lib 

MYSQLLIBS=-lmysqlclient

CFLAGS=-g -Wno-write-strings

all: test createtable inserttable updatetable selecttable deletetable filetoblob blobtofile

test:test.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o test test.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc
createtable:createtable.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o createtable createtable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

inserttable:inserttable.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o inserttable inserttable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

updatetable:updatetable.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o updatetable updatetable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

selecttable:selecttable.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o selecttable selecttable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

deletetable:deletetable.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o deletetable deletetable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

filetoblob:filetoblob.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o filetoblob filetoblob.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

blobtofile:blobtofile.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o blobtofile blobtofile.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc

clean:
	rm -rf createtable inserttable updatetable selecttable deletetable filetoblob blobtofile tset

