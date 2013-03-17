#include <string>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <osg/Group>

#include "Viewer.h"
#include "GlobFit.h"

int main(int argc, char *argv[])
{
  double paraOrthThreshold, equalAngleThreshold, coaxialThreshold, coplanarThreshold, equalLengthThreshold, equalRadiusThreshold;

  namespace po = boost::program_options;
  po::options_description options("Allowed options");
  options.add_options()
    ("help", "produce help message")
    ("input,i", po::value<std::string>(), "input file")
    ("paraOrthThreshold,o", po::value<double>(&paraOrthThreshold)->default_value(10.00, "10.00"), "parallel/orthogonal threshold")
    ("equalAngleThreshold,g", po::value<double>(&equalAngleThreshold)->default_value(10.00, "10.00"), "equal angle threshold")
    ("coaxialThreshold,a", po::value<double>(&coaxialThreshold)->default_value(0.02, "0.02"), "coaxial threshold")
    ("coplanarThreshold,p", po::value<double>(&coplanarThreshold)->default_value(0.02, "0.02"), "coplanar threshold")
    ("equalLengthThreshold,l", po::value<double>(&equalLengthThreshold)->default_value(0.02, "0.02"), "equal length threshold")
    ("equalRadiusThreshold,r", po::value<double>(&equalRadiusThreshold)->default_value(0.02, "0.02"), "equal radius threshold")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, options), vm);
  po::notify(vm);

  if (!vm.count("input")) {
    std::cout << options << "\n";
    system("pause");
    return 1;
  }

  std::vector<osg::Node*> vecViewData;
  for (size_t i = 0; i < 6; ++ i) {
    vecViewData.push_back(new osg::Group);
    vecViewData.back()->setDataVariance(osg::Object::DYNAMIC);
  }
  Viewer viewer;
  boost::thread viewerThread(viewer, vecViewData);

  std::string inputFilename = vm["input"].as<std::string>();
  size_t dotPos = inputFilename.find_last_of('.');
  std::string base = inputFilename.substr(0, dotPos);
  std::string ext = inputFilename.substr(dotPos);

  // read input file
  GlobFit globFit;
  if (!globFit.load(inputFilename)) {
    system("pause");
    return 1;
  }
  std::pair<osg::Node*, osg::Node*> pointsGeometry = globFit.convertPointsToGeometry();
  dynamic_cast<osg::Group*>(vecViewData[0])->addChild(pointsGeometry.first);
  dynamic_cast<osg::Group*>(vecViewData[1])->addChild(pointsGeometry.second);
  dynamic_cast<osg::Group*>(vecViewData[2])->addChild(globFit.convertPrimitivesToGeometry("Initial Primitives"));
  viewerThread.detach();

  if (!globFit.createMatlabArraies()) {
    system("pause");
    return 1;
  }

  // Orientation Alignment
  globFit.saveAngles(base+"_angles_before.txt");
  if (!globFit.orientationAlignment(paraOrthThreshold, equalAngleThreshold)) {
    globFit.destoryMatlabArraies();
    system("pause");
    return 1;
  }
  dynamic_cast<osg::Group*>(vecViewData[3])->addChild(globFit.convertPrimitivesToGeometry("Orientation Alignment"));
  std::string oaFilename = base+"_oa"+ext;
  globFit.save(oaFilename);
  globFit.saveAngles(base+"_angles_after.txt");

  // Placement Alignment
  globFit.saveCoaxialOffsets(base+"_coaxial_offsets_before.txt");
  globFit.saveCoplannarOffsets(base+"_coplanar_offsets_before.txt");
  if (!globFit.placementAlignment(coaxialThreshold, coplanarThreshold)) {
    globFit.destoryMatlabArraies();
    system("pause");
    return 1;
  }
  dynamic_cast<osg::Group*>(vecViewData[4])->addChild(globFit.convertPrimitivesToGeometry("Placement Alignment"));
  std::string paFilename = base+"_pa"+ext;
  globFit.save(paFilename);
  globFit.saveCoaxialOffsets(base+"_coaxial_offsets_after.txt");
  globFit.saveCoplannarOffsets(base+"_coplanar_offsets_after.txt");

  // Equality Alignment
  globFit.saveLengths(base+"_lengths_before.txt");
  globFit.saveRadiuses(base+"_radiuses_before.txt");
  if (!globFit.equalityAlignment(equalLengthThreshold, equalRadiusThreshold)) {
    globFit.destoryMatlabArraies();
    system("pause");
    return 1;
  }
  dynamic_cast<osg::Group*>(vecViewData[5])->addChild(globFit.convertPrimitivesToGeometry("Equality Alignment"));
  std::string eaFilename = base+"_ea"+ext;
  globFit.save(eaFilename);
  globFit.saveLengths(base+"_lengths_after.txt");
  globFit.saveRadiuses(base+"_radiuses_after.txt");

  globFit.destoryMatlabArraies();
  system("pause");
  return 0;
}
