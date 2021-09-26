// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)


void checkcutz()
{

  gROOT -> Reset();


  // -- OPEN FILE FOR MC FOR SIGNAL (RSG KK 3TeV) AND GET "h_tau32" HISTOGRAM

  const char *filepath = "files/mieioutputN/cutChecksRSG3TeV.root";
  const char *histoname = "h_mttbar";
  histosfromtfile gethistograms;
  TH1F *data1 = gethistograms.histo(filepath, histoname);


  // -- OPEN ROOTFILES FOR DY and get histograms
  const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/mcTRGOFF/";
  const char *ext = ".root";
  const char *histoarrayname = "NoIso/IsoNJetsFailedCut";
  const char *strip = "";
  TH1F **histoarray = gethistograms.histoarray(dirname, ext, histoarrayname, strip);
  Int_t n = gethistograms.getn();   // number of histograms in the array


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
  data1 -> Scale(w);
  for (Int_t i=0; i<n; i++)
  {
    histoarray[i] -> Scale(1./Lumi_mcBg[i]);
  }

  histoarray[0] -> Add(histoarray[1]);
  histoarray[0] -> Add(histoarray[2]);
  histoarray[0] -> Add(histoarray[3]);
  histoarray[0] -> Add(histoarray[4]);
  histoarray[0] -> Add(histoarray[5]);
  histoarray[0] -> Add(histoarray[6]);
  histoarray[0] -> Add(histoarray[7]);
  histoarray[0] -> Add(histoarray[8]);


  // APPEARANCE

  histoarray[0] -> SetLineColor(kViolet+9);
  histoarray[0] -> SetFillColor(33);
  histoarray[0] -> SetMarkerStyle(8);
  histoarray[0] -> SetMarkerSize(0.5);
  TString plotTitle = histoarrayname;
  plotTitle.ReplaceAll("NoIso/","");
  plotTitle.ReplaceAll("Tracker","");
  plotTitle.ReplaceAll("Lepton","");
  plotTitle.ReplaceAll("Pt"," pT ");
  plotTitle.ReplaceAll("Eta"," #eta ");
  plotTitle.ReplaceAll("Phi"," #phi ");
  plotTitle.ReplaceAll("NVertices"," #Vertices/Event ");
  plotTitle.ReplaceAll("IsoNJets"," Iso. (#Delta R < 0.3) #Jets ");
  plotTitle.ReplaceAll("IsoNTracks"," Iso. (#Delta R < 0.3) #Tracks ");
  histoarray[0] -> SetTitle(plotTitle);
  histoarray[0]->SetName("mergedDY");


  // -- DRAW HISTOGRAMS

  TCanvas* c1 = new TCanvas();
  histoarray[0]->Draw();

  // Ranges
  if (plotTitle.Contains("eta"))
  {
    histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    histoarray[0]->GetYaxis()->SetRangeUser(0.,0.01);
    histoarray[0] -> Draw("same");
  }
  
  if (plotTitle.Contains("phi"))
  {
    histoarray[0] -> Rebin(2);
    histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    histoarray[0]->GetYaxis()->SetRangeUser(0.,0.01);
    histoarray[0] -> Draw("same");
  }
  
  if (plotTitle.Contains("pT"))
  {
    histoarray[0] -> Rebin(10);
    histoarray[0]->GetXaxis()->SetRangeUser(0.,1500.);
    gStyle->SetStripDecimals(kFALSE);
    c1-> SetLogy();
    histoarray[0] -> Draw("same");
  }
  
  if (plotTitle.Contains("Jets"))
  {
    histoarray[0]->GetXaxis()->SetRangeUser(0,3);
    histoarray[0] -> Draw("same");
  }
  
  gPad->Update();
  TPaveStats *s = (TPaveStats*) gPad->GetPrimitive("stats");
  s->SetTextColor(1);
  s->SetFillColor(18);
  s->SetBorderSize(0);
  s -> Draw();
  gPad->Update();


}
