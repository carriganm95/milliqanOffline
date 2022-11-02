#ifndef OFFLINE_FACTORY_H
#define OFFLINE_FACTORY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include "./interface/json.h"

#include "TSystem.h"
#include "TRandom2.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TArray.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TTree.h"
#include "TChain.h"
#include <set>
#include "TMath.h"
#include "TH1F.h"
#include "TParameter.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TBranch.h"
#include "TString.h"
#include "TObjString.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TColor.h"
#include "TComplex.h"

#include "interface/GlobalEvent.h"
#include "interface/DemonstratorConfiguration.h"
#include "interface/V1743Configuration.h"
#include "interface/V1743Event.h"

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
using namespace std;

//Use struct to organise tree contents
struct offline_tree_{
    int event ;
    Long64_t runNumber;
    Long64_t fileNumber;
    Long64_t event_time_b0;
    Long64_t event_time_b1;
    double event_time_fromTDC;
    bool present_b0;
    bool present_b1;
    int t_since_fill_start;
    int t_since_fill_end;
    int t_until_next_fill;
    string event_t_string;
    Long64_t event_trigger_time_tag_b0;
    Long64_t event_trigger_time_tag_b1;
    int fillNum;
    float fillAvgLumi;
    float fillTotalLumi;
    bool beam;
    bool hardNoBeam;

    //pulse vectors
    vector<int> v_npulses;
    vector<int> v_ipulse;
    vector<int> v_chan;
    vector<int> v_chanWithinBoard;
    vector<int> v_board;
    vector<int> v_layer;
    vector<int> v_row;
    vector<int> v_column;
    vector<int> v_type;
    vector<float> v_height;
    vector<float> v_time;
    vector<float> v_triggerCandidates;
    vector<float> v_triggerCandidatesEnd;
    vector<int> v_triggerCandidatesChannel;
    vector<float> v_time_module_calibrated;
    vector<float> v_delay;
    vector<float> v_area;
    vector<float> v_nPE;
    vector<float> v_duration;
    vector<bool> v_quiet;
    vector<float> v_presample_mean;
    vector<float> v_presample_RMS;
    vector<float> v_sideband_mean;
    vector<float> v_sideband_RMS;
    vector<float> v_sideband_mean_perFile;
    vector<float> v_triggerBand_mean;
    vector<float> v_triggerBand_max;
    vector<float> v_triggerBand_maxTime;
    vector<float> v_triggerBand_RMS;
    vector<float> v_sideband_mean_calib;
    vector<float> v_sideband_RMS_calib;
    vector<Long64_t> v_groupTDC_b0;
    vector<Long64_t> v_groupTDC_b1;
    vector<float> v_bx;
    vector<float> v_by;
    vector<float> v_bz;
    vector<float> v_max;
    vector<float> v_min;
    vector<float> v_max_afterFilter;
    vector<float> v_max_threeConsec;
    vector<float> v_triggerThresholds;
    vector<bool> v_triggerEnable;
    vector<int> v_triggerLogic;
    vector<int> v_triggerMajority;
    vector<float> v_min_afterFilter;

    float tClockCycles;
    float tTime;
    float tStartTime;
    float tTrigger;
    float tTimeDiff;
    float tMatchingTimeCut;
    int tEvtNum;
    int tRunNum;
    //Trigger tree members
};
//Offline factory class used to produce offline tree output
class OfflineFactory {
    public:
	OfflineFactory(TString,TString,TString,bool);
	OfflineFactory(TString,TString,TString, bool, int, int);
	// virtual ~OfflineFactory();
	void makeOutputTree();
	void loadJsonConfig(string);
        void readMetaData();
        vector<vector<pair<float,float> > > readWaveDataPerEvent(int);
    //        void defineColors(vector<int>, vector<TColor*>, vector<float>, vector<float>, vector<float>);
    //vector<int>colors, vector<TColor*> palette, vector<float> reds, vector<float> greens, vector<float> blues
        void h1cosmetic(TH1D*,int,vector<int> &);
        void displayEvent(int,vector<vector<pair<float,float> > >&,TString);
        void displaychannelEvent(int,vector<vector<pair<float,float> > >&,TString);
        void displayEvents(vector<int> &,TString);
        void readWaveData();
        void addFriendTree();
        void setFriendFile(TString);
	void writeOutputTree();
    	void process();
	void process(TString,TString);
	void process(TString,TString,int, int);
    	void processDisplays(vector<int>&,TString);
	void processDisplays(vector<int>&,TString,TString);
	void processDisplays(vector<int>&,TString,TString,int, int);
        TString getVersion();
    private:
	void prepareOutBranches();
	void resetOutBranches();
	void prepareWave(int);
	pair<float,float> measureSideband(int);
	vector<pair<float,float>> findPulses(int);
	vector<pair<float,float>> processChannel(int);
	void loadBranches();
	void loadWavesMilliDAQ();
	void loadWavesDRS();
	void validateInput();
        void writeVersion();

	float sideband_range[2] = {0,50};
        TString versionShort;
        TString versionLong = "asddsf";
        float sampleRate = 1.6; //Dummy value here, actual value read in from MetaData
	bool applyLPFilter = false;
	TString inFileName;
	TString outFileName;
        TString friendFileName = "";
        TString appendToTag;
        int runNumber;
        int fileNumber;
	bool isDRS;
	mdaq::GlobalEvent * evt = new mdaq::GlobalEvent();
	mdaq::DemonstratorConfiguration * cfg = new mdaq::DemonstratorConfiguration();
	vector<float> highThresh = {15.};
	vector<float> lowThresh = {5.};
	vector<int> nConsecSamples = {3};
	vector<int> nConsecSamplesEnd = {1};
	vector< vector<int> > chanMap;
	vector<float> timingCalibrations;
	vector<float> pedestals;
	vector<float> speAreas;
	TArrayI * chanArray;
	TArrayI * boardArray;

	//Declare global variables
	double arrayVoltageDRS[1024];
	int numChan;
	int numBoards;
	int boardsDRS[50];
	int chansDRS[50];
	vector<TH1D*> waves;
	TTree * inTree;
	TFile * inFile;
	TFile * outFile;
	TTree * outTree;
	offline_tree_ outputTreeContents;
	vector<TColor *> palette;
	vector<int> colors;
        //Trigger friend variables
        float tClockCycles = -1.;
        float tTime = -1.;
        float tStartTime = -1.;
        float tTrigger = -1.;
        float tTimeDiff = -1.;
        float tMatchingTimeCut = -1.;
	int tEvtNum = 0;
	int tRunNum = 0;
    
};
#endif
