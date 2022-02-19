# NOCI-Parser
This program parses the NOCI output for Chronus Quantum and writes out a summary of the data.

# INSTALL
Assuming you are in the source directory

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Release ..

# Commandline arguements

parseNOCI -f filename.out 



-f {filename}                            Name of the file to parse

-ot {0.00} or --oscThreshold {0.00}      Threshold for screening NOCI state output

-wt {0.01} or --weightThreshold {0.01}   Threshold for screening Determinant weights

-v or --verbose                          Additional log information will be print to stderr
 
