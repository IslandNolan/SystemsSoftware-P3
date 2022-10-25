# SystemsSoftware-P3 - Fall 2022

## Symbol Table - SIC Assembler Pass 2: 


## Project Description:
This Project Produces a .lst, and .obj file from a Symbol Table that is created using the previous project.

The Generated Files are stored within the same directory as the file you passed as an argument. In order to maintain compliance with C++11, I have chosen to forgo the use of std::filesystem (which would allow me to place the file anywhere in the current Directory)

There are multiple internal changes done to this project. They are listed below:

- Text Formatting
- Removal of several unused #defines
- Inclusion of new methods/tweaks to exiting structures.
- Removal of several #includes, they are no longer needed.
- Removal of the initializeSymbolTable method. This is accomplished with calloc instead.
- Changes to several pre-defined methods including their return types, and parameters.



## Experiences:

I had no trouble completing the project within the specified amount of time.

## Extra Credit:
The Extra Credit as outline in the project description is completed. Two Files (testX.obj, and  testX.lst) are created
at project run-time. These file have been verfieid as accurate, and are content complete.

## Shortcomings:

This project has no shortcomings, Everything functions exactly as intended.

## Improvements:

I feel that the project is adequate in instruction, and provided material. No changes are required.

## Compile Instructions:

Required Dependencies:

- Cmake
- G++
- Glibc

To Compile:

- Navigate to the root of the project, and run:
- ```cmake -B build && cd build && make && mv P3 ../ && cd ..```

The project expects at a minimum ONE argument, which is the file you wish to parse. Any other parameters after the first will be ignored.

**You may need to clear the Cmake Build Cache for the previous build in order for this to successfully compile.**

