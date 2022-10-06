void t_graph()
{
  
  TGraph* gr = new TGraph("6446data.txt");
  TGraph* gr2 = new TGraph("6454data.txt");
  TGraph* gr3 = new TGraph("6558data.txt");
  TGraph* gr4 = new TGraph("6591data.txt");
  TGraph* gr5 = new TGraph("6549data.txt");
  TGraph* gr6 = new TGraph("6598data.txt");
  TGraph* gr7 = new TGraph("6584data.txt");
  TGraph* gr8 = new TGraph("6585data.txt");
  
  gr -> SetMarkerStyle(4);
  gr2 -> SetMarkerStyle(4);
  gr3 -> SetMarkerStyle(4);
  gr4 -> SetMarkerStyle(4);
  gr5 -> SetMarkerStyle(8);
  gr6 -> SetMarkerStyle(8);
  gr7 -> SetMarkerStyle(8);
  gr8 -> SetMarkerStyle(8);
  
  gr -> SetMarkerColor(1);
  gr2 -> SetMarkerColor(2);
  gr3 -> SetMarkerColor(3);
  gr4 -> SetMarkerColor(4);
  gr5 -> SetMarkerColor(1);
  gr6 -> SetMarkerColor(2);
  gr7 -> SetMarkerColor(3);
  gr8 -> SetMarkerColor(4);
  
  TLegend* leg = new TLegend();
  leg -> SetTextSize(.05);
  
  leg -> AddEntry(gr,"Z12.645.6446");
  leg -> AddEntry(gr2,"Z12.646.6454");
  leg -> AddEntry(gr3,"Z12.656.6558");
  leg -> AddEntry(gr4,"Z12.660.6591");
  leg -> AddEntry(gr5,"Z12.655.6549");
  leg -> AddEntry(gr6,"Z12.660.6598");
  leg -> AddEntry(gr7,"Z12.659.6584");
  leg -> AddEntry(gr8,"Z12.659.6585");
  
  gr->Draw("ap");
  leg -> Draw("samep");
  gr2->Draw("samep");
  gr3->Draw("samep");
  gr4->Draw("samep");
  gr5->Draw("samep");
  gr6->Draw("samep");
  gr7->Draw("samep");
  gr8->Draw("samep");

  gr -> SetTitle(";Week Number;PR");

  gr -> GetYaxis() -> SetRangeUser(0,1.3);

  leg -> SetBorderSize(0);
}
