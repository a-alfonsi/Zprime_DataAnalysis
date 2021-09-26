// NOTE: Required ROOT version 6.04
R__LOAD_LIBRARY(../Libraries/Build/libAlicesTools.dylib)


void emu_invMassdymass28072016()
{
  /**
       Opens a data rootfile, then a list of mc rootfiles all contained in the
       same directory, and draws (top pad) the histograms:
       "Our2012MuonsPlusMuonsMinusHistos/DileptonMass" from data file
       "EmuVetoMuonsElectronsOppSignHistos/DileptonMass" from every mc file
       and (bottom pad) the ratio DATA/MC
  */

  gROOT -> Reset();


  // -- OPEN DATA ROOTFILE AND TAKE "DileptonMass" HISTOGRAM

  const char *filepath = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2b/data_28072016/ana_datamc_data.root";
  const char *histoname = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass";
  histosfromtfile gethistograms;
  TH1F *data1 = gethistograms.histo(filepath, histoname);


  // -- OPEN MC ROOTFILES AND GET "DileptonMass" HISTOGRAMS

  const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/datamc_run2b/mc_28072016/";
  const char *ext = ".root";
  const char *histoarrayname = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass";
  const char *strip = "ana_datamc_";
  TH1F **histoarray = gethistograms.histoarray(dirname, ext, histoarrayname, strip);
  Int_t n = gethistograms.getn();   // number of histograms in the array


  // -- ADD OVERFLOW

  tools::AddOverFlow(data1);
  for (Int_t i=0; i<n; i++)
  {
    tools::AddOverFlow(histoarray[i]);
  }


  // -- NORMALIZE HISTOGRAMS

  // w_norm = Lumi_data / Lumi_mc = Lumi_data / (N_mc / xsection_mc)
  Float_t Lumi_data = 11985.917;                  // Luminosity value (*json?)
  Float_t Lumi_error_r = 0.062;                   // Luminosity relative uncertainty
  Float_t Lumi_error = Lumi_data*Lumi_error_r;   // Luminosity uncertainty

  // Retrieve mc luminosities
  lumitools lumitools;
  const char *lumifile = "lumi_mc_file.dat";
  Float_t * Lumi_mc_inv = lumitools.lumivalues(lumifile);   // (Lumi_mc)^-1

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

  histoarray[2] -> Draw();


  // -- COMBINE HISTOGRAMS

  // Merge tWt and tWtbar
  TList *tWt_list = new TList;
  for (Int_t i=0; i<n; i++)
  {
    TString histoname = histoarray[i]->GetName();
    if (histoname.Contains("tW") || histoname.Contains("Wantitop"))
    {
      tWt_list -> Add(histoarray[i]);
      cout << "\nAdded to tWt_list: ";
      cout << histoname;
    }
  }
  TH1F *tWt_merge = new TH1F;
  tWt_merge -> SetDirectory(gROOT);
  tWt_merge -> Merge(tWt_list);
  tWt_merge -> SetName("mergedtW");

  // Merge Dibosons
  TList *Di_list = new TList;
  for (Int_t i=0; i<n; i++)
  {
    TString histoname = histoarray[i]->GetName();
    if (histoname.Contains("WW") || histoname.Contains("ZZ") || histoname.Contains("WZ"))
    {
      Di_list -> Add(histoarray[i]);
      cout << "\nAdded to Di_list: ";
      cout << histoname;
    }
  }
  TH1F *Di_merge = new TH1F;
  Di_merge -> SetDirectory(gROOT);
  Di_merge -> Merge(Di_list);
  Di_merge -> SetName("mergedDi");

  // Merge QCD
  TList *QCD_list = new TList;
  for (Int_t i=0; i<n; i++)
  {
    TString histoname = histoarray[i]->GetName();
    if (histoname.Contains("qcd"))
    {
      QCD_list -> Add(histoarray[i]);
      cout << "\nAdded to QCD_list: ";
      cout << histoname;
    }
  }
  TH1F *QCD_merge = new TH1F;
  QCD_merge -> SetDirectory(gROOT);
  QCD_merge -> Merge(QCD_list);
  QCD_merge -> SetName("mergedQCD");


  // -- CREATE NEW HISTOGRAMS' ARRAY (this will speed up things later on)

  std::vector<TString> histotypes = {"ttbar", "Wjets", "tWt_merge", "Di_merge", "dy"};
  const Int_t nh = histotypes.size();     // number of histograms in the array to be used

  TH1F **final_histoarray = new TH1F*[nh];

  for (Int_t c=0; c<histotypes.size(); c++)
  {
    if ( histotypes[c] == "tWt_merge")
    {
      final_histoarray[c] = tWt_merge;
      cout << "\nfinal_histoarray[" << c << "] is: tWt_merge";
    }
    else if ( histotypes[c] == "Di_merge")
    {
      final_histoarray[c] = Di_merge;
      cout << "\nfinal_histoarray[" << c << "] is: Di_merge";
    }
    else
    {
      for (Int_t i=0; i<n; i++)
      {
        TString histoname = histoarray[i] -> GetName();
        if ( histoname.Contains(histotypes[c]) )
        {
          final_histoarray[c] = histoarray[i];
          cout << "\nfinal_histoarray[" << c << "] is: " << histoname;
        }
      }
    }
  }


  // -- MERGE QCD + WJETS
  TList *QCDjets = new TList;
  for (Int_t i=0; i<n; i++)
  {
    TString histoname = histoarray[i]->GetName();
    if (histoname.Contains("Wjets"))
    {
      QCDjets -> Add(histoarray[i]);
      cout << "\nAdded to QCDjets: ";
      cout << histoname;
    }
  }
  QCDjets -> Add(QCD_merge);
  TH1F *jets_merge = new TH1F;
  jets_merge -> SetDirectory(gROOT);
  jets_merge -> Merge(QCDjets);
  jets_merge -> SetName("mergedjets");
  final_histoarray[1] = jets_merge;


  // -- REBIN HISTOGRAMS

  data1 -> Rebin(20);
  for (Int_t i=0; i<nh; i++)
  {
    final_histoarray[i] -> Rebin(20);
    final_histoarray[i] -> SetDirectory(gROOT);
  }


  // -- STACK MC HISTOGRAMS

  // Stack histograms, sorting them by their integral so that everyone is visible
  stacktools stacktools;
  stacktools.set_nh(nh);
  THStack *hs = stacktools.orderedstack(final_histoarray);
  // Vector containing the sorted indexes
  std::vector<Int_t> w_index = stacktools.getw_index();
  // for (Int_t i=0; i<w_index.size(); i++ )
  //   std::cout << std::endl << w_index[i] << std::endl;


  // DIVIDE DATA / MC

  TH1F *datamc_division = new TH1F("datamc_division", "", 1000, 0, 20000);
  datamc_division -> SetDirectory(gROOT);
  TH1F *stacksum = (TH1F*)hs -> GetStack() -> Last();
  stacksum -> SetDirectory(gROOT);
  datamc_division -> Divide(data1, stacksum);


  // APPEARANCE

  // TStyle
  styles mystyles;
  TStyle *st1 = mystyles.AlicesStyle1();
  gROOT -> SetStyle("st1");
  gROOT -> ForceStyle();

  // Histograms appearance
  Color_t colours[] = {kGreen+1, kAzure+10, kRed, kOrange, kMagenta};
  // Color_t colours[] = {kGreen+1, kRed, kOrange, kMagenta};
  stacktools.setcolour(colours, hs, final_histoarray);
  cout << endl;
  data1 -> SetLineColor(1);
  data1 -> SetMarkerStyle(20);
  data1 -> SetMarkerSize(0.5);
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
  rangesitterb -> GetYaxis() -> SetTitle("Data / MC ");

  // Canvas & pads
  TwoPadsTCanvas *c1 = new TwoPadsTCanvas("MC and data histograms", "MC and data histograms", 0, 0, 1000, 700);
  TPad* pad1 = c1->pad1;
  TPad* pad2 = c1->pad2;
  TPad* erase = c1->erase;

  // Title
  TString slumi = Form("%.4g",Lumi_data*1e-3);
  TString plotTitle = "#sqrt{s} = 13 TeV       #scale[0.6]{#int} #font[12]{L} dt = " + slumi + " fb^{-1}";
  TLatex *title = new TLatex(0.4,0.96,plotTitle);
  title -> SetTextFont(43);
  title -> SetTextSizePixels(25);
  title -> Draw();

  // Top pad
  pad1 -> cd();
  rangesitter -> Draw();
  hs -> Draw("Same,HIST");
  data1 -> Draw("Same");
  rangesitter -> Draw("sameaxis");

  // Legend
  TLegend *leg = new TLegend(0.76,0.5,0.92,0.83);
  leg -> SetTextFont(43);
  leg -> SetTextSizePixels(14);
  leg -> SetBorderSize(0);
  leg -> SetFillColorAlpha(0, 0.);
  leg -> AddEntry(data1, "Data 2016",  "fl");
  leg -> AddEntry("mergedDi", "Dibosons",  "fl");
  leg -> AddEntry("mergedtW", "tWt + tW#bar{t}",  "fl");
  leg -> AddEntry("ttbar_lep", "t#bar{t}",  "fl");
  leg -> AddEntry("mergedjets", "Jets",  "fl");
  leg -> AddEntry("dyInclusiveTauTau", "DY #rightarrow #tau#tau",  "fl");
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
