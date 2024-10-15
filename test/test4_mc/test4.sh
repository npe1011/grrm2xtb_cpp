#!/bin/bash
#BSUB -n 20
#BSUB -W 3:00
#BSUB -J test4

# Settings
cd ${LS_SUBCWD}
# Module settings
module purge
export GRRMroot=/usr/local/GRRM23
module load Gaussian/16c02
module load GRRM/23
module load xTB/6.6.1

# Env settings
export OMP_NUM_THREADS=1
export MKL_NUM_THREADS=1
export OMP_MAX_ACTIVE_LEVELS=1

export OMP_STACKSIZE=1G
# XTB settings for grrm2xtb script
export XTB_PARAM=2
export XTB_CHARGE=0
export XTB_MULTI=1
export XTB_SCRATCH_DIR=${LS_SUBCWD}

# Run
GRRM23p test4 -p20 -s3600
