#include<iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <math.h>
#pragma warning(disable:4996)
#pragma warning(disable:26451)
using namespace std;
#define sizeFileName 128
#define sizeSector 512

bool isVerify = false;
char VolumeName[128] = "\0";
int sizeVolume = 0;

bool verify();
void printMenu(int& choice);
int hashPass(char* pass);
bool setPassword(ofstream& out);
bool createFormatVolume();
bool infoVolume();
bool changePasswordVolume();
bool importFile();
bool exportFile();
bool changePasswordFile();
bool deleteFile();
ifstream::pos_type filesize(const char* filename);
int main() {
	int choice = -1;
	while (true) {
		system("cls");
		printMenu(choice);
		switch (choice) {
		case 0:
			return 0;
		case 1:
			verify();
			system("pause");
			break;
		case 2:
			createFormatVolume();
			system("pause");
			break;
		case 3:
			infoVolume();
			system("pause");
			break;
		case 4:
			changePasswordVolume();
			system("pause");
			break;
		case 5:
			importFile();
			system("pause");
			break;
		case 6:
			exportFile();
			system("pause");
			break;
		case 7:
			changePasswordFile();
			system("pause");
			break;
		case 8:
			deleteFile();
			system("pause");
			break;
		default:
			cout << "Wrong choice, please choose again!" << endl;
			system("pause");
		}
	}
	return 0;
}
void printMenu(int& choice) {
	cout << "============= FILE SYSTEM MENU ============= ";
	if (!isVerify)
		cout << "|Not Verified, you must create or verify volume to fully access|" << endl;
	else
		cout << "(Verified with " << VolumeName << ")" << endl;
	cout << "[1]. Verify your volume to access (if exist)" << endl;
	cout << "[2]. Create and Format new volume" << endl;
	cout << "[3]. File listing and display information volume" << endl;
	cout << "[4]. Change password of Volume" << endl;
	cout << "[5]. Import new file to volume" << endl;
	cout << "[6]. Export file from volume" << endl;
	cout << "[7]. Change password of file" << endl;
	cout << "[8]. Delete file in volume" << endl;
	cout << "[0]. Quit" << endl;
	cout << "============================================ " << endl;
	cout << "==> Choose your option: ";
	cin >> choice;
	cin.ignore(1);
}
bool createFormatVolume() {
	int sizeInMegabyte = 0;
	cout << "+ Name for volume (with extensions): ";
	cin.getline(VolumeName, sizeFileName);
	cout << "+ Enter size of volume (in MB): ";
	cin >> sizeInMegabyte;
	cin.ignore(1);
	if (sizeInMegabyte <= 0) {
		cout << "\t==> Size is too small to create" << endl;	return false;
	}
	int numOfFiles = 0;
	sizeVolume = 1048576 * sizeInMegabyte;
	ofstream out;
	out.open(VolumeName, ios::out | ios::binary);
	if (!out) {
		cout << "\t==> Error when create Volume" << VolumeName << endl;
		return false;
	}
	isVerify = true;
	out.write(VolumeName, sizeFileName);
	setPassword(out);
	out.write((char*)&numOfFiles, 4);
	out.seekp(sizeVolume - 1, ios::beg);
	int nul = 0x0;
	out.write((char*)&nul, 1);
	out.close();
	cout << "\t==> " << VolumeName << " was created and formated successfully" << endl;
	return true;
}
int hashPass(char* passWord) {
	int hashValue = 0;
	for (int i = 0; i < strlen(passWord); i++) {
		hashValue += (int)passWord[i] * (i + 1);
	}
	return hashValue;
}
bool setPassword(ofstream& out) {
	if (!isVerify) {
		cout << "\t==> Volume not created yet, please create and format volume!" << endl;
		return false;
	}
	char* passWord = new char[32];
	cout << "+ Input password (enter if not set): ";
	cin.getline(passWord, 32);
	int hashValue = hashPass(passWord);
	out.write((char*)&hashValue, sizeof(int));
	return true;
}
std::ifstream::pos_type filesize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}
bool verify() {
	char* name = new char[128];
	char* pass = new char[32];
	cout << "+ Enter your volume name: ";
	cin.getline(name, sizeFileName);
	ifstream in;
	in.open(name, ios::in | ios::binary);
	if (!in) {
		cout <<"\t==> "<< name << " is not exists" << endl;
		return false;
	}

	cout << "+ Enter your password's volume (enter if not set): ";
	cin.getline(pass, 32);
	int hashValue = hashPass(pass);
	in.seekg(sizeFileName, ios::beg);
	char* passReal = new char[4];
	in.read(passReal, 4);
	if (hashValue == *(int*)passReal) {
		strcpy(VolumeName, name);
		isVerify = true;
		sizeVolume = filesize(VolumeName);
		cout << "\t==> Verified, now you can fully access with " << VolumeName << endl;
	}
	else {
		cout << "\t==> Wrong password" << endl;
	}
	return true;
}
bool infoVolume()
{
	if (!isVerify) {
		cout << "Volume is not created or verified!" << endl;
		return false;
	}
	cout << "Volume name: " << VolumeName << endl;
	cout << "Size: " << sizeVolume / 1024 / 1024 << " MB (" << sizeVolume << " bytes)" << endl;
	ifstream in;
	in.open(VolumeName, ios::in | ios::binary);
	in.seekg(132, ios::beg);
	char* data = new char[4];
	in.read(data, 4);
	int numOfFile = *(int*)data;
	cout << "Number of file(s): " << numOfFile << endl;
	for (int i = 0; i < numOfFile; i++) {
		int number = 0;
		char* name = new char[128];
		in.read(name, 128);
		cout << "\t File name: " << name << endl;
		in.read(data, 4);
		in.read(data, 4);
		number = *(int*)data;
		cout << "\t Size: " << number << " bytes" << endl;
		in.read(data, 4);
		number = *(int*)data;
		cout << "\t Started sector: " << number << endl;
		in.read(data, 4);
		number = *(int*)data;
		cout << "\t Sector allocated: " << number << endl;
		cout << endl;
	}
	return true;
}
bool changePasswordVolume() {
	if (!isVerify) {
		cout << "\t==> Volume is not created or verified!" << endl;
		return false;
	}
	char* oldPass = new char[32];
	char* newPass = new char[32];
	char* repeatNewPass = new char[32];
	cout << "+ Input old password: ";
	cin.getline(oldPass, 32);
	int OldPValue = hashPass(oldPass);
	ifstream in;
	in.open(VolumeName, ios::in | ios::binary);
	in.seekg(128, ios::beg);
	char* data = new char[4];
	in.read(data, 4);
	if (OldPValue != *(int*)data) {
		cout << "\t==> Wrong password!" << endl;
		in.close();
		return false;
	}
	in.close();
	cout << "+ Enter new password: ";
	cin.getline(newPass, 32);
	cout << "+ Re-Enter password: ";
	cin.getline(repeatNewPass, 32);
	if (strcmp(newPass, repeatNewPass) != 0) {
		cout << "\t==> No match" << endl;
		return false;
	}
	int newPassVal = hashPass(newPass);
	ofstream out;
	out.open(VolumeName, ios::binary | ios::in | ios::out);
	out.seekp(128, ios::beg);
	out.write((char*)&newPassVal, sizeof(int));
	cout << "\t==> Password has changed" << endl;
	return true;
}
bool importFile() {
	if (!isVerify) {
		cout << "\t==> Volume is not created or verified!" << endl;
		return false;
	}
	char* fileImport = new char[256];
	cout << "+ Enter file name (with path Ex:\"D:\\folder\\filename.pdf\"): ";
	cin.getline(fileImport, 256);
	string tempStr(fileImport);
	tempStr = tempStr.substr(tempStr.find_last_of('\\') + 1);
	const char* fileName = tempStr.c_str();

	ifstream in;
	in.open(fileImport, ios::in | ios::binary);
	if (!in) {
		cout << "\t==> "<<fileImport << " does not exist" << endl;
		return false;
	}
	int size = filesize(fileImport);
	char* dataFile = new char[size];
	in.read(dataFile, size);
	in.close();

	in.open(VolumeName, ios::in | ios::binary);
	in.seekg(sizeFileName + 4);
	char* num = new char[4];
	in.read(num, 4);
	int numOfFiles = *(int*)num;
	for (int i = 0; i < numOfFiles; i++) {
		char* name = new char[128];
		in.read(name, 128);
		if (_stricmp(name, fileName) == 0) {
			cout << "\t==> File existed" << endl;
			return false;
		}
		in.read(name, 16);
	}
	int lastBlock = 0;
	int numOfBlock = 0;
	if (numOfFiles == 0) {
		lastBlock = 10;
		numOfBlock = 0;
	}
	else {
		int step = 136 + numOfFiles * 144 - 8;
		in.seekg(step, ios::beg);
		in.read(num, 4);
		lastBlock = *(int*)num;
		in.read(num, 4);
		numOfBlock = *(int*)num;
	}
	in.close();
	int startNewBlock = lastBlock + numOfBlock;
	int numOfNewBlock = ceil((double)size / sizeSector);
	if (sizeSector * (startNewBlock + numOfNewBlock) > sizeVolume) {
		cout << "\t==> File's size is more than Volume's size" << endl;
		return false;
	}
	ofstream out;
	out.open(VolumeName, ios::binary | ios::in | ios::out);
	out.seekp(sizeFileName + 4, ios::beg);
	numOfFiles++;
	out.write((char*)&numOfFiles, 4);
	int step = 136 + (numOfFiles - 1) * 144;
	out.seekp(step, ios::beg);
	out.write(fileName, 128);
	setPassword(out);
	out.write((char*)&size, 4);
	out.write((char*)&startNewBlock, 4);
	out.write((char*)&numOfNewBlock, 4);
	out.seekp(sizeSector * startNewBlock, ios::beg);
	out.write(dataFile, size);
	out.seekp(sizeSector * (startNewBlock + numOfNewBlock) - 1, ios::beg);
	int nul = 0x0;
	out.write((char*)&nul, 1);
	out.close();
	cout << "\t==> Import file " << fileName << " successfully" << endl;
	return true;
}
bool exportFile() {
	if (!isVerify) {
		cout << "\t==> Volume is not created or verified!" << endl;
		return false;
	}
	ifstream in;
	in.open(VolumeName, ios::in | ios::binary);
	char* data = new char[128];
	char* num = new char[4];
	in.seekg(132, ios::beg);
	in.read(num, 4);
	int numOfFile = *(int*)num;
	bool check = false;
	int size = 0;
	int blockStart = 0;
	int numOfBlock = 0;

	char* fileName = new char[128];
	cout << "+ Enter file name: ";
	cin.getline(fileName, 128);
	for (int i = 0; i < numOfFile; i++) {
		in.read(data, 128);
		if (_stricmp(data, fileName) == 0) {
			char* pass = new char[32];
			cout << "+ Enter password: ";
			cin.getline(pass, 32);
			in.read(num, 4);
			int passVal = *(int*)num;
			if (hashPass(pass) != passVal) {
				cout << "\t==> Wrong password" << endl;
				return false;
			}
			in.read(num, 4);
			size = *(int*)num;
			in.read(num, 4);
			blockStart = *(int*)num;
			in.read(num, 4);
			numOfBlock = *(int*)num;
			check = true;
			break;
		}
		else {
			in.read(data, 16);
		}
	}
	if (check == false) {
		cout <<"\t==> "<< fileName << " is not exist" << endl;
		return false;
	}
	in.seekg(blockStart * sizeSector, ios::beg);
	char* dataFile = new char[size];
	in.read(dataFile, size);
	in.close();

	ofstream out;
	out.open(fileName, ios::out | ios::binary);
	out.write(dataFile, size);
	out.close();
	cout <<"\t==> " <<fileName << " was exported successfully" << endl;
	return true;
}
bool changePasswordFile()
{
	if (!isVerify) {
		cout << "\t==> Volume is not created or verified!" << endl;
		return false;
	}
	int step = 0;
	char* nameFile = new char[128];
	cout << "+ Enter file name to be changed password: ";
	cin.getline(nameFile, 128);
	char* oldPass = new char[32];
	char* newPass = new char[32];
	char* repeatNewPass = new char[32];
	cout << "+ Input old password: ";
	cin.getline(oldPass, 32);
	int OldPValue = hashPass(oldPass);
	ifstream in;
	in.open(VolumeName, ios::in | ios::binary);
	in.seekg(132, ios::beg);
	step += 132;
	char* numFi = new char[4];
	in.read(numFi, 4);
	step += 4;
	int numOfFile = *(int*)numFi;
	bool flag = false;
	char* file = new char[128];
	char* temp = new char[12];
	for (int i = 0; i < numOfFile; i++) {
		in.read(file, 128);
		step += 128;
		in.read(numFi, 4);
		step += 4;
		if (_stricmp(file, nameFile) == 0) {
			if (OldPValue != *(int*)numFi) {
				cout << "\t==> Wrong password" << endl;
				in.close();
				return false;
			}
			flag = true;
			break;
		}
		in.read(temp, 12);
		step += 12;
	}
	if (flag == false) {
		cout << "\t==> " <<nameFile << " does not exits" << endl;
		return false;
	}
	in.close();
	cout << "+ Enter new password: ";
	cin.getline(newPass, 32);
	cout << "+ ReEnter password: ";
	cin.getline(repeatNewPass, 32);
	if (strcmp(newPass, repeatNewPass) != 0) {
		cout << "\t==> No match" << endl;
		return false;
	}
	ofstream out;
	int newPassVal = hashPass(newPass);
	out.open(VolumeName, ios::binary | ios::in | ios::out);
	out.seekp(step - 4, ios::beg);
	out.write((char*)&newPassVal, sizeof(int));
	cout << "\t==> Password has changed" << endl;
	return true;
}

