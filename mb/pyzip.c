#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ltypes.h"

#define CPB         0x7b
#define MCP(a,b)    (((a)<<8)|(b))

#define CZB			0x00
#define MCZ(a,b)	(((uint8_t)(a)<<8)|((uint8_t)(b)))

// 7b-ff 00-3a
static const uint16_t cp_list[]={
	// ��Ƶ����
	MCP('d','e'),
	MCP('u','i'),
	MCP('y','i'),
	MCP('v','i'),
	MCP('b','u'),
	MCP('y','b'),
	MCP('r','f'),
	MCP('w','z'),
	MCP('j','i'),
	MCP('g','o'),
	MCP('l','i'),
	MCP('v','s'),
	MCP('v','e'),
	MCP('z','l'),
	MCP('y','u'),
	MCP('j','w'),
	MCP('f','a'),
	MCP('q','i'),
	MCP('g','s'),
	MCP('d','k'),
	MCP('x','y'),
	MCP('x','m'),
	MCP('j','m'),
	MCP('y','e'),
	MCP('y','r'),
	MCP('d','a'),
	MCP('x','d'),
	MCP('g','e'),
	MCP('u','g'),
	MCP('l','e'),
	MCP('u','h'),
	MCP('z','i'),
	MCP('h','e'),
	MCP('j','q'),
	MCP('k','e'),
	MCP('i','u'),
	MCP('y','k'),
	MCP('j','y'),
	MCP('i','g'),
	MCP('x','c'),
	MCP('x','t'),
	MCP('h','v'),
	MCP('d','u'),
	MCP('f','h'),
	MCP('t','i'),
	MCP('j','n'),
	MCP('e','r'),
	MCP('n','m'),
	MCP('h','l'),
	MCP('w','u'),
	MCP('g','r'),
	MCP('d','i'),
	MCP('v','g'),
	MCP('d','v'),
	MCP('q','r'),
	MCP('j','c'),
	MCP('x','n'),
	MCP('m','z'),
	MCP('g','k'),
	MCP('b','k'),
	MCP('x','x'),
	MCP('z','o'),
	MCP('t','s'),
	MCP('y','y'),
	MCP('u','b'),
	MCP('n','g'),
	MCP('v','u'),
	MCP('r','u'),
	MCP('w','f'),
	MCP('s','i'),
	MCP('d','o'),
	MCP('f','u'),
	MCP('j','u'),
	MCP('b','i'),
	MCP('l','l'),
	MCP('u','f'),
	MCP('y','n'),
	MCP('i','h'),
	MCP('x','i'),
	MCP('j','x'),
	MCP('y','j'),
	MCP('u','u'),
	MCP('v','h'),
	MCP('d','j'),
	MCP('m','f'),
	MCP('q','u'),
	MCP('d','m'),
	MCP('h','o'),
	MCP('y','h'),
	MCP('l','d'),
	MCP('d','y'),
	MCP('a','n'),
	MCP('q','y'),
	MCP('y','s'),
	MCP('q','m'),
	MCP('u','e'),
	MCP('x','u'),
	MCP('h','f'),
	MCP('m','m'),
	MCP('h','b'),
	MCP('b','c'),
	MCP('m','y'),
	MCP('f','f'),
	MCP('w','h'),
	MCP('h','w'),
	MCP('f','z'),
	MCP('d','h'),
	MCP('p','i'),
	MCP('c','i'),
	MCP('r','j'),
	MCP('n','j'),
	MCP('h','k'),
	MCP('t','a'),
	MCP('b','f'),
	MCP('g','l'),
	MCP('i','j'),
	MCP('s','o'),
	MCP('j','d'),
	MCP('h','u'),
	MCP('c','l'),
	MCP('l','v'),
	MCP('y','t'),
	MCP('u','o'),
	MCP('p','y'),
	MCP('m','u'),
	MCP('d','l'),
	MCP('g','v'),
	MCP('f','j'),

	MCP('w','o'),
	MCP('b','z'),
	MCP('x','w'),
	MCP('w','j'),
	MCP('d','s'),

	MCP('v','j'),
	MCP('b','m'),
	MCP('u','v'),
	MCP('f','g'),
	MCP('t','m'),
	MCP('g','u'),
	MCP('g','j'),
	MCP('l','m'),
	MCP('n','a'),
	MCP('v','f'),
	MCP('n','i'),
	MCP('b','y'),
	MCP('t','b'),
	MCP('t','l'),
	MCP('b','j'),
	MCP('k','l'),
	MCP('z','v'),
	MCP('l','q'),
	MCP('u','j'),
	MCP('b','l'),
	MCP('t','y'),
	MCP('h','r'),
	MCP('g','g'),
	MCP('b','a'),
	MCP('i','i'),
	MCP('m','j'),
	MCP('l','y'),
	MCP('l','u'),
	MCP('d','b'),
	MCP('s','u'),
	MCP('v','r'),
	MCP('s','j'),

	MCP('v','k'),
	MCP('j','p'),
	MCP('r','i'),
	MCP('m','o'),
	MCP('t','u'),
	MCP('g','d'),
	MCP('i','r'),

	MCP('q','d'),	// 0x27
	MCP('w','l'),
	MCP('h','j'),
	MCP('l','k'),
	MCP('p','n'),
	MCP('z','k'),
	MCP('d','g'),
	MCP('k','j'),
	MCP('m','e'),
	MCP('m','n'),
	MCP('z','u'),
	MCP('p','m'),
	MCP('s','v'),
	MCP('d','r'),
	MCP('q','q'),
	MCP('j','t'),
	MCP('t','c'),
	MCP('p','l'),
	MCP('b','o'),
	MCP('i','s'),	// 0x3a
};
static_assert(lengthof(cp_list)==5+128+59,"cp list length not allow");

