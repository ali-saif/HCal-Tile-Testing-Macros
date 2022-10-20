#!/bin/bash
export SSH_AUTH_SOCK=/run/user/1001/keyring/ssh

/bin/date >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/rsync -Pah bnlcaen:/home/hexc/Software/BNL_Kistenev/FEBDAQMULT/rootFilesDR /home/anthony/software/GSU_Data >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/rsync -Pah bnlcaen:/home/hexc/Software/BNL_Kistenev/FEBDAQMULT/rootFileREF /home/anthony/software/GSU_Data >> /home/anthony/software/GSU_Data/gsu_data.log

#/usr/bin/rsync -Pah /home/anthony/software/GSU_Data/rootFilesDR phynp6:/home/ahodges/GSU_data >> /home/anthony/software/GSU_Data/gsu_data.log
#/usr/bin/rsync -Pah /home/anthony/software/GSU_Data/rootFileREF phynp6:/home/ahodges/GSU_data >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/rsync -Pah /home/anthony/software/GSU_Data/rootFilesDR rcas2070:/gpfs/mnt/gpfs02/sphenix/user/ahodges/tempBenchData/ >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/rsync -Pah /home/anthony/software/GSU_Data/rootFileREF rcas2070:/gpfs/mnt/gpfs02/sphenix/user/ahodges/tempBenchData/ >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/scp bnlcaen:/home/hexc/Software/BNL_Kistenev/FEBDAQMULT/FEBDAQMULT_GSU.C .
/usr/bin/scp bnlcaen:/home/hexc/Software/BNL_Kistenev/FEBDAQMULT/CITIROC_SC_PROFILE1.txt . 

/usr/bin/rsync -Pah bnlcaen:/home/hexc/Software/BNL_Kistenev/FEBDAQMULT/iHCal /home/anthony/software/GSU_Data >> /home/anthony/software/GSU_Data/gsu_data.log

/usr/bin/rsync -Pah /home/anthony/software/GSU_Data/iHCal rcas2070:/gpfs/mnt/gpfs02/sphenix/user/ahodges/tempBenchData/ >> /home/anthony/software/GSU_Data/gsu_data.log
