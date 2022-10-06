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
const int colors[5] = {1,2,4,8,9};
int countLines(char *filelists);

void buildChanDist(int mode)
  {

  TDatime *datemade = new TDatime();
  TFile *output = new TFile(Form("Master_Output_%d.root",datemade -> GetDate()),"RECREATE");

  gStyle -> SetOptStat(1110);
  TH1F *globalDist = new TH1F("ChanDist","ChanDist",40,0,2);
 
  if(mode == 0)
    {
    TH1F *chans[8];
    ifstream chanSource[8];
    TCanvas *c[8];
    //TCanvas *c1[8];
    for(int i = 0; i < 8; i++)
	  {
      if(i<4)
      {
      chanSource[i].open(Form("excelLists/chanBreakdown/chan%d.txt",i+20));
      chans[i]=new TH1F(Form("C%d",i+20),Form("C%d",i+20),40,0,2);
      chans[i] -> SetTitle(Form("C%d;MPV_{Tile}/<MPV_{Refs}>;",i+20));
      for(int j = 0; j < countLines(Form("excelLists/chanBreakdown/chan%d.txt",i+20)); j++)
      {  
        string x;
        chanSource[i] >> x;
        cout <<Form("C%d: %s", i+20,x.c_str())<<endl;
        chans[i] -> Fill(stof(x));
      }
      globalDist -> Add(chans[i],1);
      c[i]= new TCanvas();
      chans[i] -> Draw();
      output -> cd();
      chans[i] -> Write();
      c[i] -> SaveAs(Form("excelLists/chanBreakdown/plots/C%d.pdf",i+20));
      }

      if(i>3)
      {
      chanSource[i].open(Form("excelLists/chanBreakdown/chan%d.txt",i+24));
      chans[i]=new TH1F(Form("C%d",i+24),Form("C%d",i+24),40,0,2);
      chans[i] -> SetTitle(Form("C%d;MPV_{Tile}/<MPV_{Refs}>;",i+24));
      for(int j = 0; j < countLines(Form("excelLists/chanBreakdown/chan%d.txt",i+24)); j++)
      {  
        string x;
        chanSource[i] >> x;
        cout <<Form("C%d: %s", i+24,x.c_str())<<endl;
        chans[i] -> Fill(stof(x));
      }
      globalDist -> Add(chans[i],1);
      c[i]=new TCanvas();
      chans[i] -> Draw();
      output -> cd();
      chans[i] -> Write();
      c[i] -> SaveAs(Form("excelLists/chanBreakdown/plots/C%d.pdf",i+24));
      }
  }

    //ch.1=20,ch.2=21,ch.3=22,ch.4=23,ch.5=28,ch.6=29,ch.7=30,ch.8=31
  
    TCanvas *all = new TCanvas();
    globalDist -> SetTitle("");
    globalDist -> SetXTitle("MPV_{Tile}/<MPV_{Refs}>");
    globalDist -> Draw();
    globalDist -> Write();
    all -> SaveAs("excelLists/chanBreakdown/plots/global.pdf");
    output -> Close();
  }
  
  if(mode == 1)
  {
    ifstream globaldist;
    globaldist.open("excelLists/chanBreakdown/excelDist.txt");
    int nLines = countLines("excelLists/chanBreakdown/excelDist.txt");
    cout << nLines << endl;
    for(int i = 0; i < nLines; i++)
	{
	  string pr;
	  globaldist >> pr;
	  globalDist -> Fill(stof(pr));
	}
    TCanvas *all = new TCanvas();
    globalDist -> SetTitle("");
    globalDist -> SetXTitle("MPV_{Tile}/<MPV_{Refs}>");
    globalDist -> Draw();
    globalDist -> Write();
    all -> SaveAs("excelLists/chanBreakdown/plots/global.pdf");
    output -> Close();
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
