# !/bin/bash
export SSH_AUTH_SOCK=/run/user/1001/keyring/ssh
/bin/date >> /home/anthony/software/uniPlastData/transferLog.log

rsync -Pah bnlcaen:/home/hexc/Dropbox/RootBase /home/anthony/software/uniPlastData >> /home/anthony/software/uniPlastData/transferLog.log

rsync -Pah bnlcaen:/home/hexc/Dropbox/DataBase /home/anthony/software/uniPlastData >> /home/anthony/software/uniPlastData/transferLog.log

/usr/bin/rsync -Pah /home/anthony/software/uniPlastData/RootBase rcas2070:/gpfs/mnt/gpfs02/sphenix/user/ahodges/tempBenchData/ >> /home/anthony/software/uniPlastData/transferLog.log

/usr/bin/rsync -Pah /home/anthony/software/uniPlastData/DataBase rcas2070:/gpfs/mnt/gpfs02/sphenix/user/ahodges/tempBenchData/ >> /home/anthony/software/uniPlastData/transferLog.log
