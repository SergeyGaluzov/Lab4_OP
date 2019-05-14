#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
class WAVE_file;
typedef struct {
	char chunkId[4];   // Завжди містить значення 0x52494646 (літери "RIFF")
	int32_t chunkSize; // 36 + розмір другого підрозділу в байтах
					   // Іншими словами 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
					   // Це розмір всього файла мінус 8 байтів які займають поля chunkId та chunkSize
	char format[4];    // Для wav-файла це завжди 0x57415645 (літери "WAVE")
	void Set_chunkId(vector <unsigned char> &text)
	{
		for (int i = 0; i < sizeof(chunkId); i++)
		{
			chunkId[i] = text[i];
		}
	}
	void Set_chunkSize(vector <unsigned char> &text)
	{

		int32_t temp = 0;
		for (int i = 7; i >= 4; i--)
		{
			temp = (temp * 256) + text[i];
		}
		chunkSize = temp;
	}
	void Set_format(vector <unsigned char> &text)
	{
		for (int i = 8; i < 8 + sizeof(chunkId); i++)
		{
			format[i-8] = text[i];
		}
	}

} RIFFHEADER;
typedef struct {
	char subchunk1Id[4];   // Завжди 0x666d7420 – літери "fmt "
	int32_t subchunk1Size; // Завжди 16 для аудіо PCM. Це розмір частини підрозділу, що слідує після цього числа
	int32_t audioFormat;   // PCM = 1
	int16_t numChannels;   // Mono = 1, Stereo = 2
	int32_t sampleRate;    // Наприклад 44100
	int32_t byteRate;      // == SampleRate * NumChannels * BitsPerSample/8a
	int32_t blockAlign;    // == NumChannels * BitsPerSample/8
	int16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.
	void Set_subchunk1Id(vector <unsigned char> &text)
	{
		for (int i = 12; i < 12 + sizeof(subchunk1Id); i++)
		{
			subchunk1Id[i-12] = text[i];
		}
	}
} SUBCHUNK1;
typedef struct {
	int32_t subchunk2Id;   // 0x64617461 – літери "data"
	int32_t subchunk2Size; // == NumSamples * NumChannels * BitsPerSample/8, кількість байтів аудіоданих
	int8_t *data;         // семпли
} SUBCHUNK2;
class WAVE_file
{
public:
	RIFFHEADER header;
	SUBCHUNK1 part1;
	SUBCHUNK2 part2;
	WAVE_file(vector <unsigned char> &text)
	{
		header.Set_chunkId(text);
		header.Set_chunkSize(text);
		header.Set_format(text);
		part1.Set_subchunk1Id(text);
		part1.Set_audioFormat(text);
		part1.Set_bitsPerSample(text);
		part1.Set_blockAlign(text);
		part1.Set_byteRate(text);
		part1.Set_numChannels(text);
		part1.Set_sampleRate(text);
		part1.Set_subchunk1Size(text);
		part2.Set_data(text);
		part2.Set_subchunk2Id(text);
		part2.Set_subchunk2Size(text);
		*/
	}
};

int main()
{
	setlocale(LC_ALL, "rus");
	ifstream input;
	ofstream output;
	input.open("Input.wav", ios::binary);
	output.open("Ouput1put.wav", ios::binary);
	char s;
	vector <unsigned char> text;
	while (input.get(s))
	{
		text.push_back(s);
	}
	WAVE_file a(text);
	string buff;
	int32_t temp = 0, temp2 = 0;
	bool flag = false;
	for (int i = 43; i >= 40; i--)
	{
		temp = (temp * 256) + text[i];
	}
	temp *= 3;
	int temp1 = temp + 36;
	int k = temp;
	string n, n1;
	while (temp > 0)
	{
		n.insert(n.end(), (temp % 2 == 0) ? '0':'1');
		temp /= 2;
	}
	while (temp1 > 0)
	{
		n1.insert(n1.end(), (temp1 % 2 == 0) ? '0' : '1');
		temp1 /= 2;
	}
	string m, m1;
	for (int i = n.size() - 1; i >= 0; i--)
	{
		m += n[i];
	}
	for (int i = n1.size() - 1; i >= 0; i--)
	{
		m1 += n1[i];
	}
	while (m.size() < 32)
	{
		m.insert(m.begin(), '0');
	}
	while (m1.size() < 32)
	{
		m1.insert(m1.begin(), '0');
	}
	unsigned int k1 = stoul(m.substr(24, 8), nullptr, 2);
	unsigned int k2 = stoul(m.substr(16, 8), nullptr, 2);
	unsigned int k3 = stoul(m.substr(8, 8), nullptr, 2);
	unsigned int k4 = stoul(m.substr(0, 8), nullptr, 2);
	text[40] = k1;
	text[41] = k2;
	text[42] = k3;
	text[43] = k4;
	
	k1 = stoul(m1.substr(24, 8), nullptr, 2);
	k2 = stoul(m1.substr(16, 8), nullptr, 2);
	k3 = stoul(m1.substr(8, 8), nullptr, 2);
	k4 = stoul(m1.substr(0, 8), nullptr, 2);
	text[4] = k1;
	text[5] = k2;
	text[6] = k3;
	text[7] = k4;
	int i = 0;
	for (int i = 0; i < 44; i++)
	{
		output << text[i];
	}
	for (int i = 45; i < text.size(); i++)
	{
		output << text[i-1];
		output << text[i];
		output << text[i - 1];
		output << text[i];
		output << text[i - 1];
		output << text[i];
		i++;
	}
	input.close();
	output.close();
}