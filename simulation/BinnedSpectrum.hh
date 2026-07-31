// BinnedSpectrum.hh

#ifndef BinnedSpectrum_h
#define BinnedSpectrum_h 1

#include "G4SystemOfUnits.hh"
#include <vector>

// 声明能谱数据和参数，使用 extern 来指示它们在别处定义
extern std::vector<int> gEnergySpectrumData;
extern const G4double gMinEnergy;
extern const G4double gMaxEnergy;
extern const G4double gBinWidth;
extern const G4int gNumBins;

#endif