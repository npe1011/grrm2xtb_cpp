#!/bin/bash
#BSUB -n 8
#BSUB -W 256:00
#BSUB -J test

# Settings
cd ${LS_SUBCWD}
# Module settings
module purge
export GRRMroot=/usr/local/GRRM23
module load Gaussian/16c02
module load GRRM/23
module load xTB/6.6.1

# Run
GRRM23p test1_min_orca -s919800


