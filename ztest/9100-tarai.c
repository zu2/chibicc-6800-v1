int tarai(int x, int y, int z)
{

	if (x>y){
		return tarai(
			tarai(x-1,y,z),
			tarai(y-1,z,x),
			tarai(z-1,x,y));
	}
	return y;
}

int main(int argc, char **argv)
{
	if(tarai(13,7,0)!=13)
		return 1;

	return 0;
}
