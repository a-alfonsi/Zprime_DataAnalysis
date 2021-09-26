#include "apclass.h"
// cpp includes
#include <iostream>
#include <fstream>
#include <map>
// ROOT includes
#include "TString.h"
#include <TROOT.h>
#include <TList.h>
#include <TObject.h>
#include <TH1.h>
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TFile.h"
#include <TMath.h>
#include "THStack.h"
#include <TStyle.h>
#include <TCanvas.h>
#include <iterator>
#include "TColor.h"










// -----------------------------------------------------------------------------
//   CLASS histosfromtfile
// -----------------------------------------------------------------------------




// GET THE HISTOGRAM "HISTONAME" FROM THE ROOTFILE WITH PATH "FILEPATH"
TH1F * histosfromtfile::histo (const char *filepath, const char * histoname)
{
  TFile *f = new TFile(filepath);
  // if (!f || f -> IsZombie())
  //   return;
  std::cout << "Loaded file: " << std::endl;
  std::cout << "* - " << filepath << std::endl;
  TH1F * histo = (TH1F*)f -> Get(histoname);
  histo -> SetDirectory(gROOT);
  f -> Close();
  return histo;
}




// OPEN A LIST OF FILES WITH SPECIFIED EXTENSION FROM A DIRECTORY, IN ALPHABETICAL
// ORDER, AND GET AN ARRAY OF HISTOGRAMS WITH THE SAME NAME, ONE FROM EACH FILE
// > dirname = absolute path of the directory where the rootfiles are
// example: const char *dirname = "/afs/cern.ch/work/a/aalfonsi/public/"
// > ext = extension of the files to open
// example:   const char *ext = ".root"
// > histoarrayname = name or path of the histograms to be retrieved
// note: this function retrieves an array of histograms all with the same name
// example: const char *histoarrayname = "EmuVetoMuonsElectronsOppSignHistos/DileptonMass"
// > strip = string to be stripped from the filename (in addiction "ext" will be
// automatically stripped); this may be useful if someone
// wants to assing a different name to every retrieved histogram of the array,
// this name recalling that of the rootfile in which its contained
// example: filename[i] = "ana_datamc_WZ.root" will be stripped of "ext" automatically
// and it will be stripped of const char *strip = "ana_datamc_", so
// histoarray[i] name will become "WZ"
TH1F ** histosfromtfile::histoarray (const char *dirname, const char *ext, const char * histoarrayname, const char * strip)
{
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
      //std::cout << tfname << std::endl;
      if (!file->IsDirectory() && tfname.EndsWith(ext))
      {
        tfnamel = tfname;
        tfnamel.ToLower();
        filenamesMap.insert(std::pair<TString, TString>(tfnamel, tfname));
        n++;
      }
      //std::cout << n << std::endl;
    }
  }
  // OPEN LISTED FILES IN THE RIGHT ORDER
  TFile **filearray = new TFile*[n];
  TH1F **histoarray = new TH1F*[n];
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
    histoarray[i] = (TH1F*)filearray[i] -> Get(histoarrayname);
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




// GETS VALUES OF N, WHICH IS PRIVATE IN THE CLASS histosfromtfile
int histosfromtfile::getn()
{
  return (n);
}










// -----------------------------------------------------------------------------
//   CLASS lumitools
// -----------------------------------------------------------------------------




Float_t * lumitools::lumivalues(const char * lumifile)
{
  static Float_t Lumi_mc_inv[8];  // Lumi_mc^(-1)
  // Retrieve Lumi_mc^-1 values from file
  // NOTE: see input_lumi.py and run evaluate_lumi.sh on it to produce
  // the correct lumi_mc_file.dat
  std::fstream finput(lumifile);
  TString line;
  Int_t mc_i = 0;
  while ( line.ReadLine(finput) )
  {
    if ( line[0] != ' ' )
    {
      Lumi_mc_inv[mc_i] = line.Atof();
      mc_i ++;
    }
  }
  return Lumi_mc_inv;
}




TH1F * lumitools::lumihisto(TH1F *hist, Float_t Lumi_data, Float_t Lumi_error)
{
  Int_t nbins = hist -> GetNbinsX();
  Float_t xlow = hist -> GetBinLowEdge(1);
  Float_t xup = hist -> GetBinLowEdge(hist->GetNbinsX()+1);
  TH1F *Luminosity = new TH1F("Luminosity", "Luminosity", nbins, xlow, xup);
  Luminosity -> SetDirectory(gROOT);
  for (Int_t i=0; i<nbins; i++)
  {
    Luminosity -> SetBinContent(i+1, Lumi_data);
    Luminosity -> SetBinError(i+1, Lumi_error);
  }
  return Luminosity;
}










// -----------------------------------------------------------------------------
//   CLASS stacktools
// -----------------------------------------------------------------------------




