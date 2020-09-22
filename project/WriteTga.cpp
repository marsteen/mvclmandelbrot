
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

//
//
//

enum ETgaItype
{
	ETGA_ITYPE_8BIT_UNCOMPRESSED            = 1,
	ETGA_ITYPE_RGB_UNCOMPRESSED             = 2,
	ETGA_ITYPE_8BIT_UNCOMPRESSED_MONOCHROME = 3,
	ETGA_ITYPE_8BIT_COMPRESSED              = 9,
	ETGA_ITYPE_RGB_COMPRESSED               = 10,
	ETGA_ITYPE_8BIT_COMPRESSED_MONOCHROME   = 11
};

struct STgaHeader
{
  unsigned char  mIdent;    /* Anzahl der Zeichen im Identificationsfeld */
  unsigned char  mCtype;    /* Color Map Type  */
  unsigned char  mItype;    /* Image Type Code */
  unsigned char  mCmap[5];  /* Color Map Specification */
  short          mXorigin;
  short          mYorigin;
  short          mWidth;
  short          mHeight;
  unsigned char  mPsize;    /* Number of Bits in a stored pixel index */
  unsigned char  mIbyte;    /* Image Descriptor Byte */
};



//---------------------------------------------------------------------------
//
// Klasse:    static
// Methode:   WriteTga
//
// Schreiben einer Datei im 8-Bit TGA-Format (Graustufen)
//
//---------------------------------------------------------------------------

void WriteTga(const char* Filename, const int* Data, int w, int h)
{
	STgaHeader TgaHeader; 

	memset(&TgaHeader, 0, sizeof(STgaHeader));

  TgaHeader.mIbyte  = 0x20;
  TgaHeader.mWidth  = w;
  TgaHeader.mHeight = h;
  TgaHeader.mPsize  = 8;
  TgaHeader.mItype  = ETGA_ITYPE_8BIT_UNCOMPRESSED_MONOCHROME;

  cout << "Writing TGA file.." << endl;

  char* ByteData = new char[w * h];
  for (int i = 0; i < w*h; i++)
	{
//    ByteData[i] = Data[i] & 0xFF;
    ByteData[i] = (Data[i] & 1) ? 255 : 1;
//    ByteData[i] = Data[i] & 255;

	}

	cout << "ok." << endl;


	fstream fs(Filename, std::ios::out | std::ios::binary);
  if (fs.good())
	{
 	  fs.write((const char*) &TgaHeader, sizeof(STgaHeader));
    fs.write((const char*) ByteData, w * h);
		fs.close();
	}
  delete[] ByteData;

}
