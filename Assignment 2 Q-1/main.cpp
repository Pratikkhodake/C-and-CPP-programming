 # include "myheader.h"

int main(int argc, char*argv[])
{
	/*int i,no1, no2,lcd;
	printf("enter two integers: \n");
	scanf_s("%d%d", &no1, &no2); */

	/*3)
	
	i = 1;
	/*int i, n;
	printf("enter any no");
	scanf_s("%d", &n);
	
	/*  1)
	
	printf("Even nos upto\t- \t\n\n");
	for (i =2; i, i <=2*n; i += 2)
	{
		printf("%d", i);
	}*/
	
	/*  2)  
	
	printf("Factors of %d are: ", n);
	for (i = 2; i <=n; ++i)
	{
		if ((n%i == 0)&&(i%2==0))
		{
			printf("%d ", i);
		}
	}*/
	    
	/*  3) 
	
	while ((i <= no1 / 2) && (i <= no2 / 2))
	{
		if ((no1%i == 0) && (no2%i == 0))
			printf("common factor is\t %d \n ", i);
		i++;
	}*/
	
	/*  4)
	
	for (i = 1; i <= no1 && i <= no2; ++i)
	{
		// Checks if i is factor of both integers
		if (no1%i == 0 && no2%i == 0)
			lcd = i;
	}

	printf("L.C.D of %d and %d is %d\n", no1, no2, lcd);*/
 
	/*  5)
	
	{
		char str[100], temp;
		int i, j = 0;

		printf("\nEnter the string :");
		cin.getline(str, 100);

		i = 0;
		j = strlen(str) - 1;

		while (i < j)
		{
			temp = str[i];
			str[i] = str[j];
			str[j] = temp;
			i++;
			j--;
		}

		printf("\nReverse string is :%s\n", str);
	
	}*/
	
	/*  6)
	
	{
		int a, b;
		int *ptra, *ptrb;
		int *temp;

		printf("Enter any two integers: ");
		scanf_s("%d%d", &a, &b);

		printf("Before swapping: a = %d, b=%d", a, b);

		ptra = &a;
		ptrb = &b;

		temp = ptra;
		*ptra = *ptrb;
		*ptrb = *temp;

		printf("\nAfter swapping: a = %d, b=%d", a, b);
	}*/
	
	/*  6)
	
	int x, y, temp;

	printf("Enter the value of x and y\n");
	scanf_s("%d%d", &x, &y);

	printf("Before Swapping\nx = %d\ny = %d\n", x, y);

	temp = x;
	x = y;
	y = temp;

	printf("After Swapping\nx = %d\ny = %d\n", x, y);*/
	
	/*  9)
	
	{
		int k, r;
		long int i = 0l, j = 1, f;

		//Taking maximum numbers form user
		printf("Enter the number range:");
		scanf_s("%d", &r);

		printf("FIBONACCI SERIES: \n");
		printf("%ld %ld", i, j); //printing firts two values.

		for (k = 2; k < r; k++)
		{
			f = i + j;
			i = j;
			j = f;
			printf(" %ld", j);
		}
	}*/
	
	/*  10)
	
	{
		char c = 0 ;
		char s[1000];

		printf("Input a string\n");
		cin.getline(s,1000);

		
		{
			if (s[c] == 'a' || s[c] == 'A' || s[c] == 'e' || s[c] == 'E' || s[c] == 'i' || s[c] == 'I' || s[c] == 'o' || s[c] == 'O' || s[c] == 'u' || s[c] == 'U')
			{
				printf(" its a vowel \n");
			}
			else
			{
				printf("it is NOT vowel \n");
			}
		}

		
	}*/


	system("pause");
	return 0;
}
