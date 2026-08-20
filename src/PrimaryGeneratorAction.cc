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
// 		{
// 			0.0034, 0.0033, 0.0036,
// 			0.0029, 0.0047, 0.0046,
// 			0.0045, 0.0055, 0.0068,
// 			0.0065, 0.0071, 0.0077,
// 			0.0085, 0.0087, 0.0103,
// 			0.0107, 0.0110, 0.0120,
// 			0.0129, 0.0126, 0.0140,
// 			0.0144, 0.0134, 0.0124,
// 			0.0129, 0.0140, 0.0121,
// 			0.0127, 0.0127, 0.0127,
// 			0.0155, 0.0182, 0.0272,
// 			0.0423, 0.0737, 0.1297,
// 			0.2369, 0.4121, 0.6443,
// 			0.8914, 1.0000, 0.9236,
// 			0.7050};

// 	//
// 	// build CDF only once
// 	//

// 	static G4bool initialized = false;
// 	static G4double cdf[nEntries];

// 	if (!initialized)
// 	{
// 		G4double sum = 0.0;

// 		for (int i = 0; i < nEntries; i++)
// 		{
// 			sum += crystalEmSpec[i];
// 		}

// 		G4double cumulative = 0.0;

// 		for (int i = 0; i < nEntries; i++)
// 		{
// 			cumulative += crystalEmSpec[i] / sum;
// 			cdf[i] = cumulative;
// 		}

// 		initialized = true;
// 	}

// 	//
// 	// sample energy
// 	//

// 	G4double r = G4UniformRand();

// 	G4double energy = photonEnergy[nEntries - 1];

// 	for (int i = 0; i < nEntries; i++)
// 	{
// 		if (r < cdf[i])
// 		{
// 			energy = photonEnergy[i];
// 			break;
// 		}
// 	}

// 	//
// 	// override GPS energy
// 	//

// 	fGParticleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy);

// 	//
// 	// generate particle
// 	//
// 	fGParticleSource->GeneratePrimaryVertex(anEvent);
// }
