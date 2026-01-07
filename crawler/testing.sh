#!/bin/bash
#
# testing.sh - Gretchen Kerfoot - Spring 2025
#
# This script tests the crawler program by running a series of tests:
#   Tests for invalid command-line arguments
#   Tests for invalid seedURL and pageDirectory
#   Tests for invalid maxDepth values
#   Tests for small valid crawls at different depths
#   Tests crawler under valgrind for memory leaks

#Invalid Argument Testing
echo ""
echo "Testing invalid arguments"

#no arguments
./crawler
#too few arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters
#too many arguments
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 1 extra

#Invalid seedURL Testing
echo ""
echo "Testing invalid seedURL"

#invalid (non-internal) URL
./crawler http://example.com ../data/letters 1

#Invalid pageDirectory Testing
echo ""
echo "Testing invalid pageDirectory"

#directory does not exist
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html nonexistentdir 1

#Invalid maxDepth Testing
echo ""
echo "Testing invalid maxDepth"

#negative depth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters -1
#depth too large
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 11

#Valid Crawl Testing (letters site depth 0)
echo ""
echo "Testing valid crawl - letters site depth 0"

#valid crawl depth 0
if ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 0; then
    echo "letters depth 0 crawl successful"
else
    echo "letters depth 0 crawl failed"
fi

#Valid Crawl Testing (letters site depth 2)
echo ""
echo "Testing valid crawl - letters site depth 2"

#valid crawl depth 2
if ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2; then
    echo "letters depth 2 crawl successful"
else
    echo "letters depth 2 crawl failed"
fi
#Valgrind Testing w Small Crawl
echo ""
echo "Valgrind test - small crawl"

#runs valgrind on a small crawl
valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 0

echo "All tests completed!"
