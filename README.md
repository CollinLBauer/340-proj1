Collin Bauer &nbsp; Terence Carey

# CSCI 340 - Operating Systems

This repository contains the code for each of our Spring 2020 projects.

Note that this repository was originally made for the first project, but has been extended for all of our projects. As such, the repo name is a little misleading, but was purposely left the same for ease of access.

---

## Project 1 - Linux Processes

### Steps to run
1. clone the repository repository
2. cd to `path/to/340-proj1/proj1`
3. run `make`
4. run `./proc`

---

## Project 2 - Paging Simulation

### Steps to run
1. clone the repository repository
2. cd to `path/to/340-proj1/proj2`
3. run `make`
4. set and export the environmnet variable `RANDOM_SEED` to any integer value.
5. run `./jobSim <mem-size> <page-size> <num-jobs> <min-time> <max-time> <min-mem> <max-mem>`
   - `<mem-size>` is the total size of memory
   - `<page-size>` is the size of each page
   - `<num-jobs>` is the total number of jobs to simulate
   - `<min-time>` is the minimum number of time slices each job may have
   - `<max-time>` is the maximum number of time slices each job may have
   - `<min-mem>` is the minimum memory each job may consume
   - `<max-mem>` is the maximum memory each job may consume

---

## Project 3 - Multi-Server Queue

This program expects a file to be piped to it through `stdin`, so it may seem to stall without this.

### Steps to run
1. clone the repository repository
2. cd to `path/to/340-proj1/proj3`
3. run `make`
4. run `./condInt <num-threads> < <file-to-read>`
   - `<num-threads>` is the number of consumer threads
   - `<file-to-read>` is a plaintext file passed through `stdin`

Also included is a couple of test files and a handy script, `removePunctuation.py`, which copies and strips out all punctuation marks from a plain text file. This helped us get thorough test cases.
