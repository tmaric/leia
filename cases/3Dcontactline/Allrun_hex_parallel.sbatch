#!/bin/bash
####---------------------------------------------------------------------------
####   Set SLURM / Job parameters
####---------------------------------------------------------------------------
### Choose a partition: 
### Set the account from which HPC resources are used. For now, this is
### fixed for our project.
#SBATCH -A special00004
### Set a name identifying the job
#SBATCH -J hex_parallel
### Do not send email notifcations, check job status with 'squeue`
#SBATCH --mail-type=NONE
#SBATCH --output=out.%A
#SBATCH --error=err.%A
### Set the number of processes. This is equivalent to the number of
### cores used usually
#SBATCH -n 4
### Set the amount of memory per core in megabyte
#SBATCH --mem-per-cpu=4000
### Set a run time limit for the job. This limit must not exceed the limit
### of the chosen partition, e.g. on test30m the runtime limit is
### 30 minutes.
### The format is hh:mm:ss
#SBATCH -t 24:00:00

####---------------------------------------------------------------------------
####    The actual command to submit as job 
####---------------------------------------------------------------------------

echo "START SLURM JOB"
echo "Current PWD: $PWD"
module purge

echo "LOAD ENVIRONMENT"
ml gcc/10.2.0 openmpi/4.1.4 python
source /work/groups/da_mma_b/OpenFOAM-v2212/etc/bashrc

echo "Modules:"
module list

echo "LD_LIBRARY_PATH : $LD_LIBRARY_PATH"
echo "PATH : $PATH"

echo "START OPENFOAM JOB"
export mpi_call="srun"
$SLURM_SUBMIT_DIR/Allrun_hex_parallel.sh

