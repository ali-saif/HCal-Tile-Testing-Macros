#include "cstdlib"
#include "iostream"
#include "TFile.h"
#include "cstring"
#include "TLegend.h"
#include "TGraph.h"
//#include "TROOT.h"
#include "TH3.h"
#include "TH2.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TText.h"
#include "TPaveText.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TAttFill.h"
#include "TColor.h"
#include "string"

using namespace std;

/*-------------------------

  This program analyzes data from the test
  stand currently stationed at GSU. It performs
  a little bit of offline cleanup of the Landau
  distributions from the sPHENIX HCal tiles
  and computes their performance ratios. 
  ----------------------------*/


const int nChans = 10; 
const int thresh = 250;
const int nBins = 40;
TH1F* makeGSUTree(string f, int iscalib, int angle);
void makeRunningDists(char *filelist, int mode);
float extractPerfRat(char* filelist, int angle, int RefFile1, int RefChan1, int isInner);
int countLines(char *filelist);
float getCorrFactor(int chan, int angle, int iscalib, int isInner);
ifstream& goToLine(ifstream& file, unsigned int num);
float get_ref_cal(int chan, int angle, int isInner);


TH1F* makeGSUTree(string f, int iscalib, int seg, int isInner)
{
  TFile *fin = new TFile(f.c_str());
  TTree *mppc = (TTree*)fin->Get("mppc");
  UShort_t chg[32];
  mppc -> SetBranchAddress("chg",&chg);
  TH1F *hcalNoTrig[nChans];
  TH1F *hcalSoftTrig[nChans];
  int angleco;



  //Code below implemented 4.29.21 to fix user error issue with inputting wrong tile shape into function.
  
  int angle;
  angle = atoi(f.substr(15,2).c_str()); //extracts the numbers from Z## in .root file
  
  if(angle == 0) // if substr extracted letters instead of numbers, throwing an error. atoi error throws a 0.
    {
      cout << "ERROR: Please ensure makeGSUTree is running from same directory as .root file." << '\n';
      return(0);
    }

   string response;
   cout << "Tile shape is: Z" << angle << '\n' << "Is this correct? y or n \n";
   cin >> response;

   if (response == "n")
     {
       cout << "ERROR: Please ensure makeGSUTree is running from same directory as .root file" << '\n';
       return(0);
     }
  
  
  if(!isInner)
    {
      angleco = angle - 20; 
    }
  else
    {
      angleco = angle;
    }
  
  int nEvents = mppc -> GetEntries();

  int chanList[nChans] = {11,31,30,29,28,23,22,21,20,10};
  
  TCanvas *c1 = new TCanvas();
  c1 -> Divide(4,3);
  TF1 *Fits[nChans];
  TGraph *positionDep;
 
  //The code here compiles the raw data from each channel into the hcalNoTrig histograms.
  //Then it applies an offline coincidence trigger on channels 0 and 1.
  for(int i = 0; i < nChans; i++)
    {
      c1 -> cd(i+1);
      gPad -> SetLogy();
      hcalNoTrig[i] = new TH1F(Form("Chan_%d_Open_Trigger",chanList[i]),Form("Chan_%d_Open_Trigger",chanList[i]),250,0,4000);
      hcalSoftTrig[i] = new TH1F(Form("Chan_%d_Soft_Trigger",chanList[i]),Form("Chan_%d_Soft_Trigger",chanList[i]),250,0,4000);

      for(int j = 0; j < nEvents; j++)
	{
	  mppc -> GetEntry(j);
	  hcalNoTrig[i] -> Fill(chg[chanList[i]]);

	  //This cut simply ensures that all hits embedded
	  //a certain amount of energy in the tile above the
	  //chosen threshold ADC of 250
	  
	  int chanCheck = 0;
	  for(int k = 0; k < nChans; k++)
	    {
	      if(chg[chanList[k]] >= thresh) chanCheck++;
	    }
	  if(chanCheck == nChans)  hcalSoftTrig[i] -> Fill(chg[chanList[i]]);	  
	  
	}
      
     
      hcalSoftTrig[i] -> Fit("landau","q","+",0,4100);
      hcalSoftTrig[i] -> GetYaxis() -> SetRangeUser(1,10e2);
      Fits[i] = (TF1*)hcalSoftTrig[i] -> GetFunction("landau");
      gStyle -> SetOptFit(111);
      hcalSoftTrig[i] -> Draw();
     
    }

  TCanvas *c3 = new TCanvas();
  TH1F *refScale = new TH1F("refScale","refScale",nBins,0,2);
  positionDep = new TGraph();
  int start = 1;
  int end = nChans - 1;
  cout << Form("Top reference MPV: %g", Fits[0] -> GetParameter(1)) << endl;
  for(int i = start; i < end ;i++)
    {
      
        float performance = (Fits[i] -> GetParameter(1)/(0.5*(Fits[0] -> GetParameter(1) + Fits[nChans-1] -> GetParameter(1))))*getCorrFactor(i,angleco,iscalib,isInner);
      
	//  float performance = (Fits[i] -> GetParameter(1)/(0.5*(Fits[0] -> GetParameter(1) + Fits[nChans-1] -> GetParameter(1))))*get_ref_cal(i-1,angleco,isInner); //pr plots w/o calibs
      
      //cout << "i = " << i << "; Channel = " << chanList[i] << "; PR = " << performance << "; MPV: " << Fits[i] -> GetParameter(1) << endl;
      cout << Form("i = %d; Channel = %d; PR, MPV: - %g - %g",i, chanList[i], performance, Fits[i] -> GetParameter(1)) << endl;
      refScale -> Fill(performance);
      positionDep -> SetPoint(i,i,performance);
    }
  cout << Form("Bottom reference MPV: %g", Fits[nChans-1] -> GetParameter(1)) << endl;

  
  if(iscalib)
    {
      TFile *output = new TFile(Form("calibFiles/B%dCal_%d.root",angle,seg),"RECREATE");
      cout << "output name" << output ->GetName() << endl;
      positionDep -> SetMarkerStyle(4);
      positionDep -> SetName("positionDep");
      output -> cd();
      positionDep -> Write();
      positionDep -> Draw();
    }
    
  TCanvas *test = new TCanvas();
  refScale -> Draw();
  refScale -> GetXaxis() -> SetTitle("MPV_{Tile}/<MPV_{Refs}>");
 
  return refScale;
}

