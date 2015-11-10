#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <mmsystem.h>
#include <string>
#include "bass.h"
#include <openssl\aes.h>
#include <openssl\rand.h>
#include <openssl\evp.h>
#include <cstring>
#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
using namespace std;
string password;
string odczytZPliku(char *location)
{
	char *buffer = NULL;
	long length;

	FILE *file = fopen(location, "rb");
	if (!file == NULL)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = (char *)malloc(length * sizeof(char));
		if (buffer)
		{
			fread(buffer, 1, length, file);
		}
		fclose(file);
		return string(buffer, length);
	}
	else
	{
		return "";
	}
}

string strinkeystore(char *sciezkadokeystora, char *id,string co)
{
	string asd = "\\";
	string pelnasciezkadokeya = sciezkadokeystora + asd +co +string(id) + ".txt";
	return pelnasciezkadokeya;
}
void odtwarzacz()
{
	BASS_Init(-1, 44100, 0, 0, NULL);
	BASS_SetVolume(1);
	HSAMPLE sample = BASS_SampleLoad(false, "C:\\Users\\Dominik\\Desktop\\pooutput.mp3", 0, 0, 1, BASS_SAMPLE_MONO);
	HCHANNEL channel = BASS_SampleGetChannel(sample, FALSE);
	BASS_ChannelPlay(channel, FALSE);
	system("pause");
}
void encrypt(char *sciezkadokeystora, char *id, char *sciezkadopliku)
{
	//tekst do zakodowania//*******************************************************************************************************
	string text = odczytZPliku(sciezkadopliku);
	unsigned char *zakoduj = (unsigned char*)text.c_str();
	//klucz//*******************************************************************************************************
	string pelnasciezkadokeya = strinkeystore(sciezkadokeystora, id, "");
	char *sciezkadokeya = (char*)pelnasciezkadokeya.c_str();
	string key = odczytZPliku(sciezkadokeya);
	unsigned char *klucz = (unsigned char*)key.c_str();
	//has³o i tablica na wyjscie//*******************************************************************************************************
	int outlen1, outlen2;
	unsigned char *zakodowane = new unsigned char[(text.length() + 1) * 3];
	unsigned char *wyjsciehasloklucz = new unsigned char[(password.length() + 1) * 2];
	unsigned char *pass = (unsigned char*)password.c_str();
	//suma keya i passworda//*******************************************************************************************************
	ofstream sumkeyfile;
	string pelnasciezkadosumkeya = strinkeystore(sciezkadokeystora, id, "suma");
	sumkeyfile.open(pelnasciezkadosumkeya);
	sumkeyfile << password;
	//szyfrowanie klucza//**********************************************************************************************
	EVP_CIPHER_CTX ctx;
	//EVP_EncryptInit(&ctx, EVP_aes_128_cbc(), klucz, (unsigned char*)pass);
	//EVP_EncryptUpdate(&ctx, zakodowane, &outlen1, zakoduj, (text.length() + 1));
	//EVP_EncryptFinal(&ctx, zakodowane + outlen1, &outlen2);

	//szyfrowanie tekstu//*******************************************************************************************************
	EVP_EncryptInit(&ctx, EVP_aes_128_cbc(), klucz, (unsigned char*)pass);
	EVP_EncryptUpdate(&ctx, zakodowane, &outlen1, zakoduj, (text.length() + 1));
	EVP_EncryptFinal(&ctx, zakodowane + outlen1, &outlen2);
	//zapis wyniku do pliku//*******************************************************************************************************
	string to_file = string((char*)zakodowane, outlen1);
	std::ofstream output1("output" + string(sciezkadopliku).substr(string(sciezkadopliku).find_last_of('.')), std::ios::trunc | std::ios::binary);
	output1 << text << endl;
	output1.close();

}
void decrypt(char *sciezkadokeystora, char *id, char *sciezkadopliku)
{
	//tekst do zakodowania//*******************************************************************************************************
	string text = odczytZPliku(sciezkadopliku);
	unsigned char *zakoduj = (unsigned char*)text.c_str();
	//klucz//*******************************************************************************************************
	string pelnasciezkadokeya = strinkeystore(sciezkadokeystora, id, "");
	char *sciezkadokeya = (char*)pelnasciezkadokeya.c_str();
	string key = odczytZPliku(sciezkadokeya);
	unsigned char *klucz = (unsigned char*)key.c_str();
	//has³o i tablica na wyjscie//*******************************************************************************************************
	int outlen1, outlen2;
	unsigned char *zakodowane = new unsigned char[(text.length() + 1) * 2];
	unsigned char *pass = (unsigned char*)password.c_str();
	//suma keya i passworda//*******************************************************************************************************
	ifstream sumkeyfile;
	string pelnasciezkadosumkeya = strinkeystore(sciezkadokeystora, id, "suma");
	sumkeyfile.open(pelnasciezkadosumkeya);
	//sprawdzenie czy haslo sie zgadza//***********************************************************************************
	string haslozpliku = "";
	std::for_each(std::istreambuf_iterator<char>(sumkeyfile), std::istreambuf_iterator<char>(), [&](char in)
	{
		haslozpliku += in;
	});
	if (password != haslozpliku)
	{
		cout << "podane bledne haslo odszyfrowanie nie udalo sie";
	}
	else
	{
		//odszyfrowanie//*******************************************************************************************************
		EVP_CIPHER_CTX ctx;
		EVP_DecryptInit(&ctx, EVP_aes_128_cbc(), klucz, (unsigned char*)pass);
		EVP_DecryptUpdate(&ctx, zakodowane, &outlen1, zakoduj, (text.length() + 1));
		EVP_DecryptFinal(&ctx, zakodowane + outlen1, &outlen2);
		//zapis wyniku do pliku//*******************************************************************************************************
		string to_file = string((char*)zakodowane, outlen1);
		std::ofstream output1("pooutput" + string(sciezkadopliku).substr(string(sciezkadopliku).find_last_of('.')), std::ios::trunc | std::ios::binary);
		output1 << to_file << endl;
		output1.close();
		odtwarzacz();
		remove("pooutput.wav");
	}
}
void main(int argc, char *argv[])
{
	//1.Typ 2.Tryb 3.Sciezka dokeystora 4.Id 5.Sciezka do pliku 6.Co zrobic e lub d
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
	getline(cin, password);

	if (argc < 6)
	{
		printf("Za ma³o danych");
	}
	else
	{
		
		if (argv[6][0] == 'e')
		{
			encrypt(argv[3],argv[4],argv[5]);
		}
		else if (argv[6][0] == 'd')
		{
			decrypt(argv[3], argv[4], argv[5]);
		}
	}
	system("pause");
}