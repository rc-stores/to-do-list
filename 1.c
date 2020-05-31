#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void set_Task();
void show_Task();

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		fprintf(stderr, "Error: command line should contain one of the keywords: \"set\" or \"show\". Try again!\n");
		return 1;
	}
	else if (strcmp(argv[1], "set") == 0)
		set_Task();
	else if (strcmp(argv[1], "show") == 0)
		show_Task();
	else
	{
		fprintf(stderr, "Error: can't recognize the keyword in the command line. Terminating...\n");
		return 2;
	}
	return 0;
}

void put_Line(char *);

void set_Task()
{
	char task[128], newline[256];
	int day, month, year;
	char strday[2], strmon[2], *time;

	printf("Enter a name of the event: ");
	if (gets(task) == NULL)
	{
		fprintf(stderr, "Error: wrong format of input. Terminating...\n");
		exit(3);
	}

	printf("Enter date and time as follows: \"dd.mm.yyyy hh:mm\": ");
	if (scanf("%d.%d.%d %s", &day, &month, &year, time) != 4)
	{
		fprintf(stderr, "Error: wrong format of input. Terminating...\n");
		exit(3);
	}
	for (int i = 1; i > -1; i--, day /= 10, month /= 10)
	{
		if (day != 0)
			strday[i] = day % 10 + '0';
		else
			strday[i] = '0';
		if (month != 0)
			strmon[i] = month % 10 + '0';
		else
			strmon[i] = '0';
	}
	sprintf(newline, "%d-%s-%s %s %s", year, strmon, strday, time, task);
	put_Line(newline);
}

void put_Line(char *newline)
{
	FILE *fp, *tmp;
	char line[256];
	int found = 0;
	fpos_t position;

	if ((fp = fopen("todolist.txt", "r+")) == NULL)
	{
		if ((fp = fopen("todolist.txt", "w")) == NULL)
		{
			fprintf(stderr, "Error: can't open a file for writing the task. Terminating...\n");
			exit(4);
		}
		else
			fprintf(fp, "%s\n", newline);
	}
	else
	{
		while (fgetpos(fp, &position) == 0 && fgets(line, 256, fp) != NULL)
			if (strncmp(line, newline, 16) > 0)
			{
				found = 1;
				break;
			}
		if (found)
		{
			if ((tmp = fopen("temp.txt", "w+")) == NULL)
			{
				fprintf(stderr, "Error: can't open a temporary file for rewriting. Terminating...\n");
				exit(4);
			}
			fputs(line, tmp);
			while (fgets(line, 256, fp) != NULL)
			{
				fputs(line, tmp);
			}
			fsetpos(fp, &position);
			fprintf(fp, "%s\n", newline);
			rewind(tmp);
			while (fgets(line, 256, tmp) != NULL)
				fputs(line, fp);
			remove("temp.txt");
		}
		else
		{
			fsetpos(fp, &position);
			fprintf(fp, "%s\n", newline);
		}
	}
	printf("New task is set!\n");
	fclose(fp);
	fclose(tmp);
}

void print_Task(char *, int);

void show_Task()
{
	char line[128], strday[2], strmon[2], timebuf[16], *timestr;
	int day, month, year, cond;
	time_t rawtime;
	struct tm *curr_t;
	int mode = 0;

	printf("Possible input:\n"
		   "1) if you want to look at the current tasks, enter \"today\";\n"
		   "2) if you want to look at every task of the specific date, point that day out as follows: \"dd.mm.yyyy;\n"
		   "3) if there is a particular time of some day that you're looking for, enter the whole date: \"dd.mm.yyyy hh:mm\".\n");

	if (scanf("%d.%d.%d", &day, &month, &year) == 3)
	{
		if (getchar() == '\n')
			timestr = "00:00";
		else if (gets(timebuf) != NULL)
		{
			timestr = timebuf;
			mode = 1;
		}
		else
		{
			fprintf(stderr, "Error: wrong format of input. Terminating...\n");
			exit(3);
		}
	}
	else if (scanf("%s", line) == 1 && strcmp(line, "today") == 0)
	{
		time(&rawtime);
		curr_t = localtime(&rawtime);
		day = curr_t->tm_mday;
		month = curr_t->tm_mon + 1;
		year = 1900 + curr_t->tm_year;
		timestr = "00:00";
	}
	else
	{
		fprintf(stderr, "Error: wrong format of input. Terminating...\n");
		exit(3);
	}
	for (int i = 1; i > -1; i--, day /= 10, month /= 10)
	{
		if (day != 0)
			strday[i] = day % 10 + '0';
		else
			strday[i] = '0';
		if (month != 0)
			strmon[i] = month % 10 + '0';
		else
			strmon[i] = '0';
	}
	strday[2] = strmon[2] = '\0';

	sprintf(line, "%d-%s-%s %s", year, strmon, strday, timestr);
	print_Task(line, mode);
}

void print_Task(char *line, int mode)
{
	FILE *fp;
	char fline[256];
	int n;

	n = 0;
	if ((fp = fopen("todolist.txt", "r")) == NULL)
	{
		fprintf(stderr, "Error: can't open a file to read your tasks. Terminating...\n");
		exit(4);
	}
	else
		while (fgets(fline, 256, fp) != NULL && strncmp(fline, line, 10) <= 0)
		{
			if (strncmp(fline, line, 16) >= 0)
			{
				n++;
				printf("%s\n", fline + 11);
				if (mode)
					break;
			}
		}
	if (n == 0)
		printf("There are no plans set on this date.\n");
}