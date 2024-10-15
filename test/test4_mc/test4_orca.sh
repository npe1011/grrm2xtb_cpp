#!/bin/bash
#BSUB -n 20
#BSUB -W 3:00
#BSUB -J test4_orca

# Settings
cd ${LS_SUBCWD}
# Module settings
module purge
export GRRMroot=/usr/local/GRRM23
module load Gaussian/16c02
module load GRRM/23
# Env settings
export OMP_NUM_THREADS=1
# Run
GRRM23p test4_orca -p20 -s3600
