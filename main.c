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

	//printf("[DEBUG] Analyzing line: \"%s\"\n", buffer);
	while (right <= len && left <= right) {
		//printf("[DEBUG] Current token: \"%.*s\"\n", right - left, buffer + left);
		//printf("[DEBUG] Current position: (%d, %d)\n", left, right);
		if (buffer[right] == '/' && buffer[right+1] == '/') {
			//printf("[DEBUG] Found comment, breaking loop\n");
			return;
		}

		if (buffer[right] == '/' && buffer[right+1] == '*' && !multiLineComment) {
			//printf("[DEBUG] Found start of multi line comment, enabling multi line comment mode\n");
			multiLineComment = true;
		}

		if (buffer[right] == '*' && buffer[right+1] == '/' && multiLineComment) {
			//printf("[DEBUG] Found end of multi line comment, disabling multi line comment mode\n");
			multiLineComment = false;
			right += 2;
			left = right;
			continue;
		}

		if (buffer[right] == '"' && !stringLiteral) {
			//printf("[DEBUG] Found start of string literal, enabling string literal mode\n");
			right++;
			left = right;
			stringLiteral = true;
		}

		if (buffer[right] == '"' && stringLiteral) {
			//printf("[DEBUG] Found end of string literal, disabling string literal mode\n");
			printf("<stringConstant> %s </stringConstant>\n", getSubstring(buffer, left, right-1));
			right++;
			left = right;
			stringLiteral = false;
		}

		if (!isDelimiter(buffer[right]) || multiLineComment || stringLiteral)
			right++;
		
		if (isDelimiter(buffer[right]) && left == right && !multiLineComment && !stringLiteral) {
			if (isSymbol(buffer[right])) {
				//printf("[DEBUG] Found symbol: '%c'\n", buffer[right]);
				switch(buffer[right]) {
						case '<':
							printf("<symbol> &lt; </symbol>\n");
							break;
						case '>':
							printf("<symbol> &gt; </symbol>\n");
							break;
						case '"':
							printf("<symbol> &quot; </symbol>\n");
							break;
						case '&':
							printf("<symbol> &amp; </symbol>\n");
							break;
						default:
							printf("<symbol> %c </symbol>\n", buffer[right]);
							brea
					}
			}
			right++;
			left = right;
		} else if (isDelimiter(buffer[right]) && left != right && !multiLineComment && !stringLiteral) {
			char* token = getSubstring(buffer, left, right-1);
			//printf("[DEBUG] Found token: \"%s\"\n", token);

			if (isKeyword(token)) {
				printf("<keyword> %s </keyword>\n", token);
			} else {
				printf("<identifier> %s </identifier>\n", token);
			}
			left = right;
		}
	}






	/*
	//printf("buffer: \"%s\"\n", buffer);
	int i = 0;

	// while not at end of line and not working inside multi line comment
	if (!multiLineComment) {
		
		char bufCpy[strlen(buffer)+1];
		strcpy(bufCpy, buffer);
		char* token = strtok(bufCpy, " {}()[].,;+-/*&|<>=~\t");
		while (token != NULL) {
			// check for keyword
			for (int j = 0; j < sizeof(keyword)/sizeof(keyword[0]); j++) {
				if (strcmp(token, keyword[j]) == 0) {
					printf("<keyword> %s </keyword>\n", keyword[j]);
				}
			}
			token = strtok(NULL, " {}()[].,;+-/*&|<>=~\t");
		}
		

		while (buffer[i] != '\0') {
			// single line comment, skip the line
			if (buffer[i] == '/' && buffer[i+1] == '/') {
				return;
			}
			// check if working with multi line comment
			else if (buffer[i] == '/' && buffer[i+1] == '*') {
				multiLineComment = true;
				break;
			}
			// skip whitespace
			else if (buffer[i] == ' ') { 
				i++;
				continue;
			}

			// check for symbol
			for (int j = 0; j < sizeof(symbol)/sizeof(symbol[0]); j++) {
				if (buffer[i] == symbol[j]) {
					switch(symbol[j]) {
						case '<':
							printf("<symbol> &lt; </symbol>\n");
							break;
						case '>':
							printf("<symbol> &gt; </symbol>\n");
							break;
						case '"':
							printf("<symbol> &quot; </symbol>\n");
							break;
						case '&':
							printf("<symbol> &amp; </symbol>\n");
							break;
						default:
							printf("<symbol> %c </symbol>\n", symbol[j]);
					}
				}
			}
			i++;
		}
	}

	// while not at end of line and working inside multi line comment
	while (buffer[i] != '\0' && multiLineComment) {
		if (buffer[i] == '*' && buffer[i+1] == '/') {
			multiLineComment = false;
		}
		i++;
	}
	*/
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

	// read the .jack file and output each line
	char buffer[1024];
	while (fgets(buffer, 1024, fp) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0'; // remove newline character
		// feed buffer into function to determine tokens within line and add to xml file
		analyzeLine(buffer);
	}
	fclose(fp);
	
	return 0;
}