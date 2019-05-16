#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

unsigned long floatLimit(float *value, float scalefactor, int offset, float max_in, float min_in)
{

	float temp;
	temp = MIN(*value, max_in);
	temp = MAX(temp, min_in);
	temp = (temp - (float)offset) * scalefactor;

	return (unsigned long)temp;
}

int int16Limit(int value, int max_in, int min_in)
{

	int temp = 0;
	temp = MIN(value, max_in);
	temp = MAX(temp, min_in);


	return temp;
}

void copyB(char *src, int len) {
	//char temp[7];
	for (int i = 1; i < len; i++) {
		*src++ = (*src)++ +1;
	}
}

//void shiftBuff()
//{
//	memmove(buff, &(buff[1]), strlen(&(buff[1])));
//}

int gethex(char *string)  //String to hex val
{
	int val=0;
	int go=1;
	while (go)
	{
		char c=toupper(*string++);
		if ((c>='0' && c<='9') || (c>='A' && c<='F'))
		{
			c=(c>='A') ? (c-'A'+10) : (c-'0');
			val=(val<<4)|c;
		}
		else
		{
			go=0;
		}
	}
	return val;
}