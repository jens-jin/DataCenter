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
  
  struct st_girls{
    long id;
    char name[31];
    double weight;
    char btime[20];
    char pic[100000];
    unsigned long picsize;
  }stgirls;

  sqlstatement stmt(&conn);
  /*
  *sqlstatement stmt; stmt.connect(&conn);
  */ 
  /*
  stmt.prepare()返回值不用判断，在stmt.execute()再判断。 分号不写 兼容性
  */
  // insert  %转义%%
  stmt.prepare("\
         select id,pic from girls where id in (1,2)");
      // ???兼容性不好,参数可以参与运算或用于函数参数，sql语句的主体不变，只需prepare一次
      // prepare->bindin->execute 
  stmt.bindout(1,&stgirls.id);
  stmt.bindoutlob(2,stgirls.pic,100000,&stgirls.picsize);

  if(stmt.execute()!=0)
  {printf("stmt.execute() failed\n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);return -1;}
  
  while(true)
  {
    memset(&stgirls,0,sizeof(struct st_girls));
    
    if(stmt.next()!=0) break;
    char filename[101]; memset(filename,0,sizeof(filename));
    sprintf(filename,"%d_out.jpg",stgirls.id);
    
    buftofile(filename,stgirls.pic,stgirls.picsize);

  }


  printf("本次查询了表girls%ld条记录 ok.\n",stmt.m_cda.rpc); 

  return 0;
}
