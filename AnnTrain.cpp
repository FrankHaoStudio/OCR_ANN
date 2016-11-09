#include "stdafx.h"
#include "AnnTrain.h"
#include <numeric>
#include <ctime>
#include "Config.h"
#include "Feature.h"

AnnTrain::AnnTrain(const char* chars_folder, const char* test_folder, const char* error_folder, const char* xml)
	: chars_folder_(chars_folder), test_folder_(test_folder), error_folder_(error_folder),ann_xml_(xml)
{
	ann_ = ml::ANN_MLP::create();//����һ���յ�ANNģ��
	ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(xml);

}

AnnTrain::~AnnTrain()
{

}

bool AnnTrain::Train(int hidenlayernumber,int neuronnumber, int charstype) 
{
	int ClassNumber = 0;//�ַ������Ŀ
	int InputNumber = 0;//�������������Ԫ��Ŀ
	int HiddenNumber = 0;//��������������Ԫ��Ŀ
	int OutputNumber = 0;//�������������Ԫ��Ŀ
	Mat Layers;//���������
	Type = charstype;//�ַ��������ͣ�0-��ĸ�����֣�1-ֻ�����֣�2-ֻ����ĸ
	HidenLayerNum = hidenlayernumber;//��������������
	if (Type == 0) {//������ĸ�����ֵ��ַ���
		ClassNumber = NumberAndLetter;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}
	else if (Type==1)//���������ֵ��ַ���
	{
		ClassNumber = OnlyNumber;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}
	else if (Type == 2) {//��������ĸ���ַ���
		ClassNumber = OnlyLetter;
		InputNumber = AnnInput;
		HiddenNumber = neuronnumber;
		OutputNumber = ClassNumber;
	}

	int N = InputNumber;
	int M = OutputNumber;
	int FirstHiddenNeurons = int(sqrt((M + 2) * N) + 2 * sqrt(N / (M + 2)));//��һ����������Ԫ��
	int SecondHiddenNeurons = int(M * sqrt(N / (M + 2)));//�ڶ�����������Ԫ��
	int ThirdHiddenNeurons = FirstHiddenNeurons;

	if (HidenLayerNum==0) 
	{//��������������
		Layers.create(1, 3, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = HiddenNumber;
		Layers.at<int>(2) = OutputNumber;
	}
	else if (HidenLayerNum == 1)//˫������������
	{
		Layers.create(1, 4, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = FirstHiddenNeurons;
		Layers.at<int>(2) = SecondHiddenNeurons;
		Layers.at<int>(3) = OutputNumber;
	}
	else if (HidenLayerNum == 2)//��������������
	{
		Layers.create(1, 5, CV_32SC1);
		Layers.at<int>(0) = InputNumber;
		Layers.at<int>(1) = FirstHiddenNeurons;
		Layers.at<int>(2) = SecondHiddenNeurons;
		Layers.at<int>(3) = ThirdHiddenNeurons;
		Layers.at<int>(4) = OutputNumber;
	}
	ann_->setLayerSizes(Layers);//�������������
	ann_->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 1, 1);//���������缤���ΪSIGMOID
	ann_->setTrainMethod(ml::ANN_MLP::TrainingMethods::BACKPROP);//����ѵ���㷨Ϊ���򴫲��㷨
	ann_->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));
	ann_->setBackpropWeightScale(0.1);//���ó�ʼȨ��
	ann_->setBackpropMomentumScale(0.1);
	auto TrainData = sdata(350);//�õ����������Լ���ǩ
	ann_->train(TrainData);//ѵ�����ݵõ�ѵ��ģ��
	ann_->save(ann_xml_);//��ѵ��ģ�ͱ���Ϊxml�ļ�
	return true;
}


pair<string, string> AnnTrain::Identify(Mat input, int type)
{
	Feature fea;
	Mat feature = fea.CharFeatures(input, PredictSize);//�õ��ַ�����
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
			return make_pair(CharsAll[index], CharsAll[index]);//���Եõ�ANNģ��Ԥ���ַ�ֵ
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
			return make_pair(CharsNumber[index], CharsNumber[index]);//���Եõ�ANNģ��Ԥ���ַ�ֵ
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
			return make_pair(CharsLetter[index], CharsLetter[index]);//���Եõ�ANNģ��Ԥ���ַ�ֵ
		}
	}
	else
	{
		const char *unknown;
		return make_pair(unknown, unknown);
	}
}

