#!/bin/csh

mysqldump --host=mysql-p.sourceforge.net \
  --user=p159762admin -p --opt \
  p159762_qa | gzip --fast > dumpfile.mysql.gz

