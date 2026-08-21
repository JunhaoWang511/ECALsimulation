// 公共数据结构：与模拟输出 root 文件 'ecal' 树的分支一一对应
#ifndef ANALYSIS_COMMON_H
#define ANALYSIS_COMMON_H

#include <vector>
#include "TString.h"

typedef struct
{
    TString *PrimaryParticle = nullptr;
    double PrimaryEnergy;
    double PrimaryTime;
    double PrimaryPosition[3];
    double PrimaryDirection[3];
    double EnergyDeposition;
    double Waveform[100];
    double WaveformNoise[100];
    std::vector<double> *PhotonEnergy = nullptr;
    std::vector<double> *PhotonGlobalTime = nullptr;
    std::vector<double> *PhotonLocalTime = nullptr;
    std::vector<double> *PhotonPositionX = nullptr;
    std::vector<double> *PhotonPositionY = nullptr;
    std::vector<double> *PhotonPositionZ = nullptr;
    int EventID;
    int RunID;
    int PhotonGeneration;
    int PhotonWLSConversion;
    int PhotonSelfAbsorption;
    int PhotonBoundaryAbsorption;
    int PhotonDetection;
    int PhotonEnergyHis[1000];
    int PhotonGlobalTimeHis[5000];
} DATA_STRUCT;


#endif
