
#include <iostream>
#include <sstream>
#include <array>
#include <string>
#include <fstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TPad.h"
#include "TF1.h"
#include "TProfile.h"

#include "../../src/mask.h"
#include "../../src/draw_map.h"

using namespace std;

void rin_test(Int_t run_num) {

  float vbs_top_high = 1.2;
  float vbs_top_low = 0.4;
  float vbs_a_high = 25000.;
  float vbs_a_low = 100.;
  float vbs_b_high = 300.;
  float vbs_b_low = 0.;

  float vbs_top;
  float vbs_a;
  float vbs_b;

  char hist0_name[512]; 
  char hist1_name[512];
  char file0_name[512];
  char dir_name[512];
  char figure0_name[512];
  char figure1_name[512];

  sprintf(dir_name,"mkdir ../../img/%i",run_num);
  system(dir_name);
  sprintf(dir_name,"mkdir ../../img/%i/rin_test",run_num);
  system(dir_name);

  TH2F *h0_temp = new TH2F();
  TProfile *p0_temp = new TProfile();
  TF1 *fitFunction = new TF1();
  TFile *_file0 =  new TFile();
  double rinSlope = 0;
  double rinOffset = 0;
  
  sprintf(file0_name,"../../dat/QIE10testing_%i_8.root",run_num);
  _file0 = TFile::Open(file0_name);

  gStyle->SetOptStat(0);


  TCanvas *c1 = new TCanvas("c1","c1",100,100,1024,768);
  //sprintf(hist2_name,"%s/%s","PedScan_qav_EV","PedScan_qav_EV");
  //sprintf(figure0_name,"../../img/%i/ped_test/Scan.png",run_num);
  //h2_temp = (TH1F*)_file0->Get(hist2_name);
  //h2_temp->Draw("box");
  //c1->SaveAs(figure0_name);
  //c1->Clear();
  //h2_temp->Delete();


  bool*** lv2_err_map_slope = create_error_map();
  bool*** lv2_err_map_zeropoint = create_error_map();
  bool*** lv2_err_map_gen = create_error_map();
  for (Int_t h = 0; h < HF_num; h++) {
    if (lv0_mask[h] == 1) {
      for (Int_t s = 0 ; s < SL_num; s++) {
	if (lv1_mask[h][s] == 1) {
	  for (Int_t q = 0; q < QI_num; q++) {
	    if (lv2_mask[h][s][q] == 1) {
	      sprintf(hist0_name,"%s/%s_HF%i_Slot%i_QIE%i","refabs_vs_rin_CH","refabs_vs_rin_CH",h+1,s+1,q+1);
	      cout << hist0_name << endl;
	      h0_temp = (TH2F*)_file0->Get(hist0_name);
	      p0_temp = (TProfile*) h0_temp->ProfileX();
	      p0_temp->Fit("pol1","Q");
	      fitFunction = p0_temp->GetFunction("pol1");
	      double rinSlope = fitFunction->GetParameter(1);
	      double rinOffset = fitFunction->GetParameter(0);
	      
	      // cout << "HF: " << h+1 << ", SL: " << s+1 << ", QI: " << q+1 << " -- TurnOnPoint: " << vbs_top << ", a(x-top)^2: " << vbs_a << ", +b: " << vbs_b << endl;
	      sprintf(figure0_name,"../../img/%i/rin_test/scan_HF%i_SL%i_QI%i.png",run_num,h+1,s+1,q+1);
	      //h0_temp->SetTitle("Bias Voltage Scan with Sequencer");
	      h0_temp->GetXaxis()->SetTitle("RinSel Value");
	      h0_temp->GetYaxis()->SetTitle("Charge in Reflection (fC)");
	      h0_temp->GetYaxis()->SetTitleOffset(1.5);
	      c1->SaveAs(figure0_name);
	      c1->Clear();

	      if ((rinSlope < 0.01)){
		lv2_err_map_slope[h][s][q] = 0;
		lv2_err_map_gen[h][s][q] = 0;
	      }	

	      if ((rinOffset>0) || ((rinSlope*15+rinOffset)<0)) {
		lv2_err_map_zeropoint[h][s][q] = 0;
		lv2_err_map_gen[h][s][q] = 0;
	      }	


	      // sprintf(hist1_name,"%s/%s_HF%i_Slot%i_QIE%i","qratio2_vb_PR","qratio2_vb_PR",h+1,s+1,q+1);
	      // h1_temp = (TH1F*)_file0->Get(hist1_name);
	      // sprintf(figure1_name,"../../img/%i/vbs_test/ratio2_HF%i_SL%i_QI%i.png",run_num,h+1,s+1,q+1);
	      // //h0_temp->SetTitle("Bias Voltage Scan with Sequencer");
	      // h1_temp->GetXaxis()->SetTitle("LED Bias Voltage (V)");
	      // h1_temp->GetYaxis()->SetTitle("(Qmax+Qnext)/Qsum");
	      // h1_temp->GetYaxis()->SetTitleOffset(1.5);
	      // h1_temp->Draw();
	      // c1->SaveAs(figure1_name);
	      // c1->Clear();

	      /*
	      if ((ped_slope > ped_slope_high) || (ped_slope < ped_slope_low) || (h1_temp->GetEntries() < 10)) {
		lv2_err_map_slope[h][s][q] = 0;
		lv2_err_map_gen[h][s][q] = 0;
	      }	
	      */
	      h0_temp->Delete();
	      p0_temp->Delete();
	    }
	  }
	}
      }
    }
  }
  
  draw_map(lv2_err_map_slope, run_num, "rin_test", "RinScanSlope");
  draw_map(lv2_err_map_zeropoint, run_num, "rin_test", "RinScanZeroPoint");
  draw_map(lv2_err_map_gen, run_num, "rin_test", "RinScanAll");
  
} // close function

