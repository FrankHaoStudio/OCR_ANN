#pragma once

	//ConfigInformation Value
	static const int CharacterInput = 120;
	static const int CharacterSize = 10;
	static const int OnlyNumber = 10;
	static const int OnlyLetter = 24;
	static const int AnnInput = CharacterInput;
	static const int PredictSize = CharacterSize;
	static const int NumberAndLetter = OnlyNumber + OnlyLetter;

	static const char *CharsAll[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
	static const char *CharsNumber[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	static const char *CharsLetter[] = { "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

	static CString TrainDataFolder = _T("Data/AnnTrain");
	static CString TestDataFolder = _T("Data/AnnTest");
	static CString ErrorDataFolder = _T("Data/AnnError");
	static CString ModelDataFolder = _T("Model/ann.xml");
	static int Progress;
	static int HidenLayerNumber = 0;
	static int NeuronsNumber = 40;
	static int CharsType = 0;
	//ConfigInformation Value

	//ConfigInformation Function
	static bool SetANNTrainDataFolder(CString path)
	{
		TrainDataFolder = path;
		return true;
	}

	static bool SetANNTestDataFolder(CString path)
	{
		TestDataFolder = path;
		return true;
	}

	static bool SetANNErrorDataFolder(CString path)
	{
		ErrorDataFolder = path;
		return true;
	}

	static bool SetANNModelDataFolder(CString path)
	{
		ModelDataFolder = path;
		return true;
	}
	static bool SetHidenLayerNumber(int number)
	{
		HidenLayerNumber = number;
		return true;
	}

	static bool SetNeuronsNumber(int number)
	{
		NeuronsNumber = number;
		return true;
	}

	static bool SetCharsType(int type)
	{
		CharsType = type;
		return true;
	}

	static bool IsSaveConfig(bool a, bool b, bool c, bool d, bool e, bool f, bool g)
	{
		if ((a&&b&&c&&d&&e&&f&&g) == true)
		{
			return true;
		}
		return false;
	}
	static void SaveConfig(CString key, CString value)
	{
		CString ConfigPath = _T(".//Config.ini");
		CString ConfigInfo = _T("ConfigInformation");
		WritePrivateProfileString(ConfigInfo, key, value, ConfigPath);
	}
	static CString LoadConfig(CString key, CString value, CString devalue)
	{
		CString ConfigPath = _T(".//Config.ini");
		CString ConfigInfo = _T("ConfigInformation");
		GetPrivateProfileString(ConfigInfo, key, devalue, value.GetBuffer(MAX_PATH), MAX_PATH, ConfigPath);
		return value;
	}
	//ConfigInformation Function
	