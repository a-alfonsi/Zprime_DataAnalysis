// -----------------------------------------------------------------------------
// See if given histogram has associated error for each bin given by sqrt(N)
// or if it is given by Sumw2()
//
// Launch this script after you launched a previous script which opens
// root histograms, or open manually some histograms to have them in memory
// proceed with .x *thisscript*.cpp in the same root session
// -----------------------------------------------------------------------------


{

std::cout << std::endl;


// MAP HISTOGRAMS IN MEMORY TO ASSOCIATED STRINGS
Int_t bin;
TString histoname;
const char* rootfilename;
map<TString,TH1F*> variableMap;
variableMap["data1_rebinned"] = data1_rebinned;
variableMap["fraction[0]"] = fraction[0];
variableMap["mcsum"] = mcsum;
variableMap["SF_weighted[0]"] = SF_weighted[0];
variableMap["SFtot"] = SFtot;

// PRINT LIST OF HISTOGRAMS
std::cout << "HISTOGRAM LIST:\n";
for ( const auto &p : variableMap )
{
   std::cout << p.first << std::endl;
}
std::cout << std::endl;

// ASK WHICH HISTOGRAM MUST BE ANALYZED
// NOTE: histogram pointer name must coincide with the pointer to an histogram
// actually opened in the current session, or it will return segmentation fault
std::cout << "Insert histogram pointer name: ";
histoname.ReadLine(std::cin);


// ANALYZE ERROR TYPE
TArrayD *sumw2s =  variableMap[histoname] -> GetSumw2();
// Ask which bin and control it is not empty
std::cout << "\nWhich bin do you want to check?\n";
cin >> bin;
while (variableMap[histoname] -> GetBinContent(bin) == 0)
{
  std::cout << "Retry: bin[" << bin << "] is empty!" << std::endl;
  std::cout << "\nWhich bin do you want to check?\n";
  cin >> bin;
}
// Analyze error type for that bin
std::cout << "Bin[" << bin << "] content: " << variableMap[histoname] -> GetBinContent(bin) << std::endl;
std::cout << "Bin[" << bin << "] error: " << variableMap[histoname] -> GetBinError(bin) << std::endl;
std::cout << "sqrt(Bin[" << bin << "] content: " <<sqrt(variableMap[histoname] -> GetBinContent(bin)) << std::endl;
std::cout << "sqrt(Sumw2(Bin[" << bin << "]) content: " <<sqrt((*sumw2s)[bin]) << std::endl;

// Computation
if (variableMap[histoname] -> GetBinError(bin) == sqrt(variableMap[histoname] -> GetBinContent(bin)))
{
  std::cout << "--- Error type: sqrt(bin)";
}
else if (variableMap[histoname] -> GetBinError(bin) == sqrt((*sumw2s)[bin]))
{
  std::cout << "--- ERROR TYPE: Sumw2";
}
else if (variableMap[histoname] -> GetBinError(bin) != sqrt((*sumw2s)[bin]))
{
  std::cout << "--- Error type: unknown";
}
std::cout << std::endl;

}
