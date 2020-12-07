#!/usr/bin/python
import random
import string

def main():
	
	# create a string to hold lower case alphabet
	letters = string.ascii_lowercase 

	# create file objects to manipulate opened/created files
	f1 = open("file1.txt", "w")
	f2 = open("file2.txt", "w")
	f3 = open("file3.txt", "w")

	# put file objects in list for easier manipulations
	files = {f1, f2, f3}

	# declare and initialize empty strings for file content
	fileContent1 = ""
	fileContent2 = ""
	fileContent3 = ""

	# fill strings with random lower-case alphabetic char
	for i in range(10):
		fileContent1 += (random.choice(letters))
		fileContent2 += (random.choice(letters))
		fileContent3 += (random.choice(letters))

	# write random strings to files
	f1.write("%s\n" % fileContent1)
	f2.write("%s\n" % fileContent2)
	f3.write("%s\n" % fileContent3)

	# close all files for writing
	for file in files:
		file.close()

	# open files for reading
	f1 = open("file1.txt", "r")
	f2 = open("file2.txt", "r")
	f3 = open("file3.txt", "r")

	# put file objects in list for easier manipulation
	files = {f1, f2, f3}

	# for each open file, read content to a string and print to screen, close file
	for file in files:
		content = file.read()
		content = content.replace("\n", "")
		print(content)
		file.close()

	# declare and initialize two random ints with range 1 to 42
	randomInt1 = random.randint(1, 42)
	randomInt2 = random.randint(1, 42)

	# multiply random ints and store product
	randProduct = randomInt1 * randomInt2

	# print random ints and their product to the screen
	print(randomInt1)
	print(randomInt2)

	print(randProduct)


main()
