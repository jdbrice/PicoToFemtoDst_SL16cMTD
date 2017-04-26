#ifndef PICO_DST_SKIMMER_H
#define PICO_DST_SKIMMER_H

#include "TreeAnalyzer.h"

#include "FemtoDstFormat/BranchReader.h"
#include "FemtoDstFormat/TClonesArrayReader.h"


#include "PicoDstSL16cMTD/StPicoEvent.h"
#include "PicoDstSL16cMTD/StPicoMtdHit.h"
#include "PicoDstSL16cMTD/StPicoTrack.h"
#include "PicoDstSL16cMTD/StPicoMtdPidTraits.h"

#include "vendor/loguru.h"

class PicoDstSkimmer : public TreeAnalyzer
{
public:
	const int DEBUG = 1;
	PicoDstSkimmer() {}
	~PicoDstSkimmer() {}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		_rEvent.setup( this->chain, "Event" );
		_rMtdHit.setup( this->chain, "MtdHit" );
		_rTrack.setup( this->chain, "Tracks" );
		_rMtdPid.setup( this->chain, "MtdPidTraits" );

	}
protected:

	TClonesArrayReader < StPicoEvent        > _rEvent;
	TClonesArrayReader < StPicoMtdHit       > _rMtdHit;
	TClonesArrayReader < StPicoTrack        > _rTrack;
	TClonesArrayReader < StPicoMtdPidTraits > _rMtdPid;


	virtual void analyzeEvent() {
		StPicoEvent *event = _rEvent.get( 0 );

		if ( nullptr == event ){
			return;
		}

		
		LOG_IF_F( INFO, DEBUG, "RunId: %d", event->runId() );
		LOG_IF_F( INFO, DEBUG, "#Tracks: %u", _rTrack.N() );
		LOG_IF_F( INFO, DEBUG, "#MtdHits: %u", _rMtdHit.N() );
		LOG_IF_F( INFO, DEBUG, "#MtdPids: %u", _rMtdPid.N() );


	}
	
};


#endif