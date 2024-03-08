/*
Create a file stream
Open the .jack file based on user input or parameters
Read the .jack file char by char to add to a string or array
Match the string or array to a keyword, symbol, identifier, integer constant, or string constant
Output the tokens in specific format to a .xml file

<tokens>
	<keyword> if </keyword>
	<symbol> ( </symbol>
	<identifier> foo </identifier>
	<integerConstant> 5 </integerConstant>
	<stringConstant> hello </stringConstant>
</tokens>

Also note that four of the symbols used in the Jack language (<, >, ", and &) are also used for
XML markup, and thus they cannot appear verbatim as XML data.

To solve the problem, and following convention, 
we require the tokenizer to output these tokens as &lt;, &gt;, &quot;, and &amp;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

FILE *xml;

char* keyword[] = {"class", "constructor", "function", "method", "int", "boolean", "char", "void", "var", "static", "field", "let", "do", "if", "else", "while", "return", "true", "false", "null", "this"};
char symbol[] = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'};

bool multiLineComment = false;
bool stringLiteral = false;

bool isDelimiter(char c) {
	for (int i = 0; i < sizeof(symbol)/sizeof(symbol[0]); i++) {
		if (c == symbol[i] || c == ' ' || c == '\t' || c == '\n') {
			return true;
		}
	}
	return false;
}

bool isSymbol(char c) {
	for (int i = 0; i < sizeof(symbol)/sizeof(symbol[0]); i++) {
		if (c == symbol[i]) {
			return true;
		}
	}
	return false;
}

bool isKeyword(char* c) {
	for (int i = 0; i < sizeof(keyword)/sizeof(keyword[0]); i++) {
		if (strcmp(c, keyword[i]) == 0) {
			return true;
		}
	}
	return false;
}

char* getSubstring(char* str, int start, int end) { 
    int length = strlen(str); 
    int subLength = end - start + 1; 
    char* subStr = (char*)malloc((subLength + 1) * sizeof(char)); 
    strncpy(subStr, str + start, subLength); 
    subStr[subLength] = '\0'; 
    return subStr; 
} 

void analyzeLine(char *buffer) {
	int left = 0, right = 0;
	int len = strlen(buffer);

	while (right <= len && left <= right) {
		if (buffer[right] == '/' && buffer[right+1] == '/') {
			return;
		}

		if (buffer[right] == '/' && buffer[right+1] == '*' && !multiLineComment) {
			multiLineComment = true;
		}

		if (buffer[right] == '*' && buffer[right+1] == '/' && multiLineComment) {
			multiLineComment = false;
			right += 2;
			left = right;
			continue;
		}

		if (buffer[right] == '"' && !stringLiteral) {
			right++;
			left = right;
			stringLiteral = true;
		}

		if (buffer[right] == '"' && stringLiteral) {
			fprintf(xml, "<stringConstant> %s </stringConstant>\n", getSubstring(buffer, left, right-1));
			right++;
			left = right;
			stringLiteral = false;
		}

		if (!isDelimiter(buffer[right]) || multiLineComment || stringLiteral)
			right++;
		
		if (isDelimiter(buffer[right]) && left == right && !multiLineComment && !stringLiteral) {
			if (isSymbol(buffer[right])) {
				switch(buffer[right]) {
						case '<':
							fprintf(xml, "<symbol> &lt; </symbol>\n");
							break;
						case '>':
							fprintf(xml, "<symbol> &gt; </symbol>\n");
							break;
						case '"':
							fprintf(xml, "<symbol> &quot; </symbol>\n");
							break;
						case '&':
							fprintf(xml, "<symbol> &amp; </symbol>\n");
							break;
						default:
							fprintf(xml, "<symbol> %c </symbol>\n", buffer[right]);
							break;
					}
			}
			right++;
			left = right;
		} else if (isDelimiter(buffer[right]) && left != right && !multiLineComment && !stringLiteral) {
			char* token = getSubstring(buffer, left, right-1);

			int i = 0;
			while (isdigit(token[i])) 
				i++;

			if (i == strlen(token)) {
				fprintf(xml, "<integerConstant> %s </integerConstant>\n", token);
			} else if (isKeyword(token)) {
				fprintf(xml, "<keyword> %s </keyword>\n", token);
			} else {
				fprintf(xml, "<identifier> %s </identifier>\n", token);
			} 
			

			left = right;
		}
	}
}

int main(int argc, char const *argv[]) {
	char destination[256];

	// get destination from user input either via command line or stdin
	if (argc == 1) {
		printf("Enter the path to the .jack file: ");
		fgets(destination, 256, stdin);
		destination[strcspn(destination, "\n")] = '\0'; // remove newline character
	} else if (argc == 2) {
		strcpy(destination, argv[1]);
		destination[strcspn(destination, "\n")] = '\0'; // remove newline character
	} else {
		printf("Invalid number of arguments.\n");
		return 1;
	}

	// open the .jack file
	FILE *fp = fopen(destination, "r");
	if (fp == NULL) {
		printf("Error opening file.\n");
		return 1;
	}

	xml = fopen("output.xml", "w");
	if (xml == NULL) {
		printf("Error opening file.\n");
		return 1;
	}

	fputs("<tokens>\n", xml);

	// read the .jack file and output each line
	char buffer[1024];
	while (fgets(buffer, 1024, fp) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0'; // remove newline character
		// feed buffer into function to determine tokens within line and add to xml file
		analyzeLine(buffer);
	}
	fclose(fp);

	fputs("</tokens>\n", xml);
	fclose(xml);
	
	return 0;
}