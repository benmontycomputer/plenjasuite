#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
struct dict {
	char a[255];
	char b[255];

};

int fsdqueue(struct dict** queue, int strt) {
	int i = strt;
	while (queue[i]!=NULL) {
		i++;
	}
	return i;
}

void popqueue(struct dict** queue, int len) {
	for (int i=0; i<len; i++) {
		queue[i]=queue[i+1];
	}

}

void pushqueue(struct dict** queue, int len, int idx) {
	for (int i=len; i>=idx; i--) {
		queue[i+1]=queue[i];
	}
}

int queuelen(struct dict** queue, int len) {
	int ret=0;
	for (int i=0;i<len;i++) {
		if (queue[i]!=NULL) {
			ret++;
		}
	}
	return ret;
}

int main(int argc, char **argv) {

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);		
	
	struct dict dictionary[255];
	struct dict* queue[255];
	int len = 0;
	
	FILE *fp;

	if (argc >= 2) {
		fp = fopen(argv[1], "r");
		while (1) {
			fscanf(fp, "%s - %s", dictionary[len].a, dictionary[len].b);	
			if (strcmp(dictionary[len].a,"end") == 0) {
				break;
			}
			queue[len]=&dictionary[len];
			len++;
		}
	} else {
	//	strcpy(dictionary[0].a,"temp");
	//	strcpy(dictionary[0].b,"delete");
	//	queue[0]=&dictionary[0];
	}
		
	// show
	//for (int i=0; i<len; i++) {
	//	printf("%s - %s\n", dictionary[i].a, dictionary[i].b);
	//}

	int idx=0;
	int ffidx=len+1;

	system ("/bin/stty raw");
	bool side=true;
	bool list=false;
	bool quit=false;
	bool opside=false;
	
	while (!quit) {
		system("clear");
		
		int pad = (w.ws_col - 14) / 2;
		printf("%*.*s--- PLENKI ---\r\n", pad, pad, " ");

		printf("%*.*squeue:%d,",pad,pad," ",queuelen(queue,255));
		if (opside) {
			printf("inverted");
		} else {
			printf("rect");
		}
		puts("\r\n\n");
		
		if (queue[0]!=NULL) {
			if ((side && !opside) || (!side && opside)) {
				pad = (w.ws_col - strlen(queue[0]->a)) / 2;
				printf("%*.*s%s", pad, pad, " ",queue[0]->a);
			} else {
				pad = (w.ws_col - strlen(queue[0]->b)) / 2;
				printf("%*.*s%s", pad, pad, " ",queue[0]->b);
			}
		}
		//pad = w.ws_col/2;
		//printf("\r\n%*.*s:",pad-1,pad-1," ",pad);

		bool act=false;
		char c;
		int v=0;
		while (!act) {
			c=getchar();
			if (isdigit(c)!=0) {
				v=v*10;
				v+=c-'0';
			}
			//printf("\n%d",v);
			switch (c) {
				case ' ':
					side=!side;
					act=true;
					break;
				case 3:
					quit=true;
					act=true;
					break;
				case 'a':
					queue[fsdqueue(queue,fsdqueue(queue,0)+v)]=queue[0];
					popqueue(queue,254);
					act=true;
					side=true;
					break;
				case 'r':
					pushqueue(queue,254,v);
					queue[v]=queue[0];
					popqueue(queue,254);
					act=true;
					side=true;
					break;
				case 'd':
					popqueue(queue,254);
					act=true;
					side=true;
					break;
				case 'l':
					system("clear");
					for (int i=0; i<v; i++) {
						printf("%s - %s\r\n", queue[i]->a, queue[i]->b);
					}
					v=0;
					break;
				case 'f':
					opside=!opside;
					act=true;
					break;
				case 19:
					char nfile[255];
					system ("/bin/stty cooked");
					system("clear");
					printf("Enter file name: ");
					scanf("%255s",nfile);
					FILE *nftpr;
					nftpr = fopen(nfile,"w");
					for (int i=0;i<255;i++) {
						if (queue[i]!=NULL) {
							fprintf(nftpr,"%s - %s\n", queue[i]->a,queue[i]->b);
						}
					}
					fprintf(nftpr,"end\n");
					fclose(nftpr);
					act=true;
					break;
				case 'n':
					if (v==0) {v=1;}
					system ("/bin/stty cooked");
					for (int i=0;i<v;i++) {
						system ("clear");

						printf("Enter fore: ");
						scanf("%255s",dictionary[len].a);
						printf("\nEnter rear: ");
						scanf("%255s",dictionary[len].b);
						queue[fsdqueue(queue,0)]=&dictionary[len];
		
						len++;
					}
					system ("/bin/stty raw");
					act=true;
					break;
				case 'e':
					system("clear");
					system ("/bin/stty cooked");
					printf("Current fore: %s New fore: ",queue[0]->a);
					scanf("%255s",queue[0]->a);
					printf("Current rear: %s New rear: ",queue[0]->b);
					scanf("%255s",queue[0]->b);

					system ("/bin/stty raw");
					act=true;
					break;
				case 's':

			}
		}

	}
		
	if (argc >= 2) {
		fclose(fp);
	}
	puts("\r");
	system ("/bin/stty cooked");
}
