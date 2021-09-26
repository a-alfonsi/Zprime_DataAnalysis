#ifndef APCLASS_H__
#define APCLASS_H__

#include <map>
#include <TH1.h>
#include "TString.h"
#include "THStack.h"
#include <TStyle.h>
#include <vector>
#include "TColor.h"
#include <TCanvas.h>



// CLASSES LIST:
// CLASS histosfromtfile
// CLASS stacktools
// CLASS lumitools
// NAMESPACE tools
// CLASS TwoPadsTCanvas
// CLASS styles










class histosfromtfile
{
  std::map<TString,TString> filenamesMap;
  Int_t n;
public:
  TH1F ** histoarray(const char *, const char *, const char *, const char*);
  TH1F * histo(const char *, const char *);
  int getn();
};










class stacktools
{
  Int_t nh;
  std::vector<Int_t> w_index;
  //Int_t *w_index = new Int_t[nh];
  //static Int_t w_index[nh];
public:
  void set_nh(Int_t);
  THStack * orderedstack(TH1F **);
  std::vector<Int_t> getw_index();
  void setcolour(Color_t colours[], THStack *, TH1F **);
};










class lumitools
{
public:
  Float_t * lumivalues(const char *);
  TH1F * lumihisto(TH1F *, Float_t, Float_t);
};










namespace tools
{
  void AddOverFlow(TH1 *);
  TH1F ** Rebintill(TH1F**, Int_t, Int_t, Double_t newBins[], Double_t lastx);
  TH1F * Rebintill(TH1F*, Int_t, Double_t newBins[], Double_t lastx);
}










class TwoPadsTCanvas : public TCanvas
{
public:
  // BiCanvas();
  // ~BiCanvas();
  // BiCanvas(Double_t frac);
  // BiCanvas(TString name, TString title, Int_t ww, Int_t wh, Double_t frac=0.5);
  TPad *pad1;
  TPad *pad2;
  TPad *erase;
  TwoPadsTCanvas(TString name, TString title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
  void Init();
};










class styles
{
public:
  TStyle * AlicesStyle1();
  TStyle * AlicesStyle2();
};










#endif
