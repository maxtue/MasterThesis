#!/usr/bin/env python3
import argparse
import os

# get command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('-q', help='queueing type', default="local")
parser.add_argument('-n', help='name of runscript', default="test")
parser.add_argument('-o', help='number of output files', default=1)
parser.add_argument('-t', help='real time between output files', default=0.001)
args = parser.parse_args()
queueing_type = args.q
testname = args.n
num_outputs = args.o
time_output = args.t

# open and thereby name runscript
filename = "run_" + queueing_type + "_" + testname + ".sh"
f = open(filename, "w")

# sbatch script format for kamino and endor
if queueing_type == "sbatch":
    # head of sbatch scripts
    f.write("#!/bin/bash\n"
            "#SBATCH --partition=gpu\n"
            f"#SBATCH -J {testname}\n"
            "#SBATCH --gres=gpu:1\n"
            "#SBATCH --time=07-00\n\n")

# pbs script format for binac
elif queueing_type == "pbs":
    # head of pbs scripts
    f.write("#!/usr/bin/env bash\n"
            "#PBS -l walltime=720:00:00\n"
            "#PBS -l nodes=1:ppn=1:gpus=1:exclusive_process\n"
            "#PBS -q gpu\n\n"
            "cd $PBS_O_WORKDIR\n"
            "module load devel/cuda/10.0\n"
            "unset CUDA_VISIBLE_DEVICES\n"
            "LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/local/lib\n\n")

# no head
elif queueing_type == "local":
    pass

else:
    raise ValueError('Unknown queueing type')

# Create material_test.cfg in case of default testname
if testname == "test":
    f.write("## Creating test file from default\n"
            f"cp material.cfg material_{testname}.cfg\n\n")

# check if folder with simulation name exists
f.write("## Checking simulation folder\n"
        f"if [ -d {testname} ]; then\n"
        f"echo 'Directory {testname} already exists!'; exit 1;\n"
        "fi\n\n")

# create new folder for simulation
f.write("## Creating simulation folder\n"
        f"mkdir {testname} && cd {testname}\n"
        f"cp ../impact.0000 ../material_{testname}.cfg "
        f"../ANEOS.basaltm.table ../miluphcuda ../weibull ../create_xdmf.py .\n\n")

# weibulling particles
f.write("## Assigning flaws to particles according to weibull distribution\n"
        "./weibull -v -k 1e61 -m 16.0 -P -f impact.0000 -o "
        f"impact_damage_{testname}.0000 -n `wc -l impact.0000` -t 0\n\n")

# Starting miluphcuda
f.write(f"## Starting miluphcuda\n./miluphcuda -v -H -f "
        f"impact_damage_{testname}.0000 -m material_{testname}.cfg -n {num_outputs} "
        f"-t {time_output} -I rk2_adaptive -Q 1e-5 > output_{testname}.log 2> "
        f"error_{testname}.log\n\n")

# creating xdmf for paraview
f.write("## Creating xdmf from h5 files\n"
        f"./create_xdmf.py --input impact_damage_{testname}.*.h5 "
        f"--output parav_impact_damage_{testname}.xdmf\n\n")

# close file and save ls for writing times of output files
f.write("## Saving ls output with writing-times of files\n"
        "ls -ltrh > ls_output.log")
f.close()

# make file executable
os.chmod("./" + filename, 0o775)