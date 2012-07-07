const int32 kStartImageWidth = 10;
const int32 kStartImageHeight = 10;
const color_space kStartImageColorSpace = B_RGB32;

const unsigned char kStartImageBits [] = {
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8
};

const int32 kStopImageWidth = 10;
const int32 kStopImageHeight = 10;
const color_space kStopImageColorSpace = B_RGB32;

const unsigned char kStopImageBits [] = {
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,
	0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8,0xd8
};

