Program Py – CS 344
Overview
In this assignment, you will be given ZERO instruction in HOW to accomplish it! The actual requirements are very simple to satisfy. This is a test of how well you can research the topic and satisfy the conditions of this Assignment on your own. This is how much of the real world works - you'll be thrown into situations that are full of unknowns, and you'll need to come up with a solution!

You are becoming a computer scientist, not a technician: you will learn how to handle ANY language thrown at you, not just the ones you are spoon-fed, by learning good research habits and techniques. The more problems you encounter and solve, the greater problem set you'll be able to infer solutions to from your background of knowledge.

This is by far the easiest assignment in this course - it's worth the least points, with the most time to accomplish it. You will use what you've learned here in later courses at OSU.

Specifications
For this assignment, you will create a script in the Python language. You can read more about Python here:

http://www.python.org/ (Links to an external site.)

All execution, compiling, and testing of this script should ONLY be done in the bash prompt on our class server!

Your script must satisfy the following requirements:

Be contained in one single file, called "mypython.py".
When executed, create 3 files in the same directory as your script, each named differently (the name of the files is up to you), which remain there after your script finishes executing. Each of these 3 files must contain exactly 10 random characters from the lowercase alphabet, with no spaces ("hoehdgwkdq", for example). The final (eleventh) character of each file MUST be a newline character. Additional runs of the script should not append to the files. Thus, running wc (wordcount) against your files in the following manner must return 11:
$ cat myfile
gkwjhcfikf
$ wc -c myfile
11 myfile
When executed, output sent to stdout should first print out the contents of the 3 files it is creating in exactly the format given below.
After the file contents of all three files have been printed, print out two random integers each on a separate line (whose range is from 1 to 42, inclusive).
Finally, on the last (sixth) line, print out the product of the two numbers.
