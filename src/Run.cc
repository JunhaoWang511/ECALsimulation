#include "Run.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::Run()
    : G4Run()
{
    fTotE = 0.0;
    fPhoGen = 0;
    fPhoGenScin = 0;
    fPhoGenCeren = 0;
    fPhoSelfAbs = 0;
    fPhoBounAbs = 0;
    fPhoEsc = 0;
    fPhoDet = 0;
    fPhoConWLS = 0;
    fPhoGenWLS = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Merge(const G4Run *run)
{
    const Run *localRun = static_cast<const Run *>(run);
    fTotE += localRun->fTotE;
    fPhoGen += localRun->fPhoGen;
    fPhoGenScin += localRun->fPhoGenScin;
    fPhoGenCeren += localRun->fPhoGenCeren;
    fPhoSelfAbs += localRun->fPhoSelfAbs;
    fPhoBounAbs += localRun->fPhoBounAbs;
    fPhoEsc += localRun->fPhoEsc;
    fPhoDet += localRun->fPhoDet;
    fPhoConWLS += localRun->fPhoConWLS;
    fPhoGenWLS += localRun->fPhoGenWLS;
    G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun()
{
    G4cout << "\n ======================== run summary ======================\n";

    G4int prec = G4cout.precision();

    G4double n_evt = (G4double)numberOfEvent;
    G4cout << "The run has " << numberOfEvent << " events." << G4endl << G4endl;

    G4cout.precision(4);

    G4double en = fTotE / n_evt;
    G4double pn = fPhoGen / n_evt;
    G4double pns = fPhoGenScin / n_evt;
    G4double pnc = fPhoGenCeren / n_evt;
    G4double psa = fPhoSelfAbs / n_evt;
    G4double pba = fPhoBounAbs / n_evt;
    G4double pes = fPhoEsc / n_evt;
    G4double pd = fPhoDet / n_evt;
    G4double pwlsc = fPhoConWLS / n_evt;
    G4double pwlsg = fPhoGenWLS / n_evt;
    G4cout << "Energy deposition per event:\t " << en / MeV << " MeV." << G4endl
           << "Photon generation per event:\t " << pn << " (scintillation: " << pns << " + Cherenkov: " << pnc << ")" << G4endl
           << "Photon generation by WLS per event:\t" << pwlsg << " (convertion: " << pwlsc << ")" << G4endl
           << "Photon self-absorption per event:\t " << psa << G4endl
           << "Photon boundary-absorption per event:\t " << pba << G4endl
           << "Photon escaption per event:\t " << pes << G4endl
           << "Photon detection per event:\t " << pd << G4endl << G4endl;
    G4cout.precision(prec);
}
