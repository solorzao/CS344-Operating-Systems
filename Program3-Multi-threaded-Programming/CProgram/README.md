C Program: Multi-threaded Producer Consumer Pipeline
The program will read in lines of characters from the standard input and write them as 80 character long lines to standard output with the following changes:

Every line separator in the input will be replaced by a space
Every adjacent pair of plus signs, i.e., "++", is replaced by a "^".
Input
A “line of input” is
A sequence of the allowed characters (see below) that does not include a line separator
Followed by a line separator.
Allowed characters: Other than the line separator, the input will only consist of ASCII characters from space (decimal 32) to tilde (decimal 126). These are sometimes termed printable characters.
The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
An input line will never be longer than 1000 characters (including the line separator).
Your program doesn't need to check the input for correctness.
Your program should process input lines until it receives an input line that contains only the characters DONE (followed immediately by the line separator) at which point the program should terminate.
Examples: The following input lines should not terminate the program
DONE!
ALLDONE
Output
The “80 character line” to be written to standard output is defined as 80 non-line separator characters plus a line separator.

Whenever your program has sufficient data for an output line, the output line must be produced. Your program must not wait to produce the output only when the terminating line is received.

No part of the terminating line should be written to the output
Your program should output only lines with 80 characters (with a line separator after each line).
For example,

If your program has some input that has not been written because the program is waiting for more data in order to make an 80 character line for output and the terminating line is received from standard input,

Then the program should terminate without printing out the partial output line.

In addition, your program should not output any user prompts, debugging information, status messages, etc.
For the second replacement requirement, pairs of plus signs should be replaced as they are seen.

Examples:

The string “abc+++def” contains only one pair of plus signs and should be converted to the string "abc^+def".

The string “abc++++def” contains two pairs of plus signs and should be converted to the string "abc^^def".

Multi-threading Requirement
Your program will create 4 threads as follows:

Input Thread: This thread performs input on a line-by-line basis from standard input.
Line Separator Thread: This thread replaces line separators with blanks.
Plus Sign Thread: This thread performs the required replacement of pair of plus signs.
Output Thread: This thread writes output lines to the standard output.
Producer Consumer Pipeline
A pipeline of 4 threads. Input thread reads data from stdin and puts it in Buffer 1. Line Separator thread gets data from Buffer 1, processes it and puts it in Buffer 2. Plus Sign Thread reads data from Buffer 2, processes it and puts it in Buffer 3. Output Thread reads data from Buffer 3 and output lines to stdout.

Pipeline of threads that gets data from stdin, processes it and displays it to stdout

Each pair of communicating threads should be constructed as a producer/consumer system.
If a thread T1 gets its input data from another thread T0, and T1 outputs data for use by another thread T2, then
T1 acts as a consumer with respect to T0 and T0 plays the role of T1’s producer
T1 acts as a producer with respect to T2 and T2 plays the role of T1’s consumer
Thus each thread in the interior of the pipeline will contain both producer code and consumer code.
Each producer/consumer pair of threads will have its own shared buffer. Thus, there will be 3 of these buffers in your program, each one shared only by its producer and consumer.
You must use condition variables for coordination. Your program must never sleep.
Your program will never receive more than 10 input lines in any 5 second interval.
If you size your buffers to hold 10 lines of adequate size, you can model the problem as Producer-Consumer with unbounded buffers which will make your program simpler.
[Clarification added August 7] Adequate size = 1000 characters each
Recall that unbounded buffers are never full, though they can be empty.
