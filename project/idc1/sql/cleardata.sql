delete from T_ZHOBTMIND where ddatetime<timestampadd(minute,-120,now());
