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
#include <string.h>


int main(int argc, char *argv[]) {
	char destination[256];

	// get destination from user input either via command line or stdin
	if (argc == 1) {
		
		printf("Enter the path to the .jack file: ");
		fgets(destination, 256, stdin);
		destination[strcspn(destination, "\n")] = '\0'; // remove newline character
	} else if (argc == 2) {
		strcpy(destination, argv[1]);
		destination[strcspn(destination, "\n")] = '\0'; // remove newline character
	}

	FILE *fp = fopen(destination, "r");
	if (fp == NULL) {
		printf("Error opening file.\n");
		return 1;
	}

	char buffer[1024];

	if (fgets(buffer, 1024, fp) != NULL) {
		puts(buffer);
	}
	fclose(fp);
	
	return 0;
}