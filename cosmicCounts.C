/*

Name: cosmicCounts.C
Author: Saif Ali
Date: Feb. 18, 2022

Updated: March 25, 2022
Updated by: Saif Ali
Purpose: Added capability to add Z05 and Z12 tiles.

Updated: April 1, 2022 
Updated by: Saif Ali
Purpose: Added TGraph for plotting percent change in ADC Count per Minute. Macro now plots both ADC Counts and the ADC Percent Change on the same canvas.

Updated: INSERT NEXT UPDATE DATE HERE 
Updated by: WHOMST?
Purpose: WHAT IS THE UPDATE?

----//----

To run code:
- Create .txt file with a list of the .root files you wish to analyze.
- File name convention should be: mppc_MM_DD_YY_BXX_WHATEVER_SUFFIX_YOU_WANT.root
- Start root with $ root -l
root [0] .L cosmicCounts.C
root [1] makeEntryNum("rootfilelist.txt", "cosmic-counts")

*/

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
#include "TTimeStamp.h"
#include "TMultiGraph.h"
#include "TAttFill.h"
#include "TColor.h"
#include "string"

using namespace std;
int countLines(char *filelist);

/*
This function below pulls the raw number of counts seen by the CAEN unit and applies a time stamp to when the data was taken.
ADC count per minute VS Date is plotted. 
You must feed it a .txt file with a list of all the .root files you wish to analyze. 
*/

