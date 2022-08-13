#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>

#define LIMIT 256 // limit for string
#define ARR_LIMIT 0

int main(int argc, char* argv[]) {
	FILE * fp; // file pointer
	char **front = malloc(ARR_LIMIT*8); // dymallocated arrays
	char **back = malloc(ARR_LIMIT*8);
	int len;

	// load input
	if (argc > 1) {
		if (strcmp(argv[1],"--help")==0) {
			//help and exit
			printf("Usage: plenki filename\nfile format which is read and written is:\nfront1\nback1\nfront2\nback\n\nIn the program many keys are used to achieve functions\nsome of these functions will take a number written before them\n\n a - appends card to the end + n\n r - puts the card n cards back\n n - creates a new card\n e - edits card\n^S - saves file\n\n");
			exit(1);
		} else {
			fp = fopen(argv[1], "r");
			bool leftfill = true; // which side to fill.
			int idx = 0; // access with ceil(idx/2)

			//fill buffer
			char * buffer = malloc(LIMIT);
			
			while (fgets(buffer,LIMIT,fp)!=NULL) { // true if not end of fill
				if ((strlen(buffer)>0) && (buffer[strlen(buffer)-1] == '\n'))
				buffer[strlen(buffer)-1] = '\0';
				if (leftfill) {
					front = realloc(front,sizeof(char*)*(idx+1)); // memory for another
					front[idx]=malloc(strlen(buffer)+1); // copy string from buf
					strcpy(front[idx],buffer);
				} else {	
					back = realloc(back,sizeof(char*)*(idx+1)); // memory for another
					back[idx]=malloc(strlen(buffer)+1);
					strcpy(back[idx],buffer);
					idx++;
				}
				leftfill=!leftfill;
			}

			free(buffer);
			len = idx;
			front = realloc(front,sizeof(char*)*len);
			back = realloc(back,sizeof(char*)*len);
					printf("%d %s\r\n",len,front[0]);

		}
	}

	fclose(fp);
	//testing file read
	for (int i=0; i<len; i++) {
		printf("%s - %s\n", front[i],back[i]);
	}
	//exit(1);

	//main loop


	system("/bin/stty raw"); //immediate input

	// for centering text
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	bool cont = true; 
	bool pop = false; //like a function, pop puts everything forward
	
	bool flip = false;
	bool inverse = false;

	while (cont) {
		if (pop) {
			for (int i=0; i<len-1; i++) {
				printf("%d %d %s %s %s %s %s\r\n", i,len, front[0], front[1], front[2],front[3],front[4]);
				front[i]=realloc(front[i],strlen(front[i+1])+1);
				strcpy(front[i],front[i+1]);
				back[i]=realloc(back[i],strlen(back[i+1])+1);
				strcpy(back[i],back[i+1]);
			}
			//free(front[len]); PROBS NOT FREED I THINK BUT JUST A VAR NO MATTER PROBS
			//free(back[len]);
			len--;
			front = realloc(front,sizeof(char*)*len);
			back = realloc(back,sizeof(char*)*len);
			pop=false;
		}

		// display
		
		system("clear");
		
		int pad = (w.ws_col - 14) / 2;
		printf("%*.*s--- PLENKI ---\r\n", pad, pad, " ");

		printf("%*.*squeue:%d,",pad,pad," ",3);
		if (inverse) {
			printf("inverted");
		} else {
			printf("rect");
		}
		puts("\r\n\n");
		
		if (front[0]!=NULL) {
			if ((!flip && !inverse) || (flip && inverse)) {
				pad = (w.ws_col - strlen(front[0])) / 2;
				printf("%*.*s%s", pad, pad, " ",front[0]);
			} else {
				pad = (w.ws_col - strlen(back[0])) / 2;
				printf("%*.*s%s", pad, pad, " ",back[0]);
			}
		}


		// input
		char c;
		int v=0; //input value;
		bool act=false; //stop looking for input
		int idx;
		
		while (!act) {
			c=getchar();
			if (isdigit(c)!=0) {
				v=v*10;
				v+=c-'0';
			}
			switch (c) {
				case 3: // control c
					cont = false;
					act = true;
					puts("\r");
					break;
				case 27: // escape
					act = true;
					break;
				case ' ':
					flip = !flip;
					act = true;
					break;
				case 'a': //append

					idx=0;
					if (v==0) // a == 1a
						v++;

					while (v>0) {
						if (idx>=len) {
							len++;
							front = realloc(front,sizeof(char*)*len);
							back = realloc(back,sizeof(char*)*len);
							front[len-1] = malloc(1);
							strcpy(front[len-1],"");
							back[len-1] = malloc(1);
							strcpy(back[len-1],"");
							v--;
						} else if (strcmp(front[idx],"")==0) {
							v--;
						}
						idx++;
					}
					
					idx--; //idx now the spot to put
					front[idx]=realloc(front[idx],strlen(front[0])+1);
					strcpy(front[idx],front[0]);
					back[idx]=realloc(back[idx],strlen(back[0])+1);
					strcpy(back[idx],back[0]);
						

					act=true;
					pop=true;
					break;
				case 'r': //append l4 v1
					
					idx=0;
					if (v==0) // v == 1v
						v++;

					while (v+1>len) {
						len++;
						front = realloc(front,sizeof(char*)*len);
						back = realloc(back,sizeof(char*)*len);
						front[len-1] = malloc(1);
						strcpy(front[len-1],"");
						back[len-1] = malloc(1);
						strcpy(back[len-1],"");
					}

					len++;	
					front = realloc(front,sizeof(char*)*len);
					back = realloc(back,sizeof(char*)*len);

					front[len-1] = malloc(1);
					strcpy(front[len-1],"");
					back[len-1] = malloc(1);
					strcpy(back[len-1],"");
				

					for (int i=len-1; i>v; i--) {
						front[i]=realloc(front[i],strlen(front[i-1])+1);
						strcpy(front[i],front[i-1]);
						back[i]=realloc(back[i],strlen(back[i-1])+1);
						strcpy(back[i],back[i-1]);
					}
					front[v]=realloc(front[v],strlen(front[0])+1);
					strcpy(front[v],front[0]);
					back[v]=realloc(back[v],strlen(back[0])+1);
					strcpy(back[v],back[0]);
					
						

					act=true;
					pop=true;
					break;
				case 19: //control s save
					system("/bin/stty cooked"); //immediate input off
					puts("\r\n");
					printf("filename: ");
					char fname[256];
					scanf("%256s",fname);
					fp = fopen(fname,"w");
					for (int i=0; i<len; i++) {
						fprintf(fp,"%s\n%s\n",front[i],back[i]);
					}
					system("/bin/stty raw"); //immediate input on
					act=true;
					break;
				case 's': //shuffle:

					break;
				case 'n': //new
					system("/bin/stty cooked"); //immediate input off
					puts("\r\n");
					printf("fore: ");
					char nfore[256];
					scanf("%256s",nfore);
					printf("back: ");
					char nback[256];
					scanf("%256s",nback);
					
					while (true) {
						if (idx>=len) {
							len++;
							front = realloc(front,sizeof(char*)*len);
							back = realloc(back,sizeof(char*)*len);
							front[len-1] = malloc(strlen(nfore)+1);
							strcpy(front[len-1],nfore);
							back[len-1] = malloc(strlen(nback)+1);
							strcpy(back[len-1],nback);
							
							break;
						} else if (strcmp(front[idx],"")==0) {
							front[idx] = realloc(front[idx],strlen(nfore)+1);
							strcpy(front[idx],nfore);
							back[idx] = realloc(back[idx],strlen(nback)+1);
							strcpy(back[idx],nback);
							break;
						}
						idx++;
					}
					
					system("/bin/stty raw"); //immediate input on
					act=true;
					break;
				case 'e': //edit
					puts("\n\r");
					char forebuf[LIMIT];
					char backbuf[LIMIT];
					int sidx=0;
					bool cancel_edit=false;

					strcpy(forebuf,front[0]);
					strcpy(backbuf,back[0]);
					
					// for realtime editing
					printf("fore: %s",forebuf);
					sidx=strlen(forebuf);
					while (!cancel_edit) {
						char c=getchar();			
						if (c==27 || c==3) {
							cancel_edit=true;
							break;
						} else if (c==13) {
							front[0]=realloc(front[0],strlen(forebuf)+1);
							strcpy(front[0],forebuf);
							break;
						} else {
							if (c>31 && c<127) {		
								forebuf[sidx]=c;
								forebuf[sidx+1]='\0';
								sidx++;
							} else if (c==127 && sidx>0) {
								sidx--;
								forebuf[sidx]='\0';
							} 

							printf("\33[2K\rfore: %s",forebuf);
						
						}

					}
					puts("\r");
					printf("back: %s",backbuf);
					sidx=strlen(backbuf);
					while (!cancel_edit) {
						char c=getchar();			
						if (c==27 || c==3) {
							cancel_edit=true;
							break;
						} else if (c==13) {
							back[0]=realloc(back[0],strlen(backbuf)+1);
							strcpy(back[0],backbuf);
							break;
						} else {
							if (c>31 && c<127) {		
								backbuf[sidx]=c;
								backbuf[sidx+1]='\0';
								sidx++;
							} else if (c==127 && sidx>0) {
								sidx--;
								backbuf[sidx]='\0';
							} 

							printf("\33[2K\rback: %s",backbuf);
						
						}

					}
					act=true;
					break;

			}
		}

	}

	for (int i=0; i<len; i++) {
		free(front[i]);
		free(back[i]);
	}
	free(front);
	free(back);

	system("/bin/stty cooked"); //immediate input off
}