// 00 80
static const uint16_t cz_list[]={
	0xb5c4,			//��
	0xd2bb,			//һ
	0xcac7,			//��
	0xc1cb,			//��
	0xb2bb,			//��
	0xd4da,			//��
	0xd3d0,			//��
	0xb8f6,			//��
	0xc8cb,			//��
	0xd5e2,			//��
	0xc9cf,			//��
	0xd6d0,			//��
	0xb4f3,			//��
	0xceaa,			//Ϊ
	0xc0b4,			//��
	0xced2,			//��
	0xb5bd,			//��
	0xb3f6,			//��
	0xd2aa,			//Ҫ
	0xd2d4,			//��
	0xcab1,			//ʱ
	0xbacd,			//��
	0xb5d8,			//��
	0xc3c7,			//��
	0xb5c3,			//��
	0xbfc9,			//��
	0xcfc2,			//��
	0xb6d4,			//��
	0xc9fa,			//��
	0xd2b2,			//Ҳ
	0xd7d3,			//��
	0xbecd,			//��
	0xb9fd,			//��
	0xc4dc,			//��
	0xcbfb,			//��
	0xbbe1,			//��
	0xb6e0,			//��
	0xb7a2,			//��
	0xcbb5,			//˵
	0xb6f8,			//��
	0xd3da,			//��
	0xd7d4,			//��
	0xd6ae,			//֮
	0xd3c3,			//��
	0xc4ea,			//��
	0xd0d0,			//��
	0xbcd2,			//��

	0xb7bd,			//��
	0xbaf3,			//��
	0xd7f7,			//��
	0xb3c9,			//��
	0xbfaa,			//��
	0xc3e6,			//��
	0xcac2,			//��
	0xbac3,			//��
	0xd0a1,			//С
	0xd0c4,			//��
	0xc7b0,			//ǰ
	0xcbf9,			//��
	0xb5c0,			//��
	0xb7a8,			//��
	0xc8e7,			//��
	0xbdf8,			//��
	0xd7c5,			//��
	0xcdac,			//ͬ
	0xbead,			//��
	0xb7d6,			//��
	0xb6a8,			//��
	0xb6bc,			//��
	0xc8bb,			//Ȼ
	0xd3eb,			//��
	0xb1be,			//��
	0xbbb9,			//��
	0xc6e4,			//��
	0xb5b1,			//��
	0xc6f0,			//��
	0xb6af,			//��
	0xd2d1,			//��
	0xc1bd,			//��
	0xb5e3,			//��
	0xb4d3,			//��
	0xceca,			//��
	0xc0ef,			//��
	0xd6f7,			//��
	0xcab5,			//ʵ
	0xccec,			//��
	0xb8df,			//��
	0xc8a5,			//ȥ
	0xcfd6,			//��
	0xb3a4,			//��
	0xb4cb,			//��
	0xc8fd,			//��
	0xbdab,			//��
	0xcede,			//��
	0xb9fa,			//��
	0xc8ab,			//ȫ
	0xcec4,			//��
	0xc0ed,			//��
	0xc3f7,			//��
	0xc8d5,			//��
	0xd0a9,			//Щ
	0xbfb4,			//��
	0xd6bb,			//ֻ
	0xb9ab,			//��
	0xb5c8,			//��
	0xcaae,			//ʮ
	0xd2e2,			//��
	0xd5fd,			//��
	0xcde2,			//��
	0xcfeb,			//��
	0xbce4,			//��
	0xb0d1,			//��
	0xc7e9,			//��
	0xd5df,			//��
	0xc3bb,			//û
	0xd6d8,			//��
	0xcfe0,			//��
	0xc4c7,			//��
	0xcff2,			//��
	0xd6aa,			//֪
	0xd2f2,			//��
	0xd1f9,			//��
	0xd1a7,			//ѧ
	0xd3a6,			//Ӧ
	0xd3d6,			//��
	0xcad6,			//��
	0xb5ab,			//��
	0xd0c5,			//��
	0xb9d8,			//��
};
static_assert(lengthof(cz_list)==0x81,"cz list length not allow");

