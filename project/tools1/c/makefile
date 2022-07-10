#开发框架头文件路径
PUBINCL = -I/project/public/

#开发框架cpp
PUBCPP = /project/public/_public.cpp

#mysql
MYSQLINCL= -I/usr/local/mysql/include -I/project/public/db/mysql
MYSQLLIB= -L/usr/local/mysql/lib
MYSQLLIBS= -lmysqlclient
MYSQLCPP=/project/public/db/mysql/_mysql.cpp

CFLAG =  -g
 
all:procctl checkproc book gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles fileserver tcpgetfiles test\
    execsql dminingmysql xmltodb inetd rinetd rinetdin

procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl ../bin/.

checkproc:checkproc.cpp
	g++ $(CFLAG) -o checkproc checkproc.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp checkproc ../bin/.

book:book.cpp
	g++ $(CFLAG) -o book book.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp book ../bin/.

gzipfiles:gzipfiles.cpp
	g++ $(CFLAG) -o gzipfiles gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp gzipfiles ../bin/.

deletefiles:deletefiles.cpp
	g++ $(CFLAG) -o deletefiles deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp deletefiles ../bin/.

ftpgetfiles:ftpgetfiles.cpp
	g++ $(CFLAG) -o ftpgetfiles ftpgetfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp -lm -lc -pthread
	cp ftpgetfiles ../bin/.

ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAG) -o ftpputfiles ftpputfiles.cpp $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp -lm -lc -pthread
	cp ftpputfiles ../bin/.

tcpputfiles:tcpputfiles.cpp
	g++ $(CFLAG) -o tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp tcpputfiles ../bin/.

fileserver:fileserver.cpp
	g++ $(CFLAG) -o fileserver fileserver.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp fileserver ../bin/.

tcpgetfiles:tcpgetfiles.cpp
	g++ $(CFLAG) -o tcpgetfiles tcpgetfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp tcpgetfiles ../bin/.

test:test.cpp
	g++ $(CFLAG) -o test test.cpp $(PUBINCL) $(PUBCPP) /project/public/libftp.a /project/public/_ftp.cpp -lm -lc -pthread
	cp test ../bin/.

execsql:execsql.cpp
	g++ $(CFLAG) -o execsql execsql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc -pthread
	cp execsql ../bin/.

dminingmysql:dminingmysql.cpp
	g++ $(CFLAG) -o dminingmysql dminingmysql.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc -pthread
	cp dminingmysql ../bin/.

xmltodb:xmltodb.cpp _tools.h _tools.cpp
	g++ $(CFLAG) -o xmltodb xmltodb.cpp _tools.cpp $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) $(MYSQLCPP) -lm -lc -pthread
	cp xmltodb ../bin/.

inetd:inetd.cpp
	g++ $(CFLAG) -o inetd inetd.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp inetd ../bin/.
rinetd:rinetd.cpp
	g++ $(CFLAG) -o rinetd rinetd.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp rinetd ../bin/.
rinetdin:rinetdin.cpp
	g++ $(CFLAG) -o rinetdin rinetdin.cpp $(PUBINCL) $(PUBCPP) -lm -lc -pthread
	cp rinetdin ../bin/.
clean:
	rm -f procctl checkproc book gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles fileserver 
	rm -f tcpgetfiles test execsql dminingmysql xmltodb inetd rinetd rinetdin
