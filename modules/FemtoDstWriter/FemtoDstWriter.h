#ifndef FEMTO_DST_WRITER_H
#define FEMTO_DST_WRITER_H

#include "PicoDstSkimmer/PicoDstSkimmer.h"

#include "ProductionUtils/RunMapFactory.h"

#include "PicoDstSL16cMtd/StPicoMtdPidTraits.h"

#include "FemtoDstFormat/BranchWriter.h"
#include "FemtoDstFormat/TClonesArrayWriter.h"
#include "FemtoDstFormat/FemtoEvent.h"
#include "FemtoDstFormat/FemtoTrack.h"
#include "FemtoDstFormat/FemtoMtdPidTraits.h"

#include "TTree.h"
#include "TAxis.h"

class FemtoDstWriter : public PicoDstSkimmer
{
protected:
	FemtoEvent _event;
	RunMapFactory rmf;

	TTree * tree;
	BranchWriter<FemtoEvent> _wEvent;
	TClonesArrayWriter<FemtoTrack> _wTrack;
	TClonesArrayWriter<FemtoMtdPidTraits> _wMtdPid;
	FemtoTrack _track;
	FemtoMtdPidTraits _mtdPid;
public:
	virtual const char* classname() const { return "FemtoDstWriter"; }
	FemtoDstWriter() {}
	~FemtoDstWriter() {}

	virtual void initialize(){
		PicoDstSkimmer::initialize();

		book->cd();
		tree = new TTree( "FemtoDst", "FemtoDst" );
		_wEvent.createBranch( tree, "Event" );
		_wTrack.createBranch( tree, "Tracks" );
		_wMtdPid.createBranch( tree, "MtdPidTraits" );

	}
protected:	

	virtual void preEventLoop(){
		TreeAnalyzer::preEventLoop();

	}
	virtual bool keepEvent( StPicoEvent *event ){

		book->fill( "events", "All" );
		if ( false == event->isDiMuon() )
			return false;

		book->fill( "events", "Trigger" );

		auto vtx = event->primaryVertex();
		float deltaVz = event->vzVpd() - vtx.z();

		book->fill( "vtx_z", vtx.z() );
		book->fill( "delta_z", deltaVz );

		if ( fabs( vtx.z() ) > 100 )
			return false;
		book->fill( "events", "vtx" );

		if ( fabs(deltaVz) > 3.0 )
			return false;
		book->fill( "events", "vtx_delta" );


		book->fill( "pass_vtx_z", vtx.z() );
		book->fill( "pass_delta_z", deltaVz );

		return true;
	}

	virtual void analyzeEvent(){
		StPicoEvent *event = _rEvent.get( 0 );
		if ( nullptr == event ){
			LOG_F( ERROR, "NULL EVENT" );
		}

		if ( false == keepEvent( event ) )
			return;

		auto vtx = event->primaryVertex();

		_event.mPrimaryVertex_mX1 = vtx.x();
		_event.mPrimaryVertex_mX2 = vtx.y();
		_event.mPrimaryVertex_mX3 = vtx.z();

		_event.mRunId    = event->runId();
		_event.mRunIndex = rmf.indexForRun( _event.mRunId );
		_event.mEventId  = event->eventId();
		_event.mTriggerWordMtd = event->triggerWordMtd();

		_wEvent.set( _event );

		_wTrack.reset();
		_wMtdPid.reset();
		size_t nTracks = _rTrack.N();
		size_t nMtdTracks = 0;
		for ( size_t i = 0; i < nTracks; i++ ){
			StPicoTrack * track = _rTrack.get( i );

			fillTrack( nMtdTracks, track );
			if ( _track.mMtdPidTraitsIndex >= 0 && fabs(_track.mPt) > 0.01 ){
				
				_wTrack.add( _track );
				_wMtdPid.add( _mtdPid );

				nMtdTracks++;
			}
		}

		if ( nMtdTracks >= 1 )
			book->fill( "events", "gte1_mtd" );
		if ( nMtdTracks >= 2 )
			book->fill( "events", "gte2_mtd" );


		tree->Fill();
	}

	virtual void fillTrack( size_t i, StPicoTrack *track ){

		_track.reset();

		_track.mId  = track->id();
		auto pMom   = track->pMom();
		_track.mPt  = pMom.perp();
		_track.mEta = pMom.pseudoRapidity();
		_track.mPhi = pMom.phi();
		
		_track.mNHitsFit  = track->nHitsFit() * track->charge();
		_track.mNHitsMax  = track->nHitsMax();
		_track.mNHitsDedx = track->nHitsDedx();

		_track.nSigmaElectron ( track->nSigmaElectron ( ) );
		_track.nSigmaPion     ( track->nSigmaPion     ( ) );
		_track.nSigmaKaon     ( track->nSigmaKaon     ( ) );
		_track.nSigmaProton   ( track->nSigmaProton   ( ) );

		_track.gDCA( track->dca() );

		_track.dEdx( track->dEdx() );

		if ( track->mtdPidTraitsIndex() >= 0 ){
			fillMtdPid( i, track );
		}
		
	}


	virtual void fillMtdPid( size_t i, StPicoTrack *track ){
		_mtdPid.reset();

		size_t index = _wMtdPid.N();
		StPicoMtdPidTraits *mtdPid = _rMtdPid.get( track->mtdPidTraitsIndex() );
		_mtdPid.mMatchFlag = mtdPid->matchFlag();
		_mtdPid.mDeltaY = mtdPid->deltaY();
		_mtdPid.mDeltaZ = mtdPid->deltaZ();
		_mtdPid.mDeltaTimeOfFlight = mtdPid->deltaTimeOfFlight();
		_mtdPid.mMtdHitChan = mtdPid->mMtdHitChan;


		size_t nMtdHits = _rMtdHit.N();
		for ( size_t iMtdHit = 0; iMtdHit < nMtdHits; iMtdHit++ ){
			StPicoMtdHit *mtdHit = _rMtdHit.get( iMtdHit );
			
			if ( nullptr != mtdHit && mtdHit->gChannel() == _mtdPid.mMtdHitChan ){
				_mtdPid.mTriggerFlag = mtdHit->triggerFlag();
				break;
			}
		} // loop iMtdHit

		_track.mMtdPidTraitsIndex = index;

	}

};


#endif