void Megan_Calibration(char *filelist)
{
   const int numOfFiles = countLines(filelist);
  float minPerf = 4;
  int minPerfChan = 0;
  string minPerfFile = ""; 
  ifstream link; link.open(filelist);
  //float prs[nChans-2][numOfFiles];
  float prs[numOfFiles][nChans-2];
  float mpvs[numOfFiles][nChans-2];
  goToLine(link,0);
  
  for(int l = 0; l < numOfFiles; l++)
    {
      
      string f;
      link >> f;
      TFile *fin = new TFile(f.c_str());
      TTree *mppc = (TTree*)fin->Get("mppc");
      TH1F *mpvDist = new TH1F("mpvDist","mpvDist",200,400,1400);
      UShort_t chg[32];
      mppc -> SetBranchAddress("chg",&chg);
      TH1F *hcalNoTrig[nChans];
      TH1F *hcalSoftTrig[nChans];
      int nEvents = mppc -> GetEntries();

      int chanList[nChans] = {11,31,30,29,28,23,22,21,20,10};
      
      TCanvas *c1 = new TCanvas();
      c1 -> Divide(4,3);
      TF1 *Fits[nChans];
      TGraph *positionDep;
 
      //The code here compiles the raw data from each channel into the hcanNoTrig histograms.
      //Then it applies an offline coincidence trigger on channels 0 and 1.
      for(int i = 0; i < nChans; i++)
	{
	  c1 -> cd(i+1);
	  gPad -> SetLogy();
	  hcalNoTrig[i] = new TH1F(Form("Chan_%d_Open_Trigger",chanList[i]),Form("Chan_%d_Open_Trigger",chanList[i]),250,0,4000);
	  hcalSoftTrig[i] = new TH1F(Form("Chan_%d_Soft_Trigger",chanList[i]),Form("Chan_%d_Soft_Trigger",chanList[i]),250,0,4000);
      
	  for(int j = 0; j < nEvents; j++)
	    {
	  
	      mppc -> GetEntry(j);
	      hcalNoTrig[i] -> Fill(chg[chanList[i]]);
	  
	      //John H's method
	  
	      int chanCheck = 0;
	      for(int k = 0; k < nChans; k++)
		{
		  if(chg[chanList[k]] >= thresh) chanCheck++;
		}
	      if(chanCheck == nChans)  hcalSoftTrig[i] -> Fill(chg[chanList[i]]);
        
	  
	  
	    }
      
	  hcalSoftTrig[i] -> Fit("landau","q","+",0,4100);
	  hcalSoftTrig[i] -> GetYaxis() -> SetRangeUser(1,10e2);
	  Fits[i] = (TF1*)hcalSoftTrig[i] -> GetFunction("landau");
	  gStyle -> SetOptFit(111);
	  mpvDist -> Fill(Fits[i] -> GetParameter(1));
	    
	}
 


      
      for(int k = 1; k < nChans-1 ;k++)
	{
      
	  
	  prs[l][k-1] = (Fits[k] -> GetParameter(1)/(0.5*(Fits[0]->GetParameter(1) + Fits[9]->GetParameter(1))) );
	  mpvs[l][k-1] = Fits[k] -> GetParameter(1);
	  //cout << Form("Performance ratio for Test %d, channel %d: %g",l,k,prs[l][k-1]) << endl;
	  
	  
	}
    }


  float chan_1_pr[nChans-2];
  float corr_factor[nChans-2] = {0};
  float mpv_factor[nChans-2] = {0};
  corr_factor[0] = 1.;
  mpv_factor[0] = 1.;
  for(int i = 0; i < numOfFiles ;i++)
    {
      
      chan_1_pr[i] = prs[i][0];
      for(int j = 1; j < nChans - 2; j++)
	{
	  corr_factor[j] += prs[i][j]/prs[i][0];
	  mpv_factor[j] += mpvs[i][j]/mpvs[i][0];
	}
    }

  for(int j = 1; j < nChans-2; j++)
    {
      corr_factor[j] /= 8;
      mpv_factor[j] /= 8;
    }

  float tiles[8];
  for(int i = 0; i < nChans-2;i++)
    {
      if(i == nChans - 3) printf("PR factor: %g; MPV Factor: %g \n",corr_factor[i], mpv_factor[i]);
      else printf("PR factor: %g ; MPV factor: %g \n",corr_factor[i],mpv_factor[i]);
      tiles[i] = i+1;
    }

  
  TGraph *factors = new TGraph(nChans-2,tiles,corr_factor);
  factors -> SetMarkerStyle(4);
  factors -> SetTitle(";Channel Num;Correction Factor");
  TCanvas *derp = new TCanvas();
  factors -> Draw("ap");
}


