#include "CondFormats/PPSObjects/interface/LHCInfoForPPS.h"
//#include "CondFormats/Common/interface/TimeConversions.h"
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdexcept>

LHCInfoForPPS::LHCInfoForPPS() {
  m_intParams.resize(ISIZE, std::vector<unsigned int>(1, 0));
  m_floatParams.resize(FSIZE, std::vector<float>(1, 0.));
}

LHCInfoForPPS::LHCInfoForPPS(const LHCInfoForPPS& rhs)
    : m_intParams(rhs.m_intParams),
      m_floatParams(rhs.m_floatParams) {}

LHCInfoForPPS::~LHCInfoForPPS() {}

namespace LHCInfoForPPSImpl {
  template <typename T>
  const T& getParams(const std::vector<T>& params, size_t index) {
    if (index >= params.size())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " is out of range.");
    return params[index];
  }

  template <typename T>
  T& accessParams(std::vector<T>& params, size_t index) {
    if (index >= params.size())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " is out of range.");
    return params[index];
  }

  template <typename T>
  const T& getOneParam(const std::vector<std::vector<T> >& params, size_t index) {
    if (index >= params.size())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " is out of range.");
    const std::vector<T>& inner = params[index];
    if (inner.empty())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " type=" + typeid(T).name() +
                              " has no value stored.");
    return inner[0];
  }

  template <typename T>
  void setOneParam(std::vector<std::vector<T> >& params, size_t index, const T& value) {
    if (index >= params.size())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " is out of range.");
    params[index] = std::vector<T>(1, value);
  }

  template <typename T>
  void setParams(std::vector<T>& params, size_t index, const T& value) {
    if (index >= params.size())
      throw std::out_of_range("Parameter with index " + std::to_string(index) + " is out of range.");
    params[index] = value;
  }

}  // namespace LHCInfoForPPSImpl

//getters
float const LHCInfoForPPS::crossingAngle() const { return LHCInfoForPPSImpl::getOneParam(m_floatParams, CROSSING_ANGLE); }

float const LHCInfoForPPS::betaStar() const { return LHCInfoForPPSImpl::getOneParam(m_floatParams, BETA_STAR); }

unsigned int const& LHCInfoForPPS::lumiSection() const { return LHCInfoForPPSImpl::getOneParam(m_intParams, LUMI_SECTION); }

//setters
void LHCInfoForPPS::setCrossingAngle(float const& angle) { LHCInfoForPPSImpl::setOneParam(m_floatParams, CROSSING_ANGLE, angle); }

void LHCInfoForPPS::setBetaStar(float const& betaStar) { LHCInfoForPPSImpl::setOneParam(m_floatParams, BETA_STAR, betaStar); }

void LHCInfoForPPS::setLumiSection(unsigned int const& lumiSection) {
  LHCInfoForPPSImpl::setOneParam(m_intParams, LUMI_SECTION, lumiSection);
}

//sets all values in one go
void LHCInfoForPPS::setInfo(float const& angle,
                      float const& beta,
                      unsigned int const& lumiSection) {
  this->setCrossingAngle(angle);
  this->setBetaStar(beta);
  this->setLumiSection(lumiSection);
}

void LHCInfoForPPS::print(std::stringstream& ss) const {
  ss << "Crossing angle (urad): " << this->crossingAngle() << std::endl
     << "Beta star (cm): " << this->betaStar() << std::endl
     << "Lumi section: " << this->lumiSection() << std::endl;
}

std::ostream& operator<<(std::ostream& os, LHCInfoForPPS beamInfo) {
  std::stringstream ss;
  beamInfo.print(ss);
  os << ss.str();
  return os;
}

bool LHCInfoForPPS::equals(const LHCInfoForPPS& rhs) const {
  if (m_isData != rhs.m_isData)
    return false;
  if (m_intParams != rhs.m_intParams)
    return false;
  if (m_floatParams != rhs.m_floatParams)
    return false;
  return true;
}

bool LHCInfoForPPS::empty() const { return m_intParams[0].empty(); }
