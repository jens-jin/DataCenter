####################################
#停止数据中心后台服务程序的脚本。
####################################
killall -9 procctl
killall gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles fileserver tcpputfiles tcpgetfiles obtcodetodb
killall obtmindtodb execsql
sleep 3

killall -9  gzipfiles crtsurfdata deletefiles ftpgetfiles ftpputfiles fileserver tcpputfiles tcpgetfiles obtcodetodbkillall -9  obtmindtodb execsql