//This is the function that selects new reference tiles.
//At the time of writing, we'll probably need to use it one more time
//for the M series tiles.
float extractPerfRat(char* filelist, int angle, int RefFile1 = 40, int RefChan1 = 40, int isInner = 1)
{
  const int numOfFiles = countLines(filelist);
  float minPerf = 4;
  int minPerfChan = 0;
  string minPerfFile = ""; 
  ifstream link; link.open(filelist);
  float prs[numOfFiles][nChans-2];
  float calib[nChans-2] = {0};
  goToLine(link,0);
  float globalAve = 0;
  float best_perf = 0;
  if(!isInner) angle -= 20;

  
  for(int l = 0; l < numOfFiles; l++)
    {
      
      string f;
      link >> f;
      TFile *fin = new TFile(f.c_str());
      TTree *mppc = (TTree*)fin->Get("mppc");
      TH1F *mpvDist = new TH1F("mpvDist","mpvDist",200,400,1400);
      UShort_t chg[32];
      mppc -> SetBranchAddress("chg",&chg);
      TH1F *hcalNoTrig[nChans];
      TH1F *hcalSoftTrig[nChans];
      int nEvents = mppc -> GetEntries();
      int chanList[nChans] = {11,31,30,29,28,23,22,21,20,10};
      
      TCanvas *c1 = new TCanvas();
      c1 -> Divide(4,3);
      TF1 *Fits[nChans];
      TGraph *positionDep;
 
      //The code here compiles the raw data from each channel into the hcanNoTrig histograms.
      //Then it applies an offline coincidence trigger on channels 0 and 1.
      for(int i = 0; i < nChans; i++)
	{
	  c1 -> cd(i+1);
	  gPad -> SetLogy();
	  hcalNoTrig[i] = new TH1F(Form("Chan_%d_Open_Trigger",chanList[i]),Form("Chan_%d_Open_Trigger",chanList[i]),250,0,4000);
	  hcalSoftTrig[i] = new TH1F(Form("Chan_%d_Soft_Trigger",chanList[i]),Form("Chan_%d_Soft_Trigger",chanList[i]),250,0,4000);
      
	  for(int j = 0; j < nEvents; j++)
	    {
	  
	      mppc -> GetEntry(j);
	      hcalNoTrig[i] -> Fill(chg[chanList[i]]);
	  
	      //John H's method
	  
	      int chanCheck = 0;
	      for(int k = 0; k < nChans; k++)
		{
		  if(chg[chanList[k]] >= thresh) chanCheck++;
		}
	      if(chanCheck == nChans)  hcalSoftTrig[i] -> Fill(chg[chanList[i]]);
        
	  
	  
	    }
      
	  hcalSoftTrig[i] -> Fit("landau","q","+",0,4100);
	  hcalSoftTrig[i] -> GetYaxis() -> SetRangeUser(1,10e2);
	  Fits[i] = (TF1*)hcalSoftTrig[i] -> GetFunction("landau");
	  gStyle -> SetOptFit(111);
	  mpvDist -> Fill(Fits[i] -> GetParameter(1));
	    
	}
 


      
      for(int k = 1; k < nChans-1 ;k++)
	{
      
	  
	  prs[l][k-1] = (Fits[k] -> GetParameter(1)/(0.5*(Fits[0]->GetParameter(1) + Fits[9]->GetParameter(1)))) * get_ref_cal(k-1,angle,isInner);
	 //cout << Form("Performance ratio for Test %d, channel %d: %g",l,k,prs[l][k-1]) << endl;
	  
	  
	}
    }
  
  /* This was an initial, mistaken calibration
  for(int q = 0; q < nChans-2; q++)
    {
      for(int a = 0; a < numOfFiles; a++)
	{
	  calib[q] += prs[a][q];
	}
      calib[q] /= numOfFiles;
      cout << Form("Calibration factor for channel %d is %g", q,1/calib[q]) << endl;
    }
  
  
  */
  for(int i = 0; i < numOfFiles; i++)
    {
      for(int j = 0; j < nChans - 2; j++)
	{
	  globalAve += prs[i][j];
	}
    }
  globalAve /= numOfFiles*(nChans-2);
  cout << Form("Global average: %g",globalAve) << endl;
  
  
  for(int g = 0; g < numOfFiles; g++)
    {
      for(int d = 0; d < nChans - 2; d++)//for(int d = 1; d < nChans-1;d++)
	{
	  if((abs(globalAve-prs[g][d]) < minPerf) && g != RefFile1 && d != RefChan1)
	    {
	      minPerfChan = d+1;
	      goToLine(link,g);
	      link >> minPerfFile;
	      minPerfFile += "\n filenum: ";
	      minPerfFile += to_string(g);
			     
	      minPerf = abs(globalAve - prs[g][d]);
	      best_perf = prs[g][d];
	    }
	  else
	    {
	      //cout << "Skip!" << endl;
	    }
	}
    }
  cout << "File: " << minPerfFile <<endl;
  cout << "Channel: " << minPerfChan << endl;
  cout << "Global average: " << globalAve << endl;
  cout << "Ratio: " << best_perf << endl;
  cout << "Uncalibrated ratio: " << best_perf*(1/get_ref_cal(minPerfChan-1,angle,isInner)) << endl;
  return 0;
}

