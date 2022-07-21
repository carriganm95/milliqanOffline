#include "/home/milliqan/milliqanOffline/offlineProduction/interface/OfflineFactory.h"

OfflineFactory::OfflineFactory(TString inFileName, TString outFileName) : 
    inFileName(inFileName),
    outFileName(outFileName)
{};

OfflineFactory::~OfflineFactory() {
    if (inFile) inFile->Close();
    if (outFile) outFile->Close();
}
void OfflineFactory::loadJsonConfig(string configFileName){
    
    std::string json;
    // configFileName = "{\"chanMap\":[[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7],[0,1,2,3],[4,5,6,7]]}";
    if (configFileName.find("{") != std::string::npos){
	json = configFileName;
    }
    else{
    std::ifstream t(configFileName);
    std::stringstream buffer;
    buffer << t.rdbuf();
    json = buffer.str();
    }

    Json::Reader reader;
    Json::Value jsonRoot;
    bool parseSuccess = reader.parse(json, jsonRoot, false);
    if (parseSuccess)
    {
	if (json.find("chanMap") != std::string::npos){
	    const Json::Value chan0 = jsonRoot["chanMap"];
	    for ( int index = 0; index < chan0.size(); ++index ){
		std::vector<int> chanMapPerChan;
		for ( int index2 = 0; index2 < chan0[index].size(); ++index2 ){
		    chanMapPerChan.push_back(chan0[index][index2].asInt());
		}
		chanMap.push_back(chanMapPerChan);
	    }
	}
	if (json.find("pulseParams") != std::string::npos){
	    nConsecSamples.clear();
	    nConsecSamplesEnd.clear();
	    lowThresh.clear();
	    highThresh.clear();
	    const Json::Value pulseParams = jsonRoot["pulseParams"];
	    const Json::Value nConsecSamplesJson = pulseParams["nConsecSamples"];
	    const Json::Value nConsecSamplesEndJson = pulseParams["nConsecSamplesEnd"];
	    const Json::Value highThreshJson = pulseParams["highThresh"];
	    const Json::Value lowThreshJson = pulseParams["lowThresh"];
	    for (int index = 0; index < nConsecSamplesJson.size(); index ++){
		nConsecSamples.push_back(nConsecSamplesJson[index].asInt());
	    }
	    for (int index = 0; index < nConsecSamplesEndJson.size(); index ++){
		nConsecSamplesEnd.push_back(nConsecSamplesEndJson[index].asInt());
	    }
	    for (int index = 0; index < highThreshJson.size(); index ++){
		highThresh.push_back(highThreshJson[index].asFloat());
	    }
	    for (int index = 0; index < lowThreshJson.size(); index ++){
		lowThresh.push_back(lowThreshJson[index].asFloat());
	    }
	}
	if (json.find("timingCalibrations") != std::string::npos){
	    const Json::Value timingCalibrationsJson = jsonRoot["timingCalibrations"];
	    for (int index = 0; index < timingCalibrationsJson.size(); index ++){
		timingCalibrations.push_back(timingCalibrationsJson[index].asFloat());
	    }
	}
	if (json.find("speAreas") != std::string::npos){
	    const Json::Value speAreasJson = jsonRoot["speAreas"];
	    for (int index = 0; index < speAreasJson.size(); index ++){
		speAreas.push_back(speAreasJson[index].asFloat());
	    }
	}
	if (json.find("pedestals") != std::string::npos){
	    const Json::Value pedestalsJson = jsonRoot["pedestals"];
	    for (int index = 0; index < pedestalsJson.size(); index ++){
		pedestals.push_back(pedestalsJson[index].asFloat());
	    }
	}
	if (json.find("sampleRate") != std::string::npos){
	    sampleRate = jsonRoot["sampleRate"].asFloat();
	}
    }
    else{
	throw invalid_argument(configFileName);
    }
}
//Validate json input
void OfflineFactory::validateInput(){
    if (nConsecSamples.size() > 1){
	if (nConsecSamples.size() != numChan) throw length_error("nConsecSamples should be length "+std::to_string(numChan) + "or 1");
    }
    else{ 
	for (int ic = 0; ic < numChan-1; ic++) nConsecSamples.push_back(nConsecSamples.at(0));
    }
    if (nConsecSamplesEnd.size() > 1){
	if (nConsecSamplesEnd.size() != numChan) throw length_error("nConsecSamplesEnd should be length "+std::to_string(numChan) + "or 1");
    }
    else{ 
	for (int ic = 0; ic < numChan-1; ic++) nConsecSamplesEnd.push_back(nConsecSamplesEnd.at(0));
    }
    if (lowThresh.size() > 1){
	if (lowThresh.size() != numChan) throw length_error("lowThresh should be length "+std::to_string(numChan) + "or 1");
    }
    else{ 
	for (int ic = 0; ic < numChan-1; ic++) lowThresh.push_back(lowThresh.at(0));
    }
    if (highThresh.size() > 1){
	if (highThresh.size() != numChan) throw length_error("highThresh should be length "+std::to_string(numChan) + "or 1");
    }
    else{ 
	for (int ic = 0; ic < numChan-1; ic++) highThresh.push_back(highThresh.at(0));
    }
////Calibrations
    if (timingCalibrations.size() > 0){
	if (timingCalibrations.size() != numChan) throw length_error("timingCalibrations should be length "+std::to_string(numChan));
    }
    else{ 
	for (int ic = 0; ic < numChan; ic++) timingCalibrations.push_back(0);
    }
    if (pedestals.size() > 0){
	if (pedestals.size() != numChan) throw length_error("pedestals should be length "+std::to_string(numChan));
    }
    else{ 
	for (int ic = 0; ic < numChan; ic++) pedestals.push_back(0);
    }
    if (speAreas.size() > 0){
	if (speAreas.size() != numChan) throw length_error("speAreas should be length "+std::to_string(numChan));
    }
    else{ 
	for (int ic = 0; ic < numChan; ic++) speAreas.push_back(1);
    }
}
//Convenience function to produce offline tree output
void OfflineFactory::process(){

    // Testing json stuff

    makeOutputTree();
    readMetaData();
    readWaveData();
    writeOutputTree();
}
void OfflineFactory::process(TString inFileName,TString outFileName)
{
    inFileName = inFileName;
    outFileName = outFileName;
    process();
}
//Declare branches for offline tree output
void OfflineFactory::prepareOutBranches(){
    outTree->Branch("triggerThreshold",&outputTreeContents.v_triggerThresholds);
    outTree->Branch("triggerEnable",&outputTreeContents.v_triggerEnable);
    outTree->Branch("triggerMajority",&outputTreeContents.v_triggerMajority);
    outTree->Branch("triggerLogic",&outputTreeContents.v_triggerLogic);
    outTree->Branch("chan",&outputTreeContents.v_chan);
    outTree->Branch("height",&outputTreeContents.v_height);
    outTree->Branch("area",&outputTreeContents.v_area);
    outTree->Branch("nPE",&outputTreeContents.v_nPE);
    outTree->Branch("ipulse",&outputTreeContents.v_ipulse);
    outTree->Branch("npulses",&outputTreeContents.v_npulses);
    outTree->Branch("time",&outputTreeContents.v_time);
    outTree->Branch("duration",&outputTreeContents.v_duration);
    outTree->Branch("delay",&outputTreeContents.v_delay);
    outTree->Branch("max",&outputTreeContents.v_max);
}
//Clear vectors and reset 
void OfflineFactory::resetOutBranches(){
    // outputTreeContents.v_triggerThresholds.clear();
    // outputTreeContents.v_triggerEnable.clear();
    // outputTreeContents.v_triggerMajority.clear();
    // outputTreeContents.v_triggerLogic.clear();
    outputTreeContents.v_chan.clear();
    outputTreeContents.v_height.clear();
    outputTreeContents.v_area.clear();
    outputTreeContents.v_nPE.clear();
    outputTreeContents.v_ipulse.clear();
    outputTreeContents.v_npulses.clear();
    outputTreeContents.v_time.clear();
    outputTreeContents.v_duration.clear();
    outputTreeContents.v_delay.clear();
    outputTreeContents.v_max.clear();
}
//Read meta data from configuration
void OfflineFactory::readMetaData(){
    inFile = TFile::Open(inFileName, "READ");
    TString baseFileName= ((TObjString*)inFileName.Tokenize("/")->Last())->String().Data();

    TTree * metadata;
    metadata = (TTree*) inFile->Get("Metadata");
    metadata->SetBranchAddress("configuration", &cfg);
    metadata->GetEntry(0);
    //Currently run and fill set to zero - I think should be given as input
    outputTreeContents.runNum = 0;
    outputTreeContents.fillNum = 0;
    int numBoards = cfg->digitizers.size();
    numChan = numBoards*16;
    chanArray = new TArrayI(numChan);
    //Read trigger info and set channel array
    for (int i =0; i < numChan; i++){
	chanArray->SetAt(i,i);
	float triggerThresh = cfg->digitizers[i/16].channels[i % 16].triggerThreshold;
	bool triggerEnable = cfg->digitizers[i/16].channels[i % 16].triggerEnable;
	int triggerMajority = cfg->digitizers[i/16].GroupTriggerMajorityLevel;
	int triggerLogic = cfg->digitizers[i/16].GroupTriggerLogic;
	outputTreeContents.v_triggerThresholds.push_back(triggerThresh);
	outputTreeContents.v_triggerEnable.push_back(triggerEnable);
	outputTreeContents.v_triggerMajority.push_back(triggerMajority);
	outputTreeContents.v_triggerLogic.push_back(triggerLogic);
    }
}

