#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <fstream.h>
#include <time.h>

// some stuff
// wall generater problem
int far* sc = (int far*)0xb8000000;
int key = 5;
time_t myseed;

// snake properties
int x,y = 0; // head's x,y
int pos[50][2] = {0}; // 0->x, 1->y
int tail = 2;
int dir = 0; // 0->right, 1->up, 2->left, 3->down
int tailx,taily = 0;

// level stuff
int isCrashed = 0;
int score = 0;
int foodlvl = 1;
int lvl = 1;
int foodx,foody = 0;
int foodAgainNeed = 0;
int speed = 60;
int bestScore = 0;
int doubleFood = 0;
// functions
void init();
void cleanScreen();
void move();
void drawSnake();
void drawSnakeOtherParts();
void removeTail();
void createFood();
void foodEatenChecker();
void checkGameStatus ();
void mapGen();
void checkWallColl();
void loseGame();
void ss(char a[],int len,int x,int y,int color,int bgcolor);


void main () {
	srand((unsigned) time(&myseed));
	init();

	Update: {
		tailx = pos[tail-1][0];
		taily = pos[tail-1][1];
		x = pos[0][0];
		y = pos[0][1];
	}
	
	drawSnakeOtherParts();
	removeTail();
	move();
	checkGameStatus();
	drawSnake();
	foodEatenChecker();

	if(dir!=1 && dir!=3) {
		delay(speed);
	}else{
		delay(speed*130/100);	
	}

	if(kbhit()) {
		doKey:key=getch();
		switch(key) {
			case 72:
				if(dir != 3)
				dir = 1;
			break;
			case 80:
				if(dir != 1)
				dir = 3;
			break;
			case 75:
				if(dir != 0)
				dir = 2;
			break;
			case 77:
				if(dir != 2)
				dir = 0;
			break;
			case 32:
				for (int a=0;a<2000;a++) {
					if (sc[a]!=0) {
						sc[a]=2*16*256;
					}
				}
				break;
			// exit
			case 27:
				exit(0);
			break;
			case 112:
				while(getch()!=112);
				break;
			case 0: goto doKey; //NULL -> getch again
		}
	}
	goto Update;
}

int randColor = 12;
int  t = 0;
int doubleFoodC = 0;
void createFood() {
	if(!doubleFood) {
		do{
			foodx = rand()%75+1;
			foody = rand()%20+1;
			if(sc[foody*80+foodx] != 0) {
				foodAgainNeed = 1;
			}else{
				foodAgainNeed = 0;
			}
		}while(foodAgainNeed);

		randColor++;
		if (randColor==7) randColor+=2;
		if (randColor>14) {
			randColor = 1;
		}
		sc[foody*80+foodx] = randColor*256 + 3;

	}else{
		if(doubleFoodC == 0) {
			foodx = 20;
			foody = 10;
			doubleFoodC++;
		}else if (doubleFoodC==1) {
			foodx = 19;
			foody = 10;
			doubleFoodC++;
		}else{
			foodx = 10;
			foody = 10;
			doubleFoodC++;
		}
		sc[foody*80+foodx] = 5*256 + 3;
	}
}

void drawSnake() {
	if(pos[0][0]!=0 && pos[0][1]!=0) {
		sc[pos[0][1]*80+pos[0][0]] = 10*256 +2;
	}
}

void drawSnakeOtherParts() {
	if(tail!=0) {
		for(int i=tail; i>0; i--) {
			pos[i][0]=pos[i-1][0];
			pos[i][1]=pos[i-1][1];
		}
		sc[y*80+x] = 2*256 +'o';
	}
}

void removeTail() {
	if(tailx!=0 && taily!=0) {
		sc[taily*80+tailx] = 0;
	}
}

void foodEatenChecker() {
	if(x == foodx && y == foody) {
		for (int i=0;i<3;i++) {
			sound(600-i*200);
			delay(20);
			nosound();
		}
		if(foodlvl < 8) {
			tail += foodlvl;
			score += foodlvl;
			foodlvl ++;
			sc[30] = 7*256 +foodlvl+48;
			createFood();
		}else{
			for (int j=2;j<50;j++) {
				sc[pos[j][1]*80+pos[j][0]] = 0;
				pos[j][0] = 0;
				pos[j][1] = 0;
			}
			tail = 2;
			foodlvl = 1;
			sc[30] = 7*256 +'1';
			lvl++;
			mapGen();
			if(lvl<10) {
				sc[53] = 7*256 +lvl+48;
			}else{
				sc[52] = 7*256 +lvl/10%10+48;
				sc[53] = 7*256 +lvl%10+48;
	}
			createFood();
			speed -= 10;
		}
	}

	if(score<10) {
		sc[12] = 7*256 +score+48;
	}else{
		sc[12] = 7*256 +score/10%10+48;
		sc[13] = 7*256 +score%10+48;
		sc[14] = 7*256 +' ';
	}
}

void cleanScreen() {
	for (int qq=0;qq<2000;qq++) {
		sc[qq] = 0;
	}
}