// -- STACK AN ARRAY OF HISTOGRAMS IN AN ORDER GIVEN BY THEIR INTEGRAL:
// THE BIGGEST INTEGRAL GOES AT THE BOTTOM, SO EVERY HISTO MAY BE VISIBLE
//THStack * stacktools::stackorder(Int_t nh, TH1F ** histoarray)
THStack * stacktools::orderedstack(TH1F ** histoarray)
{
  // Find stacking order
  Float_t weight[nh];
  for (Int_t v=0; v<nh; v++)
  {
    weight[v] = histoarray[v] -> GetSumOfWeights();
  }
  Int_t windex[nh];
  TMath::Sort(nh,weight,windex);
  //std::vector<Int_t> w_index(windex, windex + nh);
  for (Int_t i=0; i<nh; i++)
  {
    w_index[i] = windex[i];
  }
  // Stack histograms
  THStack *hs = new THStack ("hs","stacked histograms");
  for (Int_t v=nh-1; v>-1; v--)
  {
    hs -> Add(histoarray[windex[v]]);
    //std::cout << w_index[v] << ", " << windex[v] << std::endl;
  }
  return hs;
}




void stacktools::setcolour(Color_t colours[], THStack *hs, TH1F **histoarray)
{
  TObjArray *histos = hs -> GetStack();
  TIter Next(histos);
  TH1F *hist;
  Int_t icolour = 0;
  std::cout << std::endl;
  while ((hist =(TH1F*)Next()))
  {
    std::cout << icolour << " " << w_index[nh-1-icolour] << " ";
    histoarray[w_index[nh-1-icolour]] -> SetFillColor(colours[icolour]);
    hist -> SetFillColor(colours[icolour++]);
    hist -> SetOption("HIST");
  }
}




// GETS VALUES OF W_INDEX, WHICH IS PRIVATE IN THE CLASS stacktools
std::vector<Int_t> stacktools::getw_index()
{
  return w_index;
}




// DEFINES nh
void stacktools::set_nh(Int_t nhinput)
{
  nh = nhinput;
  w_index.resize(nh);
}










// -----------------------------------------------------------------------------
//   NAMESPACE tools
// -----------------------------------------------------------------------------




// ADD OVERFLOW TO A HISTOGRAM
void tools::AddOverFlow(TH1* hist)
{
  Int_t lastBin = hist -> GetNbinsX();
  hist->SetBinContent(lastBin, hist->GetBinContent(lastBin) + hist->GetBinContent(lastBin+1));
  hist->SetBinError  (lastBin, sqrt(hist->GetBinError(lastBin)*hist->GetBinError(lastBin) + hist->GetBinError(lastBin+1)*hist->GetBinError(lastBin+1)));
  hist->SetBinContent(lastBin+1, 0) ;
  hist->SetBinError  (lastBin+1, 0) ;
}



// REBIN HISTOGRAMS IN AN ARRAY TO VARIABLE SIZE BINS, THEN TRUNCATE THE LAST
// BIN TO A VALUE SPECIFIED BY THE USER
// (SO THAT THE DRAW TRUNCATED TO THAT VALUE CAN CORRECTLY SHOW THE ERROR BARS)
// WITHOUT LOSING LAST BIN CONTENT WHEN TRUNCATING
// THAT IS: SAVING ALL THE UNTRUNCATED BIN CONTENT INSIDE THE TRUNCATED BIN
TH1F ** tools::Rebintill(TH1F** histoarray, Int_t nh, Int_t numbins, Double_t newBins[], Double_t lastx)
{
  // REBIN IN VARIABLE SIZE BINS
  TH1F **histoarray_rebin = new TH1F*[nh];
  for (Int_t i=0; i<nh; i++)
  {
    histoarray_rebin[i] = (TH1F*)histoarray[i] -> Rebin(numbins, Form("%s_rebinned", histoarray[i]->GetName() ), newBins);
  }
  // TRUNCATE LAST BIN
  // Define newBinsTrunc
  Double_t newBinsTrunc[numbins+1];
  for (uint i=0; i<numbins; i++)
  {
    newBinsTrunc[i] = newBins[i];
  }
  newBinsTrunc[numbins] = lastx;
  // Truncate
  TH1F **histoarray_trunc = new TH1F*[nh];
  for (Int_t j=0; j<nh; j++)
  {
    histoarray_trunc[j] = new TH1F(Form("%s_truncated", histoarray[j]->GetName() ), "", numbins, newBinsTrunc);
    for(uint ibin=0; ibin < numbins; ibin++)
    {
      histoarray_trunc[j] -> SetBinContent( ibin+1, histoarray_rebin[j] -> GetBinContent(ibin+1) );
      histoarray_trunc[j] -> SetBinError( ibin+1, histoarray_rebin[j] -> GetBinError(ibin+1) );
    }
    histoarray_rebin[j] = histoarray_trunc[j];
  }
  return histoarray_rebin;
}



