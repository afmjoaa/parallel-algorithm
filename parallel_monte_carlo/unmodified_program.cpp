/**
 * A reference sequential program for calculating the area under the curve within a fixed 2D grid range using the Monte-Carlo
 * estimation method. The idea of Monte-Carlo area estimation for complex curves is that we take a sorrounding rectangle and
 * generate many random points within that rectangle. Then we calculate if each random point fall inside the curve or outside.
 * If we do a lot of sampling in this manner then the portion of total points that fall inside the curve gives us a good
 * approximation of the area of the complex curve. That is, if the size of the rectangle is S and we do 10M random sampling
 * tests and 6.5M points we calculated to fall inside the curve then the estimated area of the curve is (S * (6.5/10)).
 *
 * If we want to parallelize this algorithm then the easiest way to do is to divide the rectangle into adjucent blocks. Then
 * let different threads do the sampling and partial area estimation for the curve inside that block. Then when the threads are
 * done, we can sum up their partial results in the main thread.
 *
 * @author Muhammad Nur Yanhaona
 * @email nur.yanhaona@bracu.ac.bd
 * @copyright restricted
 * */

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

using namespace std;

double getTimeForAreaEstimation(int iteration, long sampleCount);

/*
 * This is similar to type or class definition in other languages and also in C++. However C/CPP structure cannot have methods.
 * Here we need the structure to access the boundary of the rectangle.
 * */
typedef struct {
    int top;
    int left;
    int right;
    int bottom;
} Rectangle;

// this is our rectangle variable to indicate the grid dimension
Rectangle sorroundingShape;


/**
 * We use this structure to keep track of the coordinates of generated random points easily
 * */
typedef struct {
    int x;
    int y;
} Point;


/*
 * Since this is just a programming assignment, we will use the equation of an ellispe as our target complex curve, which is
 * actually not complex. The following method returns 1 if a point is inside an ellipse. Otherwise, it returns false. Note that
 * the equation for an ellipse is x^2/a^2 + y^2/b^2 - 1 = 0. We use a = 100 and b = 50.
 ***/
int isPointInsideTheCurve(Point point) {

    // note that the double type in C and C++ represents high precision fractional aka. real numbers.
    double x_square = point.x * point.x;
    double y_square = point.y * point.y;
    double position = (x_square / 10000) + (y_square / 2500) - 1;
    if (position <= 0.0) {
        return true;
    } else {
        return false;
    }
}


/**
 * Function for generating a random point within a rectangular bounding box
 * */
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


/**
 * This function does sampleCount number of Monte Carlo samping experiments within the area defined by the boundingBox to estimate
 * the area of our elliptic curve that falls inside the bounding box.
 * */
double estimateArea(Rectangle boundingBox, long sampleCount) {
    

    // using a dummy thread ID here for the main thread; you should use the proper thread ID in your parallel version
    int mainThreadId = 0;
    unsigned int threadIdSeed = mainThreadId;

    // initialize the area estimate variable;
    double estimate = 0.0;
    // also initialize a variable to count the number of points we found to be inside the curve
    long insidePoints = 0;

    // doing iteration count number of sampling experiments
    for (long i = 0; i < sampleCount; i++) {
        Point randomPoint = generateRandomPointInsideBox(boundingBox, &threadIdSeed);
        if (isPointInsideTheCurve(randomPoint)) {
            insidePoints++;
        }
    }
        
    std::cout << "total points inside: " << insidePoints << "\n";

    // estimate the area under the curve from the area of the bounding box
    double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
    double curveArea = (boxArea * insidePoints) / sampleCount;

    // return the area estimate for the curve
    return curveArea;
}

/***************************************************************************************************************************************
 *                        Sample codes to help the students
 * *************************************************************************************************************************************/

/*
 * The thread count global variable, a structure to pass information to threads during their creation, and a sample thread function that
 * does nothing. This is provided here so that students do not have to remember the boiler plate operation related to Pthreads handling.
 * */
int THREAD_COUNT = 8;

typedef struct {
    int threadId;

} ThreadArg;


void *threadFunction(void *arg) {
    
    ThreadArg *argument = (ThreadArg *) arg;
    int threadId = argument->threadId;
    printf("Thread %d : says hello\n", threadId);
    return NULL;
}


/***************************************************************************************************************************************
 *                             Sample Code End
 * *************************************************************************************************************************************/


int main(int argc, char *argv[]) {
    
    if (argc < 7) {
        std::cout << "To run the program you have to provide two things.\n";
        std::cout << "\tFirst the number of samples for the Monte Carlo sampling experiments.\n";
        std::cout << "\tSecond the bounding area within which the samples should be generated.\n";
        std::cout << "The format of using the program:\n";
        std::cout << "\t./program_name sample_count bottom_left_x, bottom_left_y, top_right_x, top_right_y\n";
        std::exit(EXIT_FAILURE);
    }

    // capture the user supplied sample count for the area estimation experiment
    long sampleCount = stol(argv[1]);
    std::cout << "SampleCount is: " << sampleCount << std::endl;
    

    // assigning the input parameters to the sorrounding shape object; we are interested in finding the area of the part of the
    // curve that is inside this shape.
    sorroundingShape.left = atoi(argv[2]);
    sorroundingShape.bottom = atoi(argv[3]);
    sorroundingShape.right = atoi(argv[4]);
    sorroundingShape.top = atoi(argv[5]);
    
    
    int number_of_iteration = atoi(argv[6]);
    double total_running_time = 0;
    
    for (int i = 0; i < number_of_iteration; i++) {
        total_running_time += getTimeForAreaEstimation(i, sampleCount);
    }
    
    printf("\n\nAverage time taken to estimate area of the curve for %d iterations is: %f seconds\n", number_of_iteration, (total_running_time/number_of_iteration));
    return 0;
}

double getTimeForAreaEstimation(int iteration, long sampleCount) {
    // starting execution timer clock
    struct timeval start;
    gettimeofday(&start, NULL);
    
    // initialize the random number generator;
    srand(time(NULL));

    /********************************************************************************************************************************
     *                        Sample codes to help the students
     ********************************************************************************************************************************/

    /**
     * A sample code for creating a desired number of threads and for waiting for their work completion.
     * */
    pthread_t *threads;
    ThreadArg *threadArgs;
    threads = (pthread_t *) malloc(THREAD_COUNT * sizeof(pthread_t));
    threadArgs = (ThreadArg *) malloc(THREAD_COUNT * sizeof(ThreadArg));
    for (int i = 0; i < THREAD_COUNT; i++) {
        threadArgs[i].threadId = i;
        pthread_create(&threads[i], NULL, threadFunction, (void *) &threadArgs[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    
    /********************************************************************************************************************************
     *                            Sample code ends
     ********************************************************************************************************************************/

    // perform the Monte Carlo area estimation experiment and print the result
    double curveArea = estimateArea(sorroundingShape, sampleCount);
    std::cout << "The area estimate with " << sampleCount << " samples is: " << curveArea << "\n";
    
     
    //-------------------------------- calculate running time
    struct timeval end;
    gettimeofday(&end, NULL);
    double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0)
                    - (start.tv_sec + start.tv_usec / 1000000.0));
    std::cout << "Execution Time: " << runningTime << " Seconds, for iteration -> " << iteration << std::endl << std::endl;
    
    return runningTime;
}
