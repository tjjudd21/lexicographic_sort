#include <iostream>
#include <algorithm>
using namespace std;

int execute(int stringLength)
{
	int factorial = 1;
	
	for(int i = 1; i <= stringLength; i++)
	{
		factorial = factorial * i;
	}
	//cout << factorial;
	return factorial;
}

void lexicographic(string input)
{
	//int size = input.length();
    int posLargeChar = 0;
	int posAdjacentChar = 0;

	for(int i = 0; i < execute(input.length()); i++)
	{
	
		for(int i = 0; i < input.length(); i++)
		{
			cout << input[i];
			cout << " ";
		}
		
		//find the rightmost char which is smaller than the following char
		for(int i = 0; i < input.length(); i++)
		{
			if(input[i] < input[i + 1])
				posLargeChar = i;
				//break;
		}
		
		//find the next larger character
		for(int j = posLargeChar + 1; j < input.length(); j++)
		{
			if(input[j] > input[posLargeChar])
			 posAdjacentChar = j;
		}
		
		swap(input[posLargeChar], input[posAdjacentChar]);
		
		//sort the string on the right of 'Largest Char'
		int front = posLargeChar + 1;
		int back = input.length() - 1;
		
		while(front < back)
		{
           int begin = input[front];
           input[front] = input[back];
           input[back] = begin;
           front++;
           back--;            
        }
        cout << "\ti = " << posLargeChar << "\tj = " << posAdjacentChar;
        cout << endl;
	}
}

int main()
{
	string userString = "";
    
    cout << "Enter a string: ";
    getline(cin, userString);
    
    lexicographic(userString);

	cout << endl;
	system("pause");
	return 0;
}