TH1F * tools::Rebintill(TH1F* histo, Int_t numbins, Double_t newBins[], Double_t lastx)
{
  // REBIN IN VARIABLE SIZE BINS
  TH1F *histo_rebin = new TH1F;
  histo_rebin = (TH1F*)histo -> Rebin(numbins, Form("%s_rebinned", histo->GetName() ), newBins);
  // TRUNCATE LAST BIN
  // Define newBinsTrunc
  Double_t newBinsTrunc[numbins+1];
  for (uint i=0; i<numbins; i++)
  {
    newBinsTrunc[i] = newBins[i];
  }
  newBinsTrunc[numbins] = lastx;
  // Truncate
  TH1F *histo_trunc = new TH1F(Form("%s_truncated", histo->GetName() ), "", numbins, newBinsTrunc);
  for(uint ibin=0; ibin < numbins; ibin++)
  {
    histo_trunc -> SetBinContent( ibin+1, histo_rebin -> GetBinContent(ibin+1) );
    histo_trunc -> SetBinError( ibin+1, histo_rebin -> GetBinError(ibin+1) );
  }
  histo_rebin = histo_trunc;
  return histo_rebin;
}










// -----------------------------------------------------------------------------
//   CLASS TwoPadTCanvas
// -----------------------------------------------------------------------------




ClassImp(TwoPadsTCanvas);




TwoPadsTCanvas::TwoPadsTCanvas(TString name, TString title, Int_t wtopx,
  Int_t wtopy, Int_t ww, Int_t wh) : TCanvas(name, title, wtopx,
  wtopy, ww, wh)
  {
    Init();
  }




void TwoPadsTCanvas::Init()
{
  this -> cd();
  pad1 = new TPad( Form("%s_p1", this->GetName()), "", 0.035, 0.4, 1., 1.);
  pad1 -> SetLogy();
  pad1 -> SetBottomMargin(0.02);
  pad1 -> SetFillStyle(4000);
  pad1 -> SetFillColor(0);
  pad1->Draw();
  this -> cd();
  pad2 = new TPad( Form("%s_p2", this->GetName()), "", 0.035, 0., 1., 0.416);
  pad2 -> SetTopMargin(0.01);
  pad2 -> SetBottomMargin(0.28);
  pad2 -> SetFillStyle(4000);
  pad2 -> SetFillColor(0);
  pad2 -> Draw();
  this -> cd();
  Float_t labelsize = 0.035;
  Float_t leftmargin = pad1 -> GetLeftMargin();
  erase = new TPad("eraselabel", "eraselabel", leftmargin-0.1*leftmargin, 0.4, leftmargin+0.8*labelsize, 0.4+labelsize);
  erase -> SetFillColor(pad2 -> GetFillColor());
  erase -> SetBorderMode(0);
  erase -> Draw();
  this -> cd();
  pad1 -> Draw();
}










// -----------------------------------------------------------------------------
//   CLASS styles
// -----------------------------------------------------------------------------




// DEFINE USEFUL STYLES FOR PAD AND CANVAS
TStyle * styles::AlicesStyle1()
{
  TStyle *st1 = new TStyle("st1","my style");
  st1 -> SetOptTitle(0);
  st1 -> SetOptStat(0);
  st1 -> SetTitleXOffset(4.);
  st1 -> SetTitleXSize(20);
  st1 -> SetTitleYOffset(1.5);
  st1 -> SetTitleYSize(20);
  st1 -> SetLabelFont(43, "X");
  st1 -> SetLabelSize(16, "X");
  st1 -> SetLabelFont(43, "Y");
  st1 -> SetLabelSize(16, "Y");
  st1 -> SetLabelOffset(0.02, "X");
  st1 -> SetLabelOffset(0.01, "Y");
  st1 -> SetTitleFont(43, "Y");
  st1 -> SetTitleFont(43, "X");
  st1 -> SetCanvasColor     (0);
  st1 -> SetCanvasBorderSize(0);
  st1 -> SetCanvasBorderMode(0);
  st1 -> SetFrameFillStyle ( 0);
  st1 -> SetFrameFillColor ( 0);
  st1 -> SetFrameLineColor ( 1);
  st1 -> SetFrameLineStyle ( 0);
  st1 -> SetFrameLineWidth ( 1);
  st1 -> SetFrameBorderSize(0);
  st1 -> SetFrameBorderMode( 0);
  return st1;
}



TStyle * styles::AlicesStyle2()
{
  TStyle *st2 = new TStyle("st2", "mystyle");
  st2 -> SetOptTitle(0);
  st2 -> SetOptStat(0);
  st2 -> SetTitleXOffset(1.5);
  st2 -> SetTitleXSize(22);
  st2 -> SetTitleYOffset(1.5);
  st2 -> SetTitleYSize(22);
  st2 -> SetLabelFont(43, "XY");
  st2 -> SetLabelSize(19, "XY");
  st2 -> SetLabelOffset(0.01, "X");
  st2 -> SetLabelOffset(0.01, "Y");
  st2 -> SetTitleFont(43, "XY");
  st2 -> SetTextFont(42);
  st2 -> SetCanvasColor     (0);
  st2 -> SetCanvasBorderSize(0);
  st2 -> SetCanvasBorderMode(0);
  st2 -> SetFrameFillStyle ( 0);
  st2 -> SetFrameFillColor ( 0);
  st2 -> SetFrameLineColor ( 1);
  st2 -> SetFrameLineStyle ( 0);
  st2 -> SetFrameLineWidth ( 1);
  st2 -> SetFrameBorderSize(0);
  st2 -> SetFrameBorderMode( 0);
  return st2;
}
