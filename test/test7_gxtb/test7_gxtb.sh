#!/bin/bash
#BSUB -n 4
#BSUB -W 256:00
#BSUB -J test

# Settings
cd ${LS_SUBCWD}
# Module settings
module purge
export GRRMroot=/usr/local/GRRM23
module load Gaussian/16c02
module load GRRM/23
module load xTB/6.7.1g

# Env settings
export OMP_NUM_THREADS=4
export MKL_NUM_THREADS=4
export OMP_MAX_ACTIVE_LEVELS=1
export OMP_STACKSIZE=1G

# XTB settings for grrm2xtb script
export XTB_PARAM=gxtb
export XTB_CHARGE=0
export XTB_MULTI=none

# export XTB_SCRATCH_DIR=/scr/${USER}/${LSB_JOBID}

# Add current dir to Path (for GRRM2XTB)
# export XTB_SCRATCH_DIR="`pwd`"

# Run
GRRM23p test7_gxtb -s919800 1>test7_log.txt 2>test7_error.txt


