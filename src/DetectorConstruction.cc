#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "SteppingAction.hh"
#include "TMath.h"
#include "TString.h"
#include "globals.hh"

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(){}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    //
    // Material composition definition
    //
    fNistMan = G4NistManager::Instance();
    G4Material *air = fNistMan->FindOrBuildMaterial("G4_AIR");
    G4Material *Teflon = fNistMan->FindOrBuildMaterial("G4_TEFLON");
    G4Material *CsI = fNistMan->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    G4Material *WLS = fNistMan->FindOrBuildMaterial("G4_C-552");
    G4Material *SD = fNistMan->FindOrBuildMaterial("G4_Si");
    // Glass: window material is epoxy resin (C11H12O3)n
    G4Element *C = fNistMan->FindOrBuildElement("C");
    G4Element *H = fNistMan->FindOrBuildElement("H");
    G4Element *O = fNistMan->FindOrBuildElement("O");
    G4Material *Glass = new G4Material("EpoxyResin", 1.2 * g / cm3, 3);
    Glass->AddElement(C, 11);
    Glass->AddElement(H, 12);
    Glass->AddElement(O, 3);

    //
    // Material properties definition
    //
    G4double photonEnergy[] = {1.7714 * eV, 1.7971 * eV, 1.8235 * eV, 1.8507 * eV, 1.8788 * eV, 1.9077 * eV, 1.9375 * eV, 1.9683 * eV,
                               2.0000 * eV, 2.0328 * eV, 2.0667 * eV, 2.1017 * eV, 2.1379 * eV, 2.1754 * eV, 2.2143 * eV, 2.2545 * eV,
                               2.2963 * eV, 2.3396 * eV, 2.3846 * eV, 2.4314 * eV, 2.4800 * eV, 2.5306 * eV, 2.5833 * eV, 2.6383 * eV,
                               2.6957 * eV, 2.7556 * eV, 2.8182 * eV, 2.8837 * eV, 2.9524 * eV, 3.0244 * eV, 3.1000 * eV, 3.1795 * eV,
                               3.2632 * eV, 3.3514 * eV, 3.4444 * eV, 3.5429 * eV, 3.6471 * eV, 3.7576 * eV, 3.8750 * eV, 4.0000 * eV,
                               4.1333 * eV, 4.2759 * eV, 4.4286 * eV};

    const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

    // emission spectrum, refractive index and absorption length of undoped CsI
    G4double crystalEmSpec[] = {0.0034, 0.0033, 0.0036, 0.0029, 0.0047, 0.0046, 0.0045, 0.0055, 0.0068, 0.0065, 0.0071, 0.0077, 0.0085,
                                0.0087, 0.0103, 0.0107, 0.0110, 0.0120, 0.0129, 0.0126, 0.0140, 0.0144, 0.0134, 0.0124, 0.0129, 0.0140,
                                0.0121, 0.0127, 0.0127, 0.0127, 0.0155, 0.0182, 0.0272, 0.0423, 0.0737, 0.1297, 0.2369, 0.4121, 0.6443,
                                0.8914, 1.0000, 0.9236, 0.7050};
    assert(sizeof(crystalEmSpec) == sizeof(photonEnergy));
    G4double crystalRindex[] = {1.7736, 1.7746, 1.7756, 1.7766, 1.7777, 1.7789, 1.7801, 1.7813, 1.7827, 1.7841, 1.7856, 1.7872, 1.7888,
                                1.7906, 1.7925, 1.7945, 1.7966, 1.7989, 1.8013, 1.8039, 1.8067, 1.8097, 1.8129, 1.8164, 1.8201, 1.8242,
                                1.8286, 1.8334, 1.8386, 1.8443, 1.8506, 1.8576, 1.8654, 1.8740, 1.8836, 1.8945, 1.9069, 1.9211, 1.9374,
                                1.9565, 1.9791, 2.0062, 2.0395};
    // absorb length of CsI crystal (!undetermined pars!)
    G4double crystalAbsLen[] = {34.33 * 3 * cm, 34.01 * 3 * cm, 33.69 * 3 * cm, 33.07 * 3 * cm, 32.76 * 3 * cm, 32.31 * 3 * cm, 31.86 * 3 * cm, 31.43 * 3 * cm, 31.00 * 3 * cm,
                                30.58 * 3 * cm, 30.17 * 3 * cm, 29.63 * 3 * cm, 29.24 * 3 * cm, 28.85 * 3 * cm, 28.47 * 3 * cm, 27.97 * 3 * cm, 27.61 * 3 * cm, 27.13 * 3 * cm,
                                26.66 * 3 * cm, 26.21 * 3 * cm, 25.76 * 3 * cm, 25.65 * 3 * cm, 25.31 * 3 * cm, 24.78 * 3 * cm, 24.57 * 3 * cm, 24.05 * 3 * cm, 23.65 * 3 * cm,
                                23.25 * 3 * cm, 22.77 * 3 * cm, 22.38 * 3 * cm, 21.83 * 3 * cm, 21.29 * 3 * cm, 20.85 * 3 * cm, 20.41 * 3 * cm, 20.24 * 3 * cm, 20.07 * 3 * cm,
                                19.50 * 3 * cm, 18.93 * 3 * cm, 18.46 * 3 * cm, 17.70 * 3 * cm, 16.68 * 3 * cm, 16.19 * 3 * cm, 15.51 * 3 * cm};

    // rafractive index and absorption length of teflon
    G4double teflonRindex[] = {1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38,
                               1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38,
                               1.38, 1.38, 1.38, 1.38, 1.38, 1.38, 1.38};
    G4double teflonAbsLen[] = {200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m,
                               200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m,
                               200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m,
                               200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m, 200 * m};
    // material properties of teflon
    G4MaterialPropertiesTable *TeflonMPT = new G4MaterialPropertiesTable();
    TeflonMPT->AddProperty("RINDEX", photonEnergy, teflonRindex, nEntries);
    TeflonMPT->AddProperty("ABSLENGTH", photonEnergy, teflonAbsLen, nEntries);
    Teflon->SetMaterialPropertiesTable(TeflonMPT);

    // material properties of crystal
    G4MaterialPropertiesTable *CrystalMPT = new G4MaterialPropertiesTable();
    CrystalMPT->AddProperty("RINDEX", photonEnergy, crystalRindex, nEntries);
    CrystalMPT->AddProperty("ABSLENGTH", photonEnergy, crystalAbsLen, nEntries);
    CrystalMPT->AddProperty("FASTCOMPONENT", photonEnergy, crystalEmSpec, nEntries);
    CrystalMPT->AddConstProperty("SCINTILLATIONYIELD", 1880.0 / MeV);
    CrystalMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    CrystalMPT->AddConstProperty("FASTTIMECONSTANT", 25. * ns);
    CrystalMPT->AddConstProperty("YIELDRATIOL", 1.0);
    CsI->SetMaterialPropertiesTable(CrystalMPT);

    G4double ErefractiveIndex_air[2] = {0.4 * eV, 6.0 * eV};
    G4double refractiveIndex_air[2] = {1.00, 1.00};
    // material properties of air
    G4MaterialPropertiesTable *AirMPT = new G4MaterialPropertiesTable();
    AirMPT->AddProperty("RINDEX", ErefractiveIndex_air, refractiveIndex_air, 2);
    air->SetMaterialPropertiesTable(AirMPT);

    // material properties of glass
    G4double glass_AbsLength[2] = {420. * cm, 420. * cm};
    G4double glass_RIND[2] = {1.54, 1.54};
    G4MaterialPropertiesTable *GlassMPT = new G4MaterialPropertiesTable();
    GlassMPT->AddProperty("ABSLENGTH", ErefractiveIndex_air, glass_AbsLength, 2);
    GlassMPT->AddProperty("RINDEX", ErefractiveIndex_air, glass_RIND, 2);
    Glass->SetMaterialPropertiesTable(GlassMPT);

    // emission spectrum and absorption length of wavelength shifter(WLS) (!undetermined pars!)
    G4double wlsAbsLen[] = {10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm,
                            10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm, 10000 * cm,
                            9.27E-03 * cm, 2.85E-03 * cm, 9.27E-04 * cm, 4.88E-04 * cm, 2.99E-04 * cm, 2.02E-04 * cm, 1.50E-04 * cm,
                            1.27E-04 * cm, 1.18E-04 * cm, 1.16E-04 * cm, 1.24E-04 * cm, 1.40E-04 * cm, 1.73E-04 * cm, 2.23E-04 * cm,
                            2.99E-04 * cm, 4.21E-04 * cm, 6.08E-04 * cm, 8.06E-04 * cm, 7.57E-04 * cm, 2.99E-04 * cm, 9.13E-05 * cm,
                            4.02E-05 * cm, 2.77E-05 * cm, 2.38E-05 * cm, 2.62E-05 * cm, 3.27E-05 * cm, 4.51E-05 * cm};
    G4double wlsEmSpec[] = {0.1348, 0.1724, 0.2100, 0.2571, 0.3229, 0.3887, 0.4828, 0.5768, 0.6991, 0.8025, 0.8871, 0.9624,
                            1.0000, 1.0000, 0.9624, 0.8683, 0.7461, 0.5392, 0.3323, 0.1442, 0.0502, 0.0000, 0.0000, 0.0000,
                            0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
                            0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000};
    // refractive index of WLS (!undetermined!)
    G4double wlsRindex[] = {1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 
                            1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 1.6, 
                            1.6, 1.6, 1.6};
    // material properties of WLS
    G4MaterialPropertiesTable *wlsMPT = new G4MaterialPropertiesTable();
    // refractive index of WLS(!undetermined pars!)
    wlsMPT->AddProperty("RINDEX", photonEnergy, wlsRindex, nEntries);
    wlsMPT->AddProperty("WLSABSLENGTH", photonEnergy, wlsAbsLen, nEntries);
    wlsMPT->AddProperty("WLSCOMPONENT", photonEnergy, wlsEmSpec, nEntries);
    wlsMPT->AddConstProperty("WLSMEANNUMBERPHOTONS", 0.95);
    wlsMPT->AddConstProperty("WLSTIMECONSTANT", 7 * ns);
    WLS->SetMaterialPropertiesTable(wlsMPT);

    G4double photonEnergy1[] = {1.85 * eV, 1.9 * eV, 1.95 * eV,
                                2.0 * eV, 2.48 * eV, 2.96 * eV,
                                3.4 * eV, 3.88 * eV, 4.36 * eV};
    const G4int nEntries1 = sizeof(photonEnergy1) / sizeof(G4double);
    G4double tr[] = {1.38, 1.38, 1.38,
                     1.38, 1.38, 1.38,
                     1.38, 1.38, 1.38};
    G4double refle[] = {0.97, 0.97, 0.97,
                        0.97, 0.97, 0.97,
                        0.97, 0.97, 0.97};
    G4double specularlobe[] = {0.25, 0.25, 0.25,
                               0.25, 0.25, 0.25,
                               0.25, 0.25, 0.25};
    G4double specularspike[] = {0.7, 0.7, 0.7,
                                0.7, 0.7, 0.7,
                                0.7, 0.7, 0.7};
    G4double backscatter[] = {0.05, 0.05, 0.05,
                              0.05, 0.05, 0.05,
                              0.05, 0.05, 0.05};

    assert(sizeof(refle) == sizeof(photonEnergy1));

    // optical surface between crystal and air
    G4MaterialPropertiesTable *cryairsp = new G4MaterialPropertiesTable();
    cryairsp->AddProperty("REFLECTIVITY", photonEnergy1, refle, nEntries1);
    cryairsp->AddProperty("RINDEX", photonEnergy1, tr, nEntries1);
    cryairsp->AddProperty("SPECULARLOBECONSTANT", photonEnergy1, specularlobe, nEntries1);
    cryairsp->AddProperty("SPECULARSPIKECONSTANT", photonEnergy1, specularspike, nEntries1);
    cryairsp->AddProperty("BACKSCATTERCONSTANT", photonEnergy1, backscatter, nEntries1);

    G4OpticalSurface *wlstefSurface = new G4OpticalSurface("WLSTefSurface");
    wlstefSurface->SetType(dielectric_dielectric);
    wlstefSurface->SetModel(unified);
    wlstefSurface->SetFinish(groundbackpainted);
    wlstefSurface->SetSigmaAlpha(0.2);
    wlstefSurface->SetMaterialPropertiesTable(cryairsp);

    // optical surface between crystal and WLS
    G4OpticalSurface *crywlsSurface = new G4OpticalSurface("CryWLSSurface");
    crywlsSurface->SetType(dielectric_dielectric);
    crywlsSurface->SetModel(unified);
    crywlsSurface->SetFinish(ground);
    crywlsSurface->SetSigmaAlpha(0.2);

    // optical surface between crystal and APD window
    G4OpticalSurface *crywinSurface = new G4OpticalSurface("CryWinSurface");
    crywinSurface->SetType(dielectric_dielectric);
    crywinSurface->SetModel(unified);
    crywinSurface->SetFinish(polished);

    // optical sensitive surface of APD cathode
    G4OpticalSurface *sdSurface = new G4OpticalSurface("SDSurface");
    sdSurface->SetType(dielectric_dielectric);
    sdSurface->SetFinish(polishedfrontpainted);
    sdSurface->SetModel(unified);

    G4double Efficiency[] = {0.8577, 0.8577, 0.8577, 0.8577, 0.8577, 0.8577, 0.8577, 0.8577, 0.8548, 0.8548, 0.8519, 0.8519,
                             0.8490, 0.8462, 0.8433, 0.8375, 0.8317, 0.8288, 0.8231, 0.8173, 0.8115, 0.8029, 0.7942, 0.7827,
                             0.7712, 0.7596, 0.7452, 0.7279, 0.7077, 0.6846, 0.6500, 0.6067, 0.5692, 0.5346, 0.5144, 0.4971,
                             0.4798, 0.4596, 0.4365, 0.4134, 0.3903, 0.3672, 0.3441};
    G4double Reflectivity[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    assert(sizeof(Reflectivity) == sizeof(photonEnergy));
    G4MaterialPropertiesTable *CryDetMPT = new G4MaterialPropertiesTable();
    CryDetMPT->AddProperty("REFLECTIVITY", photonEnergy, Reflectivity, nEntries);
    CryDetMPT->AddProperty("EFFICIENCY", photonEnergy, Efficiency, nEntries);
    sdSurface->SetMaterialPropertiesTable(CryDetMPT);

    //
    // World volume definition
    //
    G4double fWorldSize = 5 * m;
    G4Box *solidWorld = new G4Box("World", fWorldSize / 2, fWorldSize / 2, fWorldSize / 2);
    G4LogicalVolume *logicalWorld = new G4LogicalVolume(solidWorld, air, "World");
    G4VPhysicalVolume *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicalWorld, "World", 0, false, 0);
    logicalWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    //
    // Basic geometry parameter
    //
    G4double th = 28 * cm;     // length of CsI crystal(along z-axis)
    G4double ath = 0.21 * mm;  // thickness of APD
    G4double gth = 0.01 * mm;  // thickness of photo-sensitive area
    G4double ttef = 0.1 * mm;  // thickness of teflon
    G4double twls = 0.03 * mm; // thickness of WLS

    //
    // Stave definition
    //
    G4double ta, tc, th1, th2, th3, th4, talpha1, talpha2, ttheta, tphi;
    ta = 4 * cm;  // length of front-end face(at - z-axis) of crystal(along y-axis)
    tc = 5 * cm;  // length of back-end face(at + z-axis) of crystal(along y-axis)
    th1 = 4 * cm; // length of front-end face of crystal(along x-axis, at smaller y-side)
    th2 = th1;    // length of front-end face of crystal(along x-axis, at bigger y-sise)
    th3 = 5 * cm; // length of back-end face of crystal(along x-axis, at smaller y-side)
    th4 = th3;    // length of back-end face of crystal(along x-axis, at bigger y-sise)
    talpha1 = 0;  // angle between y-axis and the center line of front-end face
    talpha2 = 0;  // angle between y-axis and the center line of back-end face
    ttheta = 0;   // polar angle of CsI crystal
    tphi = 0;     // azimuthal angle of CsI crystal

    //
    // detector geometry definition
    //
    G4String crystal_name[3], apd_name[3], cathode_name[3], wls_name[3], teflon_name[3], union_CW_name, union_CWT_name, union_CWTA_name[3];
    G4String crywls_surname, crywin_surname, wlstef_surname, sd_surname;
    int i = 0;
    crystal_name[0] = Form("Crystal_sol%1d", i);
    crystal_name[1] = Form("Crystal_log%1d", i);
    crystal_name[2] = Form("Crystal_phy%1d", i);
    apd_name[0] = Form("APD_sol%1d", i);
    apd_name[1] = Form("APD_log%1d", i);
    apd_name[2] = Form("APD_phy%1d", i);
    cathode_name[0] = Form("Cathode_sol%1d", i);
    cathode_name[1] = Form("Cathode_log%1d", i);
    cathode_name[2] = Form("Cathode_phy%1d", i);
    wls_name[0] = Form("WLS_sol%1d", i);
    wls_name[1] = Form("WLS_log%1d", i);
    wls_name[2] = Form("WLS_phy%1d", i);
    teflon_name[0] = Form("Teflon_sol%1d", i);
    teflon_name[1] = Form("Teflon_log%1d", i);
    teflon_name[2] = Form("Teflon_phy%1d", i);
    union_CW_name = Form("Union_CW_sol%1d", i);
    union_CWT_name = Form("Union_CWT_sol%1d", i);
    union_CWTA_name[0] = Form("Detector_sol%1d", i);
    union_CWTA_name[1] = Form("Detector_log%1d", i);
    union_CWTA_name[2] = Form("Detector_phy%1d", i);
    crywls_surname = Form("CryAirSurface%1d", i);
    crywin_surname = Form("CrySDSurface%1d", i);
    wlstef_surname = Form("WLsTefSurface%1d", i);
    sd_surname = Form("SensitiveDetectorSurface%1d", i);

    // definition of solids
    // solid definition of APD and its subcomponents of window and photo-sensitive area
    G4Box *solidApd = new G4Box(apd_name[0], 1.0 * cm, 1.0 * cm, ath * 0.5);
    G4Box *solidCathode = new G4Box(cathode_name[0], 1.0 * cm, 1.0 * cm, gth * 0.5);
    // solid definition of crystal + WLS + teflon (not subtract APD)
    G4Trap *solidCryWlsTef0 = new G4Trap(union_CWT_name, th * 0.5, ttheta, tphi, ta * 0.5, th1 * 0.5, th2 * 0.5, talpha1, tc * 0.5, th3 * 0.5, th4 * 0.5, talpha2);
    // union solid definition of crystal + WLS + teflon + APD
    G4UnionSolid *solidDetector = new G4UnionSolid(union_CWTA_name[0], solidCryWlsTef0, solidApd, 0, G4ThreeVector(0, 0, (th + ath) * 0.5 - ttef - twls));
    // solid definition of crystal + WLS (not subtract APD)
    G4Trap *solidCryWls0 = new G4Trap(union_CW_name, (th - 2 * ttef) * 0.5, ttheta, tphi, (ta - 2 * ttef) * 0.5, (th1 - 2 * ttef) * 0.5, (th2 - 2 * ttef) * 0.5, talpha1, (tc - 2 * ttef) * 0.5, (th3 - 2 * ttef) * 0.5, (th4 - 2 * ttef) * 0.5, talpha2);
    // solid definition of crystal + WLS + teflon
    G4SubtractionSolid *solidCryWlsTef = new G4SubtractionSolid(union_CWT_name, solidCryWlsTef0, solidApd, 0, G4ThreeVector(0, 0, (th + ath) * 0.5 - ttef - twls));
    // solid definition of crystal + WLS
    G4SubtractionSolid *solidCryWls = new G4SubtractionSolid(union_CW_name, solidCryWls0, solidApd, 0, G4ThreeVector(0, 0, (th + ath) * 0.5 - ttef - twls));
    // solid definition of teflon
    G4SubtractionSolid *solidTef = new G4SubtractionSolid(teflon_name[0], solidCryWlsTef, solidCryWls, 0, G4ThreeVector(0, 0, 0));
    // solid definition of CsI crystal
    G4Trap *solidCry = new G4Trap(crystal_name[0], (th - 2 * ttef - 2 * twls) * 0.5, ttheta, tphi, (ta - 2 * ttef - 2 * twls) * 0.5, (th1 - 2 * ttef - 2 * twls) * 0.5, (th2 - 2 * ttef - 2 * twls) * 0.5, talpha1, (tc - 2 * ttef - 2 * twls) * 0.5, (th3 - 2 * ttef - 2 * twls) * 0.5, (th4 - 2 * ttef - 2 * twls) * 0.5, talpha2);
    // solid definition of WLS
    G4SubtractionSolid *solidWls = new G4SubtractionSolid(wls_name[0], solidCryWls, solidCry, 0, G4ThreeVector(0, 0, 0));

    // definition of logical volumes
    G4LogicalVolume *logicDetector = new G4LogicalVolume(solidDetector, air, union_CWTA_name[1]);
    G4LogicalVolume *logicTef = new G4LogicalVolume(solidTef, Teflon, teflon_name[1]);
    G4LogicalVolume *logicWls = new G4LogicalVolume(solidWls, WLS, wls_name[1]);
    G4LogicalVolume *logicCry = new G4LogicalVolume(solidCry, CsI, crystal_name[1]);
    G4LogicalVolume *logicApd = new G4LogicalVolume(solidApd, Glass, apd_name[1]);
    G4LogicalVolume *logicCathode = new G4LogicalVolume(solidCathode, SD, cathode_name[1]);

    // definition of physical volumes
    G4PVPlacement *physDetector = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicDetector, union_CWTA_name[2], logicalWorld, false, 0, 1);
    G4PVPlacement *physTef = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicTef, teflon_name[2], logicDetector, false, 0, 1);
    G4PVPlacement *physWls = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicWls, wls_name[2], logicDetector, false, 0, 1);
    G4PVPlacement *physCry = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicCry, crystal_name[2], logicDetector, false, 0, 1);
    G4PVPlacement *physApd = new G4PVPlacement(0, G4ThreeVector(0, 0, (th + ath) * 0.5 - ttef - twls), logicApd, apd_name[2], logicDetector, false, 0, 1);
    G4PVPlacement *physCathode = new G4PVPlacement(0, G4ThreeVector(0, 0, (ath - gth) * 0.5), logicCathode, cathode_name[2], logicApd, false, 0, 1);

    // definition of optical surfaces
    new G4LogicalBorderSurface(crywls_surname, physCry, physWls, crywlsSurface);
    new G4LogicalBorderSurface(wlstef_surname, physWls, physTef, wlstefSurface);
    new G4LogicalBorderSurface(crywin_surname, physCry, physApd, crywinSurface);
    new G4LogicalSkinSurface(sd_surname, logicCathode, sdSurface);

    return physWorld;
}