void makeRunningDists(char *filelist,  char* outname, int iscalib=0, int isInner=0)
{
  ifstream list;
  int numOfFiles = countLines(filelist);
  cout << numOfFiles << endl;
  list.open(filelist);
  TH1F *runningDist = new TH1F("runningDist","runningDist",nBins,0,2);
  TFile *output = new TFile(Form("%s.root",outname), "RECREATE");
  
  for(int i = 0; i < numOfFiles; i++)
    {
      string name;
      list >> name;
      cout << name << endl;
      runningDist -> Add(makeGSUTree(name,iscalib,i, isInner));
    }
  runningDist -> Draw();
  runningDist -> SetName(outname);
  output -> cd();
  runningDist -> Write();
  output -> Close();  
}

void makePositDep(char *filelist, int angle, const int fileNum, int iscalib, int isInner)
{
  
  TMultiGraph *allChans = new TMultiGraph();
  TFile *ins[fileNum];
  ifstream tilelist;
  tilelist.open(filelist);
  TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
  float corrFac[nChans] = {0};
  float corrFacErr[nChans] = {0};
  int added[4] = {0};
  float tileCorr = 0;
  
  for(int i = 0; i < fileNum; i++)
    {
      string name;
      tilelist >> name;
      cout << name << endl;
      ins[i] = new TFile(name.c_str());
      TGraph *dummy = (TGraph*)ins[i] -> Get("positionDep");
      TGraph *dummy2 = new TGraph();
      dummy2 -> SetMarkerStyle(4);
      dummy2 -> GetYaxis() -> SetRangeUser(0,2);
      int start = 1;
      int end = nChans-1;
      
      for(int j = start; j < end; j++)
	{
	  double x,y;
	  dummy -> GetPoint(j,x,y);
	  
	  dummy2 -> SetPoint(j,x,y*getCorrFactor(j,angle-20,iscalib,isInner));
	   
	  corrFac[j] += y*getCorrFactor(j,angle-20,iscalib,isInner);
	    
	}
      
     
      if(i == 0)  leg -> AddEntry(dummy2,Form("B%d",angle),"p");

      allChans -> Add(dummy2,"p");
      
    }
 
  
  for(int i = 0; i < nChans; i++)
    {
      corrFac[i] /= fileNum;
    }
  float std[nChans] = {0};
  goToLine(tilelist,0);
  for(int i = 0; i < fileNum; i++)
    {
      
      string name;
      tilelist >> name;
      cout << name << endl;
      ins[i] = new TFile(name.c_str());
      TGraph *dummy = (TGraph*)ins[i] -> Get("positionDep");
      TGraph *dummy2 = new TGraph();
      dummy2 -> SetMarkerStyle(4);
      dummy2 -> GetYaxis() -> SetRangeUser(0,2);
      int start = 1;
      int end = nChans-1;
      
      for(int j = start; j < end; j++)
	{
	  double x,y;
	  dummy -> GetPoint(j,x,y);
	  
	  dummy2 -> SetPoint(j,x,y*getCorrFactor(j,angle-20,iscalib,isInner));
	   
	  //corrFac[j] += y*getCorrFactor(j,angle-20,iscalib);
	  std[j] = pow(corrFac[j] - y,2);
	}
      
     
      if(i == 0)  leg -> AddEntry(dummy2,Form("B%d",angle),"p");

      allChans -> Add(dummy2,"p");
      
    }
  
  
  for(int i = 0; i < nChans; i++)
    {
      std[i] /= fileNum -1;
      std[i] = sqrt(std[i]);
      cout << "Average for position " << i << ": " << corrFac[i] << "\u00b1" << " " << std[i] << endl;
    }

  
  allChans -> Draw("ap");
  allChans -> GetXaxis() -> SetTitle("SiPM Position Number");
  allChans -> GetYaxis() -> SetTitle("MPV_{Tile}/<MPV_{Test}>");
  allChans -> GetYaxis() -> SetRangeUser(0,2);
  leg -> Draw("same");

 
}



