#ifndef SP_DYNAMIC_INCLUDED
#define SP_DYNAMIC_INCLUDED
struct SP_DYNAMIC
{
	unsigned char On;
	unsigned char Falloff;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char Flags;
	unsigned char Pad[2];
};
#endif