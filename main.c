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

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Global variables
FILE *xml; // The file pointer for the XML output file

/**
 * @brief An array of keywords.
 *
 * This array contains a list of all the keywords in the Jack language.
 */
char* keyword[] = {
	"class", "constructor", "function", "method", "int", "boolean", "char", "void", "var", "static", "field", "let", "do", "if", "else", "while", "return", "true", "false", "null", "this"
};

/**
 * @brief An array of symbols.
 *
 * This array contains a list of all the symbols in the Jack language.
 */
char symbol[] = {
	'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
};

/**
 * @brief Boolean indicating if the parser is currently inside a multi-line comment.
 */
bool multiLineComment = false;

/**
 * @brief Boolean indicating if the parser is currently inside a string literal.
 */
bool stringLiteral = false;

/**
 * @brief Checks if a given character is a delimiter.
 *
 * @param c The character to check.
 * @return True if it is a delimiter, False otherwise.
 */
bool isDelimiter(char c) {
	// Loop through the symbols and check if the given character is a symbol or whitespace
	for (int i = 0; i < sizeof(symbol)/sizeof(symbol[0]); i++) {
		if (c == symbol[i] || c == ' ' || c == '\t' || c == '\n') {
			return true;
		}
	}
	return false;
}

/**
 * @brief Checks if a given character is a symbol.
 *
 * @param c The character to check.
 * @return True if it is a symbol, False otherwise.
 */
bool isSymbol(char c) {
	// Loop through the symbols and check if the given character is a symbol
	for (int i = 0; i < sizeof(symbol)/sizeof(symbol[0]); i++) {
		if (c == symbol[i]) {
			return true;
		}
	}
	return false;
}

/**
 * Checks if a given string is a keyword.
 *
 * @param c The string to check.
 * @return True if it is a keyword, False otherwise.
 */
bool isKeyword(char* c) {
	// Loop through the keywords and compare each one to the input string.
	for (int i = 0; i < sizeof(keyword)/sizeof(keyword[0]); i++) {
		if (strcmp(c, keyword[i]) == 0) {
			return true;
		}
	}
	return false;
}

/**
 * Extracts a substring from a given string.
 *
 * @param str The input string.
 * @param start The starting index of the substring.
 * @param end The ending index of the substring.
 * @return The extracted substring.
 */
char* getSubstring(char* str, int start, int end) { 
    // Calculate the length of the input string and the length of the substring.
    int length = strlen(str); 
    int subLength = end - start + 1; 
    
    // Allocate memory for the substring and copy the substring from the input string.
    char* subStr = (char*)malloc((subLength + 1) * sizeof(char)); 
    strncpy(subStr, str + start, subLength); 
    
    // Null-terminate the substring.
    subStr[subLength] = '\0'; 
    return subStr; 
} 

/**
 * Analyzes a line of code and prints XML tags representing different parts of the code.
 *
 * @param buffer The line of code to analyze.
 */