void makeEntryNum(char *filelist, char *title)
{
  ifstream list;
  int numOfFiles = countLines(filelist);
  double counts[numOfFiles];
  cout << numOfFiles << endl;
  list.open(filelist);

  TGraph *b25cosmics = new TGraph();
  TGraph *z05cosmics = new TGraph();
  TGraph *z12cosmics = new TGraph();

  TGraph *b25change = new TGraph();
  TGraph *z05change = new TGraph();
  TGraph *z12change = new TGraph();

  int shape, year, month, day;

  double sum_b25 = 0.0;
  double avg_b25 = 0.0;
  
  double sum_z05 = 0.0;
  double avg_z05 = 0.0;
  
  double sum_z12 = 0.0;
  double avg_z12 = 0.0;
  
  for(int i = 0; i < numOfFiles; i++)
    {
      string name;
      list >> name;
      //cout << name << endl;
      
      TFile *fin = new TFile(name.c_str());
      TTree *mppc = (TTree*)fin->Get("mppc");

      //cout << mppc -> GetEntries() << endl;
      
      int raw_count = mppc -> GetEntries();
    
      //atoi() converts a string into an integer
      year = atoi(name.substr(11,2).c_str()); //pulls year from .root file, but our .root files only have "20" or "21" or "22" in them, which is why the next line adds 2000 to this
      int yr = year + 2000; //see above
      month = atoi(name.substr(5,2).c_str()); //pulls month number from  .root file
      day = atoi(name.substr(8,2).c_str()); //pulls day number from .root file

      TDatime *timestamp = new TDatime(yr,month,day,0,0,0); //defines time stamp. last three paramters are min, sec, microsec

      int time = timestamp -> Convert(); // converts time stamp to seconds

  /*
      cout << "time: " << time << endl;
      cout << "year: " << yr << endl;
      cout << "month: " << month << endl;
      cout << "day: " << day << endl;
  */

      double percent_change_b25 = 0.0;
      double percent_change_z12 = 0.0;
      double percent_change_z05 = 0.0;

      int b25_i = 0;
      int z05_i = 0;
      int z12_i = 0;
      
      shape = atoi(name.substr(15,2).c_str());

       if (shape == 25)
	 {
	   // I dont like this. Will find more eloquent way to do this.
	   // Necessary because B25 tiles were sometimes ran at 30 min instead of 20 min
	   if (raw_count < 20000) //if statements here calculates counts per minute
	     {
	       counts[i] = raw_count/20; //for tests ran for 20 minutes
	     }
	   else if (raw_count > 20000)
	     {
	       counts[i] = raw_count/30; //for tests ran for 30 minutes
	     }
	   sum_b25 += counts[i];
	   b25_i += i+1;
	   avg_b25 = (sum_b25)/(b25_i); //running average
	   //cout << "b25 sum: " << sum_b25 << endl;
	   //cout << "b25 avg: " << avg_b25 << endl;
	   //cout << "i+1: " << b25_i << endl;
	   percent_change_b25 = ((counts[i] - avg_b25)/(abs(avg_b25)))*100; //compared to running average
	   b25cosmics -> SetPoint(i, time, counts[i]); //i+1 for 1st parameter registers a data point at (0,0)
	   b25change -> SetPoint(i, time, percent_change_b25);
	 }

       else if (shape == 12)
	 {
	   if (raw_count < 5000)
	     {
	       counts[i] = raw_count/30;
	     }
	   else if ( raw_count > 5000)
	     {
	       cout << "ERROR: TEST NOT RAN FOR 30 MIN." << endl;
	     }
	   sum_z12 += counts[i];
	   z12_i += i+1;
	   avg_z12 = (sum_z12)/(z12_i); //running average
	   percent_change_z12 = ((counts[i]-counts[i-1])/(abs(counts[i-1])))*100;
	   z12cosmics -> SetPoint(i, time, counts[i]);
	   z12change -> SetPoint(i, time, percent_change_z12);
	 }

       else if (shape == 05)
	 {
	   if (raw_count < 3000)
	     {
	       counts[i] = raw_count/30;
	     }
	   else if ( raw_count > 3000)
	     {
	       cout << "ERROR: TEST NOT RAN FOR 30 MIN." << endl;
	     }
	   sum_z05 += counts[i];
	   z05_i += i+1;
	   avg_z05 = (sum_z05)/(z05_i); //running average
	   percent_change_z05 = ((counts[i]-counts[i-1])/(abs(counts[i-1])))*100;
	   z05cosmics -> SetPoint(i, time, counts[i]);
	   z05change -> SetPoint(i, time, percent_change_z05);
	 }
    }

  TLegend* leg = new TLegend();

  // Define the plotting canvas
  TCanvas *c1 = new TCanvas("c1","Time Series Plots",200,10,1480,800);
  c1->Divide(1,2);
  
  b25cosmics -> SetMarkerStyle(20);
  b25cosmics -> SetMarkerColor(kRed);
  z12cosmics -> SetMarkerStyle(20);
  z12cosmics -> SetMarkerColor(1);
  z05cosmics -> SetMarkerStyle(20);
  z05cosmics -> SetMarkerColor(4);
  b25cosmics -> GetYaxis() -> SetRangeUser(0,1000);
  b25cosmics -> GetXaxis() -> SetTitle("Date");
  b25cosmics -> GetXaxis() -> SetTimeFormat("%Y/%m/%d");
  b25cosmics -> GetXaxis() -> SetTimeOffset(0,"gmt"); //sets the "time" to gmt standard so x axis is dated and isnt in seconds
  b25cosmics -> GetYaxis() -> SetTitle("ADC Count per Minute");
  b25cosmics -> GetXaxis() -> SetTitleSize(0.05); // 0.035 by default, i found that to be too small
  b25cosmics -> GetYaxis() -> SetTitleSize(0.05);
  b25cosmics -> GetYaxis() -> SetTitleOffset(0.5);

  leg -> AddEntry(b25cosmics, "B25");
  leg -> AddEntry(z05cosmics, "Z05");
  leg -> AddEntry(z12cosmics, "Z12");
  c1 -> cd(1);
  b25cosmics -> Draw("ap");
  z12cosmics -> Draw("samep");
  z05cosmics -> Draw("samep");
  leg -> Draw("samep");

  c1 -> cd(2);
  
  TLegend* leg_change = new TLegend();
  
  b25change -> SetMarkerStyle(20);
  b25change -> SetMarkerColor(kRed);
  z12change -> SetMarkerStyle(20);
  z12change -> SetMarkerColor(1);
  z05change -> SetMarkerStyle(20);
  z05change -> SetMarkerColor(4);
  b25change -> GetYaxis() -> SetRangeUser(-20,20);
  b25change -> GetXaxis() -> SetTitle("Date");
  b25change -> GetXaxis() -> SetTimeFormat("%Y/%m/%d");
  b25change -> GetXaxis() -> SetTimeOffset(0,"gmt"); //sets the "time" to gmt standard so x axis is dated and isnt in seconds
  b25change -> GetYaxis() -> SetTitle("ADC Count per Min (Percent Change)");
  b25change -> GetXaxis() -> SetTitleSize(0.05); // 0.035 by default, i found that to be too small
  b25change -> GetYaxis() -> SetTitleSize(0.05);
  b25change -> GetYaxis() -> SetTitleOffset(0.5);
 
  leg_change -> AddEntry(b25cosmics, "B25");
  leg_change -> AddEntry(z05cosmics, "Z05");
  leg_change -> AddEntry(z12cosmics, "Z12");
  
  b25change -> Draw("ap");
  z12change -> Draw("samep");
  z05change -> Draw("samep");
  leg_change -> Draw("samep");
  
}

//This code just counts how many lines there are in the text file used up above.

int countLines(char *filelist)
{ 
    
    int number_of_lines = 0;
    std::string line;
    std::ifstream myfile(filelist);

    while (std::getline(myfile, line))++number_of_lines;
    myfile.seekg (0, ios::beg);
    return number_of_lines;
     
}
