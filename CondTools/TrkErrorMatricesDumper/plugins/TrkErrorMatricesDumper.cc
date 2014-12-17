// -*- C++ -*-
//
// Package:    CondTools/TrkErrorMatricesDumper
// Class:      TrkErrorMatricesDumper
// 
/**\class TrkErrorMatricesDumper TrkErrorMatricesDumper.cc CondTools/TrkErrorMatricesDumper/plugins/TrkErrorMatricesDumper.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Carlo Battilana
//         Created:  Wed, 17 Dec 2014 13:23:16 GMT
//
//


// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "CondFormats/AlignmentRecord/interface/TrackerAlignmentErrorExtendedRcd.h"
#include "CondFormats/AlignmentRecord/interface/TrackerAlignmentErrorRcd.h"

#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "CondFormats/Alignment/interface/AlignmentErrors.h"

//
// class declaration
//

class TrkErrorMatricesDumper : public edm::EDAnalyzer {
   public:
      explicit TrkErrorMatricesDumper(const edm::ParameterSet&);
      ~TrkErrorMatricesDumper();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------

  bool m_readErrors;
  bool m_readExtendedErrors;
  bool m_compare;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TrkErrorMatricesDumper::TrkErrorMatricesDumper(const edm::ParameterSet& iConfig) :
  m_readErrors(iConfig.getParameter<bool>("readErrors")),
  m_readExtendedErrors(iConfig.getParameter<bool>("readExtendedErrors")),
  m_compare(m_readErrors &&  m_readExtendedErrors && iConfig.getParameter<bool>("compare"))

{

}


TrkErrorMatricesDumper::~TrkErrorMatricesDumper()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TrkErrorMatricesDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

}


// ------------ method called once each job just before starting event loop  ------------
void 
TrkErrorMatricesDumper::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrkErrorMatricesDumper::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------

void 
TrkErrorMatricesDumper::beginRun(edm::Run const& run, edm::EventSetup const& context)
{

  edm::ESHandle<AlignmentErrors> errors;
  edm::ESHandle<AlignmentErrorsExtended> extendedErrors;

  std::map<align::ID,CLHEP::HepSymMatrix> errorsMap;
  std::map<align::ID,CLHEP::HepSymMatrix> extendedErrorsMap;

   if (m_readErrors)
    {
      context.get<TrackerAlignmentErrorRcd>().get(errors);

      std::vector<AlignTransformError>::const_iterator errIt  = errors->m_alignError.begin();
      std::vector<AlignTransformError>::const_iterator errEnd = errors->m_alignError.end();

      for ( ; errIt!=errEnd; ++errIt)
	{

	  align::ID rawId  = errIt->rawId();
	  CLHEP::HepSymMatrix matrix = errIt->matrix();

	  std::cout << "[TrkErrorMatricesDumper::Error] ID: "
		    << rawId << " matrix: " << matrix << std::endl;

	  errorsMap[rawId] = matrix;

	}

    }

   if (m_readExtendedErrors)
    {
      context.get<TrackerAlignmentErrorExtendedRcd>().get(extendedErrors);

      std::vector<AlignTransformErrorExtended>::const_iterator errIt  = extendedErrors->m_alignError.begin();
      std::vector<AlignTransformErrorExtended>::const_iterator errEnd = extendedErrors->m_alignError.end();

      for ( ; errIt!=errEnd; ++errIt)
	{

	  align::ID rawId  = errIt->rawId();
	  CLHEP::HepSymMatrix matrix = errIt->matrix();

	  std::cout << "[TrkErrorMatricesDumper::ErrorExtnded] ID: "
		    << rawId << " matrix: " << matrix << std::endl;

	  extendedErrorsMap[rawId] = matrix;

	}      

    }

   if (m_compare)
     {

       std::map<align::ID,CLHEP::HepSymMatrix>::const_iterator errIt  = errorsMap.begin();
       std::map<align::ID,CLHEP::HepSymMatrix>::const_iterator errEnd = errorsMap.end();

       std::map<align::ID,CLHEP::HepSymMatrix>::const_iterator extErrEnd = extendedErrorsMap.end();

       int nAgreement = 0;
       int nDisagreement = 0;

       for ( ; errIt!=errEnd; ++errIt)
	 {
	   
	   align::ID rawId =  errIt->first; 
	   CLHEP::HepSymMatrix err = errIt->second;

	   if (extendedErrorsMap.find(rawId) == extErrEnd)
	     { 
	       std::cout << "[TrkErrorMatricesDumper::Compare] ID: " 
			 << rawId << " NO ExtendedError found!" << std::endl;
	       continue;
	     }

	   CLHEP::HepSymMatrix errExt = extendedErrorsMap[rawId];
	   
	   bool areEquivalent = ( std::abs(err[0][0] - errExt[0][0]) <= std::abs(err[0][0]*1E-3) 
				  &&  std::abs(err[0][1] - errExt[0][1]) <= std::abs(err[0][1]*1E-3)
				  &&  std::abs(err[0][2] - errExt[0][2]) <= std::abs(err[0][2]*1E-3)
				  &&  std::abs(err[1][1] - errExt[1][1]) <= std::abs(err[1][1]*1E-3)
				  &&  std::abs(err[1][2] - errExt[1][2]) <= std::abs(err[1][2]*1E-3)
				  &&  std::abs(err[2][2] - errExt[2][2]) <= std::abs(err[2][2]*1E-3) );
	   
	   std::cout << "[TrkErrorMatricesDumper::Compare] ID: " 
		     << rawId << " Comparison is " << (areEquivalent ? "GOOD" : "BAD") << std::endl;

	   if (areEquivalent)
	     nAgreement++;
	   else 
	     nDisagreement++;
	 }

       std::cout << "[TrkErrorMatricesDumper::Compare] Summary: # of IDs in agreement: " 
		 << nAgreement << " # of IDs in disagreement:  " << nDisagreement << std::endl;

     }

}


// ------------ method called when ending the processing of a run  ------------
/*
void 
TrkErrorMatricesDumper::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
TrkErrorMatricesDumper::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
TrkErrorMatricesDumper::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrkErrorMatricesDumper::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrkErrorMatricesDumper);