bool AnnTrain::Test(int charstype) {
	assert(test_folder_);//�жϲ����ļ����Ƿ����
	Feature fea;
	Type = charstype;
	int ClassNumber = 0;
	if (Type == 0) ClassNumber = NumberAndLetter;//��ͬ�ַ�����Ӧ�������Ŀ��ͬ,NumberAndLetter=34
	if (Type == 1) ClassNumber = OnlyNumber;//OnlyNumber=10
	if (Type == 2) ClassNumber = OnlyLetter;//OnlyLetter=24
	int corrects_all = 0, sum_all = 0;//ʶ����ȷ���ַ���Ŀ���ܵ��ַ���Ŀ
	vector<float> rate_list;//�����ַ���ʶ����ȷ����ɵļ���
	for (int i = 0; i < ClassNumber; ++i) {
		const char *char_key = 0;;
		if (Type == 0)
		{
			char_key = CharsAll[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//ȡ���ַ�����������ĵ����ַ�
		}
		char sub_folder[512] = { 0 };
		sprintf(sub_folder, "%s/%s", test_folder_, char_key);
		auto chars_files = fea.GetFiles(sub_folder);//���ַ��ļ����л�������ַ�ͼƬ�ļ�
		int corrects = 0, sum = 0;
		vector<pair<string, string>> error_files;//ʶ������ַ���
		int error_count = 0;
		for (auto file : chars_files) 
		{//���������ַ��ļ�
			auto img = imread(file, 0);//�����ַ�ͼƬ�ļ�
			Mat imgNorm;
			imgNorm = fea.NormalizedImage(img, imgNorm);
			pair<string, string> ch;
			if (Type == 0) ch = Identify(imgNorm, 0);//ʶ������ַ�
			if (Type == 1) ch = Identify(imgNorm, 1);
			if (Type == 2) ch = Identify(imgNorm, 2);
			if (ch.first == char_key) {//����ַ�ʶ�������ַ���������ȡ�����ַ�һ�£���ʶ����ȷ
				++corrects;
				++corrects_all;
			}
			else {//����ʶ����󣬽������ַ��ŵ�error_files��
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
			myfile << "�ַ����" << ":" << char_key << ",  ";
			myfile << "�ַ�����" << ":" << sum << ",  ";
			myfile << "��ȷ��Ŀ" << ":" << corrects << ",  ";
			myfile << "��ȷ��" << ":" << rate*100 << "%"<<endl;
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
		myfile << "��������" << endl;
		myfile << "-------------------------------------" << endl;
		myfile << "�ַ���Ŀ" << ":" << sum_all<< ",  ";
		myfile << "��ȷ��Ŀ" << ":" << corrects_all << ",  ";
		myfile << "��ȷ��" << ":" << correct_rate <<"%"<< endl;
		myfile << "�����Ŀ" << ":" << ClassNumber << ", ";
		myfile << "ƽ��ʶ����" << ":" << rate_mean * 100 << "%"<<endl;
		myfile << "-------------------------------------" << endl;
		myfile.close();
	}
	return true;
}

Mat GetSyntheticImage(const Mat& image) {
	Feature fea;
	int rand_type = rand();//���������
	Mat result = image.clone();//��¡ͼƬ
	if (rand_type % 2 == 0) {//�������Ϊż��
		int ran_x = rand() % 5 - 2;
		int ran_y = rand() % 5 - 2;
		result = fea.TranslateImg(result, ran_x, ran_y);//��ת
	}
	else if (rand_type % 2 != 0) {//�������Ϊ����
		float angle = float(rand() % 15 - 7);
		result = fea.RotateImg(result, angle);//��ת
	}
	return result;
}

Ptr<ml::TrainData> AnnTrain::sdata(size_t number_for_count) {
	assert(chars_folder_);//�ж�ѵ���ļ����Ƿ����
	Mat samples;//ѵ����������
	vector<int> labels;//ѵ��������ǩ
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
			char_key = CharsAll[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//ȡ���ַ�����������ĵ����ַ�
		}
		char sub_folder[512] = { 0 };
		sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
		auto chars_files = fea.GetFiles(sub_folder);
		size_t char_size = chars_files.size();//ͳ���ַ�һ���ж�����ѵ��ͼƬ
		vector<Mat> matVec;
		matVec.reserve(number_for_count);//����matVec�Ĵ�С��С��number_for_count
		for (auto file : chars_files) {//��ѵ���ַ��ļ��ŵ�matVec��
			auto img = imread(file, 0);
			Mat imgNorm;
			imgNorm = fea.NormalizedImage(img, imgNorm);
			matVec.push_back(imgNorm);
		}

		for (int t = 0; t < (int)number_for_count - (int)char_size; t++) {
			int rand_range = char_size + t;
			int ran_num = rand() % rand_range;
			auto img = matVec.at(ran_num);
			auto simg = GetSyntheticImage(img);//�õ���ת����ת���ͼƬ
			matVec.push_back(simg);
		}

		for (auto img : matVec) 
		{
			auto fps = fea.CharFeatures(img, PredictSize);//�õ��ַ�����10X10+10+10=120��ά�ȣ�
			samples.push_back(fps);//�洢�ַ���������
			labels.push_back(i);//�洢�ַ�������ǩ
		}
	}
	Mat samples_;
	samples.convertTo(samples_, CV_32F);
	Mat train_classes =Mat::zeros((int)labels.size(), ClassNumber, CV_32F);
	for (int i = 0; i < train_classes.rows; ++i) 
	{
		train_classes.at<float>(i, labels[i]) = 1.f;
	}
	return ml::TrainData::create(samples_, ml::SampleTypes::ROW_SAMPLE,train_classes);//�õ�ANNѵ������
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
			char_key = CharsAll[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 1)
		{
			char_key = CharsNumber[i];//ȡ���ַ�����������ĵ����ַ�
		}
		if (Type == 2)
		{
			char_key = CharsLetter[i];//ȡ���ַ�����������ĵ����ַ�
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