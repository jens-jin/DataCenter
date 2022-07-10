/*idcapp.h
 *
 */

#ifndef IDCAPP_H
#define IDCAPP_H


#include "_public.h"
#include "_mysql.h"

struct st_zhobtmind{
  char obtid[11];
  char ddatetime[21];
  char t[11];
  char p[11];
  char u[11];
  char wd[11];
  char wf[11];
  char r[11];
  char vis[11];
};

class CZHOBTMIND
{
public:
  connection *m_conn;
  CLogFile *m_logfile;

  sqlstatement m_stmt;

  char m_buffer[1024];
  struct st_zhobtmind m_stzhobtmind;

  CZHOBTMIND();
  CZHOBTMIND(connection *conn,CLogFile *logfile);

  ~CZHOBTMIND();

  void BindConnLog(connection *conn,CLogFile *logfile);
  bool SplitBuffer(char *buffer,bool bisxml);
  bool InsertTable();
};


#endif
