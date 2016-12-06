#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define posibleChar	 10240	//Max readable chars from file.
#define mssEmail 16			//MeSage String for Email.
#define mssSubject 32		//MeSage String for Subject.
#define mssBody 1024		//MeSage String for Body.
#define maxToken 20
typedef struct next_email{
	int mailNumber;
	char subject[mssSubject];
	char body[mssBody];
	struct next_email *next;
} email;

typedef struct next_token{
	char name[maxToken];
	int value;
	struct next_token *next;
} token;

typedef struct {
	email *noSpam,
		*spam;
} category;


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
	adds the mail to category of spam or not.
	If isspam = 0 its not spam,
	else if isspam = 1 its spam.
	then adds them to linklist of category ALL' spam  or nospam part.
*/
category *initmails(category *all, email *mail, int isspam)
{
	email *now;
	int placeC = 0;
	switch (isspam)
	{
	case 0:
		if (all->noSpam != 0)
		{
			for (now = all->noSpam; now->next != 0; now = now->next)
				placeC;
			now->next = mail;
		}
		else
			all->noSpam = mail;
		break;
	case 1:
		if (all->spam != 0)
		{
			for (now = all->spam; now->next != 0; now = now->next)
				placeC;
			now->next = mail;
		}
		else
			all->spam = mail;
		break;
	default:
		break;
	}
	mail->mailNumber = placeC;
	mail->next = 0;
	return all;
}

/*
	Searching the mail index in given array and parses it subject and object.
	Adds them to email typed themail' subject and body array. 
	links the next email to next part of themail. Linkslist' tail 
*/
email *parsingoneMail(char *subNbody, email *theMail, int witchmail)
{
	email *newmail = (email *)malloc(sizeof (email));;
	email *now;
	if(theMail != 0)
	{
		for (now = theMail; now->next != 0; now = now->next);
		now->next = newmail;
		newmail->next = 0;
	}
	else
	{
		theMail = newmail;
		newmail->next = 0;
	}
	newmail->mailNumber = witchmail;
	strcat(strcpy(subNbody, &subNbody[searchtheposition("<Subject>", subNbody) + strlen("<Subject>")]), "\0");
	strcat(strncpy(newmail->subject, subNbody, searchtheposition("</Subject>", subNbody)), "\0");
	strcat(strcpy(subNbody, &subNbody[searchtheposition("<Body>", subNbody) + strlen("<Body>")]), "\0");
	strcat(strncpy(newmail->body, subNbody, searchtheposition("</Body>", subNbody)), "\0");
	return theMail;
}

/*
	Searching the token index in given array and parses it value and name.
	adds them to token typed thetoken' name and value parts.
	liks the next token to next part of theToken. Linklist' tail.
*/
token *parsingoneToken(char all[posibleChar], token *theToken)
{
	token *newToken = (token *)malloc(sizeof(token));;
	token *now;
	if (theToken != 0)
	{
		for (now = theToken; now->next != 0; now = now->next);
		now->next = newToken;
		newToken->next = 0;
	}
	else
	{
		theToken = newToken;
		newToken->next = 0;
	}
	strncpy(theToken->name, all, searchtheposition("=", all));
	sscanf(&all[searchtheposition("=", all) + 1], "%d", &theToken->value);
	return newToken;
}

/*
	searching all mails for keywords of tokens.
	returns the categorised index.
*/
category *spamdetector(category *isSpam, email *mail, token *keyword)
{
	int forthisKeyinS = 0, forthisKeyinB = 0, allofKeys = 0, placer = 0, found = 0;
	email *now;
	token *spam;
	/*searching for all mails and categorising them as spam or not*/
	for (now = mail; now != 0; now = now->next)
	{
		/*searching for all tokens*/
		for (spam = keyword; spam != 0 && found == 0; spam = spam->next)
		{
			/*searching how many keyword in subject*/
			do
			{
				forthisKeyinS += searchinside(spam->name, &mail->subject[placer]);
				placer += searchtheposition(spam->name, &mail->subject[placer])
					+ strlen(spam->name);
			} while (forthisKeyinS > 0);
			/*searching how many keyword in body*/
			do
			{
				forthisKeyinB += searchinside(spam->name, &mail->body[placer]);
				placer += searchtheposition(spam->name, &mail->body[placer])
					+ strlen(spam->name);
			} while (forthisKeyinB > 0);
			/*2. requirement search.*/
			allofKeys += forthisKeyinS + forthisKeyinB;
			/*If we found the mail as spam then adding inside to categorised list*/
			if (forthisKeyinS + forthisKeyinB == spam->value || allofKeys >= 3)
			{
				initmails(isSpam, now, 1);
				++found;
			}
			/*after the detecting spams we need to refresh the mind*/
			forthisKeyinS = forthisKeyinB = 0;
		}
		initmails(isSpam, now, 0);
		/*2. requirement of spam work*/
		allofKeys = 0;
	}
	return isSpam;
}

void main()
{
	FILE *Foftoken = fopen("token.txt", "r"), *Fofmail = fopen("emails.txt", "r");
	int error = detectingErrorsforToken(Foftoken) + detectErrorsforEmail(Fofmail);
	if (error == 0)
	{
		/*RETURNING THE START OF FILES*/
		fseek(Fofmail, SEEK_SET, 0);
		fseek(Foftoken, SEEK_SET, 0);
		/*CATEGORY DEF*/
		category *categorysedmail = { NULL };
		/*EMAIL DEF*/
		email *tempMail = { NULL };
		int mailnumber = 0;
		char all[posibleChar], mailindex[posibleChar];
		/*TOKEN DEF*/
		token *tempToken = { NULL };
		char *end;
		/*ALL OF MAIL FILE*/
		readingFile(Fofmail, all);
		/*PARSING MAIL*/
		do
		{
			/*The first mail capturing*/
			strcpy(all, &all[searchtheposition("<email>", all) + strlen("<email>")]);
			strcat(all, "\0");
			/*Working on only the firs mail*/
			strncpy(mailindex, all, searchtheposition("</email>", all));
			strcat(mailindex, "\0");
			/*Deleting first mail from array*/
			strcpy(all, &all[searchtheposition("</email>", all) + strlen("</email>")]);
			/*Parsing it to subject and body.*/
			parsingoneMail(mailindex, tempMail, mailnumber++);
		} while (searchinside("<email>", all) != 0);
		/*PARSING TOKEN FILE*/
		do
		{
			end = fgets(all, posibleChar, Foftoken);
			parsingoneToken(all, tempToken);
			end = fgets(all, posibleChar, Foftoken);
		} while (end != NULL);
		/*DETECTING SPAMS*/
		spamdetector(categorysedmail, tempMail, tempToken);
	}
	else
		return;
	return;
}
