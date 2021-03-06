#include "SimpleAnalysis.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// ROOT
#include "TH1F.h"

// STL
#include <iostream>

SimpleAnalysis::SimpleAnalysis(double aEnergy, double aSf):
  m_energy(aEnergy), m_sf(aSf), hHitEnergy(nullptr), hCellEnergy(nullptr), hGenPt(nullptr)  {
  Initialize_histos();
}
SimpleAnalysis::~SimpleAnalysis(){}


void SimpleAnalysis::Initialize_histos() {
  hHitEnergy = new TH1F("hHitEnergy","", 200, 0, m_energy);
  m_histograms.push_back(hHitEnergy);

  hCellEnergy = new TH1F("hCellenergy","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(hCellEnergy);

  hGenPt = new TH1F("hGenPt","", 100, m_energy-0.2*m_energy, m_energy+0.2*m_energy);
  m_histograms.push_back(hGenPt);
}

void SimpleAnalysis::processEvent(podio::EventStore& store, int aEventId, bool verbose) {
  //Get the collections
  const fcc::MCParticleCollection*  colMCParticles(nullptr);
  const fcc::PositionedCaloHitCollection*     colECalPositionedHits(nullptr);

  bool colMCParticlesOK = store.get("GenParticles", colMCParticles);
  bool colECalPositionedHitsOK     = store.get("ECalPositionedHits" , colECalPositionedHits);

  //Total hit energy per event
  double SumE_hit_ecal = 0.;

  //PositionedHits collection
  if (colECalPositionedHitsOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #ECalPositionedHits:     " << colECalPositionedHits->size()    << std::endl;
    }
    //Loop through the collection
    for (auto& iecl=colECalPositionedHits->begin(); iecl!=colECalPositionedHits->end(); ++iecl) {
      //if (verbose) std::cout << "ECal hit energy " << iehit->core().energy << std::endl;
      SumE_hit_ecal += iecl->core().energy;
    }

    if (verbose) std::cout << "Total hit energy (GeV): " << SumE_hit_ecal << " total cell energy (GeV): " << SumE_hit_ecal*m_sf << " hit collection size: " << colECalPositionedHits->size() << std::endl;

    //Fill histograms
    hHitEnergy->Fill(SumE_hit_ecal);
    hCellEnergy->Fill(SumE_hit_ecal*m_sf);
  }
  else {
    if (verbose) {
      std::cout << "No CaloPositionedHits Collection!!!!!" << std::endl;
    }
  }

  //MCParticle and Vertices collection
  if (colMCParticlesOK) {
    if (verbose) {
      std::cout << " Collections: "          << std::endl;
      std::cout << " -> #MCTruthParticles:     " << colMCParticles->size()    << std::endl;
    }
    //Loop through the collection
    for (auto& iparticle=colMCParticles->begin(); iparticle!=colMCParticles->end(); ++iparticle) {
      //Fill histogram
      hGenPt->Fill( sqrt( pow(iparticle->core().p4.px,2)+
        pow(iparticle->core().p4.py,2) ) );
    }
  }
  else {
    if (verbose) {
      std::cout << "No MCTruth info available" << std::endl;
    }
  }
}

void SimpleAnalysis::finishLoop(int aNumEvents, bool aVerbose) {
  std::cout << "Total energy: " << hCellEnergy->GetMean() << std::endl;
}
