import subprocess
import sys

if __name__ == '__main__':
    total_runtime = 0
    sampleCount = int(sys.argv[1])
    number_of_iteration = int(sys.argv[2])
    with open('result.txt', 'a') as f:      
        f.write(f'Sample count {sampleCount} iteration count -> {number_of_iteration}\n\n')

    for i in range(0, number_of_iteration):
        p = subprocess.run(f"mpirun -np 8 --host localhost:8 ./a.out {sampleCount} 0 0 100 100 >> result.txt", stdout=subprocess.PIPE, shell=True, text=True)
        with open('result.txt', 'r') as f:
            this_runtime = float(f.readlines()[-2])
            total_runtime += this_runtime

    with open('result.txt', 'a') as f:      
        f.write(f'Average runtime for the MPI program for {number_of_iteration} iteration is -> {total_runtime/number_of_iteration}')   
   
    print(f'Average runtime for the MPI program for {number_of_iteration} iteration is -> {total_runtime/number_of_iteration}')

    