static inline int cp_find(const char *in)
{
	uint16_t cp=MCP(in[0],in[1]);
	for(int i=0;i<lengthof(cp_list);i++)
	{       
		if(cp==cp_list[i])
		{
			return (CPB+i)&0xff;
		}
	}
	return -1;
}

static inline int cz_find(const char *in)
{
	uint16_t cp=MCZ(in[0],in[1]);
	for(int i=0;i<lengthof(cz_list);i++)
	{       
		if(cp==cz_list[i])
			return CZB+i;
	}
	return -1;
}

int cp_zip(const char *in,char *out)
{
	const char *base=out;
	while(*in)
	{
		int ret=cp_find(in);
		if(ret>=0)
		{
			*out++=ret;
			in+=2;
		}
		else
		{
			*out++=*in++;
			if(*in)
				*out++=*in++;
		}
	}
	*out=0;
	return (int)(size_t)(out-base);
}

// ����size��ʾ�����������Ƕ��⴫�����������£���֧�ַǶ�������
int cp_unzip(const char *in,char *out,int size)
{
	int len=0;
	for(;size>0;size--)
	{
		int c=*(uint8_t*)in++;
		if(c>=CPB)
		{
			uint16_t cp=cp_list[c-CPB];
			*out++=cp>>8;
			*out++=cp&0xff;
		}
		else if(c<=0x3a)
		{
			uint16_t cp=cp_list[5+128+c];
			*out++=cp>>8;
			*out++=cp&0xff;
		}
		else
		{
			*out++=c;
			*out++=*(uint8_t*)in++;
		}
		len+=2;
	}
	*out=0;
	return len;
}

int cp_unzip_size(const char *in,int size)
{
	int len=0;
	for(;size>0;size--)
	{
		int c=*(uint8_t*)in++;
		if(c<=0x3a || c>=CPB)
		{
			len++;
		}
		else
		{
			in++;
			len+=2;
		}
	}
	return len;
}

