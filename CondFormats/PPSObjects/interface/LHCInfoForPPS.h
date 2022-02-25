#ifndef CondFormats_PPSObjects_LHCInfoForPPS_H
#define CondFormats_PPSObjects_LHCInfoForPPS_H

#include "CondFormats/Serialization/interface/Serializable.h"
#include <bitset>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class LHCInfoForPPS {
public:
  enum IntParamIndex {
    LUMI_SECTION = 0,
    ISIZE = 1
  };
  enum FloatParamIndex {
    CROSSING_ANGLE = 0,
    BETA_STAR = 1,
    FSIZE = 2
  };

  LHCInfoForPPS();
  LHCInfoForPPS(const LHCInfoForPPS& rhs);
  ~LHCInfoForPPS();

  //getters
  float const crossingAngle() const;

  float const betaStar() const;

  unsigned int const& lumiSection() const;

  //setters
  void setCrossingAngle(float const& angle);

  void setBetaStar(float const& betaStar);

  void setLumiSection(unsigned int const& lumiSection);

  //sets all values in one go
  void setInfo(float const& angle,
               float const& beta,
               unsigned int const& lumiSection);

  bool equals(const LHCInfoForPPS& rhs) const;

  bool empty() const;

  //dumping values on output stream
  void print(std::stringstream& ss) const;

private:
  bool m_isData = false;
  std::vector<std::vector<unsigned int> > m_intParams;
  std::vector<std::vector<float> > m_floatParams;

  COND_SERIALIZABLE;
};

std::ostream& operator<<(std::ostream&, LHCInfoForPPS lhcInfo);

#endif  // CondFormats_PPSObjects_LHCInfoForPPS_H