float getCorrFactor(int chan, int angle, int iscalib, int isInner)
{

  /* cout << "channel" << chan << endl;
  cout << "angle " << angle << endl;*/

  /*Calculated from Megan's method*/
  /*
  float dualFactor_outer[8][16] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},

			     {1.01817,1.02358,1.02595,1.02806,1.03664,1.00424,1.02599,1.04466,1.02418,1.01737,1.01615,1.02268,1.04851,1.02973,1.00965,0.983839},

			     {0.998495,1.02019,1.03223,1.02634,1.01937,1.01391,0.983734,1.02282,0.986605,1.01226,1.0111,1.01303,1.01086,1.03076,0.981761,1.0073},

			     {1.02026,1.03256,1.01491,1.02568,1.01937,1.01067,1.02211,1.03948,1.03135,1.0225,1.01507,1.02949,1.01695,1.07301,0.99169,1.0121},

			     {1.02171,1.02411,0.993743,1.03655,1.01775,1.01989,1.03624,1.05188,1.00428,1.02784,1.02385,1.02885,1.05491,1.07485,1.0424,1.03879},

			     {1.00865,1.03165,1.02716,1.03144,1.02821,1.02497,1.02907,1.05165,1.03411,1.02415,1.02023,1.02713,1.02549,1.02897,0.990605,0.993219},

			     {0.987429,0.987875,0.971813,0.995072,0.976231,0.975675,0.986339,0.997131,0.995258,0.977848,0.984255,0.980894,0.988101,0.981188,0.957596,0.982809},

			     {1.06346,1.06623,1.07072,1.06697,1.06533,1.04125,1.05711,1.07716,1.04637,1.04856,1.04132,1.05508,1.05645,1.05337,1.02704,1.02683}};
    */
//dualFactor_outer below is for pre-production calibration factors
    
  //  float dualFactor_outer[8][16] ={{0.810873,0.814866,0.794136,0.763176,0.935977,0.918277,0.854018,0.962745,0.870416,0.967691,0.812866,0.873582,1,1,1,1},
    //    {0.847336,0.854614,0.806812,0.769812,0.934153,0.946811,0.864736,0.972421,0.902587,0.88737,0.830939,0.888037,1.04851,1.02973,1.00965,0.983839},
    //    {0.838685,0.821466,0.778553,0.70985,0.893511,0.938996,0.826639,0.968322,0.962354,0.955715,0.922084,0.967667,1.01086,1.03076,0.981761,1.0073},
    //    {0.88497,0.841338,0.788198,0.775766,0.947077,0.938762,0.845317,0.993708,0.961574,1.04429,0.887333,0.966277,1.01695,1.07301,0.99169,1.0121},
    //    {0.838685,0.872622,0.797924,0.831189,0.939296,0.935322,0.921756,0.963874,0.979116,0.82606,0.954908,0.875859,1.05491,1.07485,1.0424,1.03879},
    //    {0.865029,0.887264,0.816234,0.833545,0.97727,0.969677,0.831858,0.985357,0.970586,1.00305,0.864868,0.8912,1.02549,1.02897,0.990605,0.993219},
    //    {0.848827,0.860359,0.791134,0.811859,0.902238,0.900005,0.793583,0.93354,0.918294,0.926915,0.7502,0.934033,0.988101,0.981188,0.957596,0.982809},
    //    {0.831169,0.923422,0.877023,0.817548,0.99918,1.01151,0.9256,0.979267,0.954242,1.00587,0.927643,0.99419,1.05645,1.05337,1.02704,1.02683}};

  //Really need to clear out some of these defunct correction factors. It's like hoarding, but with git versioning.
  /*
  //channel by channel corrections for the inner tiles. 
  float dualFactor_inner[8][12] = {{1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.},

				   {1.07165, 1.06338, 1.07223, 1.074551, 1.07455, 1.07455, 1.07455, 1.07455, 1.07455, 1.07455, 1.07455, 1.07455},

				   {0.995857, 0.987382, 0.996873, 0.997897, 0.997897, 0.997897, 0.997897, 0.997897, 0.997897, 0.997897, 0.997897, 0.997897},

				   {1.09659, 1.09202, 1.08533, 1.094751, 1.094751, 1.094751, 1.094751, 1.094751, 1.094751, 1.094751, 1.094751, 1.094751},

				   {1.08582, 1.06764, 1.07686, 1.079103, 1.079103, 1.079103, 1.079103, 1.079103, 1.079103, 1.079103, 1.079103, 1.079103},
				   
				   {1.05814, 1.04915, 1.05081, 1.054442, 1.054442, 1.054442, 1.054442, 1.054442, 1.054442, 1.054442, 1.054442, 1.054442},

				   {1.09318, 1.08139, 1.07962, 1.090255, 1.090255, 1.090255, 1.090255, 1.090255, 1.090255, 1.090255, 1.090255, 1.090255},

				   {1.05627, 1.04471, 1.05531, 1.058696, 1.058696, 1.058696, 1.058696, 1.058696, 1.058696, 1.058696, 1.058696, 1.058696}};

  */	   
  float dualFactor_inner[8][12] = {{1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1}};
  float dualFactor_outer[8][16] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};  
				   
				   
  /*
    float dualFactor[8][12] = {{0.932662,0.957156,1.0051,,,,,,,,,},
    {0.959198,0.981928,1.00981,,,,,,,,,},
    {0.940088,1.00062,1.01009,,,,,,,,,},

    {0.940884,1.00404,1.01967,,,,,,,,,},
    {0.954588,1.00085,1.01337,,,,,,,,,},
    {0.943405,1.00374,1.02564,,,,,,,,,},
    {0.903525,0.967802,0.984223,,,,,,,,,},
    {0.971448,1.04533,1.01663,,,,,,,,,}};
    */
  /*
  //outer hcal tile meanshifts
  float MeanShift_outer[16] = {0.9651, //b21
			      0.9687, //b22
			       1.015307156, //b23
			       0.826050275, //b24
			       0.8855, //b25
			       0.937601075, //b26
			       0.943933, //b27
			       0.895491, //b28
			       0.980414, //b29
			       1.076, //b30
			       1.009, //b31
			       0.999813, //b32
			       0.921295156, //M49
			       0.852388531, //M50
			       0.977862313, //M51
			      0.95566625}; //M52
*/
  float MeanShift_outer[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
   //below meanshifts are for when i run pre-pro tiles. their meanshift is inclusive to their channel calibs.
    //float MeanShift_outer[16] = {1, //b21
      //               1, //b22
        //             1, //b23
          //           1, //b24
            //         1, //b25
              //       1, //b26
                //     1, //b27
                  //   1, //b28
                   //  1, //b29
                   //  1, //b30
                   //  1, //b31
                    // 1, //b32
                    // 1, //M49
                    // 1, //M50
                    // 1, //M51
                    // 1}; //M52
  /*
  //inner hcal tile meanshifts
  float MeanShift_inner[12] = {0.937061725, //z01
			       0.9117, //z02
			       0.8483, //z03
			       0.9618, //z04
			       0.8598, //z05
			       1.0409, //z06
			       0.8924, //z07
			       0.9317348214, //z08
			       0.9373453214, //z09
			       0.9157617857, //z10  0.9157617857
			       0.9347895, //z11
			       0.9531559643}; //z12
*/
  float MeanShift_inner[12] = {1,1,1,1,1,1,1,1,1,1,1,1};
  
  if(iscalib)
    {
      return 1;
    }
  else if(!isInner)
    {
      /*cout << "dualFactor: " << ((dualFactor[chan-1][angle-1])) << endl;
	cout << "Meanshift: " << ((MeanShift[angle-1])) << endl;*/
      return 1/((dualFactor_outer[chan-1][angle-1])*(MeanShift_outer[angle-1]));
    }
  else
    {
      return 1/((dualFactor_inner[chan-1][angle-1])*(MeanShift_inner[angle-1]));
    }
}


