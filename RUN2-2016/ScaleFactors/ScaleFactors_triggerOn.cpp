// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)


void ScaleFactors_triggerOn()
{
  /**
       Calculates Scale Factors SF and draws different
       histograms ("Reconstructed events / bin", "Scale Factors", "Fractions")
       to show their relevance
  */


  gROOT -> Reset();


  // -- OPEN DATA ROOTFILE AND TAKE "DileptonMass" HISTOGRAM

  const char *filepath = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2/data/Run2016/ana_datamc_data.root";
  const char *histoname = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass";
  histosfromtfile gethistograms;
  TH1F *data1 = gethistograms.histo(filepath, histoname);


  // -- OPEN MC ROOTFILES AND GET "DileptonMass" HISTOGRAMS

  const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2/mc_triggerOn/";
  const char *ext = ".root";
  const char *histoarrayname_emu = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass";
  const char *histoarrayname_mumu = "Our2012MuonsPlusMuonsMinusHistos/DileptonMass";
  const char *strip = "ana_datamc_";
  TH1F **histoarrayemu = gethistograms.histoarray(dirname, ext, histoarrayname_emu, strip);
  TH1F **histoarraymumu = gethistograms.histoarray(dirname, ext, histoarrayname_mumu, strip);
  Int_t n = gethistograms.getn();   // number of histograms in the array


  // -- NORMALIZE HISTOGRAMS

  // w_norm = Lumi_data / Lumi_mc = Lumi_data / (N_mc / xsection_mc)
  Float_t Lumi_data = 2045.732;                  // Luminosity value (*json?)
  Float_t Lumi_error_r = 0.05;                   // Luminosity relative uncertainty
  Float_t Lumi_error = Lumi_data*Lumi_error_r;   // Luminosity uncertainty

  // Retrieve mc luminosities
  lumitools lumitools;
  const char *lumifile = "lumi_mc_file.dat";
  Float_t * Lumi_mc_inv = lumitools.lumivalues(lumifile);   // (Lumi_mc)^-1

  // Create Luminosity histogram, needed to keep track of lumi uncertainty
  TH1F * Luminosity = lumitools.lumihisto(histoarrayemu[0], Lumi_data, Lumi_error);

  // Scaling mc histograms
  TH1F **UNNORMhistoarrayemu = new TH1F*[n];
  TH1F **UNNORMhistoarraymumu = new TH1F*[n];
  for (Int_t i=0; i<n; i++)
  {
    // Keeping the un-normalized histograms for check purposes
    UNNORMhistoarraymumu[i] = (TH1F*)histoarraymumu[i]->Clone();
    UNNORMhistoarrayemu[i] = (TH1F*)histoarrayemu[i]->Clone();
    // Using Luminosity uncertainty
    histoarrayemu[i] -> Scale(Lumi_mc_inv[i]);
    histoarrayemu[i] -> Multiply(Luminosity);
    histoarraymumu[i] -> Scale(Lumi_mc_inv[i]);
    histoarraymumu[i] -> Multiply(Luminosity);
  }


  // RETITLE

  // Excluding DY histogram
  // 0 = ttbar
  // 1 = tWt
  // 2 = tWtbar
  // 3 = Wjets
  // 4 = WWincl
  // 5 = WZ
  // 6 = ZZ
  for (Int_t i=0; i<n; i++)
  {
    histoarraymumu[i] = histoarraymumu[i+1];
    histoarrayemu[i] = histoarrayemu[i+1];
    UNNORMhistoarrayemu[i] = UNNORMhistoarrayemu[i+1];
    UNNORMhistoarraymumu[i] = UNNORMhistoarraymumu[i+1];
  }
  const Int_t nh = 7;     // number of histograms in the array to be used


  // REBIN HISTOGRAMS

  // Rebin to newBins, and then truncate the last bin to a value lastx, keeping
  // the original size bin content in the truncated last bin
  // This is necessairy to visualize, for example, the error bars
  // when drawing in truncated x axis
  const Int_t numbins = 3;
  Double_t newBins[] = {121., 201., 401., 2000.};      // new bin set
  Double_t lastx = 600.;   // last x axis value to show when drawing
  Double_t newBinsTrunc[] = {121., 201., 401., lastx}; // truncated new bin set

  TH1F * data1_rebinned = tools::Rebintill(data1, numbins, newBins, lastx);
  TH1F **histoarraymumu_rebinned = tools::Rebintill(histoarraymumu, nh, numbins, newBins, lastx);
  TH1F **histoarrayemu_rebinned = tools::Rebintill(histoarrayemu, nh, numbins, newBins, lastx);
  TH1F **UNNORMhistoarraymumu_rebinned = tools::Rebintill(UNNORMhistoarraymumu, nh, numbins, newBins, lastx);
  TH1F **UNNORMhistoarrayemu_rebinned = tools::Rebintill(UNNORMhistoarrayemu, nh, numbins, newBins, lastx);


  // -- ADD OVERFLOW

  tools::AddOverFlow(data1_rebinned);
  for (Int_t i=0; i<nh; i++)
  {
    tools::AddOverFlow(histoarraymumu_rebinned[i]);
    tools::AddOverFlow(histoarrayemu_rebinned[i]);
  }


  // SCALE FACTORS FROM SINGLE CONTRIBUTIONS
  // SF_MC = N_MC -> MU+MU- / N_MC -> EMU

  TH1F **SF = new TH1F*[nh];
  for (Int_t j=0; j<nh; j++)
  {
    SF[j] = new TH1F("SF", "SF", numbins, newBinsTrunc);
    // Without considering Lumi_mc_inv in error propagation
    SF[j] -> Divide(UNNORMhistoarraymumu_rebinned[j], UNNORMhistoarrayemu_rebinned[j]);
  }


  // FRACTIONS

  // Integral of all mc backgounds
  TH1F *mcsum = new TH1F("mcsum", "mcsum", numbins, newBinsTrunc);
  for (Int_t j=0; j<nh; j++)
  {
    mcsum -> Add(histoarrayemu_rebinned[j]);
    cout << endl << j << ": " << histoarrayemu_rebinned[j]-> GetBinContent(1) << endl;
  }

  // Test alt method
  float EMu_integralW[7];
  float emumc=0;
  float scale[7];
  for (Int_t i=0; i<7; i++)
  {
    scale[i] = Lumi_mc_inv[i+1]*Lumi_data;
    EMu_integralW[i] = scale[i]* (UNNORMhistoarrayemu_rebinned[i]->Integral(1,1));
    emumc = emumc+EMu_integralW[i];
  }
  cout << "\nemumc bin 1: " << emumc << endl;


  // Fraction for every mc background
  TH1F **fraction = new TH1F*[nh];
  for (Int_t j=0; j<nh; j++)
  {
    fraction[j] = new TH1F("fraction", "fraction", numbins, newBinsTrunc);
    fraction[j] -> Divide(histoarrayemu_rebinned[j], mcsum);
  }

  // N (MU+MU-) DATA PREDICTION = N (EMU) OBSERVED * SF ONLY FROM TTBAR
  TH1F *N_mumu_data = new TH1F("N_mumu_data", "N_mumu_data", numbins, newBinsTrunc);
  N_mumu_data -> Multiply(data1_rebinned, SF[0]);

  // N (MU+MU-) SIM. PREDICTION = N (EMU) MC * SF ONLY FROM TTBAR
  TH1F *N_mumu_sim = new TH1F("N_mumu_sim", "N_mumu_sim", numbins, newBinsTrunc);
  N_mumu_sim -> Multiply(mcsum, SF[0]);


  // WEIGHTED SCALE FACTORS
  TH1F **SF_weighted = new TH1F*[nh];
  for (Int_t j=0; j<nh; j++)
  {
    SF_weighted[j] = new TH1F("SF_weighted", "SF_weighted", numbins, newBinsTrunc);
    SF_weighted[j] -> Multiply(SF[j], fraction[j]);
  }

  // TOTAL SCALE FACTOR FROM ALL CONTRIBUTIONS
  // SFtot = SUM(j) { SF(j) * Fraction(j) }
  TH1F *SFtot = new TH1F("SFtot", "SFtot", numbins, newBinsTrunc);
  for (j=0; j<nh; j++)
  {
    SFtot -> Add(SF_weighted[j]);
  }


  // N (MU+MU-) SIM. PREDICTION FROM ALL CONTRIBUTIONS = N (EMU) MC * SFtot
  TH1F *N_mumu_simtot = new TH1F("N_mumu_simtot", "N_mumu_simtot", numbins, newBinsTrunc);
  N_mumu_simtot -> Multiply(mcsum, SFtot);

  // N (MU+MU-) DATA PREDICTION FROM ALL CONTRIBUTIONS = N (EMU) OBSERVED * SFtot
  TH1F *N_mumu_datatot = new TH1F("N_mumu_datatot", "N_mumu_datatot", numbins, newBinsTrunc);
  N_mumu_datatot -> Multiply(data1_rebinned, SFtot);


  // APPEARANCE

  // TStyle
  styles mystyles;
  TStyle *st1 = mystyles.AlicesStyle2();
  gROOT -> SetStyle("st2");
  gROOT -> ForceStyle();
  //gStyle -> SetPaintTextFormat("5.2f");


  // -- DRAW TTBAR EVENTS

  histoarrayemu_rebinned[0] -> SetLineColor(kRed);
  histoarraymumu_rebinned[0] -> SetLineColor(kTeal-1);
  TCanvas *c1 = new TCanvas("TtbarEvents", "TtbarEvents", 0, 0, 1000, 800);
  c1 -> SetLogy();
  c1 -> SetLeftMargin(0.13);
  c1 -> SetBottomMargin(0.16);
  TH2D* rangesitter = new TH2D("rangesitter", "rangesetter", 100, 121, 600, 100, 50, 1e4);
  rangesitter -> GetYaxis() -> SetTitle("Reconstructed events / bin");
  rangesitter -> GetXaxis() -> SetTitle("Dilepton mass (GeV)");
  rangesitter -> Draw();
  histoarrayemu_rebinned[0] -> Draw("Same");
  histoarraymumu_rebinned[0] -> Draw("Same");

  // Legend
  TLegend *leg = new TLegend(0.76,0.73,0.87,0.86);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(20);
  leg -> SetBorderSize(0);
  leg -> SetMargin(0.5);
  leg -> SetFillColor(kGray);
  leg -> SetFillColorAlpha(1, 0.05);
  leg -> AddEntry(histoarrayemu_rebinned[0], " e^{#pm}#mu^{#mp}",  "l");
  leg -> AddEntry(histoarraymumu_rebinned[0], " #mu^{+}#mu^{-}",  "l");
  leg -> Draw();

  // Title
  TLatex *texf = new TLatex(0.45,0.94,"TTbar events");
  texf -> SetTextFont(43);
  texf -> SetTextSize(30);
  texf -> SetNDC(kTRUE);
  texf -> Draw();

  // Update
  c1 -> Update();
  gStyle -> SetLineScalePS(1);


  // -- DRAW SCALE FACTORS ttbar, WW and SF_tot

  SF[4] -> SetLineColor(kMagenta);
  SF[0] -> SetLineColor(kAzure+2);
  SFtot -> SetLineColor(kOrange+2);
  TCanvas *c2 = new TCanvas("ScaleFactors", "ScaleFactors", 0, 0, 1000, 800);
  c2 -> SetLeftMargin(0.13);
  c2 -> SetBottomMargin(0.16);
  TH2D* rangesitter2 = new TH2D("rangesitter", "rangesetter", 100, 121, 600, 100, 0.3, 0.8);
  rangesitter2 -> GetYaxis() -> SetTitle("#mu#mu/e#mu scale factor");
  rangesitter2 -> GetXaxis() -> SetTitle("Dilepton mass (GeV)");
  rangesitter2 -> Draw();
  SF[4] -> Draw("Same");
  SF[0] -> Draw("Same");
 
  // Legend
  TLegend *leg1 = new TLegend(0.19,0.74,0.3,0.87);
  leg1 -> SetTextFont(43);
  leg1 -> SetTextSizePixels(20);
  leg1 -> SetBorderSize(0);
  leg1 -> SetMargin(0.5);
  leg1 -> SetFillColor(kGray);
  leg1 -> SetFillColorAlpha(1, 0.05);
  leg1 -> AddEntry(SF[0], " t#bar{t}",  "l");
  leg1 -> AddEntry(SF[4], " WW",  "l");
  leg1 -> Draw();

  // Title
  TLatex *texf2 = new TLatex(0.45,0.94,"Scale factors");
  texf2 -> SetTextFont(43);
  texf2 -> SetTextSize(30);
  texf2 -> Draw();
  texf2 -> SetNDC(kTRUE);

  // Update
  c2 -> Update();
  gStyle -> SetLineScalePS(1);


  // -- DRAW FRACTIONS

  // Histograms appearance
  Int_t icolour = 0;
  Color_t colours[] = {kMagenta, kRed+1, kRed, kGreen+1, kAzure+2, kOrange-3, kViolet+1};
  for (Int_t j=0; j<nh; j++)
  {
    fraction[j] -> SetLineColor(colours[icolour++]);
  }

  // Canvas
  TCanvas *c3 = new TCanvas("Fractions", "Fractions", 0, 0, 1000, 800);
  c3 -> SetLeftMargin(0.13);
  c3 -> SetBottomMargin(0.16);
  c3 -> SetLogy();
  TH2D* rangesitter3 = new TH2D("rangesitter", "rangesetter", 100, 121, 600, 100, 1e-4, 1.);
  rangesitter3 -> GetYaxis() -> SetTitle("Fraction");
  rangesitter3 -> GetXaxis() -> SetTitle("Dilepton mass (GeV)");
  rangesitter3 -> Draw();
  for (Int_t j=0; j<nh; j++)
  {
    fraction[j] -> Draw("same,hist, TEXT0");
  }

  // Legend
  TLegend *leg2 = new TLegend(0.16,0.19,0.30,0.39);
  leg2 -> SetTextFont(43);
  leg2 -> SetTextSizePixels(16);
  leg2 -> SetBorderSize(0);
  leg2 -> SetMargin(0.5);
  leg2 -> SetFillColor(kGray);
  leg2 -> SetFillColorAlpha(1, 0.05);
  leg2 -> AddEntry(fraction[0], " t#bar{t}",  "l");
  leg2 -> AddEntry(fraction[3], " W + jets",  "l");
  leg2 -> AddEntry(fraction[4], " WW",  "l");
  leg2 -> AddEntry(fraction[1], " tWt",  "l");
  leg2 -> AddEntry(fraction[2], " tW#bar{t}",  "l");
  leg2 -> AddEntry(fraction[5], " WZ",  "l");
  leg2 -> AddEntry(fraction[6], " ZZ",  "l");
  leg2 -> Draw();

  // Title
  TLatex *texf3 = new TLatex(0.45,0.94,"Fractions");
  texf3 -> SetTextFont(43);
  texf3 -> SetTextSize(30);
  texf3 -> Draw();
  texf3 -> SetNDC(kTRUE);

  // Update
  c3 -> Update();
  gStyle -> SetLineScalePS(1);


  // PAINT CALCULATED VALUES

  cout << "N emu data";
  cout << endl << data1_rebinned -> GetBinContent(1) << endl;
  cout << endl << data1_rebinned -> GetBinContent(2) << endl;
  cout << endl << data1_rebinned -> GetBinContent(3) << endl;
  cout << "mcsum";
  cout << endl << mcsum -> GetBinContent(1) << endl;
  cout << endl << mcsum -> GetBinContent(2) << endl;
  cout << endl << mcsum -> GetBinContent(3) << endl;
  cout << "sf ttbar";
  cout << endl << SF[0] -> GetBinContent(1) << endl;
  cout << endl << SF[0] -> GetBinContent(2) << endl;
  cout << endl << SF[0] -> GetBinContent(3) << endl;
  cout << "sf ttbar error";
  cout << endl << SF[0] -> GetBinError(1) << endl;
  cout << endl << SF[0] -> GetBinError(2) << endl;
  cout << endl << SF[0] -> GetBinError(3) << endl;


  // SAVING FILE FOR WRITING LATEX OUTPUT

  TFile *texout = TFile::Open("texoutput.root", "RECREATE");

  // Histograms filled with errors
  TH1F *E_SFtot = new TH1F("E_SFtot", "E_SFtot", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_SFtot -> SetBinContent(i, SFtot -> GetBinError(i));
  TH1F *E_SFttbar = new TH1F("E_SFttbar", "E_SFttbar", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_SFttbar -> SetBinContent(i, SF[0] -> GetBinError(i));
  TH1F *E_N_mumu_data = new TH1F("E_N_mumu_data", "E_N_mumu_data", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_N_mumu_data -> SetBinContent(i, N_mumu_data -> GetBinError(i));
  TH1F *E_N_mumu_sim = new TH1F("E_N_mumu_sim", "E_N_mumu_sim", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_N_mumu_sim -> SetBinContent(i, N_mumu_sim -> GetBinError(i));
  TH1F *E_N_mumu_simtot = new TH1F("E_N_mumu_simtot", "E_N_mumu_simtot", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_N_mumu_simtot -> SetBinContent(i, N_mumu_simtot -> GetBinError(i));
  TH1F *E_N_mumu_datatot = new TH1F("E_N_mumu_datatot", "E_N_mumu_datatot", numbins, newBins);
  for (Int_t i=1; i<=numbins; i++)
    E_N_mumu_datatot -> SetBinContent(i, N_mumu_datatot -> GetBinError(i));

  // Create storage TGraphs
  // data
  TGraph *T_SFtot = new TGraph (SFtot);
  TGraph *T_SFttbar = new TGraph (SF[0]);
  TGraph *T_N_emu_obs = new TGraph (data1_rebinned);
  TGraph *T_N_mumu_data = new TGraph (N_mumu_data);
  TGraph *T_N_mumu_sim = new TGraph (N_mumu_sim);
  TGraph *T_N_mumu_simtot = new TGraph (N_mumu_simtot);
  TGraph *T_N_mumu_datatot = new TGraph (N_mumu_datatot);
  // errors
  TGraph *TE_SFtot = new TGraph (E_SFtot);
  TGraph *TE_SFttbar = new TGraph (E_SFttbar);
  TGraph *TE_N_mumu_data = new TGraph (E_N_mumu_data);
  TGraph *TE_N_mumu_sim = new TGraph (E_N_mumu_sim);
  TGraph *TE_N_mumu_simtot = new TGraph (E_N_mumu_simtot);
  TGraph *TE_N_mumu_datatot = new TGraph (E_N_mumu_datatot);

  // Write data (histograms and TGraphs) on file
  texout -> cd();
  T_SFtot -> Write("SFtot");
  T_SFttbar -> Write("SFttbar");
  T_N_emu_obs -> Write("N_emu_obs");
  T_N_mumu_sim -> Write("N_mumu_sim");
  T_N_mumu_data -> Write("N_mumu_data");
  T_N_mumu_simtot -> Write("N_mumu_simtot");
  T_N_mumu_datatot -> Write("N_mumu_datatot");
  TE_SFtot -> Write("E_SFtot");
  TE_SFttbar -> Write("E_SFttbar");
  TE_N_mumu_data -> Write("E_N_mumu_data");
  TE_N_mumu_sim -> Write("E_N_mumu_sim");
  TE_N_mumu_simtot -> Write("E_N_mumu_simtot");
  TE_N_mumu_datatot -> Write("E_N_mumu_datatot");
  texout -> ls();
  delete texout;
}
