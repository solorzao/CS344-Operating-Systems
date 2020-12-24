Rust Program: Multi-threaded Map Reduce

Program Specifications:

According to the Wikipedia article on MapReduce (Links to an external site.)

MapReduce is a programming model and an associated implementation for processing and generating big data sets with a parallel, distributed algorithm on a cluster.

A MapReduce program is composed of a map procedure, which performs filtering and sorting (such as sorting students by first name into queues, one queue for each name), and a reduce method, which performs a summary operation (such as counting the number of students in each queue, yielding name frequencies).

For this part of the assignment, we are providing you with a single-threaded Rust program (Links to an external site.) that processes input numbers to produce a sum. The program contains extensive comments that explain the functionality and give directions on what parts of code you are allowed to change (look for comments starting with CHANGE CODE).

Here is a description of the program: currently the main() function does the following

Generates data for the rest of the program
Calls generate_data() to generates a vector of numbers that serves as input for the rest of the program.
Partitions the data
Calls partition_data_in_two() which partitions the input numbers into two partitions
Performs the map step
Calls map_data() for each of the two partitions,  which returns the sum of all the numbers in that partition.
Performs the reduce step
Gathers the intermediate results produced by each call to map_data()
Calls reduce_data() that sums up the intermediate results produced by the map step to produce the final sum of all the input numbers.
You have to modify the program to accomplish the following tasks:

Modify the program to create 2 threads, each of which runs the map_data() function on one of the two partitions created by the program given to you.
Add code for the function partition_data() to partition the data into equal-sized partitions based on the argument num_partitions
Update August 10:
In case, num_elements is not a multiple of num_partitions, you can choose to implement one of two strategies
One partition is smaller than the others
If you have chosen this strategy, you can assume that the values of num_partitions and num_elements will be such that the smallest partition will still have at least one element.
Some partitions have one more element than other partitions
This choice will be simpler to code and makes more sense. We are leaving the 1st choice as an option because that was what the initial assignment description required and some of you may already have coded it.
Add code to the function main() to
Partition the data into equal-size partitions
Create as many threads as the number of partitions and have each thread run the map_data() function to process one partition each
Gather the intermediate results returned by each thread
Run the reduce step to process the intermediate results and produce the final result
