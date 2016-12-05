#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
I am searching for dest in source
If i find in it, i return 1.
else i return 0;
*/
int searchinside(char *dest, char *source)
{
	int counter;
	for (counter = 0; counter <= strlen(source); ++counter)
	{
		if (strncmp(dest, &source[counter], strlen(dest)) == 0)
			return 1;
	}
	return 0;
}


/*
finds the dest string's position in source file.
*/
int searchtheposition(char *dest, char *source)
{
	int counter;
	for (counter = 0; counter <= strlen(source); ++counter)
	{
		if (strncmp(dest, &source[counter], strlen(dest)) == 0)
			return counter;
	}
	return -1;
}


/*
I take a file and i look up it.
If there will be an error i return the line number of it.
If there is not any error i return -1;
*/
int detectErrorsforEmail(FILE *mailfile)
{
	char mail[2048], *ef;
	int email = 0,
		subject = 0,
		body = 0,
		line, error = 0;
	for (line = 0; error == 0; ++line)
	{
		ef = fgets(mail, 1024, mailfile);
		email += searchinside("<email>", mail);
		email -= searchinside("</email>", mail);
		subject += searchinside("<Subject>", mail);
		subject -= searchinside("</Subject>", mail);
		body += searchinside("<Body>", mail);
		body -= searchinside("</Body>", mail);
		if (ef == NULL)
		{
			if (email == 0 ||
				subject == 0 ||
				body == 0)
				--error;
			else
				++error;
		}
		else if (email < 0 || email > 1 ||
			subject < 0 || subject > 1 ||
			body < 0 || body > 1)
			++error;
	}
	if (error == 1)
		return line;
	return -1;
}

void main()
{
	return ;
}
