#!/bin/bash
#
# testing.sh    Gretchen Kerfoot    Spring 2025
#
# This testing script runs multiple tests for the TSE indexer.
# It verifies the correct operation of both indexer and indextest programs.
#
# The script performs the following:
#   - Runs indexer on a sample crawler output directory
#   - Runs indextest to copy the generated index
#   - Compares the original and copied index files using indexcmp
#   - Produces output suitable for review
#
# Usage:
#   Run this script from the indexer directory using `make test`
#   Output will be saved in testing.out


#Test 1: Run indexer on sample crawler output directory
echo "Test 1: Running indexer on letters-0 crawler output"
./indexer ../crawler/output/letters-0 index1

#checks if index1 file was created
if [ -f index1 ]; then
  echo "index1 created successfully"
else
  echo "index1 not created"
  exit 1
fi

#Test 2: Run indextest to load and save the index
echo "Test 2: Running indextest to copy index1 to index2"
./indextest index1 index2

#checks if index2 file was created
if [ -f index2 ]; then
  echo "index2 created successfully"
else
  echo "index2 not created"
  exit 1
fi

#Test 3: Compare the two index files using indexcmp
echo "Test 3: Comparing index1 and index2 using indexcmp"
~/cs50-dev/shared/tse/indexcmp index1 index2

