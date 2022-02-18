
#include<fstream>
#include<string>
#include<memory>
#include<vector>
#include<regex>
#include<iostream>
#include<iomanip>
#include<stdexcept>
#include<cmath>
#include "nocistate.hpp"
#include "parseNOCI.hpp"

void parseNOCI(const std::string& filename, std::shared_ptr<const double>& oThresh, 
               std::shared_ptr<const double>& wThresh, std::shared_ptr<const size_t>& nSingVals ){

  if( printLog ) {
    std::cerr << "File name = " << filename << "\n";
    std::cerr << "State Oscillator Strength Printing Threshold = " << *oThresh << "\n";
    std::cerr << "Chirgwin-Coulson Weights Printing Threshold = " << *wThresh << "\n";
    std::cerr << "Number of Singular Values to Print = " << *nSingVals << "\n";
    std::cerr << "\n";
  }
  std::ifstream inpFile(filename,std::ifstream::in);
  std::shared_ptr<std::vector<std::string>> detNames = parseDets(inpFile);

  // Create Singular Values vector
  size_t nDets = detNames->size();
  std::shared_ptr<std::vector<std::vector<double>>> singVals = std::make_shared<std::vector<std::vector<double>>>();
  for( size_t i=0; i<nDets; ++i )
    singVals->emplace_back();

  // Create NOCI States vector
  std::vector<NOCIState> noci(nDets, NOCIState(detNames,oThresh,wThresh,nSingVals,singVals));

  // Parse the file and populate NOCIState data
  parseInput(inpFile,noci);

  // Print the Output report
  processOutput(noci);
}

std::shared_ptr<std::vector<std::string>> parseDets( std::ifstream& inpFile ){

  if( printLog ) std::cerr << "Parsing Dets" << std::endl;

  // Reset file to the begining
  inpFile.clear();
  inpFile.seekg(0);

  // Define regex patterns
  std::regex detPattern{"dets:", std::regex_constants::icase };
  std::regex endPattern{"={10}"};

  // Parse file and look for "dets:" then add all the lines
  // to return vector (detNames). Exit loop when found =========
  bool foundDets = false;
  std::shared_ptr<std::vector<std::string>> detNames = std::make_shared<std::vector<std::string>>();
  for( std::string line; std::getline(inpFile,line); ){
    std::smatch matchDets;
    std::smatch matchEnd;
    if( std::regex_search(line,matchDets,detPattern) ){
      foundDets = true;
      if( printLog ) std::cerr << "Found Dets\n" << std::endl;
      continue;
    }
    if( std::regex_search(line,matchEnd, endPattern) and foundDets ) break;
    if( foundDets ){
      detNames->push_back(line);
    }
  }
  if( not foundDets ) {
    throw std::runtime_error("Failed to find Dets");
  }
  return detNames;
}

void parseInput(std::ifstream& inpFile, std::vector<NOCIState>& noci){

  parseSingVals(inpFile,noci[0].getSingVals() );
  parseWeights(inpFile,noci);
  parseEnergy(inpFile,noci);

}

void parseSingVals(std::ifstream& inpFile, std::shared_ptr<std::vector<std::vector<double>>> singVals ){

  if( printLog ) std::cerr << "Parsing Singular Values" << std::endl;

  // Reset file to the begining
  inpFile.clear();
  inpFile.seekg(0);

  // Define Singular Value regex patterns
  std::regex singValPattern{"\\s*\\w*\\s*Singular Values:", std::regex_constants::icase };
  std::regex singValEnd{"\\s*Determinant of U", std::regex_constants::icase };
  std::regex elementPattern{"Element:\\s*(\\d+)\\s*(\\d+)"};

  // Loop over inpFile
  bool foundSV = false;
  bool foundEle = false;
  size_t row = 0;
  size_t col = 0;
  for( std::string line; std::getline(inpFile,line); ){
    std::smatch matchElement;
    if( std::regex_match(line,matchElement,elementPattern) ){
      row = std::stoul(matchElement[1])-1;
      col = std::stoul(matchElement[2])-1;
      foundEle = true;
    }
    if( col > 0 ) break;

    std::smatch matchSingVal;
    if( row != col and std::regex_match(line,matchSingVal,singValPattern) ){
      if( not foundEle ) throw std::runtime_error("Failed to find element index");
      if( row > (*singVals).size() ) {
        std::cerr << "Row = " << row << "\n";
        throw std::runtime_error("Row index is in the domain");
      }

      foundSV = true;
      size_t cnt = 0;
      for( std::string line2; std::getline(inpFile,line2); ){

        // exit loop at end of singular values
        std::smatch matchSingValEnd;
        if( std::regex_search(line2,matchSingValEnd,singValEnd) ) break;

        // loop over doubles in columns
        std::istringstream s(line2);
        double tmp;
        while(s>>tmp){
          (*singVals)[row].push_back(tmp);
          cnt++;
        }
      }
      foundEle = false;
    }
  }
  
  if( not foundSV ) {
    throw std::runtime_error("Failed to find Singular Values");
  }else if( printLog ) {
    std::cerr << "Found Singular Values\n" << std::endl;
  }
}

