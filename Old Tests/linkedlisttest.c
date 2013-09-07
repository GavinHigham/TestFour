#include <stdio.h>

typedef struct node {
	struct node *next;
} NODE, *NODEP;


int main()
{
	int i;
	NODE nodes[5] = {{NULL}};
	for (i = 0; i < 4; i++)
		nodes[i].next = &nodes[i+1];
	NODEP temp = &nodes[0];
	i = 0;
	while (temp) {
		printf("%i\n", i);
		i++;
		temp = temp->next;
	}
	return 0;
}