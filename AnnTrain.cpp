#include "stdafx.h"
#include "AnnTrain.h"
#include <numeric>
#include <ctime>
#include "Config.h"
#include "Feature.h"

AnnTrain::AnnTrain(const char* chars_folder, const char* test_folder, const char* error_folder, const char* xml)
	: chars_folder_(chars_folder), test_folder_(test_folder), error_folder_(error_folder),ann_xml_(xml)
{
	ann_ = ml::ANN_MLP::create();//创建一个空的ANN模型
	ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(xml);

}

AnnTrain::~AnnTrain()
{

}

bool AnnTrain::Train(int hidenlayernumber,int neuronnumber, int charstype) 
{
	int ClassNumber = 0;//字符类别数目
	int InputNumber = 0;//神经网络输入层神经元数目
	int HiddenNumber = 0;//神经网络隐含层神经元数目
	int OutputNumber = 0;//神经网络输出层神经元数目
	Mat Layers;//神经网络层数
	Type = charstype;//字符集的类型：0-字母和数字，1-只含数字，2-只含字母
	HidenLayerNum = hidenlayernumber;//神经网络隐含层数
	if (Type == 0) {//包含字母和数字的字符集
		ClassNumber = NumberAndLetter;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}
	else if (Type==1)//仅包含数字的字符集
	{
		ClassNumber = OnlyNumber;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}
	else if (Type == 2) {//仅包含字母的字符集
		ClassNumber = OnlyLetter;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}

	int N = InputNumber;
	int M = OutputNumber;
	int FirstHiddenNeurons = int(sqrt((M + 2) * N) + 2 * sqrt(N / (M + 2)));//第一层隐含层神经元数
	int SecondHiddenNeurons = int(M * sqrt(N / (M + 2)));//第二层隐含层神经元数
	int ThirdHiddenNeurons = FirstHiddenNeurons;

	if (HidenLayerNum==0) 
	{//单隐含层神经网络
		Layers.create(1, 3, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = HiddenNumber;
		Layers.at<int>(2) = OutputNumber;
	}
	else if (HidenLayerNum == 1)//双隐含层神经网络
	{
		Layers.create(1, 4, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = FirstHiddenNeurons;
		Layers.at<int>(2) = SecondHiddenNeurons;
		Layers.at<int>(3) = OutputNumber;
	}
	else if (HidenLayerNum == 2)//三隐含层神经网络
	{
		Layers.create(1, 5, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = FirstHiddenNeurons;
		Layers.at<int>(2) = SecondHiddenNeurons;
		Layers.at<int>(3) = ThirdHiddenNeurons;
		Layers.at<int>(4) = OutputNumber;
	}
	ann_->setLayerSizes(Layers);//设置神经网络层数
	ann_->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 1, 1);//设置神经网络激活函数为SIGMOID
	ann_->setTrainMethod(ml::ANN_MLP::TrainingMethods::BACKPROP);//设置训练算法为误差反向传播算法
	ann_->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));
	ann_->setBackpropWeightScale(0.1);//设置初始权重
	ann_->setBackpropMomentumScale(0.1);
	auto TrainData = sdata(350);//得到数据特征以及标签
	ann_->train(TrainData);//训练数据得到训练模型
	ann_->save(ann_xml_);//将训练模型保存为xml文件
	return true;
}


pair<string, string> AnnTrain::Identify(Mat input, int type)
{
	Feature fea;
	Mat feature = fea.CharFeatures(input, PredictSize);//得到字符特征
	float maxVal = -2;
	int result = -1;
	if (type == 0)
	{
		Mat output(1, NumberAndLetter, CV_32FC1);
		ann_->predict(feature, output);

		for (int j = 0; j < NumberAndLetter; j++) {
			float val = output.at<float>(j);
			if (val > maxVal) {
				maxVal = val;
				result = j;
			}
		}
		auto index = result;
		if (index < NumberAndLetter)
		{
			return make_pair(CharsAll[index], CharsAll[index]);//测试得到ANN模型预测字符值
		}
	}
	if (type == 1)
	{
		Mat output(1, OnlyNumber, CV_32FC1);
		ann_->predict(feature, output);

		for (int j = 0; j < OnlyNumber; j++) {
			float val = output.at<float>(j);
			if (val > maxVal) {
				maxVal = val;
				result = j;
			}
		}
		auto index = result;
		if (index < OnlyNumber)
		{
			return make_pair(CharsNumber[index], CharsNumber[index]);//测试得到ANN模型预测字符值
		}
	}
	if (type == 2)
	{
		Mat output(1, OnlyLetter, CV_32FC1);
		ann_->predict(feature, output);

		for (int j = 0; j < OnlyLetter; j++) {
			float val = output.at<float>(j);
			if (val > maxVal) {
				maxVal = val;
				result = j;
			}
		}
		auto index = result;
		if (index < OnlyLetter)
		{
			return make_pair(CharsLetter[index], CharsLetter[index]);//测试得到ANN模型预测字符值
		}
	}
	else
	{
		const char *unknown;
		return make_pair(unknown, unknown);
	}
}

