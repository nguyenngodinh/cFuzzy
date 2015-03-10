A Simple Fuzzy logic inference system for Threat evaluation
Author: Ngo Dinh Nguyen
Contacts: nguyenngodinh@outlook.com | nguyennd56coltech@gmail.com
==================================================================
How to use?
0. Environment: windows os
1. compile source
=== g++ cfuzzy.cpp -o cfuzzy
2. Resource requirement:
=== init.txt: define the fuzzy set (etc. speed-low, speed-medium, . . .)
====== with 1st line as define number of "member" which is as variable
====== the next line define the fuzzy sets of each member with informations: Interval, Middle (max membership grade), Type, Name
=== rules.txt: define fuzzy if-then rules as form with two inputs in left side, and one output in the right side
====== the rule be defined as index of fuzzy set be defined in init.txt
=== data.txt: where each line is the set of values for input params
=== results.txt: the file store result as threat value corresponding with value input
3. Run
=== cfuzzy.exe