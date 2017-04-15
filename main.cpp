#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "scene.hpp"
#include "lkt_scene.hpp"
#include "roi_scene.hpp"
#include "ccontour_scene.hpp"

Scene* read_input(std::string filepath) {
	std::ifstream file("../inputs/" + filepath + ".txt");
	if(!file.is_open()) {
		std::cerr << "Coudn't open file" << std::endl;
		return 0;
	}
	
	std::string type, videopath;
	std::getline(file, videopath);
	std::getline(file, type);
	videopath = "../inputs/" + videopath;
	
	if(type == "LKT") {
		return new LktScene("LKT Tracking", videopath);
		
	} else if(type == "ROI") {
		int chanNum, threshold;
		file >> chanNum >> threshold;
		return new RoiScene("ROI Tracking", videopath, chanNum, threshold);
		
	} else if(type == "CC") {
		int chanNum, threshold, area, perim, circ;
		file >> chanNum >> threshold >> area >> perim >> circ;
		return new CContourScene("CC Tracking", videopath, chanNum, threshold, area, perim, circ);
		
	} else {
		std::cerr << "Unrecognized scene type" << std::endl;
		return 0;
	}
}

int main( int argc, char** argv ) {
	
	if(argc < 2) {
		std::cerr << "Missing input argument" << std::endl;
		return 1;
	}
	
	Scene* mScene = read_input(argv[1]);
	if(mScene == 0) return 1;
	
	int speed = 0;
	bool autostop = false;
    while(mScene->nextFrame()) {
		
		mScene->computeStep();
		mScene->display();
		
		if(autostop) {
			mScene->setRunning(false);
			autostop = false;
		}
		
        int c = waitKey(1000/mScene->fps() + speed);
        switch( c ) {
			case '-': speed += 1; break;
			case '+': speed -= 1; break;
			case 110: mScene->setRunning(true); autostop = true; break;
			case 32: mScene->toggleRunning(); break;
			case 27: return 0;
        }
    }
	
	return 0;
}
