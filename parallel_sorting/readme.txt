ASSIGNMENT:
First make the script for random array generator executable by running the following command
	
	chmod a+x random-array-generator.sh

Then geneate the random array by running the following command

	./random-array-generator.sh count filename

Then you will have the desired number of random integers in the file given by the file name. You 
should generate a large file with a lot of random numbers, for example 5000 numbers.

Then first compile the program without any optimization flag enabled by running the following command
	
	g++ sort.cpp -o unoptimized.o

unoptimized.o is the executable file. You can run it using the following command

./unoptimized.o input-file-name count-of-the-number-of-integers-to-sort

The program allow you to sort less than the total elements you have in the random number file you 
previously generated.

Run the program for a large input array. For example, for 10,000 elements. You should run the program
at least 5 times and then take the average of running time.



RESULT:
To complile the optimized_sort.cpp please run the following command

	g++ optimized_sort.cpp -o optimized_sort.o

optimized_sort.o is the executable file. We can run it using the following command

	./optimized_sort.o input-file-name count-of-the-number-of-integers-to-sort number-of-iteration-for-averaging
	example: ./optimized_sort.o random_array 10000 5 

Running the above example command will produce the following output:

	Average time taken to read the input for 5 iterations is: 0.000578 seconds

	Average time taken to sort the array for 5 iterations is: 0.000926 seconds

	Average time taken to print the sorted array for 5 iterations is: 0.000782 seconds 

	
