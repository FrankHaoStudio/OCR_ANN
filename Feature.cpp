#include "stdafx.h"
#include "Feature.h"

Feature::Feature()
{
}

Feature::~Feature()
{
}

Mat Feature::CharFeatures(Mat in, int sizeData) {
	const int VERTICAL = 0;
	const int HORIZONTAL = 1;
	Rect _rect = GetCenterRect(in);
	Mat tmpIn = CutTheRect(in, _rect);
	Mat lowData;
	resize(tmpIn, lowData, Size(sizeData, sizeData));
	Mat vhist = ProjectedHistogram(lowData, VERTICAL);
	Mat hhist = ProjectedHistogram(lowData, HORIZONTAL);
	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;
	Mat out = Mat::zeros(1, numCols, CV_32F);
	int j = 0;
	for (int i = 0; i < vhist.cols; i++) {
		out.at<float>(j) = vhist.at<float>(i);
		j++;
	}
	for (int i = 0; i < hhist.cols; i++) {
		out.at<float>(j) = hhist.at<float>(i);
		j++;
	}
	for (int x = 0; x < lowData.cols; x++) {
		for (int y = 0; y < lowData.rows; y++) {
			out.at<float>(j) += (float)lowData.at <unsigned char>(x, y);
			j++;
		}
	}
	return out;
}

Mat Feature::CutTheRect(Mat &in, Rect &rect) {
	int size = in.cols;  
	Mat dstMat(size, size, CV_8UC1);
	dstMat.setTo(Scalar(0, 0, 0));
	int x = (int)floor((float)(size - rect.width) / 2.0f);
	int y = (int)floor((float)(size - rect.height) / 2.0f);
	for (int i = 0; i < rect.height; ++i) {

		for (int j = 0; j < rect.width; ++j) {
			dstMat.data[dstMat.step[0] * (i + y) + j + x] =
				in.data[in.step[0] * (i + rect.y) + j + rect.x];
		}
	}
	return dstMat;
}

Rect Feature::GetCenterRect(Mat &in) {
	Rect _rect;
	int top = 0;
	int bottom = in.rows - 1;
	for (int i = 0; i < in.rows; ++i) {
		bool bFind = false;
		for (int j = 0; j < in.cols; ++j) {
			if (in.data[i * in.step[0] + j] > 20) {
				top = i;
				bFind = true;
				break;
			}
		}
		if (bFind) {
			break;
		}
	}
	for (int i = in.rows - 1;
		i >= 0;
		--i) {
		bool bFind = false;
		for (int j = 0; j < in.cols; ++j) {
			if (in.data[i * in.step[0] + j] > 20) {
				bottom = i;
				bFind = true;
				break;
			}
		}
		if (bFind) {
			break;
		}
	}
	int left = 0;
	int right = in.cols - 1;
	for (int j = 0; j < in.cols; ++j) {
		bool bFind = false;
		for (int i = 0; i < in.rows; ++i) {
			if (in.data[i * in.step[0] + j] > 20) {
				left = j;
				bFind = true;
				break;
			}
		}
		if (bFind) {
			break;
		}
	}
	for (int j = in.cols - 1;
		j >= 0;
		--j) {
		bool bFind = false;
		for (int i = 0; i < in.rows; ++i) {
			if (in.data[i * in.step[0] + j] > 20) {
				right = j;
				bFind = true;

				break;
			}
		}
		if (bFind) {
			break;
		}
	}
	_rect.x = left;
	_rect.y = top;
	_rect.width = right - left + 1;
	_rect.height = bottom - top + 1;
	return _rect;
}

float Feature::countOfBigValue(Mat &mat, int iValue) {
	float iCount = 0.0;
	if (mat.rows > 1) {
		for (int i = 0; i < mat.rows; ++i) {
			if (mat.data[i * mat.step[0]] > iValue) {
				iCount += 1.0;
			}
		}
		return iCount;
	}
	else {
		for (int i = 0; i < mat.cols; ++i) {
			if (mat.data[i] > iValue) {
				iCount += 1.0;
			}
		}

		return iCount;
	}
}