void OfflineFactory::makeOutputTree(){
    outFile = new TFile(outFileName,"recreate");
    outTree = new TTree("t","t");
    prepareOutBranches(); 
}

//Pulse finding and per channel processing
void OfflineFactory::readWaveData(){
    validateInput();
    if (!inFile) inFile = TFile::Open(inFileName, "READ");
    inTree = (TTree*)inFile->Get("Events");
    loadBranchesMilliDAQ();
    cout<<"Starting event loop"<<endl;
    // int maxEvents = 10000;
    int maxEvents = inTree->GetEntries();
    for(int i=0;i<maxEvents;i++){
	resetOutBranches();
	outputTreeContents.event=i;
	inTree->GetEntry(i);
	loadWavesMilliDAQ();
	//Loop over channels
	vector<vector<pair<float,float> > > allPulseBounds;
	for(int ic=0;ic<numChan;ic++){
	    //Pulse finding
	    allPulseBounds.push_back(processChannel(ic));
	    outputTreeContents.v_max.push_back(1.*waves[ic]->GetMaximum());
	}    
	outTree->Fill();
    }
}
void OfflineFactory::writeOutputTree(){
    outFile->cd();
    outTree->Write();
    outFile->Close();
    if (inFile) inFile->Close();
}
void OfflineFactory::prepareWave(int ic){
    TAxis * a = waves[ic]->GetXaxis();
    a->Set( a->GetNbins(), a->GetXmin()/sampleRate, a->GetXmax()/sampleRate);
    waves[ic]->ResetStats();
    //subtract calibrated mean
    for(int ibin = 1; ibin <= waves[ic]->GetNbinsX(); ibin++){
       waves[ic]->SetBinContent(ibin,waves[ic]->GetBinContent(ibin)-pedestals[ic]);
    }
    //Need to add sideband measurements and subtraction here
}
vector< pair<float,float> > OfflineFactory::findPulses(int ic){

    vector<pair<float,float> > bounds;
    //float tstart = sideband_range[1]+1;
    //int istart = waves[ic]->FindBin(tstart);
    int istart = 1;
    bool inpulse = false;
    int nover = 0;
    int nunder = 0;
    int i_begin = istart;
    //int i_begin = 0;
    int i_stop_searching = waves[ic]->GetNbinsX()-nConsecSamples[ic];
    int i_stop_final_pulse = waves[ic]->GetNbinsX();


    for (int i=istart; i<i_stop_searching || (inpulse && i<i_stop_final_pulse); i++) {
	float v = waves[ic]->GetBinContent(i);
	if (!inpulse) {
	    if (v<lowThresh[ic]) {   
		nover = 0;     // If v dips below the low threshold, store the value of the sample index as i_begin
		i_begin = i;
	    }
	    else if (v>=highThresh[ic]){
		nover++;       // If v is above the threshold, start counting the number of sample indices
	    }
	    else{
		i_begin = i;
	    }

	    if (nover>=nConsecSamples[ic]){   // If v is above threshold for long enough, we now have a pulse!
		inpulse = true;    // Also reset the value of nunder
		nunder = 0;
	    }
	}
	else {  // Called if we have a pulse
	    if (v<highThresh[ic]) nunder++;   // If the pulse dips below the threshold, sum the number of sample indices for which this is true
	    else if (v >= highThresh[ic]){
		nunder = 0;           // If the pulse stays above threshold, set nunder back to zero
	    }
	    // If the nunder is above or equal to 12 (or we reach the end of the file) store the values of the pulse bounds
	    if (nunder>=nConsecSamplesEnd[ic] || i==(i_stop_final_pulse-1)) { 
		bounds.push_back({(float)waves[ic]->GetBinLowEdge(i_begin), (float)waves[ic]->GetBinLowEdge(i+1)-0.01});
		// cout<<"i_begin, i: "<<i_begin<<" "<<i<<endl;       // i_begin is the 
		inpulse = false;
		nover = 0;
		nunder = 0;
		i_begin = i;
	    }
	}
    }
    return bounds;
}
//Pulse finding and per channel processing
vector< pair<float,float> > OfflineFactory::processChannel(int ic){
    //Pulse finding
    vector<pair<float,float>> pulseBounds = findPulses(ic);
    int npulses = pulseBounds.size();

    //Useful variable for defining pulses
    float maxThreeConsec = -100;
    for (int iBin = 1; iBin < waves[ic]->GetNbinsX(); iBin++){
	float maxList[] = {waves[ic]->GetBinContent(iBin),waves[ic]->GetBinContent(iBin+1),waves[ic]->GetBinContent(iBin+2)};
	float tempMax = *std::min_element(maxList,maxList+3);
	if (maxThreeConsec < tempMax) maxThreeConsec = tempMax;

    }
    outputTreeContents.v_max_threeConsec.push_back(maxThreeConsec);
    //FIXME Need to add low pass filter option back
    outputTreeContents.v_max_afterFilter.push_back(waves[ic]->GetMaximum());
    outputTreeContents.v_min_afterFilter.push_back(waves[ic]->GetMinimum());

    for(int ipulse = 0; ipulse<npulses; ipulse++){
	waves[ic]->SetAxisRange(pulseBounds[ipulse].first,pulseBounds[ipulse].second);
	if (chanMap.size() > 0 and ic < chanMap.size()){
	    outputTreeContents.v_column.push_back(chanMap[ic][0]);
	    outputTreeContents.v_row.push_back(chanMap[ic][1]);
	    outputTreeContents.v_layer.push_back(chanMap[ic][2]);
	    outputTreeContents.v_type.push_back(chanMap[ic][3]);
	}
	else{
	    outputTreeContents.v_column.push_back(0);
	    outputTreeContents.v_row.push_back(0);
	    outputTreeContents.v_layer.push_back(0);
	    outputTreeContents.v_type.push_back(0);
	}

	//FIXME need to add calibrations (when available)
	outputTreeContents.v_chan.push_back(chanArray->GetAt(ic));
	outputTreeContents.v_height.push_back(waves[ic]->GetMaximum());
	outputTreeContents.v_time.push_back(pulseBounds[ipulse].first);
	outputTreeContents.v_time_module_calibrated.push_back(pulseBounds[ipulse].first+timingCalibrations[ic]);
	outputTreeContents.v_area.push_back(waves[ic]->Integral());
	outputTreeContents.v_nPE.push_back((waves[ic]->Integral()/(speAreas[ic]))*(1.6/sampleRate));
	outputTreeContents.v_ipulse.push_back(ipulse);
	outputTreeContents.v_npulses.push_back(npulses);
	outputTreeContents.v_duration.push_back(pulseBounds[ipulse].second - pulseBounds[ipulse].first);
	if(ipulse>0) outputTreeContents.v_delay.push_back(pulseBounds[ipulse].first - pulseBounds[ipulse-1].second);
	else outputTreeContents.v_delay.push_back(9999.);

    }    

    return pulseBounds;
}

void OfflineFactory::loadBranchesMilliDAQ(){
    inTree->SetBranchAddress("event", &evt);
    for(int ic=0;ic<numChan;ic++) waves.push_back(new TH1D());

}
void OfflineFactory::loadWavesMilliDAQ(){
    int board,chan;
    //FIXME does this work if > 1 board?
    for(int i=0;i<numChan;i++){
	if(waves[i]) delete waves[i];
	//board = i<=15 ? 0 : 1;
	board = 0;
	chan = i<=15 ? i : i-16;
	waves[i] = (TH1D*)evt->GetWaveform(board, chan, Form("digitizers[%i].waveform[%i]",board,i));  
    }
}
