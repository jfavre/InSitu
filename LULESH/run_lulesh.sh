#!/bin/bash -l
#SBATCH --job-name="lulesh"
#SBATCH --account="csstaff"
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-core=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --partition=debug
#SBATCH --constraint=mc
#SBATCH --hint=nomultithread

module load daint-mc ParaView

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export VTK_SILENCE_GET_VOID_POINTER_WARNINGS=1

cp script.py buildCatalyst/bin/lulesh2.0 $SCRATCH
pushd $SCRATCH
srun ./lulesh2.0 -x script.py -s 30 -p