Mat Feature::ProjectedHistogram(Mat img, int t) {
	int sz = (t) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F);

	for (int j = 0; j < sz; j++) {
		Mat data = (t) ? img.row(j) : img.col(j);

		mhist.at<float>(j) = countOfBigValue(data, 20);//???
	}
	double min, max;
	minMaxLoc(mhist, &min, &max);

	if (max > 0)
		mhist.convertTo(mhist, -1, 1.0f / max, 0);
	return mhist;
}
vector<std::string> Feature::GetFiles(const std::string &folder,const bool all)
{
	vector<string> files;
	list<string> subfolders;
	subfolders.push_back(folder);
	while (!subfolders.empty()) {
		std::string current_folder(subfolders.back());

		if (*(current_folder.end() - 1) != '/') {
			current_folder.append("/*");
		}
		else {
			current_folder.append("*");
		}

		subfolders.pop_back();

		struct _finddata_t file_info;
		auto file_handler = _findfirst(current_folder.c_str(), &file_info);

		while (file_handler != -1) {
			if (all &&
				(!strcmp(file_info.name, ".") || !strcmp(file_info.name, ".."))) {
				if (_findnext(file_handler, &file_info) != 0) break;
				continue;
			}

			if (file_info.attrib & _A_SUBDIR) {
				if (all) {
					std::string folder(current_folder);
					folder.pop_back();
					folder.append(file_info.name);

					subfolders.push_back(folder.c_str());
				}
			}
			else {
				std::string file_path;
				file_path.assign(current_folder.c_str()).pop_back();
				file_path.append(file_info.name);

				files.push_back(file_path);
			}

			if (_findnext(file_handler, &file_info) != 0) break;
		}
		_findclose(file_handler);
	}
	return files;
}

string Feature::GetFileName(const std::string &path,const bool postfix) {
	if (!path.empty()) {
		size_t last_slash = Get_last_slash(path);
		size_t last_dot = path.find_last_of('.');
		if (last_dot < last_slash || last_dot == std::string::npos) {
			return path.substr(last_slash + 1);
		}
		else {
			if (postfix) {
				return path.substr(last_slash + 1);
			}
			return path.substr(last_slash + 1, last_dot - last_slash - 1);
		}
	}
	return "";
}

size_t Feature::Get_last_slash(const std::string &path) {
	size_t last_slash_1 = path.find_last_of("\\");
	size_t last_slash_2 = path.find_last_of("/");
	size_t last_slash;

	if (last_slash_1 != std::string::npos && last_slash_2 != std::string::npos) {
		last_slash = std::max(last_slash_1, last_slash_2);
	}
	else {
		last_slash =
			(last_slash_1 == std::string::npos) ? last_slash_2 : last_slash_1;
	}
	return last_slash;
}

Mat Feature::TranslateImg(Mat img, int offsetx, int offsety) {
	Mat dst;
	Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img, dst, trans_mat, img.size());
	return dst;
}

Mat Feature::RotateImg(Mat source, float angle) {
	Point2f src_center(source.cols / 2.0F, source.rows / 2.0F);
	Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
	Mat dst;
	warpAffine(source, dst, rot_mat, source.size());
	return dst;
}

Mat Feature::NormalizedImage(Mat src, Mat dst)
{
	resize(src, dst, Size(20, 20));
	return dst;
}
CString Feature::IntToCString(int sourcedata)
{
	CString destdata;
	destdata.Format(_T("%d"), sourcedata);
	return destdata;
}
int Feature::CStringToInt(CString sourcedata)
{
	int destdata;
	destdata = _ttoi(sourcedata);
	return destdata;
}
char* Feature::CStringToCharpoint(CString sourcedata)
{
	char *destdata;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, sourcedata, -1, NULL, NULL, 0, NULL);
	destdata = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, sourcedata, -1, destdata, dwNum, 0, NULL);;
	return destdata;
}