#!/bin/bash
#
# testing.sh   Gretchen Kerfoot    Spring 2025
#
#This script tests the querier program by running a series of tests:
#   Tests for one-word queries
#   Tests for implicit and explicit ANDs
#   Tests AND/OR precedence
#   Tests for invalid and empty queries
#   Tests for multiple matches with the same score
#   Tests querier under valgrind for memory leaks

#establishing pageDirectory and indexFile 
PAGEDIR=../../shared/tse/output/crawler/pages-letters-depth-3
INDEXFILE=../../shared/tse/output/indexer/index-letters-3

#one word valid query
echo "Test 1: one-word query"
echo "Query: search"
./querier $PAGEDIR $INDEXFILE <<< "search"

#implicit AND valid query
echo "Test 2: implicit AND"
echo "Query: computational biology"
./querier $PAGEDIR $INDEXFILE <<< "computational biology"

#explicit AND valid query
echo "Test 3: explicit AND"
echo "Query: computational and biology"
./querier $PAGEDIR $INDEXFILE <<< "computational and biology"

#OR with AND precedence valid query
echo "Test 4: OR with AND precedence"
echo "Query: eniac or home and playground"
./querier $PAGEDIR $INDEXFILE <<< "eniac or home and playground"

#invalid characters
echo "Test 5: invalid character"
echo "Query: hello!"
./querier $PAGEDIR $INDEXFILE <<< "hello!"

#leading operator invalid query
echo "Test 6: leading operator"
echo "Query: and letter"
./querier $PAGEDIR $INDEXFILE <<< "and letter"

#trailing operator invalid query
echo "Test 7: trailing operator"
echo "Query: letter or"
./querier $PAGEDIR $INDEXFILE <<< "letter or"

#adjacent operators invalid query
echo "Test 8: adjacent operators"
echo "Query: letter and or one"
./querier $PAGEDIR $INDEXFILE <<< "letter and or one"

#empty line invalid query
echo "Test 9: empty line"
echo "Query: [empty]"
./querier $PAGEDIR $INDEXFILE <<< ""

#multiple matches w same score valid query
echo "Test 10: multiple matches with same score"
echo "Query: playground"
./querier $PAGEDIR $INDEXFILE <<< "playground"

#valgrind testing
echo "Valgrind test: memory check on valid queries"
valgrind --leak-check=full --error-exitcode=1 ./querier $PAGEDIR $INDEXFILE <<EOF
search
computational and biology
eniac or home
EOF

