#include "_mysql.h"
/*
-- 超女基本信息表
create table girls(id      bigint(10),
                   name    varchar(10),
                   weight  decimal(8,2),
                   btime   datetime,
                   memo    longtext,
                   pic     longblob,
                   primary key(id));

*/
int main(int argc,char* argv[])
{
  connection conn;


  if(conn.connecttodb("127.0.0.1,root,*#u1604#*,mysql,3306","utf8")!=0 )
  {printf("conn.connecttodb() failed\n%s\n",conn.m_cda.message);return -1;}

  sqlstatement stmt(&conn);
  /*
  *sqlstatement stmt; stmt.connect(&conn);
  */ 
  stmt.prepare("create table girls(id      bigint(10),\
                   name    varchar(30),\
                   weight  decimal(8,2),\
                   btime   datetime,\
                   memo    longtext,\
                   pic     longblob,\
                   primary key (id))");
  /*
  stmt.prepare()返回值不用判断，在stmt.execute()再判断。 分号不写 兼容性
  */
  if(stmt.execute()!=0)
  {printf("stmt.execute()() failed\n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);return -1;}
  return 0;
}
