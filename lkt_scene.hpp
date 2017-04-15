#ifndef LKT_SCENE
#define LKT_SCENE

#include "scene.hpp"

class LktScene : public Scene {
	public:
		LktScene(std::string name, std::string filepath,
			  Size winSize = Size(31,31),
			  TermCriteria termcrit = TermCriteria(TermCriteria::COUNT|TermCriteria::EPS,20,0.03)
			);
		
		void compute();		
		void initKeyPoints(Size subPixWinSize = Size(10,10), int subPixCount = 500);
		void mouseEvent(int key, int x, int y, int flags);
		
	private:		
		std::vector<Point2f> mPoints[2];
		std::vector<uchar> mPointsStatus;
		std::vector<float> mPointsErrors;
		
		TermCriteria mTermCrit;
		Size mWinSize;
};

#endif