bool AnnTrain::Test(int charstype) {
	assert(test_folder_);//判断测试文件夹是否存在
	Feature fea;
	Type = charstype;
	int ClassNumber = 0;
	if (Type == 0) ClassNumber = NumberAndLetter;//不同字符集对应的类别数目不同,NumberAndLetter=34
	if (Type == 1) ClassNumber = OnlyNumber;//OnlyNumber=10
	if (Type == 2) ClassNumber = OnlyLetter;//OnlyLetter=24
	int corrects_all = 0, sum_all = 0;//识别正确的字符数目与总的字符数目
	vector<float> rate_list;//单类字符集识别正确率组成的集合
	for (int i = 0; i < ClassNumber; ++i) {
		const char *char_key = 0;;
		if (Type == 0)
		{
			char_key = CharsAll[i];//取出字符集数组里面的单个字符
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//取出字符集数组里面的单个字符
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//取出字符集数组里面的单个字符
		}
		char sub_folder[512] = { 0 };
		sprintf(sub_folder, "%s/%s", test_folder_, char_key);
		auto chars_files = fea.GetFiles(sub_folder);//从字符文件夹中获得所有字符图片文件
		int corrects = 0, sum = 0;
		vector<pair<string, string>> error_files;//识别错误字符集
		int error_count = 0;
		for (auto file : chars_files) 
		{//遍历所有字符文件
			auto img = imread(file, 0);//读入字符图片文件
			Mat imgNorm;
			imgNorm = fea.NormalizedImage(img, imgNorm);
			pair<string, string> ch;
			if (Type == 0) ch = Identify(imgNorm, 0);//识别测试字符
			if (Type == 1) ch = Identify(imgNorm, 1);
			if (Type == 2) ch = Identify(imgNorm, 2);
			if (ch.first == char_key) {//如果字符识别结果与字符集数组中取出的字符一致，则识别正确
				++corrects;
				++corrects_all;
			}
			else {//否则识别错误，将错误字符放到error_files中
				++error_count;
				error_files.push_back(make_pair(fea.GetFileName(file), ch.second));
				string temp(char_key);
				string str_char_key = temp;
				char buffer[10];
				sprintf(buffer, "%d", error_count);
				string str_error_count = buffer;
				CString cstr_char_key(str_char_key.c_str());
				CString newPath =_T( "./Data//AnnError//") + cstr_char_key;
				CreateDirectory(newPath,NULL);
				string error_data_name = "Data/AnnError/" + str_char_key+"/" + str_char_key + "_" + str_error_count + ".bmp";
				imwrite(error_data_name, img);
			}
			++sum;
			++sum_all;
		}
		float rate = (float)corrects / (sum == 0 ? 1 : sum);
		ofstream myfile("Result/Result.txt", ios::app);
		if (myfile.is_open())
		{
			time_t t = time(0);
			struct tm* now = localtime(&t);
			char buf[80];
			strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
			myfile << string(buf) << endl;
			myfile << "字符类别" << ":" << char_key << ",  ";
			myfile << "字符总数" << ":" << sum << ",  ";
			myfile << "正确数目" << ":" << corrects << ",  ";
			myfile << "正确率" << ":" << rate*100 << "%"<<endl;
			myfile.close();
		}
		rate_list.push_back(rate);
	}
	float correct_rate = ((float)corrects_all / (sum_all == 0 ? 1 : sum_all)) * 100;
	double rate_sum = accumulate(rate_list.begin(), rate_list.end(), 0.0);
	double rate_mean = rate_sum / (rate_list.size() == 0 ? 1 : rate_list.size());
	ofstream myfile("Result/Result.txt", ios::app);
	if (myfile.is_open())
	{
		time_t t = time(0);
		struct tm* now = localtime(&t);
		char buf[80];
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
		myfile << string(buf) << endl;
		myfile << "-------------------------------------" << endl;
		myfile << "总体结果：" << endl;
		myfile << "-------------------------------------" << endl;
		myfile << "字符数目" << ":" << sum_all<< ",  ";
		myfile << "正确数目" << ":" << corrects_all << ",  ";
		myfile << "正确率" << ":" << correct_rate <<"%"<< endl;
		myfile << "类别数目" << ":" << ClassNumber << ", ";
		myfile << "平均识别率" << ":" << rate_mean * 100 << "%"<<endl;
		myfile << "-------------------------------------" << endl;
		myfile.close();
	}
	return true;
}