bool deleteFile()
{
	if (!isVerify) {
		cout << "\t==> Volume is not created or verified!" << endl;
		return false;
	}
	char* fileNameDelete = new char[128];
	cout << "+ Enter file name to be deleted: ";
	cin.getline(fileNameDelete, 128);
	fstream handle;
	handle.open(VolumeName, ios::binary | ios::in | ios::out);
	handle.seekg(132);
	char* num = new char[4];
	handle.read(num, 4);
	int numFiles = *(int*)num;
	char* nameFile = new char[128];
	int seekgPosition = 136;
	int blockFileDelete = 0;
	int numBlockDelete = 0;
	bool flag = false;
	for (int i = 1; i <= numFiles; i++) {
		handle.read(nameFile, 128);
		if (_stricmp(nameFile, fileNameDelete) == 0) {
			flag = true;
			char* pass = new char[32];
			cout << "+ Enter password of file: ";
			cin.getline(pass, 32);
			handle.read(num, 4);
			if (hashPass(pass) != *(int*)num) {
				cout << "\t==> Wrong password" << endl;
				return false;
			}
			handle.read(nameFile, 4); //ignore 8 next byte;
			handle.read(num, 4);
			blockFileDelete = *(int*)num;
			handle.read(num, 4);
			numBlockDelete = *(int*)num;
			numFiles--;
			handle.seekp(132, ios::beg);
			handle.write((char*)&numFiles, 4);
			int sizeOverwrite = 10 * 512 - 136 - 144 * i;
			char* overrideData = new char[sizeOverwrite];
			handle.seekg(seekgPosition + 144, ios::beg);
			handle.read(overrideData, sizeOverwrite);
			handle.seekp(seekgPosition, ios::beg);
			handle.write(overrideData, sizeOverwrite);
			if (i == numFiles + 1) {
				cout << "\t==> Delete " << fileNameDelete << " successfully" << endl;
				break;
			}
			for (int j = 0; j < numFiles + 1 - i; j++){
				seekgPosition += 136;
				handle.seekg(seekgPosition, ios::beg);
				handle.read(num, 4);
				int temp = *(int*)num - numBlockDelete;
				handle.seekp(seekgPosition, ios::beg);
				handle.write((char*)&temp, 4);
				seekgPosition += 8;

				handle.read(num, 4);
				int temp2 = *(int*)num;
				
				handle.seekg((temp + numBlockDelete) * sizeSector, ios::beg);
				char* dataMove = new char[sizeSector * temp2];
				handle.read(dataMove, sizeSector * temp2);
				handle.seekp(temp * sizeSector, ios::beg);
				handle.write(dataMove, sizeSector * temp2);
			}
			cout << "\t==> Delete " << fileNameDelete << " successfully" << endl;
			break;
		}
		else {
			seekgPosition += 144;
			handle.read(nameFile, 16);
		}
	}
	if (flag == false) {
		cout <<"\t==> "<< fileNameDelete << " does not exist" << endl;
	}
	return true;
}
