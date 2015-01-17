#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define BUFFER_SIZE 2000
#define NAME_SIZE 500
#define MAX_LIB_NR 500


int main (void)
{
	FILE *file_in;
	FILE *file_out;
	DIR *dir;
	struct dirent *ep;
	struct dirent footprint_list[MAX_LIB_NR];
	char buffer[BUFFER_SIZE];
	char search_buffer[BUFFER_SIZE];
	char path[NAME_SIZE];
	int i;
	int j;
	int cpy;

	dir = opendir ("./in.pretty");
	if (dir != NULL)
	{
		for (i = 0; (ep = readdir (dir)) != NULL; ++i)
		{
			if (strncmp (ep->d_name + strlen (ep->d_name) - strlen (".kicad_mod"), ".kicad_mod", strlen (".kicad_mod")) == 0) //add only .kicad_mod files
				footprint_list[i] = *ep;
			else
				--i;
		}
		closedir (dir);
		strcpy (footprint_list[i].d_name, "||\\//||"); //add "||\\//||" at the end of the table to recognize when it finishes
	}
	else
	{
		perror ("Couldn't open ./in.pretty directory");
		exit (1);
	}


	for (i = 0; strcmp (footprint_list[i].d_name, "||\\//||") != 0; ++i)
	{
		printf("%s\n", footprint_list[i].d_name);

		sprintf(path, "./in.pretty/%s", footprint_list[i].d_name);
		if ((file_in = fopen (path, "rt")) == NULL)
		{
			perror ("Couldn't open input file");
			exit (1);
		}
		sprintf(path, "./out.pretty/%s", footprint_list[i].d_name);
		if ((file_out = fopen (path, "wt")) == NULL)
		{
			perror ("Couldn't open output file");
			exit (1);
		}
	
		for (fgets (buffer, BUFFER_SIZE, file_in); !feof (file_in); fgets (buffer, BUFFER_SIZE, file_in))
			if ((strncmp("  (fp_line ", buffer, strlen("  (fp_line ")) == 0))
			{
				cpy = 0;
				for (j = 0; strncmp (buffer + j - 1, "width ", strlen("width ")); ++j)
					if (!strncmp (buffer + j, "F.CrtYd", strlen("F.CrtYd")))
						cpy = 1;
				if (cpy == 0)
						fputs (buffer, file_out);
			}
			else
				fputs (buffer, file_out);
	}

	fclose (file_in);
	fclose (file_out);
	return 0;
}

