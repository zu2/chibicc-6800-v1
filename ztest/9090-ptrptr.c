typedef	struct {
	unsigned char *left;
	unsigned char *right;
} node;

equalp(node *p,node *q)
{
	unsigned char dummy;

	dummy = *(p->left) + *(q->left);

	if (*(p->left) != *(q->left))
		return	0;
	if (*(p->right) != *(q->right))
		return	0;
	return 1;
}

int main(int argc, char **argv)
{
	node	p,q;
	unsigned char a[5];

	a[0] = 0; a[1] = 1; a[2] = 2; a[3] = 3; a[4] = 4;
	p.left  = &a[1];
	p.right = &a[2];
	q.left  = &a[3];
	q.right = &a[4];

	return equalp(&p,&q);
}
