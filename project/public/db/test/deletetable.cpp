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
  int mm=0;

  if(conn.connecttodb("127.0.0.1,root,*#u1604#*,mysql,3306","utf8")!=0 )
  {printf("conn.connecttodb() failed\n%s\n",conn.m_cda.message);return -1;}
  

  sqlstatement stmt(&conn);
  /*
  *sqlstatement stmt; stmt.connect(&conn);
  */ 
  /*
  stmt.prepare()返回值不用判断，在stmt.execute()再判断。 分号不写 兼容性
  */
  // insert  _fmt格式%需要转义%%  date_format函数转化为字符串
  int minid,maxid;
  stmt.prepare("\
         delete from girls where id>=:1 and id<=:2");
      // ???兼容性不好,参数可以参与运算或用于函数参数，sql语句的主体不变，只需prepare一次
      // prepare->bindin->execute 
  stmt.bindin(1,&minid);
  stmt.bindin(2,&maxid);
  
  minid=2,maxid=6;
  if(stmt.execute()!=0)
  {printf("(stmt.execute()failed \n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);return -1;}


  printf("本次删除了%ld条记录\n",stmt.m_cda.rpc);
  conn.commit();
  return 0;
}
