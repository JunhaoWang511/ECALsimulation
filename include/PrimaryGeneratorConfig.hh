//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// *  The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// *  This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying  or distributing  the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorConfig_h
#define PrimaryGeneratorConfig_h 1

#include "globals.hh"

#include <atomic>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
/// \brief 跨线程共享的粒子源配置单例
///
/// MT 模式下每个 worker 线程有独立的 PrimaryGeneratorAction，
/// master 的 UI 命令无法直接改 worker 的成员变量。
/// 用进程级单例共享配置：messenger 写入，各 worker 读取。
/// 不能用 G4ThreadLocalSingleton，否则每线程一份副本，配置失效。
///
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorConfig
{
public:
    static PrimaryGeneratorConfig *Instance();

    void SetTimeSpread(G4bool a) { fTimeSpread.store(a); }
    G4bool GetTimeSpread() const { return fTimeSpread.load(); }

private:
    PrimaryGeneratorConfig();
    ~PrimaryGeneratorConfig() = default;
    PrimaryGeneratorConfig(const PrimaryGeneratorConfig &) = delete;
    PrimaryGeneratorConfig &operator=(const PrimaryGeneratorConfig &) = delete;

    std::atomic<G4bool> fTimeSpread;  ///< true: 初级粒子时间在 0-12.5 ns 内均匀随机
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