void move () {
	switch(dir) {
		case 0:
			if(x<78) {
				x++;
				checkWallColl();
				pos[0][0] = x;
			}
			break;
		case 1:
			if(y>1) {
				y--;
				checkWallColl();
				pos[0][1] = y;
			}
			break;
		case 2:
			if(x>1) {
				x--;
				checkWallColl();
				pos[0][0] = x;
			}
			break;
		case 3:
			if(y<23) {
				y++;
				checkWallColl();
				pos[0][1] = y;
			}
			break;
	}
}

// for checking winning or losing stat
void checkGameStatus() {
	for(int i=1; i<tail; i++) {
		if(x == pos[i][0] && y == pos[i][1]) {
			loseGame();
		}
	}
}

void checkWallColl() {
	int lose = 0;
	if(x!=foodx && y!=foody) {
		if (sc[y*80+x]!=0 && sc[y*80+x]!=10*256 +2) {
			lose = 1;
		}

		if (lose) {
			loseGame();
		}
	}
}

void mapGen () {
	if (lvl>1) {
		for (int pp=0;pp<2000;pp++) {
			if (sc[pp] == 7*256 +221 || sc[pp] == 7*256 +223) sc[pp]=0;
		}
	}
	int p,q,l = 0;
	int need = 0;
	int d = 0;

	for (int i=0;i<2000;i++) {
		if(sc[i] == 7*16*256) sc[i] = 0;
	}
	for (int qq=0;qq<5;qq++) {
		p,q,l,d,need = 0;
		d = rand()%2;
		l = rand()%10 + 5;
		
		do{
			p = rand()%60 + 5;
			q = rand()%20 + 1;
			if (lvl>1) {
				if (p == x && q == y) need=1;
			}else{
				need = 0;
			}
		}while (need);
		for (int jj=0;jj<l;jj++) {
			if(d==0) {
				sc[q*80+p+jj] = 7*256 +223;
			}else{
				if(q+jj<23) sc[(q+jj)*80+p] = 7*256 +221;
			}
		}
	}
}

void loseGame() {
	sound(100);
	delay(50);
	nosound();
	ss("Game Over",9,37,20,0,7);
	ss("Press Space to try again",24,30,21,0,7);

	if (score>bestScore) {
		if(score<10) {
			sc[71] = 7*256 +score+48;
		}else{
			sc[71] = 7*256 +score/10%10+48;
			sc[72] = 7*256 +score%10+48;
		}
		bestScore = score;
		// save
		FILE* savefile = fopen("scorehistory", "wb");
		fprintf(savefile, "%d", bestScore);
		fclose(savefile);
	}
	while(getch()!=32);
	init();
	score = 0;
}


void init() {
	///////////////default values////////////////
	key = 0;
	lvl = 1;
	x,y = 0;
	for (int j=0;j<50;j++) {
		pos[j][0] = 0;
		pos[j][1] = 0;
	}

	tail = 2;
	dir = 0;
	tailx,taily = 0;
	speed = 60;
	isCrashed = 0;
	foodlvl = 1;
	foodx,foody = 0;
	doubleFoodC = 0;
	/////////////////////////////////////

	// screen clear
	_setcursortype(_NOCURSOR);
	cleanScreen();

	// screen layout

	//borders
	for (int i=0;i<79;i++) {
		sc[i] = 7*256 +196;
		sc[24*80+i] = 7*256 +196;
	}
	for (int kk=0;kk<79;kk++) {
		sc[kk*80] = 7*256 +179;
		sc[kk*80+79] = 7*256 +179;
	}
	sc[0] = 7*256 +218;
	sc[79] = 7*256 +191;
	sc[24*80] = 7*256 +192;
	sc[24*80+79] = 7*256 +217;


	ss("Score: 0 ",9,5,0,7,0);
	ss("Level: 1 ",9,46,0,7,0);
	ss("Food Level: 1 ",14,18,0,7,0);
	ss("Best Score: ",13,59,0,7,0);
	sc[73] = 7*256 +' ';

	// load 
	FILE* loadFile = fopen("scorehistory", "r");
	fscanf(loadFile, "%d", &bestScore);
	fclose(loadFile);

	// best score stat
	if(bestScore<10) {
		sc[71] = 7*256 +bestScore+48;
	}else{
		sc[71] = 7*256 +bestScore/10%10+48;
		sc[72] = 7*256 +bestScore%10+48;
	}
	mapGen();
	createFood();
	int xyNeed = 0;

	do{
		x = rand()%73+1;
		y = rand()%22+1;
		if(sc[y*80+x]!=0) {
			xyNeed = 1;
		}else{
			xyNeed = 0;
		}
	}while(xyNeed);
	
	pos[0][0] = x;
	pos[0][1] = y;
}

void ss(char a[],int len,int x,int y,int color,int bgcolor) {
	for (int i=0;i<len;i++){
		sc[y*80+x+i] = (color+bgcolor*16)*256 + a[i];
	}
}