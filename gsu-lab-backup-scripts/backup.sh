#!/bin/bash
export SSH_AUTH_SOCK=/run/user/1001/keyring/ssh
/bin/date >> /home/anthony/software/elogBackup/eLogLog.log
/usr/bin/rsync -Pvahz bnlcaen:/home/hexc/Software/elog/logbooks /home/anthony/software/elogBackup >> /home/anthony/software/elogBackup/eLogLog.log
/usr/bin/rsync -Pah /home/anthony/software/elogBackup/logbooks phynp6:/home/ahodges/elogBackup >> /home/anthony/software/elogBackup/eLogLog.log

