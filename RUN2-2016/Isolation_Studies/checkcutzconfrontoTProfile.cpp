// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)



TProfile ** histoarrayz (const char *dirname, const char *ext, const char * histoarrayname, const char * strip)
{
  std::map<TString,TString> filenamesMap;

  // LIST FILES WITH THE CHOSEN EXTENSION AND ORDER THEM ALPHABETICALLY WITH A MAP
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  n=0;
  if (files)
  {
    TSystemFile *file;
    TString tfname;
    TString tfnamel = "empty";
    TIter next(files);
    while ( (file=(TSystemFile*)next()) )
    {
      tfname = file->GetName();
      // std::cout << tfname << std::endl;
      if (!file->IsDirectory() && tfname.EndsWith(ext))
      {
        tfnamel = tfname;
        tfnamel.ToLower();
        filenamesMap.insert(std::pair<TString, TString>(tfnamel, tfname));
        n++;
      }
    }
  }
  // OPEN LISTED FILES IN THE RIGHT ORDER
  TFile **filearray = new TFile*[n];
  TProfile **histoarray = new TProfile*[n];
  std::map<TString,TString>::iterator it;
  Int_t i = 0;
  std::cout << "Loaded files: " << std::endl;
  for (it = filenamesMap.begin(); it != filenamesMap.end(); ++it)
  {
    Char_t currentfile[500];
    TString strfile = dirname + it->second;
    strcpy(currentfile, strfile);
    filearray[i] = new TFile(currentfile);
    // if (!filearray[i] || filearray[i] -> IsZombie())
    //    return;
    TH1::AddDirectory(kFALSE);
    histoarray[i] = (TProfile*)filearray[i] -> Get(histoarrayname);
    histoarray[i] -> SetDirectory(gROOT);
    TString histoname = it->second;
    histoname.ReplaceAll(strip,"");
    histoname.ReplaceAll(ext,"");
    histoarray[i] -> SetName(histoname);
    filearray[i] -> Close();
    std::cout << i << " - " << currentfile << std::endl;
    //std::cout << currentfile << std::endl;
    i++;
  }
  return histoarray;
}



// -----------------------------------------------------------------------------

