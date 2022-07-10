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
         update girls set pic=:1 where id=:2");
      // ???兼容性不好,参数可以参与运算或用于函数参数，sql语句的主体不变，只需prepare一次
      // prepare->bindin->execute 
  stmt.bindinlob(1,stgirls.pic,&stgirls.picsize);
  stmt.bindin(2,&stgirls.id);

  for (int ii=1;ii<3;ii++)
  {
    memset(&stgirls,0,sizeof(struct st_girls));
    stgirls.id=ii;

    if(ii==1) stgirls.picsize=filetobuf("1.jpg",stgirls.pic);
    if(ii==2) stgirls.picsize=filetobuf("2.jpg",stgirls.pic);
    if(stmt.execute()!=0)
    {printf("(stmt.execute()failed \n%s\n%d\n%s\n",stmt.m_sql,stmt.m_cda.rc,stmt.m_cda.message);return -1;}
    printf("成功修改了%ld条记录\n",stmt.m_cda.rpc);
  }
  printf("update table girls ok.\n"); 
  conn.commit();

  return 0;
}