void analyzeLine(char *buffer) {
	// Initialize variables to keep track of the left and right indices,
	// and the length of the buffer.
	int left = 0, right = 0;
	int len = strlen(buffer);

	// Loop through the buffer, character by character.
	while (right <= len && left <= right) {
		// If the buffer contains "//", it means the rest of the line
		// is a comment and we can stop analyzing.
		if (buffer[right] == '/' && buffer[right+1] == '/') {
			return;
		}

		// If the buffer contains "/*" and we are not already in a
		// multiline comment, it means we just started a multiline
		// comment and we should set the flag to true.
		if (buffer[right] == '/' && buffer[right+1] == '*' && !multiLineComment) {
			multiLineComment = true;
			continue;
		}

		// If the buffer contains "*/" and we are in a multiline comment,
		// it means we just ended a multiline comment and we should set
		// the flag to false.
		if (buffer[right] == '*' && buffer[right+1] == '/' && multiLineComment) {
			multiLineComment = false;
			right += 2;
			left = right;
			continue;
		}

		// If the buffer contains a double quote and we are not already
		// in a string literal, it means we just started a string literal
		// and we should set the flag to true.
		if (buffer[right] == '"' && !stringLiteral && !multiLineComment) {
			right++;
			left = right;
			stringLiteral = true;
		}

		// If the buffer contains a double quote and we are in a string
		// literal, it means we just ended a string literal and we should
		// print the contents of the string literal as an XML tag, then set
		// the flag to false.
		if (buffer[right] == '"' && stringLiteral && !multiLineComment) {
			fprintf(xml, "<stringConstant> %s </stringConstant>\n", getSubstring(buffer, left, right-1));
			right++;
			left = right;
			stringLiteral = false;
		}

		// If the current character is not a delimiter or we are in a
		// multiline comment or string literal, move the right index forward.
		if (!isDelimiter(buffer[right]) || multiLineComment || stringLiteral)
			right++;
		
		// If the current character is a delimiter and we are not in a
		// multiline comment or string literal, check if it is a symbol
		// and print it as an XML tag.
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
			right++; // Move the right index forward.
			left = right; // Update the left index.
		} 
		else if (isDelimiter(buffer[right]) && left != right && !multiLineComment && !stringLiteral) {
			// Get the substring between left and right indices.
			char* token = getSubstring(buffer, left, right-1);

			// Check if the token contains only digits.
			int i = 0;
			while (isdigit(token[i])) 
				i++;

			// If the token contains only digits, print it as an XML tag. 
			if (i == strlen(token)) {
				fprintf(xml, "<integerConstant> %s </integerConstant>\n", token);
			} 
			// If the token is a keyword, print it as an XML tag. 
			else if (isKeyword(token)) {
				fprintf(xml, "<keyword> %s </keyword>\n", token);
			} 
			// If the token is not a keyword, check if it is a valid identifier.
			else {
				// If the token starts with a letter or underscore, print it as an XML tag.
				if (isalpha(token[0]) || token[0] == '_')
					fprintf(xml, "<identifier> %s </identifier>\n", token);
				// If the token does not start with a letter or underscore, print an error message.
				else
					printf("Invalid token: %s\nMost likely a number at the start of an identifier\n", token);
			} 
			
			left = right; // Update the left index.
		}
	}
}

/**
 * Main function to tokenize a .jack file and output an .xml file.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char const *argv[]) {
	/* Destination path of the .jack file. */
	char destination[256];

	// Get destination from user input either via command line or stdin
	if (argc == 1) {
		printf("Enter the path to the .jack file: ");
		fgets(destination, 256, stdin);
		destination[strcspn(destination, "\n")] = '\0'; // Remove newline character
	} else if (argc == 2) {
		strcpy(destination, argv[1]);
		destination[strcspn(destination, "\n")] = '\0'; // Remove newline character
	} else {
		printf("Invalid number of arguments.\n");
		return 1;
	}

	// Open the .jack file
	FILE *fp = fopen(destination, "r");
	if (fp == NULL) {
		printf("Error opening file.\n");
		return 1;
	}

	// Pointers to the start and end of the filename.
	char* start = NULL;
	char* end = NULL;

	// Find the start and end of filename
	start = strrchr(destination, '\\');
	if (start == NULL)
		start = strrchr(destination, '/');
	if (start == NULL)
		start = destination;
	else
		start++;

	end = strrchr(destination, '.');

	// Calculate the length of the output filename
	size_t len = end - start;
	// Allocate memory for the output filename
	char* outputName = (char*) malloc(len + 6); // 1 for T and 4 for ".xml" and 1 for null terminator
	if (outputName == NULL) {
		printf("Memory allocation error.\n");
		fclose(fp);
		return 1;
	}

	// Copy the filename into the output filename
	memcpy(outputName, start, len);
	strcpy(outputName + len, "T.xml");
	
	// Open the output .xml file
	xml = fopen(outputName, "w");
	if (xml == NULL) {
		printf("Error opening file.\n");
		free(outputName);
		fclose(fp);
		return 1;
	}

	// Write the beginning of the xml file
	fputs("<tokens>\n", xml);

	// Read the .jack file and output each line
	char buffer[1024];
	while (fgets(buffer, 1024, fp) != NULL) {
		buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
		// Feed buffer into function to determine tokens within line and add to xml file
		analyzeLine(buffer);
	}
	fclose(fp);

	// Write the end of the xml file
	fputs("</tokens>\n", xml);
	fclose(xml);
	
	return 0;
}
