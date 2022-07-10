#include "idcapp.h"

CZHOBTMIND::CZHOBTMIND()
{
  m_conn=0;m_logfile=0;
}

CZHOBTMIND::CZHOBTMIND(connection *conn,CLogFile *logfile)
{
  m_conn=conn;
  m_logfile=logfile;
}

CZHOBTMIND::~CZHOBTMIND()
{
}

void CZHOBTMIND::BindConnLog(connection *conn,CLogFile *logfile)
{
  m_conn=conn;
  m_logfile=logfile;
}

bool CZHOBTMIND::SplitBuffer(char *buffer,bool bisxml)
{
  memset(&m_stzhobtmind,0,sizeof(struct st_zhobtmind));
  if(bisxml==true)
  {
    GetXMLBuffer(buffer,"obtid",m_stzhobtmind.obtid,10);
    GetXMLBuffer(buffer,"ddatetime",m_stzhobtmind.ddatetime,14);
    char tmp[11];
    GetXMLBuffer(buffer,"t",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
    GetXMLBuffer(buffer,"p",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.p,10,"%d",(int)(atof(tmp)*10));
    GetXMLBuffer(buffer,"u",m_stzhobtmind.u,10);
    GetXMLBuffer(buffer,"wd",m_stzhobtmind.wd,10);
    GetXMLBuffer(buffer,"wf",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
    GetXMLBuffer(buffer,"r",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
    GetXMLBuffer(buffer,"vis",tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));
  }
  else
  {
    CCmdStr CmdStr;
    CmdStr.SplitToCmd(buffer,",");
    CmdStr.GetValue(0,m_stzhobtmind.obtid,10);
    CmdStr.GetValue(1,m_stzhobtmind.ddatetime,14);
    char tmp[11];
    CmdStr.GetValue(2,tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.t,10,"%d",(int)(atof(tmp)*10));
    CmdStr.GetValue(3,tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.p,10,"%d",(int)(atof(tmp)*10));
    CmdStr.GetValue(4,m_stzhobtmind.u,10);
    CmdStr.GetValue(5,m_stzhobtmind.wd,10);
    CmdStr.GetValue(6,tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.wf,10,"%d",(int)(atof(tmp)*10));
    CmdStr.GetValue(7,tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.r,10,"%d",(int)(atof(tmp)*10));
    CmdStr.GetValue(8,tmp,10); if(strlen(tmp)>0) snprintf(m_stzhobtmind.vis,10,"%d",(int)(atof(tmp)*10));
  }  
  STRCPY(m_buffer,sizeof(m_buffer),buffer);
  return true;
}
bool CZHOBTMIND::InsertTable()
{
    if(m_stmt.m_state==0)
    {
      m_stmt.connect(m_conn);
      m_stmt.prepare("insert into T_ZHOBTMIND(obtid,ddatetime,t,p,u,wd,wf,r,vis) values(:1,str_to_date(:2,'%%Y%%m%%d%%H%%i%%s'),:3,:4,:5,:6,:7,:8,:9)");
      m_stmt.bindin(1,m_stzhobtmind.obtid,10);
      m_stmt.bindin(2,m_stzhobtmind.ddatetime,14);
      m_stmt.bindin(3,m_stzhobtmind.t,10);
      m_stmt.bindin(4,m_stzhobtmind.p,10);
      m_stmt.bindin(5,m_stzhobtmind.u,10);
      m_stmt.bindin(6,m_stzhobtmind.wd,10);
      m_stmt.bindin(7,m_stzhobtmind.wf,10);
      m_stmt.bindin(8,m_stzhobtmind.r,10);
      m_stmt.bindin(9,m_stzhobtmind.vis,10);
    }
  
    if(m_stmt.execute()!=0)
    {
      //1062 记录重复
      if(m_stmt.m_cda.rc!=1062)
        {
          m_logfile->Write("buffer=%s\n",m_buffer);
          m_logfile->Write("m_stmt.execute() failed\n%s\n%s\n",m_stmt.m_sql,m_stmt.m_cda.message);
        }
      return false;
    } 
  return true;
}

