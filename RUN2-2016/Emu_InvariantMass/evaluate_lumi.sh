#!/bin/bash
# To make the script executable: chmod u+x evaluate_lumi.sh
# To run the script: ./evaluate_lumi.sh
#
# Takes values from an input file containing mathematical formulas
# and evaluates the result, saving it in a lumi_mc_file.dat file
#
echo
echo "EVALUATING:"
# This code indeed does:
cat input_lumi.py
cat input_lumi.py | while read -r line; do python -c "print  $line"; done > lumi_mc_file.dat
#echo "Result:"
#cat result.dat
echo
