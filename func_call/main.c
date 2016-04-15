void (*fb)();

void fa()
{
	volatile a=0;
	a++;
}

int main()
{
	fa();

	fb();
	
	return 0;
}