void parseWeights(std::ifstream& inpFile, std::vector<NOCIState>& noci ){

  if( printLog ) std::cerr << "Parsing Weights" << std::endl;
  // Reset file to the begining
  inpFile.clear();
  inpFile.seekg(0);

  // Define Chirgwin-Coulson Weights patterns
  std::regex ccwPattern{"\\s*Chirgwin-Coulson Weights:\\s*", std::regex_constants::icase };

  bool foundWeights = false;
  for( std::string line; std::getline(inpFile,line); ){

    // find Weights then begin parsing
    std::smatch matchCC;
    if( std::regex_match(line,matchCC,ccwPattern) ) {
      if( printLog ) std::cerr << "Found Weights\n" << std::endl;
      foundWeights = true;
      std::getline(inpFile,line);// throw away --------- line
      std::vector<std::vector<double>> matrix = parseMatrix(inpFile,noci.size());
      for( size_t i=0; i<noci.size(); ++i )
        noci[i].setCCWeights(matrix[i]);
    }
  }
  if( not foundWeights )
    throw std::runtime_error("Failed to find Chirgwin-Coulson Weights");
}

std::vector<std::vector<double>> parseMatrix(std::ifstream& inpFile, const size_t nDets ){
  if( printLog ) std::cerr << "Parsing Matrix" << std::endl;

  // Initialize matrix
  std::vector<std::vector<double>> matrix;
  for( size_t i=0; i<nDets; ++i )
    matrix.emplace_back(nDets,0.);

  // Define regex patterns
  std::regex indexPattern{"(\\s+\\d+)+\\s*"};
  std::regex elementPattern{"\\s*\\d+(\\s+-*\\d+.\\d+e\\+*-*\\d+)+\\s*", std::regex_constants::icase };
  std::regex endPattern{"\\s*-{10,}\\s*"};

  // Loop over lines of matrix
  bool foundIndices = false;
  bool foundElements = false;
  std::vector<size_t> indices;
  for(std::string line; std::getline(inpFile,line); ){

    // match column index headers
    std::smatch matchIndex;
    if( std::regex_match(line,matchIndex,indexPattern) ){

      foundIndices = true;
      indices.clear();
      std::istringstream c(line);
      size_t tmp;
      while(c>>tmp){
        indices.push_back(tmp);
      }
    }

    // match row and matrix elements
    std::smatch matchElements;
    if( std::regex_match(line,matchElements,elementPattern) ){
      if( not foundIndices ) throw std::runtime_error("Failed to find column indices in matrix");
      foundElements = true;
      std::istringstream c(line);
      size_t row;
      c >> row;
      --row;
      double tmp;
      size_t i=0;
      while( c>>tmp ){
        size_t col = indices[i]-1;
        matrix[col][row] = tmp;
        ++i;
      }
    }

    // match the end of the matrix
    std::smatch matchEnd;
    if( std::regex_match(line,matchEnd,endPattern) ) break;
  }
  if( not foundElements ) throw std::runtime_error("Failed to find elements in Matrix");
  return matrix;
}

