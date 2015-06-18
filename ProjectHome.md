An implementation of SIGGRAPH 2011 paper _[GlobFit: Consistently Fitting Primitives by Discovering Global Relations](http://web.siat.ac.cn/~vcc/publications/2011/globfit/)_([acm digital library](http://dl.acm.org/citation.cfm?id=1964947)), aiming for **research use** only.

We thank [Dirk Holz](http://www.ais.uni-bonn.de/~holz/) for pointing out some bugs and helping on converting the the code from VS project into CMake project!

Researchers at the [many universities](http://web.siat.ac.cn/~vcc/publications/2011/globfit/user.html) are using and testing our GlobFit algorithm/software. If you are using GlobFit for projects, we will be happy to know about your experience and feedback. It helps us improve the software for future releases.

Tips for the optimization parameters:

The parameters should be set large enough to fix the deviations to extract relations, but can not be too large that the right properties are considered as big deviations. The parameters can be estimated from the error range of the scan as well as some pirors of the objects being scanned, and even safer it the constraints can be visualized and edited in interactive sessions.

Suggest parameters for the testing data sets that can be downloaded from the [Downloads](https://code.google.com/p/globfit/downloads/list) section:

-i fig9.globfit

-i fig11.globfit -o 1.5 -g 1.5

-i fig12.globfit -o 3.0 -g 3.0 -a 0.1 -p 0.1 -l 0.1 -r 0.1

-i fig13.globfit -o 2.5 -g 2.5

-i fig14.globfit