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
	int16_t audioFormat;   // PCM = 1
	int16_t numChannels;   // Mono = 1, Stereo = 2
	int32_t sampleRate;    // Наприклад 44100
	int32_t byteRate;      // == SampleRate * NumChannels * BitsPerSample/8a
	int16_t blockAlign;    // == NumChannels * BitsPerSample/8
	int16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.
	void Set_subchunk1Id(vector <unsigned char> &text)
	{
		for (int i = 12; i < 12 + sizeof(subchunk1Id); i++)
		{
			subchunk1Id[i-12] = text[i];
		}
	}
	void Set_subchunk1Size(vector <unsigned char> &text)
	{
		int32_t temp = 0;
		for (int i = 19; i >= 16; i--)
		{
			temp = (temp * 256) + text[i];
		}
		subchunk1Size = temp;
	}
	void Set_audioFormat(vector <unsigned char> &text)
	{
		int16_t temp = 0;
		for (int i = 21; i >= 20; i--)
		{
			temp = (temp * 256) + text[i];
		}
		audioFormat = temp;
	}
	void Set_numChannels(vector <unsigned char> &text)
	{
		int16_t temp = 0;
		for (int i = 23; i >= 22; i--)
		{
			temp = (temp * 256) + text[i];
		}
		numChannels = temp;
	}
	void Set_sampleRate(vector <unsigned char> &text)
	{
		int32_t temp = 0;
		for (int i = 27; i >= 24; i--)
		{
			temp = (temp * 256) + text[i];
		}
		sampleRate = temp;
	}
	void Set_byteRate(vector <unsigned char> &text)
	{
		int32_t temp = 0;
		for (int i = 31; i >= 28; i--)
		{
			temp = (temp * 256) + text[i];
		}
		byteRate = temp;
	}
	void Set_blockAlign(vector <unsigned char> &text)
	{
		int16_t temp = 0;
		for (int i = 33; i >= 32; i--)
		{
			temp = (temp * 256) + text[i];
		}
		blockAlign = temp;
	}
	void Set_bitsPerSample(vector <unsigned char> &text)
	{
		int16_t temp = 0;
		for (int i = 35; i >= 34; i--)
		{
			temp = (temp * 256) + text[i];
		}
		bitsPerSample = temp;
	}
} SUBCHUNK1;
typedef struct {
	char subchunk2Id[4];   // 0x64617461 – літери "data"
	int32_t subchunk2Size; // == NumSamples * NumChannels * BitsPerSample/8, кількість байтів аудіоданих
	vector <unsigned char> data;         // семпли
	void Set_subchunk2Id(vector <unsigned char> &text)
	{
		for (int i = 36; i < 36 + sizeof(subchunk2Id); i++)
		{
			subchunk2Id[i - 36] = text[i];
		}
	}
	void Set_subchunk2Size(vector <unsigned char> &text)
	{
		int32_t temp = 0;
		for (int i = 43; i >= 40; i--)

		{
			temp = (temp * 256) + text[i];
		}
		subchunk2Size = temp;
	}
	void Set_data(vector <unsigned char> &text)
	{
		for (int i = 44; i < text.size(); i++)
		{
			data.push_back(text[i]);
		}
	}
} SUBCHUNK2;
class WAVE_file
{
public:
	RIFFHEADER header;
	SUBCHUNK1 part1;
	SUBCHUNK2 part2;
	unsigned int koef;
	int new_subchunk2Size[4];
	int new_ChunkSize[4];
	WAVE_file(vector <unsigned char> &text)
	{
		header.Set_chunkId(text);
		header.Set_chunkSize(text);
		header.Set_format(text);
		part1.Set_subchunk1Id(text);
		part1.Set_subchunk1Size(text);
		part1.Set_audioFormat(text);
		part1.Set_numChannels(text);
		part1.Set_sampleRate(text);
		part1.Set_byteRate(text);
		part1.Set_blockAlign(text);
		part1.Set_bitsPerSample(text);
		part2.Set_subchunk2Id(text);
		part2.Set_subchunk2Size(text);
		part2.Set_data(text);
		koef = 1;
	}
	void increase_track_length(int n)
	{
		koef = n;
		string s_subchunk2Size;
		int temp = part2.subchunk2Size * n;
		while (temp > 0)
		{
			s_subchunk2Size.push_back((temp % 2 == 0) ? '0' : '1');
			temp /= 2;
		}
		reverse(s_subchunk2Size.begin(), s_subchunk2Size.end());
		while (s_subchunk2Size.size() < 8 * sizeof(int32_t))
		{
			s_subchunk2Size.insert(s_subchunk2Size.begin(), '0');
		}
		int k;
		for (int i = 3; i >=0; i--)
		{
			k = stoul(s_subchunk2Size.substr(i*8, 8), nullptr, 2);
			new_subchunk2Size[3-i] = k;
		}
		part2.subchunk2Size *= n;
		temp = part2.subchunk2Size + 36;
		header.chunkSize = temp;
		string s_ChunkSize;
		while (temp > 0)
		{
			s_ChunkSize.push_back((temp % 2 == 0) ? '0' : '1');
			temp /= 2;
		}
		reverse(s_ChunkSize.begin(), s_ChunkSize.end());
		while (s_ChunkSize.size() < 8 * sizeof(int32_t))
		{
			s_ChunkSize.insert(s_ChunkSize.begin(), '0');
		}
		for (int i = 3; i >= 0; i--)
		{
			k = stoul(s_ChunkSize.substr(i * 8, 8), nullptr, 2);
			new_ChunkSize[3 - i] = k;
		}
	}
	void out(ofstream &output, vector <unsigned char> &text)
	{
		if (koef == 1)
		{
			for (int i = 0; i < text.size(); i++)
			{
				output << text[i];
			}
		}
		else
		{
			for (int i = 0; i < text.size(); i++)
			{
				if (i == 44) continue;
				if (i >= 45)
				{
					for (int k = 0; k < koef; k++)
					{
						output << text[i - 1];
						output << text[i];
					}
					i++;
				}
				else
				{
					if (i >= 4 && i <= 7)
					{
						output << (char)new_ChunkSize[i-4];
					}
					else
					{
						if (i >= 40 && i <= 43)
						{
							output << (char)new_subchunk2Size[i-40];
						}
						else output << text[i];
					}
				}
			}
		}
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
	a.increase_track_length(5);
	a.out(output, text);
	input.close();
	output.close();
}