float get_ref_cal(int chan, int angle, int isInner)
{
  //these are temporary calibration factors used in
  //reference tile selection
  float ref_calib_inner[8][12] = {{1,1,1,1,1,1,1,1,1,1,1,1},
				  {1.09254,1.08072,1.06152,1.06284,1.07263,1.08286,1.06282,1.0925,1.08144,1.07029,1.08234,1.05211},
				  {1.01463,0.977425,0.996254,0.991647,1.00133,1.003,0.995271,1.01575,1.0081,0.995584,0.98616,0.989607},
				  {1.10383,1.07998,1.08392,1.0978,1.10092,1.10635,1.08708,1.11422,1.10278,1.09214,1.09615,1.07184},
				  {1.09812,1.08305,1.06946,1.08178,1.07486,1.08627,1.06243,1.07449,1.07583,1.08294,1.10083,1.05917},
				  {1.07396,1.05123,1.03923,1.05756,1.04146,1.0692,1.04043,1.08238,1.05441,1.05853,1.03547,1.04944},
				  {1.12227,1.09626,1.08433,1.08972,1.08429,1.09871,1.08115,1.08722,1.09142,1.08283,1.08957,1.07529},
				  {1.07143,1.06519,1.05266,1.05821,1.0592,1.0739,1.03377,1.06451,1.06081,1.06519,1.049891,1.04959}};

  float ref_calib_outer[8][16] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
				  {1,1,1,1,1,1,1,1,1,1,1,1,1.01355,1.04565,1.01552,1.01196},
				  {1,1,1,1,1,1,1,1,1,1,1,1,0.97204,0.990323,0.959554,0.96515},
				  {1,1,1,1,1,1,1,1,1,1,1,1,1.02477,1.02193,1.02547,1.02192},
				  {1,1,1,1,1,1,1,1,1,1,1,1,1.02764,1.04103,1.02464,1.0105},
				  {1,1,1,1,1,1,1,1,1,1,1,1,0.994073,1.01793,0.973989,0.995516},
				  {1,1,1,1,1,1,1,1,1,1,1,1,0.945205,0.977925,0.953201,0.969303},
				  {1,1,1,1,1,1,1,1,1,1,1,1,1.03831,1.06787,1.0227,0.997852}};
 
  
  
  if(!isInner)
    {
      return 1/ref_calib_outer[chan][angle-1];
    }
  else
    {
      return 1/ref_calib_inner[chan][angle-1];
    }
}

int countLines(char *filelist) { 
  int number_of_lines = 0;
  std::string line;
  std::ifstream myfile(filelist);

  while (std::getline(myfile, line))++number_of_lines;
  myfile.seekg (0, ios::beg);
  return number_of_lines;
     
}

ifstream& goToLine(ifstream& file, unsigned int num)
{
  file.seekg(0);
  for(int i = 0; i < num; i++)
    {
      file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
  return file;
}
