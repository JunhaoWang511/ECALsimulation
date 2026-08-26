// FitWaveform：拟合波形文件中的波形，得到时间、能量信息并保存
// 用法: ./FitWaveform <input_withwaveform.root>
// 输出: 与输入同目录的 fitresult.root 和 time_resolution.png
#include <iostream>
#include <ctime>
#include <unistd.h>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLatex.h"
#include "TF1.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TLegend.h"
#include "Math/MinimizerOptions.h"
#include "common.h"

TTree *ReadTree(TFile *infile, DATA_STRUCT &Data)
{
    TTree *tr = (TTree *)infile->Get("ecal");
    tr->SetBranchAddress("RunID", &Data.RunID);
    tr->SetBranchAddress("EventID", &Data.EventID);
    tr->SetBranchAddress("PrimaryParticle", &Data.PrimaryParticle);
    tr->SetBranchAddress("PrimaryEnergy", &Data.PrimaryEnergy);
    tr->SetBranchAddress("PrimaryTime", &Data.PrimaryTime);
    tr->SetBranchAddress("PrimaryPosition", &Data.PrimaryPosition);
    tr->SetBranchAddress("PrimaryDirection", &Data.PrimaryDirection);
    tr->SetBranchAddress("PhotonGeneration", &Data.PhotonGeneration);
    // tr->SetBranchAddress("PhotonWLSConversion", &Data.PhotonWLSConversion);
    // tr->SetBranchAddress("PhotonSelfAbsorption", &Data.PhotonSelfAbsorption);
    // tr->SetBranchAddress("PhotonBoundaryAbsorption", &Data.PhotonBoundaryAbsorption);
    tr->SetBranchAddress("PhotonDetection", &Data.PhotonDetection);
    tr->SetBranchAddress("EnergyDeposition", &Data.EnergyDeposition);
    tr->SetBranchAddress("PhotonEnergyHis", &Data.PhotonEnergyHis);
    tr->SetBranchAddress("PhotonGlobalTimeHis", &Data.PhotonGlobalTimeHis);
    tr->SetBranchAddress("Waveform", &Data.Waveform);
    tr->SetBranchAddress("WaveformNoise", &Data.WaveformNoise);
    // tr->SetBranchAddress("PhotonEnergy", &Data.PhotonEnergy);
    // tr->SetBranchAddress("PhotonGlobalTime", &Data.PhotonGlobalTime);
    // tr->SetBranchAddress("PhotonLocalTime", &Data.PhotonLocalTime);
    // tr->SetBranchAddress("PhotonPositionX", &Data.PhotonPositionX);
    // tr->SetBranchAddress("PhotonPositionY", &Data.PhotonPositionY);
    // tr->SetBranchAddress("PhotonPositionZ", &Data.PhotonPositionZ);
    return tr;
}

void fit_single_waveform(double *wave, double *pars)
{
    // 拟合结果输出标志，拟合函数形状参数是否固定
    bool fitresult_output = false;
    bool fix_shape = true;
    static TLatex *tex = new TLatex();
    tex->SetNDC();
    // cc points of all range, dd points before peak
    int cc = 10, dd = 10;
    static TGraph *gr_wave = new TGraph(100);
    gr_wave->SetTitle("digitized waveform;time/ns;ADC value[AU]");
    static TF1 *f = new TF1("f", "x>[9] ? [10]*(([0] * exp(-(x-[9]) / [1]) + [2] * exp(-(x-[9]) / [3]) + [4] * exp(-(x-[9]) / [5]) + [6] * exp(-(x-[9]) / [7])) * pow((x-[9]),[8])) :0", -100, 1000);
    double parameters[11] = {9.1162e-06, 45.1368, -1.1769e-05, 33.2937, 2.44203e-05, 38.6403, -1.89478e-05, 39.3289, 3.00856, 65, 1};

    int MaxID = 0;
    double MaxAmp = 0;
    for (int j = 0; j < 40; j++)
    {
        if (wave[j] >= MaxAmp)
        {
            MaxAmp = wave[j];
            MaxID = j;
        }
    }
    // 设置时间参数初值，基于波形峰值位置
    parameters[9] = 12.5 * (MaxID - 10);
    parameters[10] = MaxAmp;
    f->SetParameters(parameters);
    // 固定拟合函数的形状参数
    if (fix_shape)
    {
        for (int i = 0; i < 9; i++)
        {
            f->FixParameter(i, parameters[i]);
        }
    }

    for (int j = 0; j < 100; j++)
    {
        gr_wave->SetPoint(j, j * 12.5, wave[j]);
    }
    gr_wave->SetMarkerStyle(8);
    gr_wave->SetMarkerSize(0.5);
    if (!fitresult_output)
        gr_wave->Fit(f, "QR", "", (MaxID - dd) * 12.5, (MaxID - dd + cc) * 12.5);
    else
    {
        gr_wave->Fit(f, "R", "", (MaxID - dd) * 12.5, (MaxID - dd + cc) * 12.5);
        // gr_wave->GetXaxis()->SetRangeUser((MaxID - dd) * 12.5 - 30, (MaxID - dd + cc) * 12.5 + 30);
        gr_wave->Draw("ap");
        tex->DrawLatex(0.5, 0.6, Form("T=%.2f#pm%.2f", f->GetParameter(9), f->GetParError(9)));
        gPad->Update();
        // gPad->SaveAs("digitized_waveform.png");
        sleep(1);
    }

    pars[0] = f->GetParameter(9);
    // calibrate factor from waveform peak to energy is 0.003827 (ch/MeV)
    pars[1] = f->GetMaximum() * 0.003827;
    pars[2] = f->GetChisquare();
}

