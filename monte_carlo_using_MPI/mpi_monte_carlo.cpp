#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <sstream>
#include <iostream>


typedef struct {
    int top;
    int left;
    int right;
    int bottom;
} Rectangle;

typedef struct {
    int x;
    int y;
} Point;

Rectangle sorroundingShape;

int isPointInsideTheCurve(Point point) {
    double x_square = point.x * point.x;
    double y_square = point.y * point.y;
    double position = (x_square / 10000) + (y_square / 2500) - 1;
    if (position <= 0.0) {
        return true;
    } else {
        return false;
    }
}

Point generateRandomPointInsideBox(Rectangle box, unsigned int *seed) {

    int horizontal = (box.right - box.left);
    int vertical = (box.top - box.bottom);
                
    // random point generation; the modulus then adding the left and bottom bound push the arbitrary random point within the
    // bounding box
    int x = (rand_r(seed) % horizontal) + box.left;
    int y = (rand_r(seed) % vertical) + box.bottom;

    // generate and return the point
    Point randomPoint;
    randomPoint.x = x;
    randomPoint.y = y;
    return randomPoint;
}

long countInsideTargetAreaPoints(int rankId, Rectangle boundingBox, long sampleCount) {
    
    // using a rankID to create rankIdSeed
	srand(rand());
    unsigned int rankIdSeed = rankId + (unsigned int)rand();

    // also initialize a variable to count the number of points we found to be inside the curve
    long insidePoints = 0;

    // doing iteration count number of sampling experiments
    for (long i = 0; i < sampleCount; i++) {
        Point randomPoint = generateRandomPointInsideBox(boundingBox, &rankIdSeed);
        if (isPointInsideTheCurve(randomPoint)) {
            insidePoints++;
        }
    }
    //printf("Point inside target found by rank ->  %d is %ld \n", rankId, insidePoints);

    return insidePoints;
}

double estimateArea(long totalInsidePoints, Rectangle boundingBox, long sampleCount) {

    // estimate the area under the curve from the area of the bounding box
    double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
    double curveArea = (boxArea * totalInsidePoints) / sampleCount;

    // return the area estimate for the curve
    return curveArea;
}

// function declarations
void reductionSum(int myRank, int procCount, long sampleCount);

int main(int argc, char** argv) {

	if (argc < 6) {
		printf("To run the program you have to provide three things.\n");
        printf("\tFirst the number of samples for the Monte Carlo sampling experiments.\n");
        printf("\tSecond the bounding area within which the samples should be generated.\n");	
        printf("The format of using the program:\n");
        printf("\t./program_name sample_count bottom_left_x, bottom_left_y, top_right_x, top_right_y\n");
		exit(1);
    }

	long totalSampleCount = atol(argv[1]);
    sorroundingShape.left = atoi(argv[2]);
    sorroundingShape.bottom = atoi(argv[3]);
    sorroundingShape.right = atoi(argv[4]);
    sorroundingShape.top = atoi(argv[5]);

	// starting execution timer clock
	struct timeval start;
	gettimeofday(&start, NULL);
	
	

	MPI_Init(NULL, NULL);

	int procCount;
    MPI_Comm_size(MPI_COMM_WORLD, &procCount);

	int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	reductionSum(rank, procCount, totalSampleCount);

	MPI_Finalize();

	//-------------------------------- calculate running time -------------------------------------------
	if (rank == 0) {
    	struct timeval end;
    	gettimeofday(&end, NULL);
    	double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0) - (start.tv_sec + start.tv_usec / 1000000.0));
		printf("Average time taken for one iteration is -> %f seconds\n", runningTime);
		printf("%f", runningTime);
		printf("\n\n");
	}

    return 0;
}


/**
 * This function illustrates how the sum of output of all processes can be collected in a single process directly using a
 * reduction primitive.
 * */
void reductionSum(int myRank, int procCount, long totalSampleCount) {

	long sampleCount = totalSampleCount/procCount;

    long insidePoints = countInsideTargetAreaPoints(myRank, sorroundingShape, sampleCount);

	long reducedSumOfInsidePoints = 0;
	int root = 0;
	int count = 1;

	MPI_Reduce(&insidePoints, &reducedSumOfInsidePoints, count, MPI_LONG, MPI_SUM, root, MPI_COMM_WORLD);

	if(myRank == 0){
		printf("Proc rank  -> %d  reducedSumOfInsidePoints -> %ld\n", myRank, reducedSumOfInsidePoints);
		double curveArea = estimateArea(reducedSumOfInsidePoints, sorroundingShape, totalSampleCount);
		printf("The estimated area is -> %lf unitSquare \n", curveArea);
	}
}

