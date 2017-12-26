Ethan Schutzman && Steven Mcateer
    ehschutzman && smcateer

This is for Part1 & Part2 of Kernel Interceptor



Part 1 Description:

 Part 1 of this program is the On-Access Virus scanner. It opens the interceptor, 
reads it and scans a file for the word "VIRUS". It then writes to the syslog.



USAGE FOR PART 1 (part1.c):


1. Run the make command

2. sudo insmod part1.ko

3. Check the syslog for output




Part 2 Description: 

Part 2 of this program checks the pids of the parents, children, 
and siblings of processes. It then stores these values into a struct called ancestry which 
consists of three lists for pids. The program then prints these pids out of the lists and shows 
which are parents, children, and siblings.



USAGE FOR PART 2 (part2.c):


1. Run the make command

2. sudo insmod part2.ko

3. Run the ./test file to test the code




Sample output is shown in the respected files for each part. (part1SyslogOutput.txt and part2Output.txt)
