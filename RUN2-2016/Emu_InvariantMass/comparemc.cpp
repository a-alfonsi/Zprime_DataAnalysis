// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)


void comparemc()
{
  /**
       Opens a data rootfile, then a list of mc rootfiles all contained in the
       same directory, and draws (top pad) the histograms:
       "Our2012MuonsPlusMuonsMinusHistos/DileptonMass" from data file
       "EmuVetoMuonsElectronsOppSignHistos/DileptonMass" from every mc file
       and (bottom pad) the ratio DATA/MC
  */


  gROOT -> Reset();


  TH1F **histoarray = new TH1F*[2];

  const char *filepath = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2b/mc/ana_datamc_WWinclusive.root";
  const char *histoname = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass";

  const char * ext = ".root";
  const char * strip = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2b/mc/ana_datamc_";

  histosfromtfile gethistograms;
  histoarray[0] = gethistograms.histo(filepath, histoname);

  TString histogname = filepath;
  histogname.ReplaceAll(strip,"");
  histogname.ReplaceAll(ext,"");
  histoarray[0] -> SetName(histoname);

  const char *filepath2 = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2b/mc_28072016/ana_datamc_WWinclusive.root";
  histoarray[1] = gethistograms.histo(filepath2, histoname);

  Int_t n=2;


  // -- NORMALIZE HISTOGRAMS

  // w_norm = Lumi_data / Lumi_mc = Lumi_data / (N_mc / xsection_mc)
  Float_t Lumi_data = 6312.068;                  // Luminosity value (*json)
  Float_t Lumi_error_r = 0.062;                   // Luminosity relative uncertainty
  Float_t Lumi_error = Lumi_data*Lumi_error_r;   // Luminosity uncertainty

  // Retrieve mc luminosities
  lumitools lumitools;
  Float_t Lumi_mc_inv[2];   // (Lumi_mc)^-1
  Lumi_mc_inv[0] = 87.31/104649474.;
  Lumi_mc_inv[1] = 87.31/104649474.;

  // Create Luminosity histogram, needed to keep track of lumi uncertainty
  TH1F * Luminosity = lumitools.lumihisto(histoarray[0], Lumi_data, Lumi_error);
  cout << Luminosity -> GetBinContent(100);
  // FIXME perché mette una cifra in meno???

  // Scale mc histograms
  for (Int_t i=0; i<n; i++)
  {
    histoarray[i] -> Scale(Lumi_mc_inv[i]);
    histoarray[i] -> Multiply(Luminosity);
  }


  cout << "Integral:\n";
  for (Int_t i=0; i<n; i++)
  {
    histoarray[i] -> Integral();
    cout << endl << histoarray[i] -> Integral();
  }
  cout << "\n int diff" << histoarray[0] -> Integral() - histoarray[1] -> Integral();


  // -- REBIN HISTOGRAMS

  for (Int_t i=0; i<n; i++)
  {
    histoarray[i] -> Rebin(20);
    histoarray[i] -> SetDirectory(gROOT);
  }


  // DIVIDE DATA / MC

  TH1F *datamc_division = new TH1F("datamc_division", "", 1000, 0, 20000);
  datamc_division -> SetDirectory(gROOT);
  datamc_division -> Divide(histoarray[0], histoarray[1]);

  cout << "\nOLD - NEW VALUES: Printing values different from 0:\n";
  for (Int_t i=0; i<datamc_division->GetNbinsX(); i++)
  {
    Float_t diff = histoarray[0]->GetBinContent(i+1) - histoarray[1]->GetBinContent(i+1);
    if (diff != 0.)
    {
      cout << "\nbin " << i+1 << ": " << diff;
    }
  }


  // APPEARANCE

  // TStyle
  styles mystyles;
  TStyle *st1 = mystyles.AlicesStyle1();
  gROOT -> SetStyle("st1");
  gROOT -> ForceStyle();

  // Histograms appearance
  Color_t colours[] = {kGreen+1, kRed, kOrange, kMagenta};
  cout << endl;
  histoarray[0] -> SetLineColor(kBlue);
  histoarray[0] -> SetLineColor(kBlue);
  histoarray[1] -> SetLineColor(kRed);
  histoarray[1] -> SetLineColor(kRed);
  datamc_division -> SetMarkerSize(0.5);
  datamc_division -> SetMarkerStyle(20);
  datamc_division -> SetLineColor(1);



  // -- DRAW HISTOGRAMS

  // To easily set the x, y range for all the histograms I am creating a new
  // empty TH2D for each pad and then printing my histograms on top of it
  TH2D* rangesitter = new TH2D("rangesitter", "rangesetter", 100, 100, 2000, 100, 1e-2, 1e4);
  TH2D* rangesitterb = new TH2D("rangesitterb", "rangesetterb", 100, 100, 2000, 100, 0., 2.);
  rangesitter -> GetXaxis() -> SetTickLength(0);
  rangesitter -> GetXaxis() -> SetLabelOffset(999);
  rangesitter -> GetYaxis() -> SetTitle("Events / 20 GeV");
  rangesitterb -> GetXaxis() -> SetTitle("mass_{e#mu} (GeV)");
  rangesitterb -> GetYaxis() -> SetTitle("Old / New  ");

  // Canvas & pads
  TwoPadsTCanvas *c1 = new TwoPadsTCanvas("MC and data histograms", "MC and data histograms", 0, 0, 1000, 700);
  TPad* pad1 = c1->pad1;
  TPad* pad2 = c1->pad2;
  TPad* erase = c1->erase;
// TString histogname = histoarray[0] -> GetName();
  TString comparison = histogname + " comparison";
  TLatex *texq = new TLatex(0.4,0.97,comparison);
  texq -> SetTextFont(43);
  texq -> SetTextSizePixels(28);
  texq -> Draw();

  // Top pad
  pad1 -> cd();
  rangesitter -> Draw();
  histoarray[1] -> Draw("Same,HIST");
  histoarray[0] -> Draw("Same,hist");
  rangesitter -> Draw("sameaxis");

  // Legend
  TLegend *leg = new TLegend(0.6,0.7,0.95,0.80);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(14);
  leg -> SetBorderSize(0);
  leg -> SetFillColorAlpha(0, 0.);
  leg -> AddEntry(histoarray[0], "Old HEEPid",  "fl");
  leg -> AddEntry(histoarray[1], "New HEEPid",  "fl");
  leg -> Draw();


  // Bottom pad
  pad2 -> cd();
  rangesitterb -> Draw();
  TLine *lined = new TLine(100,1,2000,1);
  lined -> SetLineColor(kGray+1);
  lined -> Draw();
  datamc_division -> Draw("Same");



  // UPDATE

  c1 -> Update();
  gPad -> Update();
  gStyle -> SetLineScalePS(2);

}
