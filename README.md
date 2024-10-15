# grrm2xtb (C++ version)


## Abstract
This is a interface program to use Grimme's XTB [https://github.com/grimme-lab/xtb] as a QM engine for GRRM (17 and 23 tested).
Original python3 version [https://github.com/npe1011/grrm2xtb] was re-written in C++ for speed-up.
While we tried several jobs and this interface seems to reasonably work, we provide *NO guarantee*.
Caution: input using of "External Atoms" has not been tested. 


## Software version
We tested this interface with GRRM23, xtb-6.6.1 and newers.


## Settings

 - Set environmental variables and PATH for XTB. XTB should be able to run with a command `xtb`. 
 - Copy the compiled binary (grrm2xtb) where PATH is set and make it executable [`chmod +x grrm2xtb`].

## Compilation

 - The released binary file has been statically built and may require no dependencies. 
 - If you want to build your self, modify makefile and do make.
 - The current makefile is written to use intel compiler (icpx) for static build.


## GRRM Job and XTB calculation settings

`%link=non-supported` should be in the first line and `sublink=script_name` should be in the options section in your GRRM input.
In the `#` section, simply write a job type. The following is a MIN job.
```
%link=non-supported
#MIN

0 1
C 0.000 0.000 0.000
C 1.200 0.000 0.000
....
Options
...
sublink=grrm2xtb
```

The calculation settings for XTB (charge, spin multiplicity, solvation, XTB version) need to be given as environmental variables (see below), NOT in the GRRM input.
The charge and multiplicity in the GRRM input are neglected and have no effect.
In case no environmental variables are set, the default settings are used (charge = 0, multiplicity = 1, gas phase, GFN2).
`XTB_MULTI` should be a spin multiplicity like in Gaussian or GRRM, not Na-Nb (--uhf for XTB). Internally, `--uhf` is set to multiplicity-1. 
XTB calculations are done in sub-directories in XTB_SCRATCH_DIR. If not provided, sub-directories are prepared in the GRRM working directory. 

```
export XTB_CHARGE=0
export XTB_MULTI=1
export XTB_SOLVATION=ALPB
export XTB_SOLVENT=CH2Cl2
export XTB_PARAM=2
export XTB_SCRATCH_DIR=/path/to/scratch/directory
```

## Parallelization

How many cores are used in each XTB job can be set as a standard way for XTB [https://xtb-docs.readthedocs.io/en/latest/setup.html#environment-variables-for-xtb].

In AFIR or other multi-process jobs in GRRM, these variables should be appropriately set.
If you set `OMP_NUM_THREADS=4,1` and run GRRM with 4 parallel processes, 4\*4=16 cpus will be used.
Generally, XTB calculations are very fast for small organic and organometallic compounds (less than 100-200 atoms),
setting `OMP_NUM_THREADS=1,1` and increasing the GRRM processes as many as possible may be a good way for SC-AFIR and MC-AFIR search.

## Run GRRM

After setting the all required envirionmental variables, run GRRM as usual.

*Caution:* In the output, DIPOLE,POLARIZABILITY, and S\**2 are always 0. This script is just for a rough search for structures.

## Difference of Overhead and Hessian Issue in ORCA/otool_xtb

Since calculation of small-medium systems by XTB is very fast, the overhead cost by the interface program may not be negligible.
This C++ version is faster than the original python version and ORCA5 (otool_xtb).
The difference is larger when the system is smaller and larger OMP_NUM_THREADS is set for each process.
Hessian calculation with ORCA/otool_xtb is somewhat problematic and not very convenient; numfreq option is required in a template file but it causes Hessian calculation in all steps (Is there any work-around for this?).
In addition, numerical Hessian calculation with ORCA (MPI parallelization) is much slower than the that with Grimme's xtb binary.

Comparison of several calculation timinges is summarized below.

### Ex1. Comparison of MIN Job
MIN of organic molecule with 34 atoms with NoFC option (GFN2/gas).
With 8 cores (OMP_NUM_THREADS=8 or orcaproc=8). 
with grrm2xtb [test1_min_freq/test1_min.log]: 12.0 sec / 220 forces = 0.055 sec/force
with ORCA5 [test1_min_freq/test1_min_orca.log]: 82.0 sec / 220 forces = 0.37 sec/force

### Ex2. Comparison of FREQ Job
With 8 cores (OMP_NUM_THREADS=8 or orcaproc=8). For ORCA5, num freq=true. 
with grrm2xtb [test1_min_freq/test1_freq.log]: 1.0 sec
with ORCA5 [test1_min_freq/test1_freq_orca.log]: 38.0 sec

### Ex.3 Comparison of Parallelized Single-threaded MC-AFIR Job
MC-AFIR seach for two small organic molecules (total 24 atoms) with NoFC Option (GFN2/gas).
Run 20 process with 1 core/process in 1 hour (-p20 -s3600).
with grrm2xtb [test4_mc/test4.log]: 1216 runs, 314632 forces, (0.011 sec/force).
with ORCA5 [test4_mc/test4_orca.log]: 874 runs, 256333 forces, (0.014 sec/force).

### Ex4. Large Molecules
MIN of organic molecule with 252 atoms with NoFC option (GFN2/gas).
With 8 cores (OMP_NUM_THREADS=8 or orcaproc=8). 
with grrm2xtb [test5_large/test5_min.log]: 2876.0 sec / 619 forces = 4.65 sec/force
with ORCA5 [test5_large/test5_min_orca.log]: 3005.0 sec / 641 forces = 4.69 sec/force


Even for single-threaded jobs (orcaproc=1/OMP_NUM_THREADS=1), we got averaged 20% speed-up by using grrm2xtb, which may be attributed to a larger overhead with ORCA5's parent process before calling otool_xtb (Ex.3).

The overhead is more significant when xtb force calculation takes less time (Ex.1), but not important when the system is enough large (Ex.4).
