
#pragma once

#include<fstream>
#include<string>
#include<memory>
#include<vector>
#include<regex>
#include<iostream>
#include<sstream>
#include<iomanip>
#include<stdexcept>
#include<cmath>

#include "nocistate.hpp"

// Global variable for printing log info
extern bool printLog;

// Entrance function that processes the input
void parseNOCI(const std::string& filename, std::shared_ptr<const double>& oThresh,
               std::shared_ptr<const double>& wThresh, std::shared_ptr<const size_t>& nSV );

// Final function that prints the output
void processOutput(const std::vector<NOCIState>& noci);

// Parsing functions
void parseInput(std::ifstream& inpFile, std::vector<NOCIState>& noci);
void parseWeights(std::ifstream& inpFile, std::vector<NOCIState>& noci );
void parseEnergy(std::ifstream& inpFile, std::vector<NOCIState>& noci );
void parseSingVals(std::ifstream& inpFile, std::shared_ptr<std::vector<std::vector<double>>> singVals );
void printSingVals(std::ostream& out, const NOCIState& noci, const size_t index);
size_t computeMaxString(std::shared_ptr<std::vector<std::string>> );
std::vector<std::vector<double>> parseMatrix(std::ifstream& inpFile, const size_t nDets );
std::shared_ptr<std::vector<std::string>> parseDets( std::ifstream& inpFile );
