
#include<string>
#include<cstring>
#include<stdexcept>
#include "parseNOCI.hpp"


/*
 *    SUMMARY:
 *    This program parses the output file for an NOCI calculation
 *    then prints out a summary of the NOCI states information
 *    including the energy, oscillator strength, corresponding
 *    orbital singular values to the first determinant, and
 *    the Chirgwin-Coulson weights above a certain printing 
 *    threshold. There is also an oscillator strength threshold
 *    for printing state info.
 *
 *    PROGRAM FLAGS:
 *
 *    Necessary:
 *    -f {filename}.....Inputs the output file you wish to parse
 *
 *    Optional:
 *    -v or --verbose...........................Prints log information
 *    -wt {0.001} or --weightThreshold {0.001}..Determines the threshold for printing weights
 *    -ot {0.0  } or --oscThreshold {0.0  }.....Determines the threshold for printing a state
 *                                              according to the oscillator strength
 *    -n {3}  or --numberSingVal {3}............Number of singular values to print
 *
 */
bool printLog = false;

int main( int argc, char** argv ){

  std::string fileName;
  std::shared_ptr<size_t> nSingVal = std::make_shared<size_t>(3);
  std::shared_ptr<double> wThresh = std::make_shared<double>(1.E-2);
  std::shared_ptr<double> oThresh = std::make_shared<double>(0.);
  
  // Process arguments
  std::vector<std::string> args;

  if (argc > 1) {
    args.assign(argv + 1, argv + argc);
  }

  for( size_t i=0; i<args.size(); ++i ) {
    if( args[i] == "-f" ) fileName =  args[i+1];
    if( args[i] == "-wt" or args[i] == "--weightThreshold" ) *wThresh = std::stod(args[i+1]);
    if( args[i] == "-ot" or args[i] == "--oscThreshold" ) *oThresh = std::stod(args[i+1]);
    if( args[i] == "-v"  or args[i] == "--verbose"  ) printLog = true;
    if( args[i] == "-n"  or args[i] == "--numberSingVal" ) *nSingVal = std::stoi(args[i+1]);
  }

  if( fileName.empty() ) throw std::runtime_error("No Input file was given");

  // Cast pointers to const to prevent program from modifying values
  std::shared_ptr<const size_t> nP = std::const_pointer_cast<const size_t>(nSingVal);
  std::shared_ptr<const double> wP = std::const_pointer_cast<const double>(wThresh);
  std::shared_ptr<const double> oP = std::const_pointer_cast<const double>(oThresh);

  // Parse NOCI output file
  parseNOCI(fileName,oP,wP,nP);

  std::cerr << "Normal Termination\n";
}
