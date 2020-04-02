# This is just a tool we can use to strip punctuation from a file.
# This helps us conform to Dr. C's "all non-alphabetic characters removed" condition.
from sys import argv

def main():
    if len(argv) != 3:
        print("Expected {} arguments; found {}".format(2, len(argv) - 1))
        print("Usage: python3 removePunctuation <inFile> <outFile>")
        exit()
        
    with open(argv[1], "r") as inFile:
        with open(argv[2], "w") as outFile:
            for line in inFile:
                line = line.replace(".", "").replace("[", "").replace("]", "").replace("'", "").replace("\"", "")
                line = line.replace(",", "").replace("-", "").replace("?", "").replace("!", "").replace(";", "")
                line = line.replace("(", "").replace(")", "").replace(":", "").replace("&", "")
                outFile.write(line)

main()