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
  }stgirls;

  sqlstatement stmt(&conn);
  /*
  *sqlstatement stmt; stmt.connect(&conn);
  */ 
  /*
  stmt.prepare()返回值不用判断，在stmt.execute()再判断。 分号不写 兼容性
  */
  // insert  %转义%%
  stmt.prepare("update girls set name='万法',weight=13.5,btime=now() where id in (5,6)");
  stmt.execute();
  printf("成功修改了%d条记录\n",stmt.m_cda.rpc);
  conn.commit();
  return 0;
  stmt.prepare("\
         update girls set name=:1,weight=:2,btime=str_to_date(:4,'%%Y-%%m-%%d %%H:%%i:%%s') where id=:4");
      // ???兼容性不好,参数可以参与运算或用于函数参数，sql语句的主体不变，只需prepare一次
      // prepare->bindin->execute 
  stmt.bindin(4,&stgirls.id);
  stmt.bindin(1,stgirls.name,30);
  stmt.bindin(2,&stgirls.weight);
  stmt.bindin(3,stgirls.btime,19);

  for (int ii=0;ii<10;ii++)
  {
    memset(&stgirls,0,sizeof(struct st_girls));
    stgirls.id=ii+1;
    sprintf(stgirls.name,"杨玉环%05dgirls",ii+1);
    stgirls.weight=43.25+ii;
    sprintf(stgirls.btime,"2021-01-03 12:21:%02d",ii+1);
    if(stmt.execute()!=0)
    {printf("(stmt.execute()failed \n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);return -1;}
    printf("成功修改了%d条记录\n",stmt.m_cda.rpc);
  }
  printf("update table girls ok.\n"); 
  conn.commit();

  return 0;
}