int cp_unzip_py(const char *in,char *out,int size)
{
	int s,y;
	char *orig=out;
	for(;size>0;size--)
	{
		s=*(uint8_t*)in++;
		if(s>=CPB)
		{
			uint16_t cp=cp_list[s-CPB];
			s=cp>>8;
			y=cp&0xff;
		}
		else if(s<=0x3a)
		{
			uint16_t cp=cp_list[5+128+s];
			s=cp>>8;
			y=cp&0xff;
		}
		else
		{
			y=*(uint8_t*)in++;
		}
		switch(s){
			case 'i':
				*out++='c';
				*out++='h';
				break;
			case 'u':
				*out++='s';
				*out++='h';
				break;
			case 'v':
				*out++='z';
				*out++='h';
				break;
			default:
				*out++=s;
				break;
		}
		switch(y){
			case 'a':
				if(s!='a')
					*out++='a';
				break;
			case 'b':
				*out++='o';*out++='u';
				break;
			case 'c':
				*out++='i';*out++='a';*out++='o';
				break;
			case 'd':
				if(s=='j' || s=='l' || s=='n' || s=='q' || s=='x')
					*out++='i';
				else
					*out++='u';
				*out++='a';*out++='n';*out++='g';
				break;
			case 'e':
				if(s!='e')
					*out++='e';
				break;
			case 'f':
				*out++='e';*out++='n';
				break;
			case 'g':
				if(s!='e')
				{
					*out++='e';
				}
				*out++='n';*out++='g';
				break;
			case 'h':
				if(s!='a')
				{
					*out++='a';
				}
				*out++='n';*out++='g';
				break;
			case 'j':
				*out++='a';*out++='n';
				break;
			case 'k':
				*out++='a';*out++='o';
				break;
			case 'l':
				*out++='a';*out++='i';
				break;
			case 'm':
				*out++='i';*out++='a';*out++='n';
				break;
			case 'n':
				if(s!='a' && s!='e')
					*out++='i';
				*out++='n';
				break;
			case 'o':
				if(s=='o')
				{
				}
				else if(s=='a' || s=='b' || s=='f' || s=='m' || s=='p' || s=='q' || s=='w')
				{
					*out++='o';
				}
				else
				{
					*out++='u';*out++='o';
				}
				break;
			case 'p':
				*out++='u';*out++='n';
				break;
			case 'q':
				*out++='i';*out++='u';
				break;
			case 'r':
				if(s=='e')
				{
					*out++='r';
				}
				else
				{
					*out++='u';*out++='a';*out++='n';
				}
				break;
			case 's':
				if(s=='j' || s=='q' || s=='x')
				{
					*out++='i';
				}
				*out++='o';*out++='n';*out++='g';
				break;
			case 't':
				*out++='u';*out++='e';
				break;
			case 'v':
				if(s=='l' || s=='n')
				{
					*out++='v';
				}
				else
				{
					*out++='u';
					*out++='i';
				}
				break;
			case 'w':
				if(s=='d' || s=='j' || s=='l' || s=='q' || s=='x')
					*out++='i';
				else
					*out++='u';
				*out++='a';
				break;
			case 'x':
				*out++='i';*out++='e';
				break;
			case 'y':
				if(s=='i' || s=='g' || s=='h' || s=='k' || s=='u' || s=='v')
				{
					*out++='u';*out++='a';*out++='i';
				}
				else
				{
					*out++='i';*out++='n';*out++='g';
				}
				break;
			case 'z':
				*out++='e';
				*out++='i';
				break;
			default:
				*out++=y;
				break;
		}
	}
	*out=0;
	return (int)(size_t)(out-orig);
}

int cp_unzip_jp(const char *in,char *out,int size)
{
	int s;
	char *orig=out;
	for(;size>0;size--)
	{
		s=*(uint8_t*)in++;
		if(s>=CPB)
		{
			uint16_t cp=cp_list[s-CPB];
			s=cp>>8;
		}
		else if(s<=0x3a)
		{
			uint16_t cp=cp_list[5+128+s];
			s=cp>>8;
		}
		else
		{
			in++;
		}
		switch(s){
			case 'i':*out++='c';break;
			case 'u':*out++='s';break;
			case 'v':*out++='z';break;
			default:*out++=s;break;
		}
	}
	*out=0;
	return (int)(size_t)(out-orig);
}

int cz_zip(const char *in,char *out)
{
	const char *base=out;
	while(*in)
	{
		int ret;
		ret=cz_find(in);
		if(ret>=0)
		{
			*out++=ret;
			in+=2;
		}
		else
		{
			*out++=*in++;
			*out++=*in++;
		}
	}
	*out=0;
	return (int)(size_t)(out-base);
}

int cz_unzip(const char *in,char *out,int size)
{
	int len=0;
	for(;size>0;size--)
	{
		int c=*(uint8_t*)in++;
		if(c<lengthof(cz_list))
		{
			uint16_t cp=cz_list[c-CZB];
			*out++=cp>>8;
			*out++=cp&0xff;
			len+=2;
		}
		else
		{
			*out++=c;
			c=*(uint8_t*)in++;
			if(c>=0x40)
			{
				*out++=c;
				len+=2;
			}
			else
			{
				*out++=c;
				*out++=*(uint8_t*)in++;
				*out++=*(uint8_t*)in++;
				len+=4;
			}
		}
	}
	*out=0;
	return len;
}



