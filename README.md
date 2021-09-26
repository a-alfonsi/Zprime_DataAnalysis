# README #

Scripts and libraries I developed for the Z' background and isolation studies performed during my master's thesis project. 
Note: they are made to be run in the CERN LXPLUS cluster and require ROOT.
Plots or datasets are not shared, but the provided code skeleton could easily be readapted to other projects.



### Folders ###
Please find the last version of the scripts in "RUN2_2016" folder.

* **Isolation_Studies**

    Study of the muon isolation variables.
    
* **Emu_InvariantMass**

    Study and plotting of the invariant e-mu mass spectrum.

* **Libraries**

    Libraries needed to run the ROOT macros.

* **ScaleFactors**

    Calculation and plotting of the e-mu scale factors


### Scripts ###

* **_Emu_InvariantMass/emu_invMass.cpp_**

    Draws the ratio "data / total MC", and the invariant e-mu mass spectrum from stacked MC processes and data


* **_ScaleFactors/ScaleFactors_triggerOn.cpp_**

    Draws: scale factors from ttbar and WW, e-mu and mu-mu ttbar reconstructed events, and the fractions from the different mc processes that contribute to SF calculation
    Calculates SF from ttbar only and then from the weighted contribution of every mc background process

* **_ScaleFactors/texoutput.cpp_**

    Produces a .tex output file containing a table with all the output values collected from the scalefactors script

* **_[.]/evaluate_lumi.sh_**

    Computes the mc luminosities taken from values contained in input_lumi.py, and saves the result in lumi_mc_file.dat


### Notes on how to run this code ###
Just execute the root macros

`root -l macro.cxx`