void parseEnergy(std::ifstream& inpFile, std::vector<NOCIState>& noci ){

  if( printLog ) std::cerr << "Parsing Energy" << std::endl;
  // Reset file to the begining
  inpFile.clear();
  inpFile.seekg(0);

  // Define Singular Value regex patterns
  std::regex energyPattern{"\\s*NOCI Energies.*", std::regex_constants::icase };

  // Parse file and look for "NOCI Energies" 
  bool foundEnergy = false;
  for( std::string line; std::getline(inpFile,line); ){
    std::smatch matchEnergy;
    if( std::regex_match(line,matchEnergy,energyPattern) ){
      foundEnergy = true;
      if( printLog ) std::cerr << "Found Energies\n" << std::endl;
      continue;
    }
    if( foundEnergy ){
      for( auto& n : noci ){
        std::istringstream c(line);
        double tmp;
        c >> tmp;
        n.setEnergy(tmp);
        c >> tmp;
        n.setFreq(tmp);
        c >> tmp;
        n.setOscStr(tmp);
        std::getline(inpFile,line);
      }
      break;
    }
  }
  if( not foundEnergy )
    throw std::runtime_error("Failed to find Energies/Frequencies/Oscillator Strengths");
}

void processOutput(const std::vector<NOCIState>& noci){

  // Print State Information then print NOCI
  size_t i = 0;
  for( auto& n : noci ){
    ++i;
    if( n.getOscStr() < n.getOscThresh() ) continue;
    std::cout << "NOCI State " << i << ":\n";
    std::cout << n;
  }
}

const std::string prefix = "    ";
std::ostream& operator<<(std::ostream& out, const NOCIState& noci ){

  // Print State information
  out << prefix << "Energy = " << std::setprecision(10) << std::setw(15) << std::fixed << noci.getEnergy() << "\n";
  out << prefix << "Frequency = " << std::setprecision(5) << std::setw(10) << std::fixed << noci.getFreq() << "\n";
  out << prefix << "Oscillator Strength = " << std::setprecision(3) << std::setw(10) << std::scientific << noci.getOscStr() << "\n";

  const double thresh = noci.getWeightThresh();
  const size_t prec = std::ceil(std::abs(std::log10(std::abs(thresh))));
  const size_t stringLen = computeMaxString(noci.getDetNames())+4;
  size_t cnt = 0;
  double sum = 0.;
  std::shared_ptr<std::vector<std::string>> detNames = noci.getDetNames();

  out << prefix << prefix;
  out << std::setw(10) << std::left << "Major Dets:";
  out << std::setw(stringLen) << std::left << "    Determinant File";
  out << std::setw(prec+6) << std::right << "Weights";
  if( noci.getNSingVals() > 0 ) out << prefix << std::setw(10) << "Singular Values";
  out << "\n";
  for( auto& cgw : noci.getCCWeights() ){
    if( cgw > thresh ){
      out << prefix << prefix;
      out << "Det #" << std::setw(5) << std::left << cnt+1;
      out << std::setw(stringLen) << std::left << (*detNames)[cnt];
      out << std::setw(prec+6) << std::setprecision(prec) << std::right << std::fixed << cgw;
      sum += cgw;
      printSingVals(out,noci,cnt);
      out << "\n";
    }
    ++cnt;
  }
  out << prefix << prefix << "Sum of Printed Weights = ";
  out << std::setw(prec+6) << std::setprecision(prec+1) << std::left << sum << "\n";
  out << "\n";
  return out;
}

size_t computeMaxString(const std::shared_ptr<std::vector<std::string>> detNames ){
  size_t max = 0;
  for( auto& dN : *detNames )
    if( dN.length() > max ) max = dN.length();
  return max;
}

void printSingVals( std::ostream& out, const NOCIState& noci, const size_t index ){

  const std::shared_ptr<std::vector<std::vector<double>>> singVal = noci.getSingVals();
  const size_t n = (*singVal)[index].size();
  size_t nPrint = n < noci.getNSingVals() ? n : noci.getNSingVals();
  out << prefix;
  if( nPrint > 0 ){
    for( size_t i=0; i<nPrint; ++i )
      out << std::setw(10) << std::setprecision(5) << std::fixed << (*singVal)[index][(n-nPrint)+i];
  }

}
