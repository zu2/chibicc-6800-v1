unsigned char cresult[8];
unsigned int  iresult[8];

int main(int argc, char **argv)
{
	unsigned char cd = 0xff, i;
	unsigned int  id = 0xff;

	for (i = 0; i < 8; i++)
	  iresult[i] = id >> i;

	if (iresult[0]!=0xff)
	  return 1;

	if (iresult[1]!=0x7f)
	  return 2;

	if (iresult[2]!=0x3f)
	  return 3;

	if (iresult[3]!=0x1f)
	  return 4;

	if (iresult[4]!=0x0f)
	  return 5;

	if (iresult[5]!=0x07)
	  return 6;

	if (iresult[6]!=0x03)
	  return 7;

	if (iresult[7]!=0x01)
	  return 8;


	for (i = 0; i < 8; i++)
	  cresult[i] = cd >> i;

	if (cresult[0]!=0xff)
	  return 21;

	if (cresult[1]!=0x7f)
	  return 22;

	if (cresult[2]!=0x3f)
	  return 23;

	if (cresult[3]!=0x1f)
	  return 24;

	if (cresult[4]!=0x0f)
	  return 25;

	if (cresult[5]!=0x07)
	  return 26;

	if (cresult[6]!=0x03)
	  return 27;

	if (cresult[7]!=0x01)
	  return 28;

	return 0;
}
