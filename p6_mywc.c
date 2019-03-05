/* Project 6: mywc(using forks and pipes) */

#include <stdio.h>

  struct Data
   {
        int c;  
        int w;
        int l;
   };

   struct Data mywc();
   int print();
   void error_exit(char *s);
   int pipe(int pd[2]);
   int fork(void);
   int read(int fd, void *buf, unsigned len);
   int write(int fd, void *buf, unsigned len);
   
   enum {OPT_C = 4, OPT_W = 2, OPT_L = 1};

    int main(int argc,char * argv[])
   {
      pid_t pid;
      int chars = 0;
      int words = 0;
      int lines = 0;
      int nfiles = 0;
      int opt = 0;
      int n, i, z, j;
	int k = 0;
      struct Data d;
      d.c = 0;
      d.w = 0;
      d.l = 0;
      int tot_words = 0;
      int tot_chars = 0;
      int tot_lines = 0;
      int pd[2];

	if(pipe(pd) == -1)
		error_exit("pipe() failed");
	
	if(*argv[1] == '-') //first arg in command line
		j = 2;
	else
	{
	opt = 0;
	j = 1;
	}
     for(i = j; i < argc; ++i)
	{	
	while((++argv)[0] && argv[0][0] == '-'){
		n = 1;
		while(argv[0][n]){
			switch(argv[0][n++]){
			case 'c': opt |= OPT_C;
				break;
			case 'w': opt |= OPT_W;
				break;
			case 'l': opt |= OPT_L;
				break;
			default:
				fprintf(stderr, "mywc: illegal option %c\n", argv[0][n-1]);
				fprintf(stderr, "usage: mywc \n");
				return -1;
			}
		}
	}

	if(!opt)
		opt = OPT_L | OPT_W | OPT_C;
	
	if(fork() == 0){ //start fork

          	      d = mywc(*argv, opt, &chars, &words, &lines);

		      if(write(pd[1], &d, sizeof(d)) == -1) 
		  //writing to pipe and checking for errors at the same time 
				error_exit("write() failed");
			if(i == (argc - 1))
				exit(1);
			return;
		      }	
	}

	for(i = 1; i < argc; ++i)
	{
	 if(read(pd[0], &d, sizeof(d)) == -1) 
	   //read and check errors at the same time 
		error_exit("read() failed");
 		 ++nfiles;
	tot_words += d.w;
 	tot_lines += d.l;
	tot_chars += d.c;
        }
 
	if(nfiles == 0)
		return;

        else  if(nfiles >  1)
                print("total", opt, tot_words, tot_chars, tot_lines);
	return 0;
   }

    struct Data mywc(const char *fname, int opt, int *tot_chars, 
		     int *tot_words, int *tot_lines) {
   
     struct Data d;
      int char_count = 0;
      int line_count = 0;
      int word_count = 0;
      int inword = 0;
      int c;
      FILE *fp;
   
      if(fname[0] != '-')
	fp = fopen(fname, "r"); //opening file(fname) and reading 
	else
		fp = stdin;

      if(!fp) //if file doesnt exist, exit!
		exit(1);
   
      while((c = getc(fp)) != EOF) { //c = current letter
         ++char_count; 
         if(c == '\n')
            ++line_count;
         if(inword == 0 && !isspace(c)){ 
	   //if there is no space, increase word count and inword becomes 1
            ++word_count;
            ++inword;
         }
         else if(inword == 1 && isspace(c))
	   //in a word, go through word find space then decrease inword 0
            --inword;
      }
   
      print(fname, opt, word_count, char_count, line_count);
	
	if(fname[0] != '-')
	  fclose(fp); //close file 

      *tot_chars += char_count;
      *tot_words += word_count;
      *tot_lines += line_count;
      //get all char, word, line and get sum up for total 

      d.c = *tot_chars;
      d.w = *tot_words;
      d.l = *tot_lines;
      //pass into struct
      //assiging pointer to char to d.c (data.char)

      return d; //return struct
   }

    int print(const char *fname, int opt, int words, int chars, int lines) {
      if(opt & OPT_L)
		printf("Lines: %d  ", lines);
	if(opt & OPT_W)
		printf("Words: %d  ", words);
	if(opt & OPT_C)
		printf("Chars: %d  ", chars);
	if(fname[0] != '-')
		printf("File Name: %s  ", fname);
	printf("Process Id: %d  ", getpid());

	putchar('\n');

      return 0;
   }

   void error_exit(char *s)
   {
	fprintf(stderr, "\nERROR: %s\n", s);
	exit(1);
   } 