void checkcutzconfrontoTProfile()
{

  gROOT -> Reset();

  // -- OPEN ROOTFILES FOR DY and get histograms NOISO AND NONO
  histosfromtfile gethistograms;
  const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/mcTRGOFF/";
  const char *ext = ".root";
  const char *strip = "";
  Int_t n = 9;   // number of histograms in the array

  // CHOOSE WHICH HISTOGRAM
  TString NoIso = "NoIso/";
  TString NoNo = "NoNo/";
  const char *histoarraynameTP = "RelIsoSumPtVsPhiTracker";

  TProfile **histoarrayTP = histoarrayz(dirname, ext, NoIso+histoarraynameTP, strip);
  TProfile **histoarrayNoNoTP = histoarrayz(dirname, ext, NoNo+histoarraynameTP, strip);


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
    histoarrayTP[i] -> Scale(1./Lumi_mcBg[i]);
    histoarrayNoNoTP[i] -> Scale(1./Lumi_mcBg[i]);
  }

  // ADD DY NOISO
  histoarrayTP[0] -> Add(histoarrayTP[1]);
  histoarrayTP[0] -> Add(histoarrayTP[2]);
  histoarrayTP[0] -> Add(histoarrayTP[3]);
  histoarrayTP[0] -> Add(histoarrayTP[4]);
  histoarrayTP[0] -> Add(histoarrayTP[5]);
  histoarrayTP[0] -> Add(histoarrayTP[6]);
  histoarrayTP[0] -> Add(histoarrayTP[7]);
  histoarrayTP[0] -> Add(histoarrayTP[8]);
  // ADD DY NONO
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[1]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[2]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[3]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[4]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[5]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[6]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[7]);
  histoarrayNoNoTP[0] -> Add(histoarrayNoNoTP[8]);

  // APPEARANCE
  histoarrayTP[0] -> SetLineColor(kViolet+7);
  histoarrayTP[0] -> SetFillColor(33);
  histoarrayTP[0] -> SetMarkerStyle(8);
  histoarrayTP[0] -> SetMarkerColor(kViolet+7);
  histoarrayTP[0] -> SetMarkerSize(0.5);
  histoarrayNoNoTP[0] -> SetLineColor(kRed);
  histoarrayNoNoTP[0] -> SetFillColor(33);
  histoarrayNoNoTP[0] -> SetMarkerStyle(8);
  histoarrayNoNoTP[0] -> SetMarkerColor(kRed);
  histoarrayNoNoTP[0] -> SetMarkerSize(0.5);

  // TITLES
  TString plotTitle = histoarraynameTP;
  histoarrayTP[0]->SetName("mergedDY");
  histoarrayNoNoTP[0]->SetName("mergedDY");

  // -- DRAW HISTOGRAMS
  TCanvas* c1 = new TCanvas();
  histoarrayTP[0] -> GetXaxis() -> SetTitleOffset(1.15);
  histoarrayTP[0] -> GetYaxis() -> SetTitleOffset(1.15);
  histoarrayTP[0] -> GetYaxis() -> SetTitle("Events");
  histoarrayTP[0] -> GetXaxis() -> SetTitle(histoarrayTP[0]->GetTitle());

  // Ranges
  if (plotTitle.Contains("Eta"))
  {
    histoarrayTP[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    // histoarray[0]->GetYaxis()->SetRangeUser(1e-4,3);
    // c1-> SetLogy();
    histoarrayTP[0] -> DrawNormalized("same");
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  else if (plotTitle.Contains("Tracks"))
  {
    // histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    // histoarray[0]->GetYaxis()->SetRangeUser(1e-4,1e1);
    // c1-> SetLogy();
    histoarrayTP[0] -> DrawNormalized("same");
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  else if (plotTitle.Contains("Vertices"))
  {
    // histoarray[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    // histoarray[0]->GetYaxis()->SetRangeUser(1e-9,3e2);
    // c1-> SetLogy();
    histoarrayTP[0] -> DrawNormalized("same");
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  else if (plotTitle.Contains("Phi"))
  {
    histoarrayTP[0] -> GetXaxis() -> SetTitleOffset(1);
    histoarrayTP[0] -> Rebin(2);
    histoarrayNoNoTP[0] -> Rebin(2);
    histoarrayTP[0]->GetXaxis()->SetRangeUser(-2.5,2.5);
    // gStyle->SetStripDecimals(kTRUE);
    // histoarray[0]->GetYaxis()->SetRangeUser(1e-3,1);
    // c1-> SetLogy();
    histoarrayTP[0] -> DrawNormalized("same");
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  else if (plotTitle.Contains("Pt"))
  {
    histoarrayTP[0] -> Rebin(10);
    histoarrayNoNoTP[0] -> Rebin(10);
    histoarrayTP[0]->GetXaxis()->SetRangeUser(0.,1500.);
    // gStyle->SetStripDecimals(kFALSE);
    // histoarray[0]->GetYaxis()->SetRangeUser(1e-10,1e1);
    c1-> SetLogy();
    histoarrayTP[0] -> DrawNormalized();
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  else if (plotTitle.Contains("Jets"))
  {
    histoarrayTP[0]->GetXaxis()->SetRangeUser(0,3);
    histoarrayTP[0]->GetYaxis()->SetRangeUser(0.,0.1903);
    // c1-> SetLogy();
    // histoarray[0] -> Draw("axis");
    histoarrayTP[0] -> DrawNormalized("same");
    histoarrayNoNoTP[0] -> DrawNormalized("same");
  }
  std::cout << "\nSOMMA HISTO: " << histoarrayTP[0]->GetEntries();
  std::cout << "\nSOMMA HISTONONO: " << histoarrayNoNoTP[0]->GetEntries();
  
  
  gStyle -> SetOptStat(0);
  gStyle->SetStatX(0.885);		//Stat box x position (top right hand corner)
  gStyle->SetStatY(0.875); 		//Stat box y position
  gStyle->SetLineColor(797);		//Stat box fill style!
  gStyle->SetStatBorderSize(0);		//Stat box border thickness
  gStyle->SetStatColor(18);
  gPad->Update();

  // Legend
  TString zero = "";
  TString entries = " entries)";
  TLegend *leg = new TLegend(0.72,0.76,0.88,0.87);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(14);
  leg -> SetBorderSize(0);
  leg -> SetFillColorAlpha(18, 1);
  leg -> AddEntry(histoarrayTP[0], "DY NoIso",  "l");
  leg -> AddEntry(histoarrayNoNoTP[0], "DY Isolated",  "l");
  leg -> Draw();


}
