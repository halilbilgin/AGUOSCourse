#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* loadFile(char *filename) {
	char *buffer;
	long lSize;
	FILE* fp = fopen(filename, "r");
	if(! fp)
		fclose(fp),fputs("file does not exist or not accesible",stderr),exit(1);

	fseek( fp , 0 , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	buffer = (char *) calloc( 1, lSize+1 );
	if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);

	if( 1!=fread( buffer , lSize, 1 , fp) )
  	fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

	return buffer;
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* DoWorkInChild(char *searchString, char *filename) {
	char *result = "";
	char *text = loadFile(filename);

    size_t n = strlen( searchString );

    /* get the first line */
    char *token;
    token = strtok(text, "\n");
    int line = 1;
    char *tmp;

    /* walk through other lines */
    while( token != NULL ) {
		char *p = token;

		while ( ( p = strstr( p, searchString ) ) != NULL )
		{
			 char *q = p + n;
			 if ( p == text || isblank( ( unsigned char ) *( p - 1 ) ) )
			 {

				 if ( *q == '\0' || isblank( ( unsigned char ) *q ) ) {

					 tmp = (char *) malloc(strlen(filename) +
			 	 	 	 	 strlen(token) + 30);
					 sprintf(tmp, "%s, %d:%s\n", filename, line, token);
					 result = concat(result, tmp);
					 free(tmp);

					 break;
				 }
			 }
			 p = q;
		}
        token = strtok(NULL, "\n");
        line++;
    }

    free(text);
    free(token);
    return result;
}
