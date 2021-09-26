// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)


void checkcutzconfrontoStack()
{

  gROOT -> Reset();


  // -- OPEN FILE FOR MC FOR SIGNAL (RSG KK 3TeV) AND GET "h_tau32" HISTOGRAM

  const char *filepath = "files/mieioutputN/cutChecksRSG3TeV.root";
  const char *histoname = "h_mttbar";
  histosfromtfile gethistograms;
  TH1F *data1 = gethistograms.histo(filepath, histoname);


  // -- OPEN ROOTFILES FOR DY and get histograms NOISO
  const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/mcTRGOFF/";
  const char *ext = ".root";
  const char *histoarrayname = "NoIso/LeptonEtaTrackerFailedCut";
  const char *strip = "";
  TH1F **histoarray = gethistograms.histoarray(dirname, ext, histoarrayname, strip);
  Int_t n = gethistograms.getn();   // number of histograms in the array


  // -- OPEN ROOTFILES FOR DY and get histograms NONO
  const char *histoarraynameNoNo = "NoNo/LeptonEtaTracker";
  TH1F **histoarrayNoNo = gethistograms.histoarray(dirname, ext, histoarraynameNoNo, strip);


  // -- NORMALIZE HISTOGRAMS

  // w_norm = Lumi_data / Lumi_mc = Lumi_data / (N_mc / xsection_mc)
  Float_t Lumi_data = 1.;                  // Luminosity value

  // Retrieve mc luminosities
  lumitools lumitools;
  Float_t Lumi_mcSignal = 99755./12.99;
  Float_t Lumi_mcBg[n]; // Lumi
  Float_t w_norm[n];
  Lumi_mcBg[0] = 99200. / (19.32*1.006);    // 120to200
  Lumi_mcBg[1] = 100000. / (0.00139*1.006)    ; // 1400to2300
  Lumi_mcBg[2] = 100000. / (2.731*1.006)  ;   // 200to400
  Lumi_mcBg[3] = 99200. / (0.00008948*1.006)  ;   // 2300to3500
  Lumi_mcBg[4] = 100000. / (0.0000041*1.006)   ;   // 3500to4500
  Lumi_mcBg[5] = 100000. / (0.241*1.006)   ;  // 400to800
  Lumi_mcBg[6] = 100000. / (4.56E-7*1.006)   ;  // 4500to6000
  Lumi_mcBg[7] = 2967200. / (1975*1.006)  ; // 50to120
  Lumi_mcBg[8] = 100000. / (0.01678*1.006)   ;  // 800to1400

  // Scale mc histograms
  Float_t w = Lumi_data/Lumi_mcSignal;
  for (Int_t i=0; i<n; i++)
  {
    histoarray[i] -> Scale(1./Lumi_mcBg[i]);
    histoarrayNoNo[i] -> Scale(1./Lumi_mcBg[i]);
  }

  // Histograms appearance
  histoarray[0] -> SetLineColor(kAzure);
  histoarray[0] -> SetFillColor(33);
  histoarray[0] -> SetMarkerStyle(8);
  histoarray[0] -> SetMarkerColor(kAzure);
  histoarray[0] -> SetMarkerSize(0.5);

  histoarrayNoNo[0] -> SetLineColor(kRed);
  histoarrayNoNo[0] -> SetFillColor(33);
  histoarrayNoNo[0] -> SetMarkerStyle(8);
  histoarrayNoNo[0] -> SetMarkerColor(kRed);
  histoarrayNoNo[0] -> SetMarkerSize(0.5);


  // -- SUM BG HISTOGRAMS
  THStack* hs = new THStack();
  THStack hsNoNo("hs","test stacked histograms");
  for (Int_t i=0; i<9; i++)
  {
    hs->Add(histoarray[i]);
    hsNoNo.Add(histoarrayNoNo[i]);
  }


  // ADD DY NONO
  histoarrayNoNo[0] -> Add(histoarrayNoNo[1]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[2]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[3]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[4]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[5]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[6]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[7]);
  histoarrayNoNo[0] -> Add(histoarrayNoNo[8]);


  // APPEARANCE
  TString plotTitle = histoarrayname;
  plotTitle.ReplaceAll("FailedCut","");
  plotTitle.ReplaceAll("NoIso/","");
  plotTitle.ReplaceAll("Tracker","");
  plotTitle.ReplaceAll("Lepton","");
  plotTitle.ReplaceAll("Pt"," p_{T} ");
  plotTitle.ReplaceAll("Eta"," #eta ");
  plotTitle.ReplaceAll("Phi"," #phi ");
  plotTitle.ReplaceAll("NVertices"," #Vertices ");
  plotTitle.ReplaceAll("IsoNJets"," Iso. (#Delta R < 0.3) #Jets ");
  plotTitle.ReplaceAll("IsoNTracks"," Iso. (#Delta R < 0.3) #Tracks ");

  // TLatex *title = new TLatex(-2.9,0.118,plotTitle);
  histoarray[0] -> SetTitle(plotTitle);
  histoarray[0]->SetName("mergedDY");
  histoarrayNoNo[0] -> SetTitle(plotTitle);
  histoarrayNoNo[0]->SetName("mergedDY");


  // -- DRAW HISTOGRAMS

  TCanvas* c1 = new TCanvas();
  // c1->cd();
  TH2D* rangesitter = new TH2D("rangesitter", "rangesetter", 100, 100, 2000, 100, 1e-2, 1e4);
  histoarray[0] -> GetXaxis() -> SetTitleOffset(1.1);
  histoarray[0] -> GetYaxis() -> SetTitleOffset(1.15);
  histoarray[0] -> GetYaxis() -> SetTitle("Events");
  histoarray[0] -> GetXaxis() -> SetTitle(plotTitle);


  // Ranges
  if (plotTitle.Contains("eta"))
  {
    histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    hs->GetStack()->Last()->Draw("same");
  }
  
  if (plotTitle.Contains("Tracks"))
  {
    histoarray[0] -> DrawNormalized("same");
    histoarrayNoNo[0] -> DrawNormalized("same");
  }
  
  if (plotTitle.Contains("Vertices"))
  {
    histoarray[0] -> DrawNormalized("same");
    histoarrayNoNo[0] -> DrawNormalized("same");
  }
  
  if (plotTitle.Contains("phi"))
  {
    histoarray[0] -> GetXaxis() -> SetTitleOffset(1);
    histoarray[0] -> Rebin(2);
    histoarrayNoNo[0] -> Rebin(2);
    histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    histoarray[0] -> DrawNormalized("same");
    histoarrayNoNo[0] -> DrawNormalized("same");
  }
  
  if (plotTitle.Contains("p_{T}"))
  {
    histoarray[0] -> Rebin(10);
    histoarrayNoNo[0] -> Rebin(10);
    histoarray[0]->GetXaxis()->SetRangeUser(0.,1500.);
    c1-> SetLogy();
    histoarray[0] -> DrawNormalized();
    histoarrayNoNo[0] -> DrawNormalized("same");
  }
  
  if (plotTitle.Contains("Jets"))
  {
    cout << "ok";
    histoarray[0]->GetXaxis()->SetRangeUser(0,3);
    histoarray[0]->GetYaxis()->SetRangeUser(0.,0.1903);
    histoarray[0] -> Draw("axis");
    hs->GetStack()->Last()->Draw();
    hs->Draw("same");
  }
  std::cout << "\nSOMMA HISTO: " << histoarray[0]->GetEntries();
  std::cout << "\nSOMMA HISTONONO: " << histoarrayNoNo[0]->GetEntries();
  
  gStyle -> SetOptStat(0);
  gPad->Update();
  
  // Legend
  TString zero = "";
  TString entries = " entries)";
  TLegend *leg = new TLegend(0.72,0.76,0.88,0.87);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(14);
  leg -> SetBorderSize(0);
  leg -> SetFillColorAlpha(18, 1);
  leg -> AddEntry(histoarray[0], "DY IsoFailed",  "l");
  leg -> AddEntry(histoarrayNoNo[0], "DY Isolated",  "l");
  leg -> Draw();


}