Mat GetSyntheticImage(const Mat& image) {
	Feature fea;
	int rand_type = rand();//产生随机数
	Mat result = image.clone();//克隆图片
	if (rand_type % 2 == 0) {//若随机数为偶数
		int ran_x = rand() % 5 - 2;
		int ran_y = rand() % 5 - 2;
		result = fea.TranslateImg(result, ran_x, ran_y);//翻转
	}
	else if (rand_type % 2 != 0) {//若随机数为奇数
		float angle = float(rand() % 15 - 7);
		result = fea.RotateImg(result, angle);//旋转
	}
	return result;
}

Ptr<ml::TrainData> AnnTrain::sdata(size_t number_for_count) {
	assert(chars_folder_);//判断训练文件夹是否存在
	Mat samples;//训练样本特征
	vector<int> labels;//训练样本标签
	Feature fea;
	int ClassNumber = 0;
	if (Type == 0) ClassNumber = NumberAndLetter;
	if (Type == 1) ClassNumber = OnlyNumber;
	if (Type == 2) ClassNumber = OnlyLetter;
	srand((unsigned)time(0));
	for (int i = 0; i < ClassNumber; ++i) {
		const char *char_key=0;
		if (Type == 0)
		{
			char_key = CharsAll[i];//取出字符集数组里面的单个字符
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//取出字符集数组里面的单个字符
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//取出字符集数组里面的单个字符
		}
		char sub_folder[512] = { 0 };
		sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
		auto chars_files = fea.GetFiles(sub_folder);
		size_t char_size = chars_files.size();//统计字符一共有多少张训练图片
		vector<Mat> matVec;
		matVec.reserve(number_for_count);//设置matVec的大小不小于number_for_count
		for (auto file : chars_files) {//将训练字符文件放到matVec中
			auto img = imread(file, 0);
			Mat imgNorm;
			imgNorm = fea.NormalizedImage(img, imgNorm);
			matVec.push_back(imgNorm);
		}

		for (int t = 0; t < (int)number_for_count - (int)char_size; t++) {
			int rand_range = char_size + t;
			int ran_num = rand() % rand_range;
			auto img = matVec.at(ran_num);
			auto simg = GetSyntheticImage(img);//得到翻转或旋转后的图片
			matVec.push_back(simg);
		}

		for (auto img : matVec) 
		{
			auto fps = fea.CharFeatures(img, PredictSize);//得到字符特征10X10+10+10=120（维度）
			samples.push_back(fps);//存储字符样本特征
			labels.push_back(i);//存储字符样本标签
		}
	}
	Mat samples_;
	samples.convertTo(samples_, CV_32F);
	Mat train_classes =Mat::zeros((int)labels.size(), ClassNumber, CV_32F);
	for (int i = 0; i < train_classes.rows; ++i) 
	{
		train_classes.at<float>(i, labels[i]) = 1.f;
	}
	return ml::TrainData::create(samples_, ml::SampleTypes::ROW_SAMPLE,train_classes);//得到ANN训练数据
}

Ptr<ml::TrainData> AnnTrain::tdata() {
	assert(chars_folder_);
	Mat samples;
	vector<int> labels;
	Feature fea;
	int ClassNumber = 0;
	if (Type == 0) ClassNumber = NumberAndLetter;
	if (Type == 1) ClassNumber = OnlyNumber;
	if (Type == 2) ClassNumber = OnlyLetter;

	for (int i = 0; i < ClassNumber; ++i) {
		const char *char_key=0;
		if (Type == 0)
		{
			char_key = CharsAll[i];//取出字符集数组里面的单个字符
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//取出字符集数组里面的单个字符
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//取出字符集数组里面的单个字符
		}
		char sub_folder[512] = { 0 };
		sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
		auto chars_files = fea.GetFiles(sub_folder);
		for (auto file : chars_files) {
			auto img = imread(file, 0);
			Mat imgNorm;
			imgNorm=fea.NormalizedImage(img, imgNorm);
			auto fps = fea.CharFeatures(imgNorm, PredictSize);
			samples.push_back(fps);
			labels.push_back(i);
		}
	}
	Mat samples_;
	samples.convertTo(samples_, CV_32F);
	Mat train_classes =Mat::zeros((int)labels.size(), ClassNumber, CV_32F);
	for (int i = 0; i < train_classes.rows; ++i) 
	{
		train_classes.at<float>(i, labels[i]) = 1.f;
	}
	return ml::TrainData::create(samples_, ml::SampleTypes::ROW_SAMPLE,train_classes);
}