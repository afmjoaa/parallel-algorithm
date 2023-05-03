#!/bin/bash
# this script takes an integer number as input and generates that many random integers then write them to
# a file that is also given as an input.
# Input 1: the number of random integers to generate
# Input 2: the name of the file where the integers should be written
#
#
#@author: Muhammad Nur Yanhaona
#@email: nur.yanhaona@bracu.ac.bd

# check if proper number of parameters have been given
if [ "$#" -ne 2 ]
then
  echo "Please supply all parameters"	
  echo "Usage: ./random-number-generator.sh count filename"
  exit 1
fi

# generate random numbers and dump them on a temporary file that has one number in each line
shuf -i 0-$1 -n $1 > .temp

# replace the newlines with whitespace to bring all numbers in a single line and dump them in the destination
# file
cat .temp | tr "\n" " " > $2

# insert a new line in the destination file at the end of random number array
echo "" >> $2

# remove executable permission from the output file
chmod a-x $2

# delete the temp file
rm .temp



