#pragma once
#include <memory>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
class AnnTrain
{

public:
	explicit AnnTrain(const char* chars_folder, const char* test_folder, const char *error_folder, const char* xml);
	~AnnTrain();

public:
	virtual bool Train(int hidenlayernumber, int neuronnumber, int charstype);
	virtual bool Test(int charstype);
	pair<string, string> Identify(Mat input,int type);

private:
	virtual Ptr<ml::TrainData> tdata();
	Ptr<ml::TrainData> sdata(size_t number);
	Ptr<ml::ANN_MLP> ann_;
	const char* ann_xml_;
	const char* chars_folder_;
	const char* test_folder_;
	const char* error_folder_;
	int Type;//字符集类型
	int HidenLayerNum;//隐含层数

};

