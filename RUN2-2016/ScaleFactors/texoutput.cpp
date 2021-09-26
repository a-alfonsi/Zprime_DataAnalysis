{


  // RETRIEVE STORED CONTENT
  // Open ROOT file and get storage TGraphs
  TFile *texout = TFile::Open("texoutput.root", "READ");
  texout -> ls();
  TGraph *T_SFtot;
  TGraph *T_N_emu_obs;
  TGraph *T_N_mumu_data;
  TGraph *T_N_mumu_sim;
  TGraph *T_N_mumu_simtot;
  TGraph *T_N_mumu_datatot;
  TGraph *T_SFttbar;
  TGraph *TE_SFtot;
  TGraph *TE_SFttbar;
  TGraph *TE_N_mumu_sim;
  TGraph *TE_N_mumu_simtot;
  TGraph *TE_N_mumu_data;
  TGraph *TE_N_mumu_datatot;
  texout -> GetObject("SFtot", T_SFtot);
  texout -> GetObject("N_emu_obs", T_N_emu_obs);
  texout -> GetObject("N_mumu_sim", T_N_mumu_sim);
  texout -> GetObject("N_mumu_simtot", T_N_mumu_simtot);
  texout -> GetObject("N_mumu_data", T_N_mumu_data);
  texout -> GetObject("N_mumu_datatot", T_N_mumu_datatot);
  texout -> GetObject("SFttbar", T_SFttbar);
  texout -> GetObject("E_SFtot", TE_SFtot);
  texout -> GetObject("E_SFttbar", TE_SFttbar);
  texout -> GetObject("E_N_mumu_sim", TE_N_mumu_sim);
  texout -> GetObject("E_N_mumu_simtot", TE_N_mumu_simtot);
  texout -> GetObject("E_N_mumu_datatot", TE_N_mumu_datatot);
  texout -> GetObject("E_N_mumu_data", TE_N_mumu_data);

  //delete texout;
  // Retrieve arrays
  Double_t *SFtot = T_SFtot -> GetY();
  Double_t *N_emu_obs = T_N_emu_obs -> GetY();
  Double_t *N_mumu_data = T_N_mumu_data -> GetY();
  Double_t *N_mumu_sim = T_N_mumu_sim -> GetY();
  Double_t *N_mumu_simtot = T_N_mumu_simtot -> GetY();
  Double_t *N_mumu_datatot = T_N_mumu_datatot -> GetY();
  Double_t *SFttbar = T_SFttbar -> GetY();
  Double_t *E_SFtot = TE_SFtot -> GetY();
  Double_t *E_SFttbar = TE_SFttbar -> GetY();
  Double_t *E_N_mumu_sim = TE_N_mumu_sim -> GetY();
  Double_t *E_N_mumu_simtot = TE_N_mumu_simtot -> GetY();
  Double_t *E_N_mumu_data = TE_N_mumu_data -> GetY();
  Double_t *E_N_mumu_datatot = TE_N_mumu_datatot -> GetY();


  // WRITE .TEX TABLE
  // Then just copy and paste this table into the .tex file to use it
  FILE *out_data;
  out_data = fopen("table.tex", "w");


  fprintf (out_data,
  "\\documentclass[]{article}\n"
  "\\begin{document}\n"
  );

  fprintf (out_data,
  "\\section{Dati complessivi (usando SF$_{tot}$)}\n"
  "\\begin{center}\n"
  "\\begin{tabular}{| c | c | c | c | c |}\n"
  "\\hline\n"
  "Mass range "
  "& \\begin{tabular}[c]{@{}c@{}}$N (e^{\\pm} \\mu^{\\pm})$\\\\ observed\\\\ \\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$\\mu\\mu / e\\mu$\\\\scale factor\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$\\\\data prediction\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$\\\\sim. prediction\\end{tabular}\n"
  "\\\\ \\hline\n"
  "120$-$200 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "200$-$400 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "$>$ 400 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "\\end{tabular}\n"
  "\\end{center}\n"
  "\n\n"
  "\\section{Confronto SF$_{t\\bar{t}}$ e SF$_{tot}$}\n"
  "\\begin{center}\n"
  "\\begin{tabular}{| c | c | c |}\n"
  "\\hline\n"
  "Mass range \n"
  "& $\\mu\\mu / e\\mu $ $ $ SF$_{t\\bar{t}}$\n"
  "& $\\mu\\mu / e\\mu $ $ $ SF$_{tot}$\n"
  "\\\\ \\hline\n"
  "120$-$200 GeV & %.3f $\\pm$ %.3f & %.3f $\\pm$ %.3f  \\\\ \\hline\n"
  "200$-$400 GeV & %.3f $\\pm$ %.3f & %.3f $\\pm$ %.3f  \\\\ \\hline\n"
  "$>$ 400 GeV & %.3f $\\pm$ %.3f & %.3f $\\pm$ %.3f  \\\\ \\hline\n"
  "\\end{tabular}\n"
  "\\end{center}\n"
  "\n\n",
  N_emu_obs[0], SFtot[0], E_SFtot[0], N_mumu_data[0], E_N_mumu_data[0], N_mumu_sim[0], E_N_mumu_sim[0],
  N_emu_obs[1], SFtot[1], E_SFtot[1], N_mumu_data[1], E_N_mumu_data[1], N_mumu_sim[1], E_N_mumu_sim[1],
  N_emu_obs[2], SFtot[2], E_SFtot[2], N_mumu_data[2], E_N_mumu_data[2], N_mumu_sim[2], E_N_mumu_sim[2],
  SFttbar[0], E_SFttbar[0], SFtot[0], E_SFtot[0],
  SFttbar[1], E_SFttbar[1], SFtot[1], E_SFtot[1],
  SFttbar[2], E_SFttbar[2], SFtot[2], E_SFtot[2]);


  fprintf (out_data,
  "\\section{Dati complessivi (usando SF$_{t\\bar{t}}$)}\n"
  "\\begin{center}\n"
  "\\begin{tabular}{| c | c | c | c | c |}\n"
  "\\hline\n"
  "Mass range "
  "& \\begin{tabular}[c]{@{}c@{}}$N (e^{\\pm} \\mu^{\\pm})$\\\\ observed\\\\ \\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$\\mu\\mu / e\\mu$\\\\scale factor\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$\\\\data prediction\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$\\\\sim. prediction\\end{tabular}\n"
  "\\\\ \\hline\n"
  "120$-$200 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "200$-$400 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "$>$ 400 GeV & %.0f & %.3f $\\pm$ %.3f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "\\end{tabular}\n"
  "\\end{center}\n"
  "\n\n",
  N_emu_obs[0], SFttbar[0], E_SFttbar[0], N_mumu_data[0], E_N_mumu_data[0], N_mumu_sim[0], E_N_mumu_sim[0],
  N_emu_obs[1], SFttbar[1], E_SFttbar[1], N_mumu_data[1], E_N_mumu_data[1], N_mumu_sim[1], E_N_mumu_sim[1],
  N_emu_obs[2], SFttbar[2], E_SFttbar[2], N_mumu_data[2], E_N_mumu_data[2], N_mumu_sim[2], E_N_mumu_sim[2]);




  fprintf (out_data,
  "\\section{Confronto $N (\\mu^{+} \\mu^{-})$}\n"
  "\\begin{center}\n"
  "\\begin{tabular}{| c | c | c | c | c |}\n"
  "\\hline\n"
  "Mass range \n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$ $t\\bar{t}$ \\\\data prediction\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$ $tot$\\\\data prediction\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$ $t\\bar{t}$ \\\\sim prediction\\end{tabular}\n"
  "& \\begin{tabular}[c]{@{}c@{}}$N (\\mu^{+} \\mu^{-})$ $tot$\\\\sim prediction\\end{tabular}\n"
  "\\\\ \\hline\n"
  "120$-$200 GeV & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "200$-$400 GeV & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f \\\\ \\hline\n"
  "$>$ 400 GeV & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f & %.0f $\\pm$ %.0f  \\\\ \\hline\n"
  "\\end{tabular}\n"
  "\\end{center}\n",
  N_mumu_data[0], E_N_mumu_data[0], N_mumu_datatot[0], E_N_mumu_datatot[0], N_mumu_sim[0], E_N_mumu_sim[0], N_mumu_simtot[0], E_N_mumu_simtot[0],
  N_mumu_data[1], E_N_mumu_data[1], N_mumu_datatot[1], E_N_mumu_datatot[1], N_mumu_sim[1], E_N_mumu_sim[1], N_mumu_simtot[1], E_N_mumu_simtot[1],
  N_mumu_data[2], E_N_mumu_data[2], N_mumu_datatot[2], E_N_mumu_datatot[2], N_mumu_sim[2], E_N_mumu_sim[2], N_mumu_simtot[2], E_N_mumu_simtot[2]);

  fprintf (out_data,
  "\\end{document}\n"
  );

  fclose (out_data);


}
