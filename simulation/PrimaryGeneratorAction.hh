// ----------- PrimaryGeneratorAction.hh -----------
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"  // 【核心修复】：引入 Geant4 基础核心类型（G4double, G4int等），解决报错
#include <vector>

class G4ParticleGun;
class G4Event;

// =========================================================================
// 【全局变量 extern 声明】
// =========================================================================
extern G4double gCurrentIncidentEnergy;        // 当前事件的入射光子能量
extern std::vector<G4int> gHistIncidentCounts; // 基础入射能量分道计数器

// 新增：高阶数据统计容器声明
extern std::vector<G4double> gHistScintDepthEdep;       // 闪烁体深度剂量剖面
extern G4double gTotalScintEdepPhotoelectric;           // 闪烁体内光电效应总能量沉积
extern G4double gTotalScintEdepCompton;                 // 闪烁体内康普顿散射总能量沉积
extern std::vector<G4int> gHistTransmittedPhotonCounts; // 到达闪烁体表面的透射能谱通量
// =========================================================================

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

private:
    G4ParticleGun* fParticleGun;
};

#endif