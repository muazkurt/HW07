#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define posibleChar	 1024	//Max readable chars from file.
#define mssEmail 16			//MeSage String for Email.
#define mssSubject 32		//MeSage String for Subject.
#define mssBody 1024		//MeSage String for Body.
#define maxToken 20
typedef struct {
	int mailNumber;
	char subject[mssSubject];
	char body[mssBody];
} email;

typedef struct {
	char name[maxToken];
	int value;
} token;


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
	int emails = 0,
		subject = 0,
		body = 0,
		line, error = 0;
	for (line = 0; error == 0; ++line)
	{
		ef = fgets(mail, 1024, mailfile);
		emails += searchinside("<email>", mail);
		emails -= searchinside("</email>", mail);
		subject += searchinside("<Subject>", mail);
		subject -= searchinside("</Subject>", mail);
		body += searchinside("<Body>", mail);
		body -= searchinside("</Body>", mail);
		if (ef == NULL)
		{
			if (emails == 0 ||
				subject == 0 ||
				body == 0)
				--error;
			else
				++error;
		}
		else if (emails < 0 || emails > 1 ||
			subject < 0 || subject > 1 ||
			body < 0 || body > 1)
			++error;
	}
	if (error == 1)
	{
		printf("THE MAIL GIVES AN ERROR IN: %d LINE.\n", line);
		return 1;
	}
	return 0;
}

/*
	detecting errors on token file
*/
int detectingErrorsforToken(FILE *tokenfile)
{
	char tokenc[60], *ef;
	int i, error = 0, sign, value = 0, positive;
	for (i = 0; error == 0; ++i)
	{
		ef = fgets(tokenc, 60, tokenfile);
		sign = searchinside("=", tokenc);
		if (sign == -1)
			++error;
		else
		{
			positive = sscanf(&tokenc[searchtheposition("=", tokenc) + 1], "%d", &value);
			if (positive != 1)
				++error;
		}
		if (ef == NULL)
			--error;
	}
	if (error > 0)
	{
		printf("THE TOKEN GIVES AN ERROR IN: %d LINE.\n", i);
		return 1;
	}
	return 0;
}

/*
	I can only read posibleChar characters from a file.
	If the file finish before posibleChar,
	then my job ends.
*/
char *readingFile(FILE* data, char *fullreadenfile)
{
	int counter;
	char q;
	for (counter = 0; fscanf(data, "%c", &q) != EOF && counter < posibleChar - 1; ++counter)
		fullreadenfile[counter] = q;
	fullreadenfile[counter] = '\0';
	return fullreadenfile;
}

/*
	I am searching for a keyword inside a string,
	fullreadenfile is the searched string,
	start is the keyword.
	If I find keyword, then i update the string with after keyword
	and return it.
	If there is no keyword, then I return NULL for saying there will not be keyword.
*/
char *whatsAfterThis(char *fullreadenfile, char *start)
{
	int counter;
	for (counter = 0; counter < strlen(fullreadenfile); ++counter)
	{
		if (strncmp(&fullreadenfile[counter], start, strlen(start)) == 0)
		{
			if (counter > 0)
				return strcpy(fullreadenfile, &fullreadenfile[counter + strlen(start)]);
			else if (counter == 0)
				return strcpy(fullreadenfile, &fullreadenfile[counter + strlen(start) - 1]);
		}
	}
	return "ERROR";
}

/*
	I am searching for a keyword in a string.
	fullreadenfile is the searched string.
	If i find the keyword, then i return the string before the keyword.
	I am not update the given string because it possibly cause data loss.
	If i cant find the keyword in given string,
	Then i return ERROR as there is no keyword.
*/
char *whatsBeforeThis(char *fullreadenfile, char *finish, char *whatsBefore)
{
	int counter;
	for (counter = 0; counter < strlen(fullreadenfile); ++counter)
	{
		if (strncmp(&fullreadenfile[counter], finish, strlen(finish)) == 0)
			return strcpy(strncpy(whatsBefore, fullreadenfile, counter), 0);
	}
	return "ERROR";
}

/*
	I am searching for 2 keywords in a string.
	fullreadenfile is the searched string.
	I'm finding the string between first keyword and sec keyword.
	I'm searching with early defined functions "whatsAfterThis" and "whatsBeforeThis".
	I return the string between them.
	If there will be some errors, then return value will be ERROR.
*/
char *whatsBetweenThese(char *fullreadenfile, char *start, char *finish, char *placetoPut)
{
	whatsBeforeThis(fullreadenfile, finish, placetoPut);
	whatsAfterThis(placetoPut, start);
	whatsAfterThis(fullreadenfile, finish);
	return placetoPut;
}

/*
	I am the hearth of all early defined functions.
	I am using all of them inside me.
	My job is finding <email> , </email> tags to finding emails
	Then I find <Subject>, </Subject tags to find the subject
	After that I find <Body>, </Body> tags and find the body.
	I need opened file pointer, two double dimentional arrays to put subject and body.
*/
email *parsingoneMail(char *subNbody, email *newmail, int witchmail)
{
	newmail->mailNumber = witchmail;
	strcat(strcpy(subNbody, &subNbody[searchtheposition("<subject>", subNbody) + strlen("<subject>")]), "\0");
	strcat(strncpy(newmail->subject, subNbody, searchtheposition("</subject>", subNbody) - 1), "\0");
	strcat(strcpy(subNbody, &subNbody[searchtheposition("<body>", subNbody) + strlen("<body>")]), "\0");
	strcat(strncpy(newmail->body, subNbody, searchtheposition("</body>", subNbody) - 1), "\0");
	return newmail;
}

token *parsingoneToken(char all[posibleChar], token *newtoken)
{
	strncpy(newtoken->name, all, searchtheposition("=", all));
	sscanf(&all[searchtheposition("=", all) + 1], "%d", &newtoken->value);
	return newtoken;
}

void main()
{
	FILE *Foftoken = fopen("token.txt", "r"), *Fofmail = fopen("emails.txt", "r");
	int error = detectingErrorsforToken(Foftoken) + detectErrorsforEmail(Fofmail);
	if (error == 0)
	{
		fseek(Fofmail, SEEK_SET, 0);
		fseek(Foftoken, SEEK_SET, 0);
		/*EMAIL DEF*/
		email *tempMail = (email *)malloc(sizeof(email));
		int mailnumber = 0;
		char all[posibleChar], mailindex[posibleChar];
		/*TOKEN DEF*/
		token *tempToken = (token *)malloc(sizeof(token));
		char *end;
		/*NECESSARY THINGS*/
		readingFile(Fofmail, all);
		for (mailnumber; searchinside("<email>", all) != 0; ++mailnumber)
		{
			strcpy(all, &all[searchtheposition("<email>", all) + strlen("<email>")]);
			strcat(all, "\0");
			strncpy(mailindex, all, searchtheposition("</email>", all) - 1);
			strcat(mailindex, "\0");
			parsingoneMail(mailindex, tempMail, mailnumber);
			for (end = fgets(all, posibleChar, Foftoken);
				end != NULL;
				end = fgets(all, posibleChar, Foftoken))
					parsingoneToken(all, tempToken);
		}
	}
	return;
}
