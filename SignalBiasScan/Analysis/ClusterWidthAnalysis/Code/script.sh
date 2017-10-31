#Create TEC DecoMode files for multiple runs in a row ! #

####

cp Run_TreeMaker_files/Run_TreeMaker_compiled_276437.C  Run_TreeMaker_compiled.C

sed -i 's/subdet=0/subdet=4/g' Run_TreeMaker_compiled.C

make -j5

./Run_TreeMaker_compiled

mv TEC_output_DecoMode.root Outputs/TEC_output_DecoMode_276437.root



###

cp Run_TreeMaker_files/Run_TreeMaker_compiled_271056.C  Run_TreeMaker_compiled.C

sed -i 's/subdet=0/subdet=4/g' Run_TreeMaker_compiled.C

make -j5

./Run_TreeMaker_compiled

mv TEC_output_DecoMode.root Outputs/TEC_output_DecoMode_271056.root
