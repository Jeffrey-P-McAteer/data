#include "PPMImage.h"


PPMImage::PPMImage(void)
{
	image = nullptr;
}

PPMImage::~PPMImage(void)
{
	if (image != nullptr)
		delete[] image;
}

void PPMImage::AllocateMemory(int width, int height)
{
	this->width = width;
	this->height = height;
	image = new unsigned char[width * height * 3];
}

void PPMImage::ReadFile(string fileName)
{
	file.open(fileName, ios_base::binary);

	if (!file.is_open())
	{
		cout << fileName << " couldn't be opened.\n";
		system("pause");
		exit(1);
	}

	string fileType;
	file >> fileType;

	CheckComment();
	file >> width >> height;
	image = new unsigned char[width * height * 3];

	int maxValue;

	file >> maxValue;

	if (fileType == "P3")
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width * 3; j++)
			{
				string temp;
				file >> temp;
				image[i * width * 3 + j] = (unsigned char)atoi(temp.c_str());
			}
		}
	}

	else if (fileType == "P6")
	{
		file.getline(buf, 127);
		file.read((char*)image, height * width * 3);
	}

	file.close();
}

void PPMImage::WriteFile(string fileName, string fileType)
{
	// Read and understand the code.
	if (fileType == "P6")
	{
		outFile.open(fileName, ios_base::binary);

		if (!outFile.is_open())
		{
			cout << fileName << " couldn't be opened.\n";
			system("pause");
			exit(1);
		}

		outFile << fileType << endl;
		outFile << width << " " << height << endl;
		outFile << 255 << endl;

		outFile.write((char*)image, height * width * 3);
	}
	else if (fileType == "P3")
	{
		outFile.open(fileName);

		if (!outFile.is_open())
		{
			cout << fileName << " couldn't be opened.\n";
			system("pause");
			exit(1);
		}

		outFile << fileType << endl;
		outFile << width << " " << height << endl;
		outFile << 255 << endl;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width * 3; j++)
			{
				outFile << (int)image[i * width * 3 + j] << " ";
			}
			outFile << endl;
		}
	}
	outFile.close();
}

void PPMImage::CheckComment()
{
	c = file.peek();

	while (c == '#' || c == '\n' || c == ' ')
	{
		file.getline(buf, 127);
		c = file.peek();
	}
}

void PPMImage::VerticalFlip()
{
	if (image == nullptr) {
		return; // refuse to flip null image
	}

	unsigned char* flippedImage = new unsigned char[this->width * this->height * 3];
	if (flippedImage == nullptr) {
		return; // alloc failure
	}

	// Do a vertical flip.  You need to use the help variable declared above.
	// Also do dynamic memory allocation for the variable based on the image size.

	for (int y = 0; y < this->height; y++)
	{
		int flipped_y = this->height - y;
		for (int x = 0; x < this->width * 3; x++)
		{
			int image_i = (y * this->width * 3) + x;
			int flipped_i = (flipped_y * this->width * 3) + x;
			if (flipped_i < this->width * this->height * 3) {
				flippedImage[flipped_i] = this->image[image_i];
			}
		}
	}

	// Write flipped pixels back into this->image and de-allocate flippedImage
	for (int y = 0; y < this->height; y++)
	{
		int flipped_y = this->height - y;
		for (int x = 0; x < this->width * 3; x++)
		{
			int i = (y * this->width * 3) + x;
			if (i < this->width * this->height * 3) {
				this->image[i] = flippedImage[i];
			}
		}
	}

	delete[] flippedImage;

}