
//
// Package:    SiStripRadMonitoring/NoiseBiasScan
// Class:      NoiseBiasScan
//
/**\class NoiseBiasScan NoiseBiasScan.cc SiStripRadMonitoring/NoiseBiasScan/plugins/NoiseBiasScan.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Jean-Laurent Agram
//         Created:  Fri, 03 May 2019 13:47:40 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/SiStripDigi/interface/SiStripProcessedRawDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"

#include "DataFormats/Common/interface/DetSetVector.h"

#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TTree.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class NoiseBiasScan : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
	public:
		explicit NoiseBiasScan(const edm::ParameterSet&);
		~NoiseBiasScan();

		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


	private:
		virtual void beginJob() override;
		virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
		virtual void endJob() override;

		// ----------member data ---------------------------
		long counter;
		const edm::EDGetTokenT< edm::DetSetVector<SiStripRawDigi> > srcZSdigi_;
		std::map< int, std::map< int, long> > adc_per_strip;
		std::map< int, std::map< int, long> > adcw2_per_strip;
		std::set< int > mod_list;

		int run;
		int run_min;
		int run_max;
		int evt;
		int evt_min;
		int evt_max;
		double evt_avg;
		long unixTime;
		long unixTime_min;
		long unixTime_max;

		edm::Service<TFileService> fs;
		TTree* noisetree;
		int tree_detid;
		int tree_nevt;
		int tree_run;
		int tree_evt_avg;
		int tree_evt_min;
		int tree_evt_max;
		int tree_time_min;
		int tree_time_max;
		float tree_ped;
		float tree_ped_low;
		float tree_ped_up;
		float tree_noise;
		float tree_noise_low;
		float tree_noise_up;
		float tree_ped_64;
		float tree_noise_64;

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
NoiseBiasScan::NoiseBiasScan(const edm::ParameterSet& iConfig):
	srcZSdigi_(consumes< edm::DetSetVector<SiStripRawDigi> >( iConfig.getParameter<edm::InputTag>( "srcZSdigi" )))
{
	//now do what ever initialization is needed
	counter=0;
	run=0; run_min=999999999; run_max=0;
	evt=0; evt_min=999999999; evt_max=0; evt_avg=0;
	unixTime=0; unixTime_min=9999999999; unixTime_max=0;

	noisetree = fs->make<TTree>("noisetree", "noisetree");
	noisetree->Branch("detid", &tree_detid);
	noisetree->Branch("nevt", &tree_nevt);
	noisetree->Branch("run", &tree_run);
	noisetree->Branch("evt_avg", &tree_evt_avg);
	noisetree->Branch("evt_min", &tree_evt_min);
	noisetree->Branch("evt_max", &tree_evt_max);
	noisetree->Branch("time_min", &tree_time_min);
	noisetree->Branch("time_max", &tree_time_max);
	noisetree->Branch("ped", &tree_ped);
	noisetree->Branch("ped_low", &tree_ped_low);
	noisetree->Branch("ped_up", &tree_ped_up);
	noisetree->Branch("noise", &tree_noise);
	noisetree->Branch("noise_low", &tree_noise_low);
	noisetree->Branch("noise_up", &tree_noise_up);
	noisetree->Branch("ped_64", &tree_ped_64);
	noisetree->Branch("noise_64", &tree_noise_64);
}


NoiseBiasScan::~NoiseBiasScan()
{

	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void NoiseBiasScan::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace std;

	counter++;
	// run should stay the same in a given input file
	run = iEvent.id().run();
	run_min = run<run_min ? run : run_min;
	run_max = run>run_max ? run : run_max;

	evt = iEvent.id().event(); 
	cout<<" Event id: "<<evt<<endl;
	evt_min = evt<evt_min ? evt : evt_min;
	evt_max = evt>evt_max ? evt : evt_max;
	evt_avg+=evt;

	unixTime = iEvent.time().unixTime();
	cout<<"  time: "<<unixTime<<endl;
	unixTime_min = unixTime<unixTime_min ? unixTime : unixTime_min;
	unixTime_max = unixTime>unixTime_max ? unixTime : unixTime_max;

	Handle< DetSetVector<SiStripRawDigi> > moduleZSdigi;
	iEvent.getByToken(srcZSdigi_,moduleZSdigi);

	// Get modules
	int prefix=0;
	int prev_prefix=0;
	DetSetVector<SiStripRawDigi>::const_iterator itRawDigis = moduleZSdigi->begin();
	for (; itRawDigis != moduleZSdigi->end(); ++itRawDigis) {
		uint32_t detId = itRawDigis->id;

		// first event, select some modules id
		if(counter==1){
			prefix = detId/1000;
			if(prefix!=prev_prefix || detId==369174804) mod_list.insert(detId);
			prev_prefix=prefix;
			map< int, long> map1;
			map< int, long> map2;
			for (int istrip=0; istrip<127; istrip++){
				map1[istrip] = 0;
				map2[istrip] = 0;
			}
			adc_per_strip[detId] = map1;
			adcw2_per_strip[detId] = map2;
		}

		// filter modules
		if(mod_list.find(detId)==mod_list.end()) continue;

		// loop over strips of 1st APV
		DetSet<SiStripRawDigi>::const_iterator itRaw = itRawDigis->begin();
		uint16_t strip=0;
		float totADC=0;
		for(;itRaw != itRawDigis->end(); ++itRaw, ++strip){
			// CONSIDER FIRST APV ONLY
			if(strip>128) break;
			float adc = itRaw->adc();
			totADC+= adc;
			adc_per_strip[detId][strip] +=adc;
			adcw2_per_strip[detId][strip] +=adc*adc;
			//if(strip%32==0) cout<<strip<<" "<<adc<<endl;
		}
		//cout<<detId<<" Avg ADC: "<<totADC/128<<endl;

	}

}


// ------------ method called once each job just before starting event loop  ------------
void NoiseBiasScan::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void NoiseBiasScan::endJob()
{
	// loop over modules, for keeping only median value in APV
	std::set<int>::iterator itDetId;
	for(itDetId=mod_list.begin(); itDetId!=mod_list.end(); itDetId++){
		std::vector<float> v_ped;
		std::vector<float> v_noise;
		for (int istrip=0; istrip<127; istrip++){
			float pedestal =  adc_per_strip[(*itDetId)][istrip]/(float) counter;
			v_ped.push_back(pedestal);
			float noise = sqrt(adcw2_per_strip[(*itDetId)][istrip]/float(counter)-pedestal*pedestal);
			v_noise.push_back(noise);
			// Fill middle strip info
			if(istrip==63){
				tree_ped_64 = pedestal;
				tree_noise_64 = noise;
			}
			//cout<<istrip<<" ped: "<<pedestal<<" noise: "<<noise<<endl;
		}

		// pedestals median // similar to average common mode
		sort(v_ped.begin(), v_ped.end());
		float median_ped;
		if (v_ped.size() % 2 == 0) median_ped = (v_ped[v_ped.size()/2 - 1] + v_ped[v_ped.size()/2]) / 2;
		else median_ped = v_ped[v_ped.size()/2];

		// confidence interval at 68%
		int lower_rank = (v_ped.size() - sqrt(v_ped.size()))/2.;
		float median_ped_min = v_ped[lower_rank];
		int upper_rank = 1 + (v_ped.size() + sqrt(v_ped.size()))/2.;
		float median_ped_max = v_ped[upper_rank];

		// noise median
		sort(v_noise.begin(), v_noise.end());
		float median_noise;
		if (v_noise.size() % 2 == 0) median_noise = (v_noise[v_noise.size()/2 - 1] + v_noise[v_noise.size()/2]) / 2;
		else median_noise = v_noise[v_noise.size()/2];

		// confidence interval at 68%
		lower_rank = (v_noise.size() - sqrt(v_noise.size()))/2.;
		float median_noise_min = v_noise[lower_rank];
		upper_rank = 1 + (v_noise.size() + sqrt(v_noise.size()))/2.;
		float median_noise_max = v_noise[upper_rank];

		tree_detid = (*itDetId);
		tree_nevt = counter;
		tree_ped = median_ped;
		tree_ped_low = median_ped_min;
		tree_ped_up = median_ped_max;
		tree_noise = median_noise;
		tree_noise_low = median_noise_min;
		tree_noise_up = median_noise_max;
		tree_run = run;
		tree_evt_avg = int(evt_avg/counter);
		tree_evt_min = evt_min;
		tree_evt_max = evt_max;
		tree_time_min = unixTime_min;
		tree_time_max = unixTime_max;
		noisetree->Fill();

		if((*itDetId)/1000==369120){
			if(run_min==run_max) std::cout<<"N_evt: "<<counter<<" run: "<<run<<" evt_range: ["<<evt_min<<":"<<evt_max<<"] evt_avg: "<<evt_avg/counter<<" time_range: ["<<unixTime_min<<":"<<unixTime_max<<"] median_noise: "<<median_noise<<" median_conf_interv: "<<median_noise_min<<"-"<<median_noise_max<<std::endl;
			else std::cout<<"N_evt: "<<counter<<" run: "<<run_min<<"-"<<run_max<<" evt_range: ["<<evt_min<<":"<<evt_max<<"] evt_avg: "<<evt_avg/counter<<" time_range: ["<<unixTime_min<<":"<<unixTime_max<<"] median_noise: "<<median_noise<<" median_conf_interv: "<<median_noise_min<<"-"<<median_noise_max<<std::endl;
		}
	}
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void NoiseBiasScan::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);

}

//define this as a plug-in
DEFINE_FWK_MODULE(NoiseBiasScan);
