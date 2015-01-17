#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define BUFFER_SIZE 2000
#define NAME_SIZE 500
#define CRTYD_CLEARANCE 250000
#define CRTYD_WIDTH 50000
#define MAX_LIB_NR 500

//////////////////////////convert nanometers to double/////////////////////
double n2d(int nano)
{
	double milim;
	char str[20]; //string to put nanometers
	int i = 0;
	sprintf(str, "%d", nano); //convert nanometers to string
	for(i = 0; str[i] != '\0'; ++i) {} //find end of number
	if(nano >= 1000000 || nano <= -1000000) //move franctional part one position to the right and place dot
	{
		strncpy(str + i - 5, str + i - 6, 7);
		str[i - 6] = '.';
	}
	else if(nano >= 100000 || nano <= -100000) //move number two position to the right and place "0."
	{
		strncpy(str + i - 4, str + i - 6, 7);
		str[i - 6] = '0';
		str[i - 5] = '.';
	}
	else if(nano >= 10000 || nano <= -10000) //move number three position to the right and place "0.0"
	{
		strncpy(str + i - 2, str + i - 5, 6);
		str[i - 5] = '0';
		str[i - 4] = '.';
		str[i - 3] = '0';
	}
	else if(nano >= 1000 || nano <= -1000)
	{
		strncpy(str + i, str + i - 4, 5);
		str[i - 4] = '0';
		str[i - 3] = '.';
		str[i - 2] = '0';
		str[i - 1] = '0';
	}
	else if(nano >= 100 || nano <= -100)
	{
		strncpy(str + i + 2, str + i - 3, 4);
		str[i - 3] = '0';
		str[i - 2] = '.';
		str[i - 1] = '0';
		str[i - 0] = '0';
		str[i + 1] = '0';
	}
	else if(nano >= 10 || nano <= -10)
	{
		strncpy(str + i + 4, str + i - 2, 3);
		str[i - 2] = '0';
		str[i - 1] = '.';
		str[i - 0] = '0';
		str[i + 1] = '0';
		str[i + 2] = '0';
		str[i + 3] = '0';
	}
	else	//if value less than 10 nm, set it to 0;
	{
		strcpy(str, "0000000000000000000");
		if(nano != 0)
		printf("value %d nm rounded to %f\n", nano, atof(str));
	}
	milim = atof(str);
	return milim;
}
//////////////////////////round/////////////////////
int r05(int nano_unrounded)
{
	int nano_rounded;
	int nano_prerounded;
	nano_prerounded = nano_unrounded - (nano_unrounded % 50000);
	if(nano_unrounded >= 0)
		if(nano_unrounded - nano_prerounded >= 25000)
			nano_rounded = nano_prerounded + 50000;
	else
		nano_rounded = nano_prerounded;
	if(nano_unrounded < 0)
		if(nano_unrounded - nano_prerounded <= -25000)
			nano_rounded = nano_prerounded - 50000;
	else
		nano_rounded = nano_prerounded;
	return nano_rounded;
}


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
	char buff_beg[BUFFER_SIZE];
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;
	double line_start_x_double;
	double line_start_y_double;
	double line_end_x_double;
	double line_end_y_double;
	double line_width_double;
	double pad_pos_x_double;
	double pad_pos_y_double;
	double pad_size_x_double;
	double pad_size_y_double;
	int line_start_x;
	int line_start_y;
	int line_end_x;
	int line_end_y;
	int line_width;
	int pad_pos_x;
	int pad_pos_y;
	int pad_size_x;
	int pad_size_y;
	int i;
	int j;

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


	for (j = 0; strcmp (footprint_list[j].d_name, "||\\//||") != 0; ++j)
	{
		left = 0;
		right = 0;
		top = 0;
		bottom = 0;
		line_start_x = 0;
		line_start_y = 0;
		line_end_x = 0;
		line_end_y = 0;
		line_width = 0;
		pad_pos_x = 0;
		pad_pos_y = 0;
		pad_size_x = 0;
		pad_size_y = 0;
		printf("%s\n", footprint_list[j].d_name);

		sprintf(path, "./in.pretty/%s", footprint_list[j].d_name);
		if ((file_in = fopen (path, "rt")) == NULL)
		{
			perror ("Couldn't open input file");
			exit (1);
		}
		sprintf(path, "./out.pretty/%s", footprint_list[j].d_name);
		if ((file_out = fopen (path, "wt")) == NULL)
		{
			perror ("Couldn't open output file");
			exit (1);
		}
	
		for (fgets (buffer, BUFFER_SIZE, file_in); !feof (file_in); fgets (buffer, BUFFER_SIZE, file_in))
		{
			if ((strncmp("  (fp_line ", buffer, strlen("  (fp_line ")) == 0))
			{
				for (i = 0; strncmp (buffer + i - 1, "width ", strlen("width ")); ++i)
				{
					if (!strncmp (buffer + i, "start ", strlen("start ")))
					{
						sscanf(buffer + i + strlen("start "), "%lg", & line_start_x_double);
						sscanf(buffer + i + strlen("start "), "%s", search_buffer);
						sscanf(buffer + i + strlen("start ") + strlen(search_buffer), "%lg", & line_start_y_double);
						line_start_x = (int) (line_start_x_double * 1000000);
						line_start_y = (int) (line_start_y_double * 1000000);
						printf("sx %d sy %d ", line_start_x, line_start_y);
					}
					if (!strncmp (buffer + i, "end ", strlen("end ")))
					{
						sscanf(buffer + i + strlen("end "), "%lg", & line_end_x_double);
						sscanf(buffer + i + strlen("end "), "%s", search_buffer);
						sscanf(buffer + i + strlen("end ") + strlen(search_buffer), "%lg", & line_end_y_double);
						line_end_x = (int) (line_end_x_double * 1000000);
						line_end_y = (int) (line_end_y_double * 1000000);
						printf("ex %d ey %d ", line_end_x, line_end_y);
					}
					if (!strncmp (buffer + i, "width ", strlen("width ")))
					{
						sscanf(buffer + i + strlen("width "), "%lg", & line_width_double);
						line_width = (int) (line_width_double * 1000000);
						printf("lw %d\n", line_width);
					}

				}
			}
			else if ((strncmp("  (pad ", buffer, strlen("  (pad ")) == 0))
			{
				for (i = 0; strncmp (buffer + i, " (layers ", strlen(" (layers ")); ++i)
				{
					if (!strncmp (buffer + i, "at ", strlen("at ")))
					{
						sscanf(buffer + i + strlen("at "), "%lg", & pad_pos_x_double);
						sscanf(buffer + i + strlen("at "), "%s", search_buffer);
						sscanf(buffer + i + strlen("at ") + strlen(search_buffer), "%lg", & pad_pos_y_double);
						pad_pos_x = (int) (pad_pos_x_double * 1000000);
						pad_pos_y = (int) (pad_pos_y_double * 1000000);
					//	printf("px %d py %d ", pad_pos_x, pad_pos_y);
					}
					if (!strncmp (buffer + i, "size ", strlen("size ")))
					{
						sscanf(buffer + i + strlen("size "), "%lg", & pad_size_x_double);
						sscanf(buffer + i + strlen("size "), "%s", search_buffer);
						sscanf(buffer + i + strlen("size ") + strlen(search_buffer), "%lg", & pad_size_y_double);
						pad_size_x = (int) (pad_size_x_double * 1000000);
						pad_size_y = (int) (pad_size_y_double * 1000000);
					//	printf("dx %d dy %d\n", pad_size_x, pad_size_y);
					}
				}
			}


			if (line_start_x - line_width / 2 - CRTYD_CLEARANCE < left)
				left = line_start_x - line_width / 2 - CRTYD_CLEARANCE;
			if (line_end_x - line_width / 2 - CRTYD_CLEARANCE < left)
				left = line_end_x - line_width / 2 - CRTYD_CLEARANCE;
			if ((pad_pos_x - pad_size_x / 2 - CRTYD_CLEARANCE) < left)
				left = (pad_pos_x - pad_size_x / 2 - CRTYD_CLEARANCE);
			if (line_start_x - line_width / 2 - CRTYD_CLEARANCE > right)
				right = line_start_x + line_width / 2 + CRTYD_CLEARANCE;
			if (line_end_x + line_width / 2 + CRTYD_CLEARANCE > right)
				right = line_end_x + line_width / 2 + CRTYD_CLEARANCE;
			if ((pad_pos_x + pad_size_x / 2 + CRTYD_CLEARANCE) > right)
				right = (pad_pos_x + pad_size_x / 2 + CRTYD_CLEARANCE);
			if (line_start_y - line_width / 2 - CRTYD_CLEARANCE < top)
				top = line_start_y - line_width / 2 - CRTYD_CLEARANCE;
			if (line_end_y - line_width / 2 - CRTYD_CLEARANCE < top)
				top = line_end_y - line_width / 2 - CRTYD_CLEARANCE;
			if ((pad_pos_y - pad_size_y / 2 - CRTYD_CLEARANCE) < top)
				top = (pad_pos_y - pad_size_y / 2 - CRTYD_CLEARANCE);
			if (line_start_y - line_width / 2 - CRTYD_CLEARANCE > bottom)
				bottom = line_start_y + line_width / 2 + CRTYD_CLEARANCE;
			if (line_end_y - line_width / 2 - CRTYD_CLEARANCE > bottom)
				bottom = line_end_y + line_width / 2 + CRTYD_CLEARANCE;
			if ((pad_pos_y + pad_size_y / 2 + CRTYD_CLEARANCE) > bottom)
				bottom = (pad_pos_y + pad_size_y / 2 + CRTYD_CLEARANCE);


		}
	
		fseek (file_in, 0, 0);
		for (fgets (buffer, BUFFER_SIZE, file_in); !feof (file_in); fgets (buffer, BUFFER_SIZE, file_in))
		{printf("qwerasfdgdfa %s",buffer);
			if ((strncmp("  (fp_text reference ", buffer, strlen("  (fp_text reference ")) == 0))
			{printf("aaaaaa");
				for (i = 0; strncmp (buffer + i, "at ", strlen("at ")); ++i)
					buff_beg[i] = buffer[i];
				buff_beg[i] = '\0';
				fprintf (file_out, "%sat 0 %lg) (layer F.SilkS)\n", buff_beg, n2d(r05(top - 3500000)));
			}
			else if ((strncmp("  (fp_text value ", buffer, strlen("  (fp_text value ")) == 0))
			{
				for (i = 0; strncmp (buffer + i, "at ", strlen("at ")); ++i)
					buff_beg[i] = buffer[i];
				buff_beg[i] = '\0';
				fprintf (file_out, "%sat 0 %lg) (layer F.SilkS) hide\n", buff_beg, n2d(r05(top - 1500000)));
			}
			else
				fputs (buffer, file_out);	
		}

	}

	fclose (file_in);
	fclose (file_out);
	return 0;
}

