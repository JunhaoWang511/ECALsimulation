#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorConfig.hh"

#include "G4Event.hh"
#include "G4HEPEvtInterface.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4GeneralParticleSource.hh"

#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction()
	: G4VUserPrimaryGeneratorAction(), fGParticleSource(), fParticleGun()
{
	fGParticleSource = new G4GeneralParticleSource();
	fParticleGun = new G4ParticleGun();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fGParticleSource;
	delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
	// 读共享配置，master 的 UI 命令对所有 worker 生效
	if (PrimaryGeneratorConfig::Instance()->GetTimeSpread())
	{
		double timespread = G4UniformRand() * 12.5 * ns;
		fGParticleSource->SetParticleTime(timespread);
	}
	fGParticleSource->GeneratePrimaryVertex(anEvent);
}

// 产生从pCsI发射谱抽样的光学光子
// void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
// {
// 	//
// 	// only re-sample optical photon energy
// 	// other parameters:
// 	//   - position
// 	//   - direction
// 	//   - polarization
// 	//   - time
// 	// are controlled by macro (/gps/)
// 	//

// 	static const G4int nEntries = 43;

// 	static G4double photonEnergy[nEntries] =
// 		{
// 			1.7714 * eV, 1.7971 * eV, 1.8235 * eV,
// 			1.8507 * eV, 1.8788 * eV, 1.9077 * eV,
// 			1.9375 * eV, 1.9683 * eV, 2.0000 * eV,
// 			2.0328 * eV, 2.0667 * eV, 2.1017 * eV,
// 			2.1379 * eV, 2.1754 * eV, 2.2143 * eV,
// 			2.2545 * eV, 2.2963 * eV, 2.3396 * eV,
// 			2.3846 * eV, 2.4314 * eV, 2.4800 * eV,
// 			2.5306 * eV, 2.5833 * eV, 2.6383 * eV,
// 			2.6957 * eV, 2.7556 * eV, 2.8182 * eV,
// 			2.8837 * eV, 2.9524 * eV, 3.0244 * eV,
// 			3.1000 * eV, 3.1795 * eV, 3.2632 * eV,
// 			3.3514 * eV, 3.4444 * eV, 3.5429 * eV,
// 			3.6471 * eV, 3.7576 * eV, 3.8750 * eV,
// 			4.0000 * eV, 4.1333 * eV, 4.2759 * eV,
// 			4.4286 * eV};

// 	static G4double crystalEmSpec[nEntries] =
// 		{0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
// 		 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
// 		 0.0000, 0.0000, 0.0000, 0.0000, 0.0155, 0.0182, 0.0272, 0.0423, 0.0737, 0.1297, 0.2369, 0.4121, 0.6443,
// 		 0.8914, 1.0000, 0.9236, 0.7050};

// 	//
// 	// build piecewise-linear PDF and segment CDF only once
// 	//
// 	// PDF is linearly interpolated between (photonEnergy[i], crystalEmSpec[i])
// 	// and (photonEnergy[i+1], crystalEmSpec[i+1]); its integral inside a
// 	// segment is a quadratic function of t, so the inverse can be solved
// 	// analytically (continuous energy sampling).
// 	//

// 	static G4bool initialized = false;
// 	static G4double segArea[nEntries - 1]; // trapezoid area of each segment
// 	static G4double segCdf[nEntries - 1];  // cumulative probability up to each segment
// 	static G4double totalArea = 0.0;

// 	if (!initialized)
// 	{
// 		totalArea = 0.0;

// 		for (int i = 0; i < nEntries - 1; i++)
// 		{
// 			G4double dE = photonEnergy[i + 1] - photonEnergy[i];
// 			segArea[i] = 0.5 * (crystalEmSpec[i] + crystalEmSpec[i + 1]) * dE;
// 			totalArea += segArea[i];
// 		}

// 		G4double cumulative = 0.0;

// 		for (int i = 0; i < nEntries - 1; i++)
// 		{
// 			cumulative += segArea[i] / totalArea;
// 			segCdf[i] = cumulative;
// 		}

// 		initialized = true;
// 	}

// 	//
// 	// sample energy: pick segment, then invert the quadratic CDF inside it
// 	//

// 	G4double r = G4UniformRand();

// 	G4int k = 0;

// 	while (k < nEntries - 1 && r >= segCdf[k])
// 	{
// 		k++;
// 	}

// 	// guard against floating-point round-off at the last segment
// 	if (k > nEntries - 2)
// 	{
// 		k = nEntries - 2;
// 	}

// 	// target (unnormalized) area inside segment k
// 	G4double c = (r - (k > 0 ? segCdf[k - 1] : 0.0)) * totalArea;

// 	G4double dE = photonEnergy[k + 1] - photonEnergy[k];
// 	G4double p0 = crystalEmSpec[k];
// 	G4double a = crystalEmSpec[k + 1] - p0; // slope of the linear PDF inside the segment

// 	// exact root of  a*t^2 + 2*p0*t - 2*c/dE = 0  (numerically stable form,
// 	// denominator is always positive; reduces to t = c/(dE*p0) when a = 0)
// 	G4double t = 2.0 * c / (dE * (p0 + std::sqrt(p0 * p0 + 2.0 * a * c / dE)));

// 	G4double energy = photonEnergy[k] + t * dE;

// 	//
// 	// override GPS energy
// 	//

// 	fGParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy);

// 	//
// 	// generate particle
// 	//
// 	fGParticleSource->GeneratePrimaryVertex(anEvent);
// }
