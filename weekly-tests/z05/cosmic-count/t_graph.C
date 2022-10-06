void t_graph()
{
  
  TGraph* gr = new TGraph("2818data.txt");
  TGraph* gr2 = new TGraph("2810data.txt");
  TGraph* gr3 = new TGraph("2819data.txt");
  TGraph* gr4 = new TGraph("2803data.txt");
  TGraph* gr5 = new TGraph("2794data.txt");
  TGraph* gr6 = new TGraph("3001data.txt");
  TGraph* gr7 = new TGraph("3002data.txt");
  TGraph* gr8 = new TGraph("3003data.txt");
  
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
  
  leg -> AddEntry(gr,"Z05.282.2818");
  leg -> AddEntry(gr2,"Z05.281.2810");
  leg -> AddEntry(gr3,"Z05.282.2819");
  leg -> AddEntry(gr4,"Z05.281.2803");
  leg -> AddEntry(gr5,"Z05.280.2794");
  leg -> AddEntry(gr6,"Z05.301.3001 ");
  leg -> AddEntry(gr7,"Z05.301.3002");
  leg -> AddEntry(gr8,"Z05.301.3003");
  
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
