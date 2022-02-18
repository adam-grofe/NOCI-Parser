
#pragma once

class NOCIState{

  private:
    double oscStr = 0.;
    double freq = 0.;
    double energy = 0.;
    std::shared_ptr<std::vector<std::string>> detNames;
    std::shared_ptr<const double> oscThresh;
    std::shared_ptr<const double> weightThresh;
    std::shared_ptr<const size_t> nSingVals;
    std::shared_ptr<std::vector<std::vector<double>>> singVals;
    std::vector<double> ccWeights;

  public:
    // Constructor
    NOCIState( std::shared_ptr<std::vector<std::string>>& dN, std::shared_ptr<const double>& ot, 
               std::shared_ptr<const double>& wt, std::shared_ptr<const size_t>& nSV,
               std::shared_ptr<std::vector<std::vector<double>>>& sV):
      detNames(dN), oscThresh(ot),  weightThresh(wt), nSingVals(nSV), singVals(sV) {};

    // Setters
    inline void setOscStr( const double& o ){ oscStr = o; };
    inline void setFreq(   const double& f ){ freq   = f; };
    inline void setEnergy( const double& e ){ energy = e; };
    inline void setCCWeights( const std::vector<double>& ccw ){ ccWeights = ccw; };

    // Getters
    inline double getOscStr() const { return oscStr; };
    inline double getFreq() const { return freq; };
    inline double getEnergy() const { return energy; };
    inline std::shared_ptr<std::vector<std::string>> getDetNames() const { return detNames; };
    inline std::vector<double> getCCWeights() const { return ccWeights; };
    inline double getOscThresh() const { return *oscThresh; };
    inline double getWeightThresh() const { return *weightThresh; };
    inline std::shared_ptr<std::vector<std::vector<double>>> getSingVals() const { return singVals; };
    inline size_t getNSingVals() const { return *nSingVals; };
};

std::ostream& operator<<(std::ostream& out, const NOCIState& noci );

