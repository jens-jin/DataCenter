####################################
#启动数据中心后台服务程序的脚本。
####################################

#检查服务程序是否超时，配置在/etc/rc.local 中由root用户执行。
#/project/tools1/bin/procctl 30 /project/tools1/bin/checkproc

#压缩数据中心后台服务的备份日志。
/project/tools1/bin/procctl 300 /project/tools1/bin/gzipfiles /log/idc "*.log.20*" 0.04

#生成用于测试的全国气象站点观测的分钟数据。
/project/tools1/bin/procctl 60 /project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv
 
#清理/tmp/idc/surfdata 目录下的历史数据文件
/project/tools1/bin/procctl 300 /project/tools1/bin/deletefiles /tmp/idc/surfdata "*" 0.04

#采集全国气象站点观测的分钟数据
/project/tools1/bin/procctl 30 /project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>jens</username><password>123456</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"

#上传全国气象站点观测的分钟数据
/project/tools1/bin/procctl 30 /project/tools1/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>jens</username><password>123456</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputtest</remotepath><matchname>SURF_ZH*.XML</matchname><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>"


#清理/idcdata/surfdata 目录下的历史数据文件
/project/tools1/bin/procctl 300 /project/tools1/bin/deletefiles /idcdata/surfdata "*" 0.04

#清理 /tmp/ftpputtest 目录下的历史数据文件
/project/tools1/bin/procctl 300 /project/tools1/bin/deletefiles /tmp/ftpputtest "*" 0.04

#启动文件传输的服务端程序
/project/tools1/bin/procctl 10 /project/tools1/bin/fileserver 5005 /log/idc/fileserver.log

#把目录/tmp/ftpputtest文件上传到/tmp/tcppputtest
/project/tools1/bin/procctl 20 /project/tools1/bin/tcpputfiles /log/idc/tcpputfiles.log "<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><clientpath>/tmp/ftpputtest</clientpath><clientpathbak>/tmp/ftpputtestbak</clientpathbak><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><srvpath>/tmp/tcpputtest</srvpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpputfiles_surfdata</pname>"
#把目录/tmp/tcpputtest文件下载到/tmp/tcpgettest
/project/tools1/bin/procctl 20 /project/tools1/bin/tcpgetfiles /log/idc/tcpgetfiles.log "<ip>127.0.0.1</ip><port>5005</port><ptype>1</ptype><srvpath>/tmp/tcpputtest</srvpath><srvpathbak>/tmp/tcpputtestbak</srvpathbak><andchild>true</andchild><matchname>*.XML,*.CSV,*.JSON</matchname><clientpath>/tmp/tcpgettest</clientpath><timetvl>10</timetvl><timeout>50</timeout><pname>tcpgetfiles_surfdata</pname>"
#把目录/tmp/tcpgettest历史文件清理
/project/tools1/bin/procctl 300 /project/tools1/bin/deletefiles /tmp/tcpgettest "*" 0.04

#站点参数文件入库
/project/tools1/bin/procctl 120 /project/idc1/bin/obtcodetodb /project/idc1/ini/stcode.ini          "127.0.0.1,root,*#u1604#*,mysql,3306" utf8 /log/idc/obtcodetodb.log

#观测数据入库
/project/tools1/bin/procctl 10 /project/idc1/bin/obtmindtodb /idcdata/surfdata "127.0.0.1,root,*#u1604#*,mysql,3306" utf8 /log/idc/obtmindtodb.log

#定期清理数据库（120min）
/project/tools1/bin/procctl 120 /project/tools1/bin/execsql /project/idc1/sql/cleardata.sql "127.0.0.1,root,*#u1604#*,mysql,3306" utf8 /log/idc/execsql.log