void fit_waveform(TString filename)
{
    gStyle->SetOptFit(1111);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetTitleOffset(0.8, "X");
    gStyle->SetTitleOffset(1.0, "Y");
    gStyle->SetTitleSize(0.05, "XYZ");
    gStyle->SetLabelSize(0.04, "XYZ");
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

    ROOT::Math::MinimizerOptions MiniOpt;
    MiniOpt.SetDefaultMinimizer("Minuit", "Migrad");
    MiniOpt.SetDefaultErrorDef(1);
    MiniOpt.SetDefaultTolerance(1);
    MiniOpt.SetDefaultMaxFunctionCalls(1000);

    gRandom->SetSeed(time(nullptr));

    // 输出文件与输入文件同目录
    TString dir = filename;
    Int_t slash = dir.Last('/');
    if (slash != kNPOS)
        dir.Remove(slash + 1);
    else
        dir = "";
    TString fitroot = dir + "fitresult.root";
    TString pngname = dir + "time_resolution.png";

    TFile *infile = TFile::Open(filename.Data(), "READ");
    DATA_STRUCT Data;
    TTree *tr = ReadTree(infile, Data);
    int eventNum = tr->GetEntries();

    double parameter[3] = {0}, wave[100] = {0};
    TGraph *calibration_gr = new TGraph();
    TH1D *energy_his = new TH1D("energy_his", "hit energy distribution;energy/MeV;counts", 3000, 0, 3000);
    energy_his->SetDirectory(nullptr);
    TH1D *time_his = new TH1D("time_his", "hit time distribution;time/ns;counts", 2000, 0, 100);
    time_his->SetDirectory(nullptr);
    TH2D *ET_his = new TH2D("his", "hit energy-hit time;energy/MeV;time/ns", 3000, 0, 3000, 1000, 0, 100);
    ET_his->SetDirectory(nullptr);

    TFile *outfile = TFile::Open(fitroot.Data(), "RECREATE");
    TTree *tr_fit = new TTree("par_fit", "par_fit");
    tr_fit->SetDirectory(outfile);
    {
        tr_fit->Branch("EnergyDeposit", &Data.EnergyDeposition);
        tr_fit->Branch("TimeFit", &parameter[0]);
        tr_fit->Branch("AmplitudeFit", &parameter[1]);
        tr_fit->Branch("Chi2Fit", &parameter[2]);
    }

    for (int i = 0; i < eventNum; i++)
    {
        std::cout << "\rProcessing " << i << " events.";
        std::cout.flush();
        tr->GetEntry(i);
        // 筛选能量沉积占比过半的事例
        // if (Data.EnergyDeposition > 500)
        {
            fit_single_waveform(Data.WaveformNoise, parameter);
            parameter[0] -= Data.PrimaryTime;
            time_his->Fill(parameter[0]);
        }
        tr_fit->Fill();
    }

    new TCanvas();
    double mean_time = time_his->GetMean();
    double sigma_time = time_his->GetRMS();
    time_his->GetXaxis()->SetRangeUser(mean_time - 5 * sigma_time, mean_time + 5 * sigma_time);
    time_his->Draw();
    time_his->Fit("gaus", "QR", "", mean_time - 5 * sigma_time, mean_time + 5 * sigma_time);
    gPad->SaveAs(pngname.Data());

    std::cout << "time mean= " << time_his->GetFunction("gaus")->GetParameter(1) << " ns" << std::endl;
    std::cout << "time sigma= " << time_his->GetFunction("gaus")->GetParameter(2) * 1000 << " ps" << std::endl;

    infile->Close();
    outfile->Write();
    outfile->Close();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <input_withwaveform.root>" << std::endl;
        return 1;
    }
    fit_waveform(argv[1]);
    return 0;
}
