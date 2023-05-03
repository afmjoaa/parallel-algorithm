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

typedef struct {
    int top;
    int left;
    int right;
    int bottom;
} Rectangle;

Rectangle sorroundingShape;

typedef struct {
    int x;
    int y;
} Point;



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

double estimateArea(long totalInsidePoints, Rectangle boundingBox, long sampleCount) {

    // estimate the area under the curve from the area of the bounding box
    double boxArea = (boundingBox.right - boundingBox.left) * (boundingBox.top - boundingBox.bottom);
    double curveArea = (boxArea * totalInsidePoints) / sampleCount;

    // return the area estimate for the curve
    return curveArea;
}

long countInsideTargetAreaPoints(int threadId, Rectangle boundingBox, long sampleCount) {
    
    // using a threadId to create threadIdSeed
    unsigned int threadIdSeed = threadId + (unsigned int)time(NULL);

    // also initialize a variable to count the number of points we found to be inside the curve
    long insidePoints = 0;

    // doing iteration count number of sampling experiments
    for (long i = 0; i < sampleCount; i++) {
        Point randomPoint = generateRandomPointInsideBox(boundingBox, &threadIdSeed);
        if (isPointInsideTheCurve(randomPoint)) {
            insidePoints++;
        }
    }
    printf("Point inside target found by thread ->  %d is %ld \n", threadId, insidePoints);

    return insidePoints;
}

/***************************************************************************************************************************************
 *                        Sample codes to help the students
 * *************************************************************************************************************************************/

int THREAD_COUNT = 8;

typedef struct {
    int threadId;
    long sampleCount;
    long* insidePointsCountArrayPointer;
} ThreadArg;


void *threadFunction(void *arg) {
    
    ThreadArg *argument = (ThreadArg *) arg;
    int threadId = argument->threadId;
    long samplePoll = argument->sampleCount;
    
    long *insidePointsCountArrayPointer = argument->insidePointsCountArrayPointer;
    printf("Thread -> %d , samplePoll -> %ld \n", threadId, samplePoll);
    long insidePoints = countInsideTargetAreaPoints(threadId, sorroundingShape, samplePoll);
    *insidePointsCountArrayPointer = insidePoints;
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
    std::cout << "Total sample Count is -> " << sampleCount << std::endl << std::endl;

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
    printf("Starting iteration -> %d\n\n", iteration);
    
    // starting execution timer clock
    struct timeval start;
    gettimeofday(&start, NULL);

    pthread_t *threads;
    ThreadArg *threadArgs;
    long *insidePointsCountArray;
    
    threads = (pthread_t *) malloc(THREAD_COUNT * sizeof(pthread_t));
    threadArgs = (ThreadArg *) malloc(THREAD_COUNT * sizeof(ThreadArg));
    insidePointsCountArray = (long *) malloc(THREAD_COUNT * sizeof(long));
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        threadArgs[i].threadId = i;
        threadArgs[i].sampleCount = sampleCount/THREAD_COUNT;
        threadArgs[i].insidePointsCountArrayPointer = &insidePointsCountArray[i];
        pthread_create(&threads[i], NULL, threadFunction, (void *) &threadArgs[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
            pthread_join(threads[i], NULL);
    }

    
    long totalInsidePointCount = 0;
    for(int j = 0; j < THREAD_COUNT; j++) {
        long* jThreadInsidePointsCountPointer = &insidePointsCountArray[j];
        totalInsidePointCount += *jThreadInsidePointsCountPointer;
    }
    
    
    double curveArea = estimateArea(totalInsidePointCount, sorroundingShape, sampleCount);
    std::cout << "The estimated area is "<< curveArea << " unitSquare with samples " << sampleCount << " where " << totalInsidePointCount << " points were found inside target curve\n";
     
    
    free(threads);
    free(threadArgs);
    free(insidePointsCountArray);
    
    //-------------------------------- calculate running time -------------------------------------------
    struct timeval end;
    gettimeofday(&end, NULL);
    double runningTime = ((end.tv_sec + end.tv_usec / 1000000.0) - (start.tv_sec + start.tv_usec / 1000000.0));
    std::cout << "Execution Time -> " << runningTime << " Seconds, for iteration -> " << iteration << std::endl << std::endl;
    return runningTime;
}

