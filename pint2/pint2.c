#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>

void text_set(char ** text, int line, char* new_text) {
	if (text[line]) free(text[line]);
	text[line] = malloc(strlen(new_text)+1);
	strcpy(text[line],new_text);
}

int digits_in(int n) {
	if (n < 10) return 1;
	return 1 + digits_in (n / 10);
}

int main(int argc, char * argv[]) {
	
	char ** text;
	int text_lines=1;
	
	int selx=0;
	int sely=0;

	int w_top=0;
	int w_left=0;

	struct winsize max;
	ioctl(0, TIOCGWINSZ , &max);

	bool update=true;
	char c;
	char cont=true;
	int changed=false;
	char status[255];

	text = malloc(text_lines * 8 * 1);
	text_set(text,0,"");

	
	// open file
	if (argc>1) {
		FILE *fptr;

		if ((fptr = fopen(argv[1],"r")) == NULL){
			printf("Error! opening file");
			exit(1);

		}
		size_t len;
		char * line;
		int i=0;
		while (getline(&line, &len, fptr) != -1) {
			text_lines++;
			text_set(text,i,line);
			text[i][strlen(text[i])-1]='\0';
			text[i]=realloc(text[i],strlen(text[i])+1);
			i++;
		}
		text_lines--;

		if (line)
			free(line);

		fclose(fptr);
	}

	system("/bin/stty raw");

	while (cont) {
		if (update) {
			// do ui
			system("clear");
			printf("\r\e[?25l\033[7m Pint 2.0 \033[m - exclusive Hamud edition\n\r");
			for (int i=w_top; i<w_top + max.ws_row-3; i++) {
				if (text_lines>i-w_top) {
					for (int j=w_left; j<w_left+max.ws_col; j++) {
						if (i==selx && j==sely) {
							printf("\033[7m");
						}
						if (j<strlen(text[i])) {
							printf("%c",text[i][j]);
						} else if (j>=strlen(text[i])) {
							printf(" ");
						}
						if (i==selx && j==sely) {
							printf("\033[m");
						}
					}
					puts("\r");
				} else {
					puts("");
				}
			}
			printf("\033[7m^W\033[m: Write  \033[7m^C\033[m: Exit%*cloc:%d,%d\r\n%s",max.ws_col-digits_in(selx)-digits_in(sely)-5-strlen("^W: Write  ^C: Exit"),' ',selx,sely,status);
			update=false;
		}

		if (strcmp(status,"")!=0) {
			memset(status,0,255);
		}

		c=getchar();
		if (c == '\033') { 
			getchar(); 
			switch(getchar()) { 
				case 'A':
					selx--;
					update=true;	
					break;
				case 'B':
					selx++;
					update=true;
					break;
				case 'C':
					sely++;
					update=true;
					break;
				case 'D':
					sely--;
					update=true;
					break;
			}
			if (selx<0) {
				selx=0;
				sely=0;
			}
			if (selx==1 && text_lines==1) {
				selx=0;
				sely=strlen(text[selx]);
			} else if (selx>=text_lines) {
				selx=text_lines-1;
				sely=strlen(text[selx-1]);
			}
			if (sely<0) {
				sely=0;
			}
			if (sely>strlen(text[selx])) {
				sely=strlen(text[selx]);
			}

			
		} else {			
			if (c==13) {
				text_lines++;
				text=realloc(text,8*text_lines);
				for (int i=text_lines-1; i>selx; i--) {
					text_set(text,i,text[i-1]);
				}

				free(text[selx+1]);
				text[selx+1]=malloc(strlen(text[selx])-sely+1);

				for (int i=0; i<strlen(text[selx])-sely+1; i++) {
					text[selx+1][i]=text[selx][sely+i];
				}

				
				text[selx]=realloc(text[selx],sely+1);
				text[selx][sely]='\0';


				sely=0;
				selx++;
				if (sely>strlen(text[selx]))
					sely=strlen(text[selx]);
				update=true;
				changed=true;
			} else if (c==3) {
				if (changed) {
					printf("\rWarning: File has unsaved changes, use ^W to save. Press again to confirm.");
					changed=false;
				} else {
					cont=false;
				}	
			} else if (c==23) {
				printf("filename: ");
				char inpbuf[255];
				int idx=0;
				bool cancel=false;
				if (argc>1) {
					strcpy(inpbuf,argv[1]);
				}
				printf("\e[?25h");
				while (c!=13) {
					printf("\rfilename: %s",inpbuf);
					c=getchar();
					if (c>31 && c<127) {
						inpbuf[idx]=c;
						idx++;
					} else if (c==127 && idx>1) {
						idx--;
						inpbuf[idx]='\0';
					} else if (c==27) {
						cancel=true;
						break;
					}
				}
				if (!cancel) {
					FILE *fptr;

					if ((fptr = fopen(inpbuf,"w")) == NULL){
						printf("Error! opening file");
						exit(1);

					}
					for (int i=0; i<text_lines; i++) {
						fprintf(fptr,"%s\n",text[i]);
					}
					strcpy(status, "file saved");
					changed=false;
	
				}
				update=true;

			} else {
				if (c>31 && c<127) {
			
					char *buf=malloc(strlen(text[selx])+2);
					strcpy(buf,text[selx]);
					buf[sely]=c;
					buf[sely+1]='\0';
					strcat(buf,text[selx]+sely);
					buf[strlen(text[selx])+1]='\0';
					
					printf("%d",strlen(buf));
					text[selx]=realloc(text[selx],strlen(buf)+1);
					strcpy(text[selx],buf);

					free(buf);
						
					sely++;
					update=true;
					changed=true;
				} else if (c==127) {
					if (sely>0) {
						for (int i=sely-1; i<strlen(text[selx]); i++) {
							text[selx][i]=text[selx][i+1];
						}
						text[selx]=realloc(text[selx],strlen(text[selx])+1);
						text[selx][strlen(text[selx])]='\0';
						sely--;
						changed=true;
						
					} else if (selx>0) {
						int tlen = strlen(text[selx-1]);
						text[selx-1]=realloc(text[selx-1],tlen+strlen(text[selx])+1);
						for (int i=tlen;i<tlen+strlen(text[selx]);i++) {
							text[selx-1][i]=text[selx][i-tlen];
						}

						text_lines--;
						for (int i=selx;i<text_lines;i++) {
							text_set(text,i,text[i+1]);
						}
						text = realloc(text,text_lines*8);

						sely=tlen;
						selx--;
						changed=true;
	
					} 	
					update=true;
				} 

			
			}	
		}


	}

	for (int i=0; i<text_lines; i++) {
		free(text[i]);
	}
	free(text);
	system("/bin/stty cooked");
				printf("\e[?25h");

}
