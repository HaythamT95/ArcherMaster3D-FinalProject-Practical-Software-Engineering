#include <stdlib.h>
#include <glut.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <functional>
//#include "TextureLoader.h" 

using namespace std;

#define N 20 //Draw circle constant
#define gravity 9.81 //decide gravity variable
#define PI 3.1415927 
#define MAX &buildGameMAX //get number of objects to build from user
#define WorldZ 101 //world size in Z axis
#define WorldX 49 //world size in X axis

int grassPicW;
int grassPicH;
unsigned char* picBytes;
unsigned int  grassPic;

bool pressEnter = false, pressMouse = false, moveObject = false;//print messages during simulation
int* buildGameMAX = NULL;// number of objects in game
int fileflag = 2;//open score table of all users
int objectnum;// number of objects in game
int xscreen = 1920, yscreen = 1080;//screen size
char strOfUserName[20] = "";// save username string
char strOfUserNamepass[20] = "";//save password string
char strOfUserLevel[2] = "1";//current user level
int indexOfUserName = 0;//variable for knowing current index of username entered 
int indexOfUserNamePass = 0;//variable for knowing current index of password entered 
bool name = false, pass = false;//check if pressed on username/password
int screen = 1, GuideScreen;//change screens
GLfloat BowR = 0.4, BowG = 0.3, BowB = 0;//colors of arrow
int buildGamebtn = 0, Treebtn = 0, Targetbtn = 0;//know if tree/target was chosen to build game
char Treestr[5] = "TREE", * Tree_, * Target_, Targetstr[7] = "TARGET";//print messages
int targBehindTree = 0, ObjCenter = 0;//flags to let know the user where to put objects
char  strObjNum[3] = "2", * strObjNum_;//print messages
int targetind = 0, treeind = 0;//target/tree array sizes
float xborder = 1, zborder = 1; //border of target
double speed = 0.5;//moving objects while building game with 0.5 speed
char Game[WorldZ][WorldX];//save objects location in matrix
int gamearrayflag = 0;//read file to array flag
GLfloat CircleRadius = 0.2;//radius of target
GLfloat LineX = 0, LineY = 0, LineZ = 0;//thread draw of bow
GLfloat eyeX = 0, eyeY = 0.5, eyeZ = 5;//camera for gluLookAt
GLfloat cx = 0, cy = 0.5, cz = -1;//camera for gluLookAt
GLfloat angle = 0, angleI;// calculate rotation of mouse
GLfloat ArrowAngle = 0;//calculate rotation of mouse (up/down)
GLfloat rotx = 0, roty = 0;//rotation of x/y axis
GLfloat Arrowposx, Arrowposy = 0, Arrowposz = 1, ArrowPower = 0;//arrow location and it's initiate power 
GLfloat TimeofFlight, time_ = 0, t = 0, msgtime, tmpTimer = 0;//timers
int randomResistance;//random variable to save generated random resistance for level 3
bool ArrowThrow = false;// check if arrow was shot or not
char Score[10] = "0";//save current points of user and print
int ArrowStock, score = 0;// arrows quantity remaining
int ArrowHits = 0, ArrowHitforTimer = 0;//  arrows quantity shot
int leftclick = 0;//check if left click on mouse was pressed
GLfloat Ry = 6;//draw rain variable
int LevelMalloc = 1;//flag for building level
int  AtLeastTarget = 2, AtLeastTargFLAG = 0;//minimum targets allowed
int TargetArraylen, TreeArraylen, ObjNumUser = 2;//save target/tree array length
GLfloat TargetSpeed = 0.004;//moving target speed
map<string, string> ScoresTable;//all users and their scores
GLfloat wing = 1;//wing of bird
int wingflag = 0;//moving wing of bird
bool birdhit;//hit bird
GLfloat rotscore = 0;//rotation of unique sign(diamond) to the 1st place
GLfloat AirplaneX = -15;//plane location
GLfloat BoxX, BoxY = 8, BoxZ = -7, BoxR = 0.3, BoxG = 0.6, BoxB = 0;//color and location of box
int playcnt = 0, screenflag = 0;//music variables
char charMinus[2] = "-", * charMinus_, charplus[2] = "+", * charplus_;//print message
char hitargetMSG[5] = "+", * hitargetMSG_;//print if hit target
float hitargetMSGY = 3;//location print if hit target
int points = 0;//current points
bool hiTarMSGYvissible = false;//print visible/invisible
char tutstr1[50] = "", * tutstr1_;//print message in simulation
int order = 10, r = 1, g = 1, b = 1, r1 = 1, g1 = 1, b1 = 1;//order of simulation and colors


struct Bird {
	GLfloat X = -20, Y = 2, Z = -12;
};

struct Bird birds[4];

struct Arrow {
	float x, y, z;
	float r = 0.392, g = 0.423, b = 0.176;
	GLfloat radius = 0.015, angle, height, midx, midy;
	bool hit_target = false;
	bool hit = false;
};

struct Object {
	GLfloat x = 0.0, y = 0.0, z = -12.0;
	bool hit = false;
	int status = 0;
	int bounDirection = 0;
	int fr, fl;
};

struct Object* Target = NULL, * tree = NULL, treeOB, targetOB;
struct Arrow Arrow;
void* font = GLUT_BITMAP_TIMES_ROMAN_24;

unsigned char* loadTexture(const char* fname, int* w, int* h) {
	unsigned char* bytes;
	int i;
	int v;
	FILE* f = fopen(fname, "r");
	if (f) {
		fscanf(f, "%*s");
		fscanf(f, "%d %d %d", w, h, &v);
		bytes = (unsigned char*)malloc(sizeof(unsigned char) * (*w) * (*h) * 3);
		for (i = 0; i < *w * *h * 3; i++) {
			fscanf(f, "%d", &v);
			bytes[i] = (unsigned char)v;
		}
		return bytes;
	}
	else
	{
		printf("Could not load texture %s\n", fname);
		return NULL;
	}
}



// print message on screen
void output(int x, int y, const char* string)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}
//print 2 messages on screen
void text(char* str1, char* str2, float x, float y, float z)
{
	str2 = str1;
	glRasterPos3f(x, y, z);
	do glutBitmapCharacter(font, *str2); while (*(++str2));
	glutPostRedisplay();
}
// generate random num
int randfrom(int min, int max)
{
	return ((rand() % (max - min + 1)) + min);
}
//save/read to files
void OpenFile(int status) throw(int)
{
	FILE* file; //pointer to file
	char str_compare[255]; //compare from file string and input from user 
	if (status == 1) // new username
	{
		int compare, compare1; //variable to hold the result of comparing both strings
		compare = strcmp("", strOfUserName);
		compare1 = strcmp("", strOfUserNamepass);
		if (compare != 0 && compare1 != 0)
		{
			//check if username exists
			file = fopen(strOfUserName, "r");
			if (file != NULL)
			{
				fclose(file);
				strcpy(strOfUserName, "");
				strcpy(strOfUserNamepass, "");
				screen = 9;
				return;
			}
			// if username does not exists then create new username
			file = fopen(strOfUserName, "w+");
			strOfUserNamepass[indexOfUserNamePass] = '\n';
			strOfUserNamepass[++indexOfUserNamePass] = '\0';
			fputs(strOfUserNamepass, file);
			fputs(strOfUserLevel, file);
			fputs("\n", file);
			fputs(Score, file);
			fclose(file);
			OpenFile(7);
			screen = 4;
		}
		else // if username or password missing (empty input) -> error
		{
			strcpy(strOfUserName, "");
			strcpy(strOfUserNamepass, "");
			screen = 9;
		}
	}
	//if username exists
	if (status == 2)
	{
		file = fopen(strOfUserName, "r"); // search for username file
		if (file == NULL)
		{
			screen = 9;
			return;
		}
		else
		{
			//if username exists check password 
			int compare; //compare string
			fscanf(file, "%s", str_compare);
			compare = strcmp(str_compare, strOfUserNamepass);                                          
			if (compare == 0)
			{
				fscanf(file, "%*[^\n]\n");
				fscanf(file, "%s", strOfUserLevel);
				fscanf(file, "%*[^\n]\n");
				fscanf(file, "%s", Score);
				score = atoi(Score);
				screen = 4;
			}
			else
			{
				fclose(file);
				strcpy(strOfUserName, "");
				strcpy(strOfUserNamepass, "");
				screen = 9;
			}
		}
		fclose(file);
	}

	//enter new level
	if (status == 3)
	{
		char filename[25];
		if (strOfUserLevel[0] == '1')//enter level 1
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_1");
			file = fopen(filename, "wb");
			fwrite(Game, sizeof(char), sizeof(Game), file);
			fclose(file);
			return;
		}

		if (strOfUserLevel[0] == '2')///enter level 2
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_2");
			file = fopen(filename, "wb");
			fwrite(Game, sizeof(char), sizeof(Game), file);
			fclose(file);
			return;
		}

		if (strOfUserLevel[0] == '3')//enter level 3
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_3");
			file = fopen(filename, "wb");
			fwrite(Game, sizeof(char), sizeof(Game), file);
			fclose(file);
			randomResistance = randfrom(15, 25);
			return;
		}
	}

	//load level
	if (status == 4)
	{
		char filename[25];
		file = fopen(strOfUserName, "r");
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%s", strOfUserLevel);
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%s", Score);
		score = atoi(Score);
		fclose(file);

		if (strOfUserLevel[0] == '1')
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_1");
			file = fopen(filename, "rb");
			if (file == NULL)//display message if level does not exists
			{
				throw 4;
			}
			fread(Game, sizeof(char), sizeof(Game), file);
			fclose(file);

			treeind = targetind = 0;
			for (int rows = 0; rows < WorldZ; rows++)
			{
				for (int cols = 0; cols < WorldX; cols++)
				{
					if (Game[rows][cols] == '1')
						treeind++;
					else if (Game[rows][cols] == '2')
						targetind++;
				}
			}

			return;
		}

		if (strOfUserLevel[0] == '2')
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_2");
			file = fopen(filename, "rb");
			if (file == NULL)//if level 1 is finished and their is no level 2 yet
			{
				gamearrayflag = 1;
				screenflag = 1;
				//objectnum = *buildGameMAX;
				throw 13;
			}
			fread(Game, sizeof(char), sizeof(Game), file);
			fclose(file);
			treeind = targetind = 0;

			for (int rows = 0; rows < WorldZ; rows++)
			{
				for (int cols = 0; cols < WorldX; cols++)
				{
					if (Game[rows][cols] == '1')
						treeind++;
					else if (Game[rows][cols] == '2')
						targetind++;
				}
			}
			return;
		}

		if (strOfUserLevel[0] == '3')
		{
			strcpy(filename, strOfUserName);
			strcat(filename, "_3");
			file = fopen(filename, "rb");
			if (file == NULL)//if level 2 is finished and their is no level 3 yet
			{
				gamearrayflag = 1;
				//objectnum = *buildGameMAX;
				screenflag = 1;
				randomResistance = randfrom(15, 25);
				throw 13;
			}
			fread(Game, sizeof(char), sizeof(Game), file);
			fclose(file);
			treeind = targetind = 0;

			for (int rows = 0; rows < WorldZ; rows++)
			{
				for (int cols = 0; cols < WorldX; cols++)
				{
					if (Game[rows][cols] == '1')
						treeind++;
					else if (Game[rows][cols] == '2')
						targetind++;
				}
			}
			randomResistance = randfrom(15, 25);
			return;
		}
	}

	if (status == 5) // update level
	{
		file = fopen(strOfUserName, "r");
		fscanf(file, "%s", strOfUserNamepass);
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%s", strOfUserLevel);
		fclose(file);
		remove(strOfUserName);

		if (strOfUserLevel[0] == '1')//update from level 1 to level 2
		{
			strOfUserLevel[0] = '2';
			file = fopen(strOfUserName, "w");
			fputs(strOfUserNamepass, file);
			fputs("\n", file);
			fputs(strOfUserLevel, file);
			fputs("\n", file);
			fputs(_itoa(score, Score, 10), file);
			fclose(file);
			return;
		}

		if (strOfUserLevel[0] == '2')//update from level 2 to level 3
		{
			strOfUserLevel[0] = '3';
			file = fopen(strOfUserName, "w");
			fputs(strOfUserNamepass, file);
			fputs("\n", file);
			fputs(strOfUserLevel, file);
			fputs("\n", file);
			fputs(_itoa(score, Score, 10), file);
			fclose(file);
			return;
		}
	}

	//update score table
	if (status == 6)
	{
		char tmpstr[20], tmp2str[20];
		map <string, string> strings;

		ifstream infile("scores");
		string line, line2;
		while (getline(infile, line)) {
			getline(infile, line2);
			strings.insert(pair<string, string>(line, line2));
		}
		remove("scores");

		strcpy(tmpstr, "");
		strcpy(tmp2str, "");

		string scor(Score);
		strings[strOfUserName] = scor;

		file = fopen("scores", "w");

		map<string, string>::iterator itr;
		for (itr = strings.begin(); itr != strings.end(); itr++)
		{
			strcpy(tmpstr, itr->first.c_str());
			fputs(tmpstr, file);
			fputs("\n", file);
			strcpy(tmp2str, itr->second.c_str());
			fputs(tmp2str, file);
			fputs("\n", file);
		}
		fclose(file);
		return;
	}

	//insert to score table
	if (status == 7)
	{
		char tmpstr[20], tmp2str[20];
		map <string, string> strings;

		ifstream infile("scores");
		string line, line2;

		while (getline(infile, line)) {
			getline(infile, line2);
			strings.insert(pair<string, string>(line, line2));
		}
		remove("scores");

		file = fopen("scores", "w");

		map<string, string>::iterator itr;
		for (itr = strings.begin(); itr != strings.end(); itr++)
		{
			strcpy(tmpstr, itr->first.c_str());
			fputs(tmpstr, file);
			fputs("\n", file);
			strcpy(tmp2str, itr->second.c_str());
			fputs(tmp2str, file);
			fputs("\n", file);
		}

		fputs(strOfUserName, file);
		fputs("\n", file);
		fputs(_itoa(score, Score, 10), file);
		fclose(file);
		return;
	}

	//read into <map>
	if (status == 8)
	{
		ifstream infile("scores");
		string line, line2;

		while (getline(infile, line)) {
			getline(infile, line2);
			ScoresTable.insert(pair<string, string>(line, line2));
		}
	}
}


//draw cross to aim
void Aim()
{
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(-0.05, 0);
	glVertex2f(0.05, 0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex2f(0, -0.05);
	glVertex2f(0, 0.05);
	glEnd();
}
// draw thread of bow
void Bow_Line()
{
	glBegin(GL_LINES);
	glNormal3f(0, 0, 0.5);
	glColor3f(1, 1, 1);
	glVertex3f(0, 1, 0);
	glVertex3f(LineX, LineY, LineZ);
	glVertex3f(LineX, LineY, LineZ);
	glVertex3f(0, -1, 0);
	glEnd();
}
//draw bow
void init_Bow()
{
	glTranslatef(eyeX + cx, (cy - 0.5), eyeZ + cz);
	glRotatef(-(rotx * 2.868), 0, 1, 0);
	glRotatef(-(roty * 45), 1, 0, 0);
	glRotatef(-30, 0, 0, 1);

	glBegin(GL_TRIANGLES);
	//upperside
	glColor3f(BowR, BowG, BowB);

	glNormal3f(0, 0, 0.5);
	glVertex3f(-0.06, 0, -0.2);
	glVertex3f(0.06, 0, -0.2);
	glVertex3f(0, 1, 0);

	glNormal3f(0, 0, 0.5);
	glVertex3f(-0.06, 0, -0.2);
	glVertex3f(-0.06, 0, -0.1);
	glVertex3f(0, 1, 0);


	glNormal3f(0, 0, 0.5);
	glVertex3f(0.06, 0, -0.2);
	glVertex3f(0.06, 0, -0.1);
	glVertex3f(0, 1, 0);

	//down side
	glNormal3f(0, 0, 0.5);
	glVertex3f(-0.06, 0, -0.2);
	glVertex3f(0.06, 0, -0.2);
	glVertex3f(0, -1, 0);

	glNormal3f(0, 0, 0.5);
	glVertex3f(-0.06, 0, -0.2);
	glVertex3f(-0.06, 0, -0.1);
	glVertex3f(0, -1, 0);


	glNormal3f(0, 0, 0.5);
	glVertex3f(0.06, 0, -0.2);
	glVertex3f(0.06, 0, -0.1);
	glVertex3f(0, -1, 0);
	glEnd();

	glTranslatef(0, 0, -0.15);
	double x = 0, y = 0;
	double radius = 0.04;
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}
	glEnd();
	glTranslatef(0, 0, 0.15);

	Bow_Line();
	glRotatef(30, 0, 0, 1);
	glTranslatef(-(eyeX + cx), -(cy - 0.5), -(eyeZ + cz));
}
// draw arrow
void init_Arrows()
{
	glTranslatef(Arrowposx, Arrowposy, Arrowposz);
	//Cylinder (עמוד/עץ)	
	glBegin(GL_QUAD_STRIP);
	glColor3f(Arrow.r, Arrow.g, Arrow.b);
	Arrow.midx = 0;
	Arrow.midy = 0;
	Arrow.height = 1;
	Arrow.angle = 0;
	while (Arrow.angle < 2 * PI) {
		Arrow.midx = Arrow.radius * cos(Arrow.angle);
		Arrow.midy = Arrow.radius * sin(Arrow.angle);
		glVertex3f(Arrow.midx, Arrow.midy, Arrow.height);
		glVertex3f(Arrow.midx, Arrow.midy, 0.0);
		Arrow.angle = Arrow.angle + 0.1;
	}
	glVertex3f(Arrow.radius, 0.0, Arrow.height);
	glVertex3f(Arrow.radius, 0.0, 0.0);
	glEnd();
	glTranslatef(-Arrowposx, -Arrowposy, -Arrowposz);

	//Arrow on start (חץ)
	glTranslatef(Arrowposx, Arrowposy, Arrowposz + 0.1);
	glRotatef(180, 1, 0, 0);
	glutSolidCone(0.05, 0.2, 20, 20);
	glRotatef(-180, 1, 0, 0);
	glutSolidSphere(0.04, 20, 20);
	glTranslatef(-Arrowposx, -Arrowposy, -(Arrowposz + 0.1));

	//Feathers
	glTranslatef(Arrowposx, Arrowposy, Arrowposz + 1);

	glRotatef(45, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	glColor3f(0.3, 0.6, 0.3);
	glVertex3f(0, 0, -0.3);
	glVertex3f(0, 0.1, 0);
	glVertex3f(0, -0.1, 0);
	glEnd();
	glRotatef(-45, 0, 0, 1);

	glRotatef(-45, 0, 0, 1);
	glBegin(GL_TRIANGLES);
	glColor3f(0.3, 0.6, 0.3);
	glVertex3f(0, 0, -0.3);
	glVertex3f(0, 0.1, 0);
	glVertex3f(0, -0.1, 0);
	glEnd();
	glRotatef(45, 0, 0, 1);

	double x = 0, y = 0;
	double radius = 0.02;
	glNormal3f(0.5, 0, 0);
	glColor3f(0.392, 0.423, 0.176);
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glVertex2f(x, y); // center of circle
	for (int i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}
	glEnd();
	glTranslatef(-Arrowposx, -Arrowposy, -(Arrowposz + 1));

	glTranslatef(eyeX + cx, cy - 0.1, eyeZ + cz - 1);
	Aim();
	glTranslatef(-(eyeX + cx), -(cy - 0.1), -(eyeZ + cz - 1));
}

//draw tree
void Tree()
{
	glColor3f(0.3, 0.6, 0.3);
	glNormal3f(0, 0, 0.5);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360; i++) {
		glVertex3f(cos(i) / 2, 7, sin(i) / 2);
		glVertex3f(cos(i) / 2, -7, sin(i) / 2);
	}
	glEnd();


	glColor3f(0.2, 0.6, 0.2);
	glTranslatef(0, 0.7, 0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(1.5, 12, 5, 5);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, -0.7, 0);

}
// draw stand of target
void regel()
{
	glColor3f(0.5, 0.35, 0.05);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 0.5);
	glVertex3f(-0.2, 1, 0);
	glVertex3f(0.2, 1, 0);
	glVertex3f(0.2, -1, 0);
	glVertex3f(-0.2, -1, 0);
	glEnd();
}
// draw target
void target()
{
	float z = 0.001; //draw each circle a bit forward so that we can see the circles perfectly

	glTranslatef(0, 1, 0);
	glTranslatef(0, 0, z);
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glColor3f(1, 1, 0);
	glNormal3f(0, 0, 1);
	glVertex2f(0, 0); // center of circle
	for (int i = 0; i <= N; i++) {
		glVertex2f(
			(0 + (CircleRadius * cos(i * 2 * PI / N))), (0 + (CircleRadius * sin(i * 2 * PI / N)))
		);
	}
	glEnd();
	glTranslatef(0, 0, -z);
	z -= 0.001;
	CircleRadius += 0.2;

	glTranslatef(0, 0, z);
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glColor3f(1, 0, 0);
	glNormal3f(0, 0, 1);
	glVertex2f(0, 0); // center of circle
	for (int i = 0; i <= N; i++) {
		glVertex2f(
			(0 + (CircleRadius * cos(i * 2 * PI / N))), (0 + (CircleRadius * sin(i * 2 * PI / N)))
		);
	}
	glEnd();
	glTranslatef(0, 0, -z);
	z -= 0.001;
	CircleRadius += 0.2;

	glTranslatef(0, 0, z);
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glColor3f(0, 0, 1);
	glNormal3f(0, 0, 1);
	glVertex2f(0, 0); // center of circle
	for (int i = 0; i <= N; i++) {
		glVertex2f(
			(0 + (CircleRadius * cos(i * 2 * PI / N))), (0 + (CircleRadius * sin(i * 2 * PI / N)))
		);
	}
	glEnd();
	glTranslatef(0, 0, -z);
	z -= 0.001;
	CircleRadius += 0.2;

	glTranslatef(0, 0, z);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0, 0, 0);
	glNormal3f(0, 0, 1);
	glVertex2f(0, 0); // center of circle
	for (int i = 0; i <= N; i++) {
		glVertex2f(
			(0 + (CircleRadius * cos(i * 2 * PI / N))), (0 + (CircleRadius * sin(i * 2 * PI / N)))
		);
	}
	glEnd();
	glTranslatef(0, 0, -z);
	z -= 0.001;
	CircleRadius += 0.2;

	glTranslatef(0, 0, z);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex2f(0, 0); // center of circle
	for (int i = 0; i <= N; i++) {
		glVertex2f(
			(0 + (CircleRadius * cos(i * 2 * PI / N))), (0 + (CircleRadius * sin(i * 2 * PI / N)))
		);
	}
	glEnd();
	glTranslatef(0, 0, -z);
	z -= 0.001;
	CircleRadius += 0.2;
	glTranslatef(0, -1, 0);

	if (CircleRadius > 0.2)
		CircleRadius = 0.2;

	glTranslatef(0, 0, z);
	regel();
	glTranslatef(0, 0, -z);
}
//draw ellipse
void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments)
{
	float theta = 2 * PI / float(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = 1;//we start at angle = 0 
	float y = 0;

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		//apply radius and offset
		glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

											 //apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}
// draw bird
void Bird()
{
	for (int bird = 0; bird < 3; bird++)
	{
		if (bird >= 1) {
			birds[bird].Z = birds[bird - 1].Z - 6;
			birds[bird].Y = birds[bird - 1].Y + 3;
			birds[bird].X = birds[bird - 1].X - 2;
		}
	}

	for (int bird = 0; bird < 3; bird++) {
		glTranslatef(birds[bird].X, birds[bird].Y, birds[bird].Z);
		glColor3f(1, 1, 0);
		//body
		DrawEllipse(0, 0, 1, 0.5, 100);

		//wings
		glTranslatef(0, 0, 0.1);
		glRotatef(wing, 1, 0, 0);
		glBegin(GL_TRIANGLES);
		glColor3f(0.8, 1, 0.5);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.6, 0.8, 0);
		glVertex3f(0.6, 0.8, 0);
		glEnd();
		glRotatef(-wing, 1, 0, 0);
		glTranslatef(0, 0, -0.1);

		//head
		glTranslatef(1.35, 0, 0);
		glutSolidCone(0.4, 0, 20, 20);
		//eye
		glTranslatef(0.07, 0.07, 0.05);
		glColor3f(1, 1, 1);
		glutSolidCone(0.05, 0, 20, 20);
		glTranslatef(-0.07, -0.07, -0.05);
		glTranslatef(-1.35, 0, 0);

		//tail
		glTranslatef(-1, 0, 0);
		glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.3, 0.4, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.3, 0, 0);
		glEnd();

		glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.3, -0.4, 0);
		glEnd();
		glTranslatef(1, 0, 0);

		//mouth
		glTranslatef(1.75, 0, 0);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0.1, 0);
		glVertex3f(0.1, 0, 0);
		glVertex3f(0, -0.1, 0);
		glEnd();
		glTranslatef(-1.75, 0, 0);

		glTranslatef(-birds[bird].X, -birds[bird].Y, -birds[bird].Z);
	}

	for (int bird = 0; bird < 3; bird++) {
		if (bird >= 1) {
			birds[bird].Z = birds[bird - 1].Z + 6;
			birds[bird].Y = birds[bird - 1].Y - 3;
			birds[bird].X = birds[bird - 1].X + 2;
		}
	}
}
// move bird
void movebirds()
{
	for (int bird = 0; bird < 3; bird++)
	{
		if (birds[bird].X < 25) {
			birds[bird].X += 0.03;
		}
		else
			birds[bird].X = -20;

		if (wing < 180 && wingflag == 0) {
			wing += 0.1;
			birds[bird].Y -= 0.001;
		}
		else {
			wing -= 0.1;
			birds[bird].Y += 0.001;
			wingflag = 1;
		}

		if (wingflag == 1 && wing < 0)
			wingflag = 0;
	}
}
//draw box
void Box()
{
	if (AirplaneX < 5.1)
		BoxX = AirplaneX - 0.2;

	glTranslatef(BoxX, BoxY - 1, BoxZ);
	glColor3f(BoxR, BoxG, BoxB);
	glutSolidCube(1);
	glTranslatef(-BoxX, -(BoxY - 1), -BoxZ);

	BoxR += 0.01;
	BoxG += 0.01;
	BoxB += 0.01;

	if (BoxR > 1)
		BoxR = 0;

	if (BoxG > 1)
		BoxG = 0;

	if (BoxB > 1)
		BoxB = 0;
}
// draw plane
void Plane()
{

	glTranslatef(AirplaneX, 8, -7);
	// Main body
	glColor3d(0.5, 1, 0);
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(3, 0.4, 0.5);
	glutSolidSphere(1, 30, 30);
	glPopMatrix();

	glColor3d(0, 0, 0);
	glPushMatrix();
	glTranslated(1.7, 0.1, 0);
	glScaled(1.5, 0.7, 0.8);
	glRotated(40, 0, 1, 0);
	glutSolidSphere(0.45, 30, 30);
	glPopMatrix();


	//Right
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glRotated(-50, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(25, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();

	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-0.3, -0.15, 1.5);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.2, -0.15, 0.9);
	glRotated(90, 0, 1, 0);


	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	//Left
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0, 0, -1.2);
	glRotated(50, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(-25, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();

	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-0.3, -0.15, -1.5);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.2, -0.15, -0.9);
	glRotated(90, 0, 1, 0);
	glScaled(0.1, 0.1, 0.9);
	glutSolidTorus(0.5, 0.5, 50, 50);
	glPopMatrix();

	//draw rear side wing of plane
	glPushMatrix();
	glTranslated(-2.8, 0, 0);
	glScaled(0.8, 0.5, 0.3);

	//Right
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.4, 0, 1.5);
	glRotated(-30, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(10, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();

	//left
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(0.4, 0, -1.5);
	glRotated(30, 0, 1, 0);
	glScaled(0.7, 0.1, 3);
	glRotated(-10, 0, 1, 0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	// draw rear wing of plane center
	glColor3d(0.8, 1, 0);
	glPushMatrix();
	glTranslated(-2.7, 0.5, 0);
	glRotated(45, 0, 0, 1);
	glScaled(0.8, 2, 0.1);
	glRotated(-20, 0, 0, 1);
	glutSolidCube(0.5);
	glPopMatrix();

	glTranslatef(-AirplaneX, -8, 7);
}
//move plane
void movingPlane()
{
	AirplaneX += 0.01;
	if (AirplaneX > 5)
	{
		if (BoxY > 0)
			BoxY -= 0.05;
	}
}
// draw sun
void Sun()
{
	glTranslatef(0, 15, -40);
	glColor3f(1, 1, 0);
	glutSolidCone(5, 0, 20, 20);
	glTranslatef(0, -15, 40);
}
// draw moon
void Moon()
{
	glTranslatef(0, 25, -50);
	glColor3f(1, 1, 1);
	glutSolidCone(4, 0, 20, 20);
	glTranslatef(0, -25, 50);
}
// draw cloud
void Cloud()
{
	int i;
	double x = 0, y = 0;
	double radius = 0.8;
	glBegin(GL_TRIANGLE_FAN); //BEGIN CIRCLE
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	x++;
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}
	glEnd();

	x++;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}

	glEnd();

	x = x - 1.5;
	y++;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}

	glEnd();

	x++;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= 20; i++) {
		glVertex2f(
			(x + (radius * cos(i * 2 * PI / 20))), (y + (radius * sin(i * 2 * PI / 20)))
		);
	}

	glEnd();
}
// set location of clouds
void init_Clouds()
{
	glColor3f(0.5, 0.5, 0.5);
	glTranslatef(-6, 12, -20);
	Cloud();
	glTranslatef(6, -12, 20);

	glTranslatef(-20, 11, -20);
	Cloud();
	glTranslatef(20, -11, 20);

	glTranslatef(8, 10, -20);
	Cloud();
	glTranslatef(-8, -10, 20);

	glTranslatef(15, 9, -20);
	Cloud();
	glTranslatef(-15, -9, 20);

	glTranslatef(8, 17, -20);
	Cloud();
	glTranslatef(-8, -17, 20);

	glTranslatef(-15, 15, -20);
	Cloud();
	glTranslatef(15, -15, 20);
}
// draw bucket for arrows
void CylinderofArrows()
{
	float x = 0, y = 0, height = 1, angle = 0, radius = 0.5;
	float Ax = -5.4, Az = 0.3;//place all arrows in bucket

	//bucket						 				   
	glTranslatef(-5, 0, 0.5);
	glRotatef(90, 1, 0, 0);
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.9, 0.9, 0.3);
	while (angle < 2 * PI) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		glVertex3f(x, y, 0.0);
		angle = angle + 0.1;
	}
	glVertex3f(radius, 0, height);
	glVertex3f(radius, 0, 0);
	glEnd();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(5, 0, -0.5);

	//arrows
	for (int index = 0; index < ArrowStock; index++, Ax += 0.2) {

		if (Ax > -4.8)
		{
			Ax = -5.4;
			Az += 0.2;
		}

		glTranslatef(Ax, -0.5, Az);
		glRotatef(-90, 1, 0, 0);

		glBegin(GL_QUAD_STRIP);
		glColor3f(Arrow.r, Arrow.g, Arrow.b);
		Arrow.midx = 0;
		Arrow.midy = 0;
		Arrow.height = 1;
		Arrow.angle = 0;
		while (Arrow.angle < 2 * PI) {
			Arrow.midx = Arrow.radius * cos(Arrow.angle);
			Arrow.midy = Arrow.radius * sin(Arrow.angle);
			glVertex3f(Arrow.midx, Arrow.midy, Arrow.height);
			glVertex3f(Arrow.midx, Arrow.midy, 0.0);
			Arrow.angle = Arrow.angle + 0.1;
		}
		glVertex3f(Arrow.radius, 0.0, Arrow.height);
		glVertex3f(Arrow.radius, 0.0, 0.0);
		glEnd();

		glTranslatef(0, 0, 1);

		glRotatef(45, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(0.3, 0.6, 0.3);
		glVertex3f(0, 0, -0.3);
		glVertex3f(0, 0.1, 0);
		glVertex3f(0, -0.1, 0);
		glEnd();
		glRotatef(-45, 0, 0, 1);

		glRotatef(-45, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(0.3, 0.6, 0.3);
		glVertex3f(0, 0, -0.3);
		glVertex3f(0, 0.1, 0);
		glVertex3f(0, -0.1, 0);
		glEnd();
		glRotatef(45, 0, 0, 1);

		glTranslatef(0, 0, -1);


		glRotatef(90, 1, 0, 0);
		glTranslatef(-Ax, 0.5, -Az);
	}
}
// draw floor
void floor()
{
	glColor3f(0.2, 0.7, 0.3);
	glTranslatef(0, -1, 0);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, grassPic);

	glBegin(GL_QUADS);

	glNormal3f(0, 2, 0);	// normal straight up
	glTexCoord2f(1, 1);
	glVertex3f(-100, -1.5, -100);
	glTexCoord2f(0, 1);
	glVertex3f(100, -1.5, -100);
	glTexCoord2f(0, 0);
	glVertex3f(100, -1.5, 100);
	glTexCoord2f(1, 0);
	glVertex3f(-100, -1.5, 100);
	glEnd();

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(0, 1, 0);
}

//delay function
void delay(GLfloat number_of_seconds)
{
	// Converting time into milli_seconds 
	GLfloat milli_seconds = 1000 * number_of_seconds;

	// Stroing start time 
	clock_t start_time = clock();

	// looping till required time is not acheived 
	while (clock() < start_time + milli_seconds);
}
// collision calculations while building level
bool collisionBuildGame(unsigned char key)
{
	if (Targetbtn == 1)//if target picked
	{
		if (key == 'w' || key == 'W')//if moving object forward
		{
			for (int index = 0; index < treeind; index++)//check with tree array for collision
			{
				if (targetOB.z - zborder < tree[index].z + 2 && targetOB.z - zborder > tree[index].z - 2 && targetOB.x - xborder - 1.5 < tree[index].x && targetOB.x + xborder + 1.5 > tree[index].x) {
					return false;
				}
			}
			for (int index = 0; index < targetind; index++)//check with target array for collision
			{
				if (targetOB.z - zborder <= Target[index].z + zborder * 4 && targetOB.z - zborder >= Target[index].z - zborder && targetOB.x + xborder >= Target[index].x && targetOB.x - xborder <= Target[index].x) {
					return false;
				}
			}
			return true;
		}


		if (key == 's' || key == 'S')//if moving object backward
		{
			for (int index = 0; index < treeind; index++)
			{
				if (targetOB.z + zborder > tree[index].z - 2 && targetOB.z + zborder < tree[index].z + 2 && targetOB.x - xborder - 1.5 < tree[index].x && targetOB.x + xborder + 1.5 > tree[index].x) {
					targBehindTree = 1;
					return false;
				}
				targBehindTree = 0;
			}
			for (int index = 0; index < targetind; index++)
			{

				if (targetOB.z + zborder * 4 >= Target[index].z - zborder && targetOB.z + zborder <= Target[index].z + zborder && targetOB.x + xborder >= Target[index].x && targetOB.x - xborder <= Target[index].x) {
					return false;
				}
			}
			return true;
		}


		if (key == 'd' || key == 'D')//if moving object to right
		{
			for (int index = 0; index < treeind; index++)
			{
				if (targetOB.x + xborder > tree[index].x - 2 && targetOB.x + 2 < tree[index].x + 2 && targetOB.z - zborder - 1 < tree[index].z && targetOB.z + zborder + 1 > tree[index].z) {
					return false;
				}
			}
			for (int index = 0; index < targetind; index++)
			{
				if (targetOB.x + xborder >= Target[index].x - xborder && targetOB.x + xborder <= Target[index].x + xborder && targetOB.z - zborder <= Target[index].z + zborder && targetOB.z - zborder >= Target[index].z - zborder) {
					return false;
				}
			}
			return true;
		}

		if (key == 'a' || key == 'A')// if moving object left
		{
			for (int index = 0; index < treeind; index++)
			{
				if (targetOB.x - xborder < tree[index].x + 2 && targetOB.x - 2 > tree[index].x - 2 && targetOB.z - zborder - 1 < tree[index].z && targetOB.z + zborder + 1 > tree[index].z) {
					return false;
				}
			}
			for (int index = 0; index < targetind; index++)
			{
				if (targetOB.x - xborder <= Target[index].x + xborder && targetOB.x - xborder >= Target[index].x - xborder && targetOB.z - zborder <= Target[index].z + zborder && targetOB.z - zborder >= Target[index].z - zborder) {
					return false;
				}
			}
			return true;
		}
	}

	if (Treebtn == 1)// if tree was picked
	{
		if (key == 'w' || key == 'W')//if moving object forward
		{
			for (int index = 0; index < targetind; index++)
			{
				if (treeOB.z - 2 <= Target[index].z + zborder && treeOB.z - 2 >= Target[index].z - zborder && treeOB.x - 2 <= Target[index].x && treeOB.x + 2 >= Target[index].x) {
					targBehindTree = 1;
					return false;
				}
				targBehindTree = 0;
			}
			for (int index = 0; index < treeind; index++)
			{
				if (treeOB.z - 2.5 <= tree[index].z + 2.5 && treeOB.z - 2.5 >= tree[index].z - 2.5 && treeOB.x + 2.5 >= tree[index].x && treeOB.x - 2.5 <= tree[index].x) {
					return false;
				}
			}
			return true;
		}

		if (key == 's' || key == 'S')//if moving object backwards
		{
			for (int index = 0; index < targetind; index++)
			{
				if (treeOB.z + 2 > Target[index].z - zborder && treeOB.z + 2 < Target[index].z + zborder && treeOB.x - 2 < Target[index].x && treeOB.x + 2 > Target[index].x) {
					targBehindTree = 1;
					return false;
				}
				targBehindTree = 0;
			}
			for (int index = 0; index < treeind; index++)
			{
				if (treeOB.z + 2.5 >= tree[index].z - 2.5 && treeOB.z + 2.5 <= tree[index].z + 2.5 && treeOB.x + 2.5 >= tree[index].x && treeOB.x - 2.5 <= tree[index].x) {
					return false;
				}
			}
			return true;
		}

		if (key == 'd' || key == 'D')//if moving object right
		{
			for (int index = 0; index < targetind; index++)
			{
				if (treeOB.x + 2 >= Target[index].x && treeOB.x + 2 <= Target[index].x && treeOB.z - zborder <= Target[index].z && treeOB.z + zborder >= Target[index].z) {
					return false;
				}
			}
			for (int index = 0; index < treeind; index++)
			{
				if (treeOB.x + 2 >= tree[index].x - 2 && treeOB.x + 2 <= tree[index].x + 2 && treeOB.z - 2 <= tree[index].z && treeOB.z + 2 >= tree[index].z) {
					return false;
				}
			}
			return true;
		}

		if (key == 'a' || key == 'A')//if moving object left
		{
			for (int index = 0; index < targetind; index++)
			{
				if (treeOB.x - 2 <= Target[index].x && treeOB.x - 2 >= Target[index].x && treeOB.z - zborder <= Target[index].z && treeOB.z + zborder >= Target[index].z) {
					return false;
				}
			}
			for (int index = 0; index < treeind; index++)
			{
				if (treeOB.x - 2 <= tree[index].x + 2 && treeOB.x - 2 >= tree[index].x - 2 && treeOB.z - 2 <= tree[index].z && treeOB.z + 2 >= tree[index].z) {
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
//initialize player stats
void ResetStats()
{
	Arrowposx = eyeX + cx;
	Arrowposy = 0;
	Arrowposz = eyeZ + cz - 1;
	ArrowPower = 0;
	time_ = 0;
	ArrowThrow = false;
}
//calculate collision of arrow with targets
bool isHit()
{
	//collision with box
	if (Arrowposx >= BoxX - 0.5 && Arrowposx <= BoxX + 0.5 && Arrowposy >= BoxY - 1.2 && Arrowposy <= BoxY + 1 && Arrowposz <= BoxZ + 1 && Arrowposz >= BoxZ - 4)
	{
		score += 25;
		ArrowStock += 5;
		AirplaneX = -15;
		BoxY = 8;
		ResetStats();
		return true;
	}


	// check if collision with any target
	for (targetind = 0; targetind < TargetArraylen; targetind++)
	{
		if (Target[targetind].x - 1 < Arrowposx && Target[targetind].x + 1 > Arrowposx && Target[targetind].y - 1 < Arrowposy && Target[targetind].y + 1 > Arrowposy && Target[targetind].z > Arrowposz)
		{
			ArrowHits++;
			hiTarMSGYvissible = true;

			if (Target[targetind].x - 0.2 < Arrowposx && Target[targetind].x + 0.2 > Arrowposx && Target[targetind].y - 0.2 < Arrowposy && Target[targetind].y + 0.2 > Arrowposy)
			{
				points = 125;
				score += points;
			}
			else if (Target[targetind].x - 0.4 < Arrowposx && Target[targetind].x + 0.4 > Arrowposx && Target[targetind].y - 0.4 < Arrowposy && Target[targetind].y + 0.4 > Arrowposy)
			{
				points = 100;
				score += points;
			}
			else if (Target[targetind].x - 0.6 < Arrowposx && Target[targetind].x + 0.6 > Arrowposx && Target[targetind].y - 0.6 < Arrowposy && Target[targetind].y + 0.6 > Arrowposy)
			{
				points = 75;
				score += points;
			}
			else if (Target[targetind].x - 0.8 < Arrowposx && Target[targetind].x + 0.8 > Arrowposx && Target[targetind].y - 0.8 < Arrowposy && Target[targetind].y + 0.8 > Arrowposy)
			{
				points = 50;
				score += points;
			}
			else if (Target[targetind].x - 1 < Arrowposx && Target[targetind].x + 1 > Arrowposx && Target[targetind].y - 1 < Arrowposy && Target[targetind].y + 1 > Arrowposy)
			{
				points = 25;
				score += points;
			}
			else
			{
				points = 25;
				score += points;
			}

			Target[targetind].x = -50;
			Target[targetind].y = -50;
			Target[targetind].z = 50;
			Target[targetind].hit = true;
			ResetStats();
			return true;
		}
	}

	//collision with bird
	for (int bird = 0; bird < 3; bird++)
	{
		if (birds[bird].X + 1.4 >= Arrowposx && birds[bird].X - 1 <= Arrowposx && birds[bird].Y + 0.5 >= Arrowposy && birds[bird].Y - 0.5 <= Arrowposy && birds[bird].Z + 1 >= Arrowposz && birds[bird].Z - 1 <= Arrowposz)
		{
			score -= 25;
			birdhit = true;
			msgtime = t;
			ResetStats();
			return true;
		}
	}
	return false;
}
//calculate track time of arrow according to power and angle of arrow
GLfloat Time_of_Flight()
{
	if (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2')
	{
		if (((2 * ArrowPower * sinf(ArrowAngle * (PI / 180.0))) / gravity) > 0)
			return (((2 * ArrowPower * sinf(ArrowAngle * (PI / 180.0))) / gravity));
		else
			return((-1) * ((2 * ArrowPower * sinf(ArrowAngle * (PI / 180.0))) / gravity));
	}

	//for level 3
	return (((2 * ArrowPower * sinf(ArrowAngle * (PI / 180.0))) / (gravity + randomResistance)));
}

// calculate projectile of arrow
void Projectile(int t)
{
	if (time_ < TimeofFlight) //keep drawing the arrow while their is no collision with target or end of track
	{
		//calculate current location of arrow 
		Arrowposx = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * sinf(angleI * (PI / 180.0)) * time_);
		Arrowposy = (ArrowPower * sinf(ArrowAngle * (PI / 180.0)) * time_ - 0.5 * gravity * powf(time_, 2));
		Arrowposz = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * cosf(angleI * (PI / 180.0)) * time_) * (-1);
		time_ += 0.0035;

		//check if arrow hit target
		if (isHit()) {
			printf("HIT\n");
			return;
		}

		if (Arrowposy < -2.5) {
			ResetStats();
			return;
		}

		ArrowThrow = true;
		glutPostRedisplay();
		glutTimerFunc(1, Projectile, 0);
	}
	else if (Arrowposy > -2.5 && time_ >= TimeofFlight)// if arrow location off track and track time passed
	{
		//calculate current location of arrow 
		Arrowposx = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * sinf(angleI * (PI / 180.0)) * time_);
		Arrowposy = (ArrowPower * sinf(ArrowAngle * (PI / 180.0)) * time_ - 0.5 * gravity * powf(time_, 2));
		Arrowposz = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * cosf(angleI * (PI / 180.0)) * time_) * (-1);
		time_ += 0.0035;

		//check if arrow hit target
		if (isHit()) {
			printf("HIT\n");
			return;
		}

		ArrowThrow = true;
		glutPostRedisplay();
		glutTimerFunc(1, Projectile, 0);
	}
	else // if track of arrow ended then reset all stats
	{
		ResetStats();
		if (ArrowStock == 0)
		{
			screen = 11;
			return;
		}
		return;
	}
}
// projectile of arrow with wind resistance
void ProjectileResistance(int t)
{
	if (time_ < TimeofFlight)
	{
		//calculate current location of arrow 
		Arrowposx = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * sinf(angleI * (PI / 180.0)) * time_);
		Arrowposy = (ArrowPower * sinf(ArrowAngle * (PI / 180.0)) * time_ - 0.5 * (gravity + randomResistance) * powf(time_, 2));
		Arrowposz = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * cosf(angleI * (PI / 180.0)) * time_) * (-1);

		time_ += 0.0035;

		if (isHit())
			return;

		if (Arrowposy < -2.5) {
			ResetStats();
			return;
		}

		ArrowThrow = true;
		glutPostRedisplay();
		glutTimerFunc(1, ProjectileResistance, 0);
	}
	else if (time_ >= TimeofFlight && Arrowposy > -2.5)
	{
		//calculate current location of arrow 
		Arrowposx = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * sinf(angleI * (PI / 180.0)) * time_);
		Arrowposy = (ArrowPower * sinf(ArrowAngle * (PI / 180.0)) * time_ - 0.5 * (gravity + randomResistance) * powf(time_, 2));
		Arrowposz = (ArrowPower * cosf(ArrowAngle * (PI / 180.0)) * cosf(angleI * (PI / 180.0)) * time_) * (-1);

		time_ += 0.0035;

		if (isHit())
			return;

		ArrowThrow = true;
		glutPostRedisplay();
		glutTimerFunc(1, ProjectileResistance, 0);
	}
	else {
		ResetStats();
		if (ArrowStock == 0)
		{
			screen = 11;
			return;
		}
		return;
	}
}
//reset level screen
void ResetLevel(int status)
{
	//after finishing a level reset all stats
	if (status == 1)
	{
		glClearColor(1, 1, 0, 1);
		glColor3f(0, 0, 0);
		output(-40, 40, "you are out arrows your level will reset in 5 seconds");
		glutSwapBuffers();
		OpenFile(4);
		Sleep(5000);

		if (strOfUserLevel[0] == '1')
			ArrowStock = TargetArraylen + 8;
		if (strOfUserLevel[0] == '2')
			ArrowStock = TargetArraylen + 9;
		else
			ArrowStock = TargetArraylen + 7;

		AirplaneX = -15;
		BoxY = 8;
		t = 0;
		Arrowposx = eyeX + cx;
		Arrowposz = eyeZ + cz - 1;
		ArrowHits = 0;
		gamearrayflag = 1;
		screen = 6;
		return;
	}

	//if button pressed then reset level
	if (status == 2)
	{
		OpenFile(4);

		if (strOfUserLevel[0] == '1')
			ArrowStock = TargetArraylen + 8;
		if (strOfUserLevel[0] == '2')
			ArrowStock = TargetArraylen + 9;
		else
			ArrowStock = TargetArraylen + 7;

		AirplaneX = -15;
		BoxY = 8;
		t = 0;
		Arrowposx = eyeX + cx;
		Arrowposz = eyeZ + cz - 1;
		ArrowHits = 0;
		gamearrayflag = 1;
		screen = 6;
		return;
	}
}
// input from keyboard
void keyboard(unsigned char key, int x, int y)
{
	if (screen == 3)
	{
		if (key == '\r')
		{
			OpenFile(1);
		}
	}
	if (screen == 2)
	{
		if (key == '\r')
		{
			OpenFile(2);
		}
	}
	if (name == true)//enter username information
	{
		if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '1' && key <= '9'))
			if (indexOfUserName < 10) {
				strOfUserName[indexOfUserName++] = key;
				strOfUserName[indexOfUserName] = '\0';
			}
	}
	if (pass == true)//enter password information
	{
		if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '1' && key <= '9'))
			if (indexOfUserNamePass < 10) {
				strOfUserNamepass[indexOfUserNamePass++] = key;
				strOfUserNamepass[indexOfUserNamePass] = '\0';
			}
	}

	if (screen == 13)
	{
		if (key == '\r')
		{
			buildGameMAX = &ObjNumUser;
			gamearrayflag = 1;
			objectnum = *buildGameMAX;
			screen = 5;
		}
	}

	if (key == 27) exit(1);

	if (screen == 6)
		if (key == 'r' || key == 'R')
			ResetLevel(2);

	if (buildGamebtn == 1)
	{
		if (key == '\r')//save object in array
		{
			if (targBehindTree == 0)//check if tree is covering target
			{
				if (Treebtn == 1)
				{
					if (treeOB.x < -2 || treeOB.x > 2 || treeOB.z > -10 || treeOB.z < -14)//check if object location is not in center
					{
						//save tree to tree array and add to game array
						int x, z;
						treeOB.status = 1;
						tree = ((Object*)realloc(tree, sizeof(Object) * (treeind + 1)));
						if (tree == NULL)  exit(1);
						tree[treeind] = treeOB;
						x = 2 * tree[treeind].x + 22;
						z = 2 * -(tree[treeind].z);
						Game[z][x] = '1';
						treeind++;
						Treebtn = 0;
						Targetbtn = 0;
						objectnum--;
					}
					else
					{
						ObjCenter = 1;
						targBehindTree = 0;
						AtLeastTargFLAG = 0;
					}
				}
				else if (Targetbtn == 1)
				{
					if (targetOB.x < -2 || targetOB.x>2 || targetOB.z > -10 || targetOB.z < -14)//check if object location is not in center
					{
						//save target to target array and add to game array
						int x, z;
						targetOB.status = 1;
						Target = ((Object*)realloc(Target, sizeof(Object) * (targetind + 1)));
						if (Target == NULL) exit(1);
						Target[targetind] = targetOB;
						x = 2 * Target[targetind].x + 22;
						z = 2 * -(Target[targetind].z);
						Game[z][x] = '2';
						targetind++;
						Targetbtn = 0;
						Treebtn = 0;
						objectnum--;
					}
					else
					{
						ObjCenter = 1;
						targBehindTree = 0;
						AtLeastTargFLAG = 0;
					}
				}
			}
		}
	}

	if (Treebtn == 1)// moving the tree in building game screen
	{
		if (key == 'w' || key == 'W')
			if (collisionBuildGame(key) && treeOB.z > -50)
				treeOB.z -= speed;

		if (key == 's' || key == 'S')
			if (collisionBuildGame(key) && treeOB.z < -7)
				treeOB.z += speed;

		if (key == 'a' || key == 'A')
			if (collisionBuildGame(key) && treeOB.x > -11)
				treeOB.x -= speed;

		if (key == 'd' || key == 'D')
			if (collisionBuildGame(key) && treeOB.x < 13)
				treeOB.x += speed;
	}

	if (Targetbtn == 1)//moving the target in building game screen
	{
		if (key == 'w' || key == 'W') {
			if (strOfUserLevel[0] != '3') {
				if (collisionBuildGame(key) && targetOB.z > -30)
					targetOB.z -= speed;
			}
			else if (strOfUserLevel[0] == '3') {
				if (collisionBuildGame(key) && targetOB.z > -17)
					targetOB.z -= speed;
			}
		}

		if (key == 's' || key == 'S')
			if (collisionBuildGame(key) && targetOB.z < -7)
				targetOB.z += speed;

		if (key == 'a' || key == 'A')
			if (collisionBuildGame(key) && targetOB.x > -11)
				targetOB.x -= speed;

		if (key == 'd' || key == 'D')
			if (collisionBuildGame(key) && targetOB.x < 13)
				targetOB.x += speed;
	}
	glutPostRedisplay();
}

//draw power bar of arrow (calculate by mouse left click)
void drawPower(GLfloat power)
{
	const int numDiv = 15;
	const float sep = 0.04;
	const float barHeight = 1.0 / (float)numDiv;
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	for (float i = 0; i < power; i += (sep + barHeight)) {
		glVertex2f(0, i);
		glVertex2f(1, i);
		glVertex2f(1, i + barHeight);
		glVertex2f(0, i + barHeight);
	}
	glEnd();
}
// left click on mouse
void holdclick()
{
	
	if (leftclick && ArrowThrow == false && ArrowStock > 0)
	{
		LineZ += 0.0005;//move thread backwards
		Arrowposz += 0.0005;//move arrow backwards
		ArrowPower += 0.0225;// add power to arrow
		if (ArrowPower < 25)
		{
			glTranslatef(-7, -2, -4);
			drawPower(ArrowPower / 5);
			glTranslatef(7, 2, 4);
		}

		if (ArrowPower > 25 && (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2'))// maximum power
		{
			ArrowPower = 25;
			glTranslatef(-7, -2, -4);
			drawPower(ArrowPower / 5);
			glTranslatef(7, 2, 4);
		}


		if (ArrowPower > 30 && strOfUserLevel[0] == '3')
		{
			ArrowPower = 30;
			glTranslatef(-7, -1, -4);
			drawPower(ArrowPower / 5);
			glTranslatef(7, 1, 4);
		}

		if (LineZ > 0.3)
		{
			LineZ = 0.3;
			if (rotx < -14)
			{
				Arrowposz = 3.7;
			}
			else if (rotx > 14) {
				Arrowposz = 3.7;
			}
			else
				Arrowposz = 3.3;
		}
	}
	glutPostRedisplay();
}
// mouse moving and rotating
void mouse(int x, int y)
{
	//printf("%d %d\n", x, y);
	//calculations for rotations of mouse and moving
	if (ArrowThrow == false && screen == 6) {
		if (x < xscreen / 2)
		{
			if (rotx > -20)
			{
				angle -= 0.025f;
				cx = sin(angle);
				cz = -cos(angle);
				Arrowposx = sin(angle);
				if (!leftclick)
					Arrowposz = -cos(angle);
				angleI = angle * 57.0;
				rotx -= 0.5;
				//printf("angleI=%f,rotx=%f,angle=%f,cx=%f,cz=%f,ArrowX=%f,ArrowZ=%f,eyeX=%f,eyeZ=%f\n", angleI, rotx, angle, cx, cz, Arrowposx, Arrowposz, eyeX, eyeZ);
			}

		}
		if (x > xscreen / 2)
		{
			if (rotx < 20)
			{
				angle += 0.025f;
				cx = sin(angle);
				cz = -cos(angle);
				Arrowposx = sin(angle);
				if (!leftclick)
					Arrowposz = -cos(angle);

				angleI = angle * 57.0;
				rotx += 0.5;
				//printf("angleI=%f,rotx=%f,angle=%f,cx=%f,cz=%f,ArrowX=%f,ArrowZ=%f,eyeX=%f,eyeZ=%f\n", angleI, rotx, angle, cx, cz, Arrowposx, Arrowposz, eyeX, eyeZ);
			}
		}
		if (y < yscreen / 2)
		{
			roty -= 0.0125;
			if (roty < 0)
				ArrowAngle += 1;
			cy += 0.0125;
			Arrowposy += 0.0125;

			if (ArrowAngle > 16)
				ArrowAngle = 16;

			if (roty < -0.2)
			{
				roty = -0.2;
				cy = 0.7;
				Arrowposy = 0.2;
				ArrowAngle = 16;
			}
			//printf("roty=%f ,ArrowAngle=%f\n", roty, ArrowAngle);

		}
		if (y > yscreen / 2)
		{
			roty += 0.0125;
			if (roty > 0)
				ArrowAngle -= 1;
			Arrowposy -= 0.0125;
			cy -= 0.0125;

			if (ArrowAngle < 0)
				ArrowAngle = 0;

			if (roty > 0.2)
			{
				cy = 0.3;
				ArrowAngle = 0;
				Arrowposy = -0.2;
				roty = 0.2;
			}
			//printf("roty=%f ,ArrowAngle=%f\n", roty, ArrowAngle);
		}
	}
}
// draw buttons on level building
void buttons()
{
	glBegin(GL_QUADS);
	glColor3f(1, 0.4, 0.3);
	glNormal3f(1, 0, 0);
	glVertex3f(-1, 1, 0);
	glVertex3f(-1, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glEnd();
}
// build game function
void buildGame()
{
	//print on tree button (Tree)
	glTranslatef(-2.5, 5.5, -6);
	buttons();
	glTranslatef(2.5, -5.5, 6);
	glColor3f(1, 1, 1);
	Tree_ = Treestr;
	glRasterPos3f(-3, 5.8, -5.8);
	do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *Tree_); while (*(++Tree_));

	//print on target button (Target)
	glTranslatef(1, 5.5, -6);
	buttons();
	glTranslatef(-1, -5.5, 6);
	glColor3f(1, 1, 1);
	Target_ = Targetstr;
	glRasterPos3f(0.5, 5.8, -5.8);
	do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *Target_); while (*(++Target_));

	if (targBehindTree == 1)// if tree covers target then print message
	{
		char TargBehindTree[50] = "YOU CAN'T SET A TARGET  BEHIND A TREE", * TargBehindTree_;//הדפסת הודעה
		AtLeastTargFLAG = 0;
		ObjCenter = 0;
		glColor3f(1.0, 0.0, 0.0);
		TargBehindTree_ = TargBehindTree;
		glRasterPos3f(3, 5, -6);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *TargBehindTree_); while (*(++TargBehindTree_));
	}

	if (AtLeastTargFLAG == 1) // if there is no 2 targets at least then print message
	{
		targBehindTree = 0;
		ObjCenter = 0;
		char AtLeastTargets[50] = "YOU MUST SET AT LEAST TWO TARGETS", * AtLeastTargets_;
		glColor3f(1.0, 0.0, 0.0);
		AtLeastTargets_ = AtLeastTargets;
		glRasterPos3f(3, 5, -6);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *AtLeastTargets_); while (*(++AtLeastTargets_));
	}

	if (ObjCenter == 1)// if user tries to place object in the center then print message
	{
		targBehindTree = 0;
		AtLeastTargFLAG = 0;
		char objectInTheCenter[50] = "YOU CAN'T PLACE OBJECT IN THE CENTER", * objectInTheCenter_;
		glColor3f(1.0, 0.0, 0.0);
		objectInTheCenter_ = objectInTheCenter;
		glRasterPos3f(3, 5, -6);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *objectInTheCenter_); while (*(++objectInTheCenter_));
	}

	if (Treebtn == 1 && Targetbtn == 0)//temp drawing of tree until saving to array
	{
		if (treeind < *buildGameMAX - AtLeastTarget)
		{
			glTranslatef(treeOB.x, treeOB.y, treeOB.z);
			Tree();
			glTranslatef(-treeOB.x, -treeOB.y, -treeOB.z);
		}
	}
	else if (Targetbtn == 1 && Treebtn == 0)//temp drawing of target until saving to array
	{
		glTranslatef(targetOB.x, targetOB.y, targetOB.z);
		target();
		glTranslatef(-targetOB.x, -targetOB.y, -targetOB.z);
	}

	for (int index = 0; index < treeind; index++)// draw all trees
	{
		if (tree[index].status == 1)
		{
			glTranslatef(tree[index].x, tree[index].y, tree[index].z);
			Tree();
			glTranslatef(-tree[index].x, -tree[index].y, -tree[index].z);
		}
	}

	for (int index = 0; index < targetind; index++)//  draw all targets
	{
		if (Target[index].status == 1)
		{
			glTranslatef(Target[index].x, Target[index].y, Target[index].z);
			target();
			glTranslatef(-Target[index].x, -Target[index].y, -Target[index].z);
			printf("x=%f, z=%f\n", Target[index].x, Target[index].z);
		}
	}
}

//calculations collision of moving targets
int movingTargetsCollisions(Object target)
{
	//collision of target with tree
	for (int treeind = 0; treeind < TreeArraylen; treeind++)
	{
		if (tree[treeind].x > 0)
		{
			if (target.x + xborder > tree[treeind].x - 2 && target.x + xborder <= tree[treeind].x + 2 && target.z - zborder < tree[treeind].z && target.z + zborder  > tree[treeind].z)
				return 1;


			if (target.x - xborder < tree[treeind].x + 2 && target.x - xborder >= tree[treeind].x - 2 && target.z - zborder < tree[treeind].z && target.z + zborder > tree[treeind].z)
				return -1;

		}
		else if (tree[treeind].x <= 0)
		{
			if (target.x + xborder > tree[treeind].x - 2 && target.x + xborder <= tree[treeind].x + 2 && target.z - zborder < tree[treeind].z && target.z + zborder  > tree[treeind].z)
				return 1;

			if (target.x - xborder < tree[treeind].x + 2 && target.x - xborder >= tree[treeind].x - 2 && target.z - zborder < tree[treeind].z && target.z + zborder > tree[treeind].z)
				return -1;

		}
	}

	//collision with two targets
	for (int targetind = 0; targetind < TargetArraylen; targetind++)
	{

		if (target.x != Target[targetind].x)
		{
			if (Target[targetind].x > 0) {

				if (target.x + xborder >= Target[targetind].x - xborder && target.x + xborder <= Target[targetind].x + xborder && target.z + zborder >= Target[targetind].z && target.z - zborder <= Target[targetind].z)
					return 2;

				else if (target.x - xborder <= Target[targetind].x + xborder && target.x - xborder >= Target[targetind].x - xborder && target.z + zborder >= Target[targetind].z && target.z - zborder <= Target[targetind].z)
					return -2;
			}

			if (Target[targetind].x <= 0) {

				if (target.x + xborder >= Target[targetind].x - xborder && target.x + xborder <= Target[targetind].x + xborder && target.z + zborder >= Target[targetind].z && target.z - zborder <= Target[targetind].z)
					return 2;

				else if (target.x - xborder <= Target[targetind].x + xborder && target.x - xborder >= Target[targetind].x - xborder && target.z + zborder >= Target[targetind].z && target.z - zborder <= Target[targetind].z)
					return -2;
			}
		}
	}
	return 0;
}
// moving targets in level 2
void movingTargets()
{
	// move targets and change directions
	for (int index = 0; index < TargetArraylen; index++)
	{
		if (Target[index].x > 0)
		{
			if (Target[index].fr == 1 && Target[index].x + xborder < 13 && (movingTargetsCollisions(Target[index]) != 1) && (movingTargetsCollisions(Target[index]) != 2))
				Target[index].x += TargetSpeed;


			else if (Target[index].fr == 1 && (Target[index].x + xborder > 13 || (movingTargetsCollisions(Target[index]) == 1) || (movingTargetsCollisions(Target[index]) == 2)))
			{
				Target[index].fr = 0;
				Target[index].fl = 1;
			}
			else if (Target[index].fl == 1 && Target[index].x - xborder > 0 && (movingTargetsCollisions(Target[index]) != -1) && (movingTargetsCollisions(Target[index]) != -2))
				Target[index].x -= TargetSpeed;

			else if (Target[index].fl == 1 && (Target[index].x - xborder <= 0 || (movingTargetsCollisions(Target[index]) == -1) || (movingTargetsCollisions(Target[index]) == -2)))
			{
				Target[index].fr = 1;
				Target[index].fl = 0;
			}
		}

		if (Target[index].x <= 0)
		{
			if (Target[index].fl == 1 && Target[index].x - xborder > -11 && (movingTargetsCollisions(Target[index]) != -1) && (movingTargetsCollisions(Target[index]) != -2))
				Target[index].x -= TargetSpeed;

			else if (Target[index].fl == 1 && (Target[index].x - xborder < -11 || (movingTargetsCollisions(Target[index]) == -1) || (movingTargetsCollisions(Target[index]) == -2)))
			{
				Target[index].fr = 1;
				Target[index].fl = 0;
			}

			else  if (Target[index].fr == 1 && Target[index].x + xborder <= 0 && (movingTargetsCollisions(Target[index]) != 1) && (movingTargetsCollisions(Target[index]) != 2))
				Target[index].x += TargetSpeed;

			else if (Target[index].fr == 1 && (Target[index].x + xborder >= 0 || (movingTargetsCollisions(Target[index]) == 1) || (movingTargetsCollisions(Target[index]) == 2)))
			{
				Target[index].fr = 0;
				Target[index].fl = 1;
			}
		}
	}
}
// save level in array then build level
void buildLevel()
{
	if (LevelMalloc == 1)
	{
		free(tree);
		free(Target);
		tree = Target = NULL;
		//allocate arrays sizes according to user choosing to targets and trees
		tree = (Object*)malloc(sizeof(Object) * treeind);
		Target = (Object*)malloc(sizeof(Object) * targetind);
		LevelMalloc = 0;
		TargetArraylen = targetind;
		TreeArraylen = treeind;

		targetind = treeind = 0;
		for (int rows = 0; rows < WorldZ; rows++) {
			for (int cols = 0; cols < WorldX; cols++) {
				if (Game[rows][cols] == '1')
				{
					tree[treeind].x = (float)(cols - 22) / 2;
					tree[treeind].y = 0;
					tree[treeind].z = -((float)rows) / 2;
					treeind++;
				}

				if (Game[rows][cols] == '2')
				{
					Target[targetind].x = (float)(cols - 22) / 2;
					if (Target[targetind].x <= 0) {
						Target[targetind].fr = 0;
						Target[targetind].fl = 1;
					}
					if (Target[targetind].x > 0) {
						Target[targetind].fr = 1;
						Target[targetind].fl = 0;
					}
					Target[targetind].y = 0;
					Target[targetind].z = -((float)rows) / 2;
					targetind++;
				}
			}
		}
		for (targetind = 0; targetind < TargetArraylen; targetind++)
			printf("ind=%d, X=%f, Y=%f, Z=%f\n", targetind, Target[targetind].x, Target[targetind].y, Target[targetind].z);
	}

	if (strOfUserLevel[0] == '2')
		movingTargets();

	for (targetind = 0; targetind < TargetArraylen; targetind++)
	{
		glTranslatef(Target[targetind].x, Target[targetind].y, Target[targetind].z);
		target();
		glTranslatef(-Target[targetind].x, -Target[targetind].y, -Target[targetind].z);
	}

	for (treeind = 0; treeind < TreeArraylen; treeind++)
	{
		glTranslatef(tree[treeind].x, tree[treeind].y, tree[treeind].z);
		Tree();
		glTranslatef(-tree[treeind].x, -tree[treeind].y, -tree[treeind].z);
	}
}

//display register screen
void registerscreen()
{
	glColor3f(0, 0, 0);
	glRasterPos2f(-17, 50);
	int ind = 0;
	while (strOfUserName[ind]) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, strOfUserName[ind++]);

	}
	int ind1 = 0;
	glRasterPos2f(-13, 45);
	while (strOfUserNamepass[ind1])//print * when entering password
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '*');
		ind1++;
	}
	output(-25, 60, "RIGESTER SCREEN");
	output(-35, 50, "Enter your name :");
	output(-35, 45, "Enter your password :");
	output(-35, 40, "Back");
	output(20, 0, "Copy Right to WALAA & HAYTHAM");
}
//display login screen
void loginscreen()
{
	glColor3f(1, 0, 0);
	glRasterPos2f(-17, 50);
	int ind = 0;
	while (strOfUserName[ind]) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, strOfUserName[ind++]);

	}
	int ind1 = 0;
	glRasterPos2f(-13, 45);
	while (strOfUserNamepass[ind1]) //print * when entering password
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '*');
		ind1++;
	}
	output(-25, 60, "LOGIN SCREEN");
	output(-35, 50, "Enter your name :");
	output(-35, 45, "Enter your password :");
	output(-35, 40, "Back");
	output(20, 0, "Copy Right to WALAA & HAYTHAM");
}
//display start screen
void startscreen()
{
	glColor3f(0.2, 0.5, 0.2);
	output(-25, 60, "WELCOME TO THE ARCHER MASTER 3D GAME ");
	output(-35, 50, "Login");
	output(-35, 45, "Register");
	output(-35, 40, "Guide");
	output(-35, 35, "Scores");
	output(-35, 30, "Tutorial");
	output(-35, 25, "Exit");
	output(20, 0, "Copy Right to WALAA & HAYTHAM");

	glTranslatef(7, 25, 70);
	target();
	glTranslatef(-7, -25, -70);

	glTranslatef(-2, 24.5, 78);
	init_Bow();
	glTranslatef(2, -24.5, -78);
}
//display toturial screen
void Guide()
{
	if (GuideScreen == 0)
	{
		glClearColor(1, 1, 0, 1);
		output(-25, 60, "ARCHER MASTER 3D GUIDE");
		output(-40, 50, "in this game you are going to have to hit the targets on your screen to be able to continue to the next level.");
		output(-40, 45, "each level will have it's own challanges and you have to pass them.");
		output(-40, 40, "every level will be built by you, you will choose how to design your own level.");

		glTranslatef(7, 12, 60);
		glRotatef(-90, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 1.5, 0);
		glVertex3f(1, 0, 0);
		glEnd();
		glRotatef(90, 0, 0, 1);
		glTranslatef(-7, -12, -60);
	}

	else if (GuideScreen == 1) {
		glClearColor(0.7, 0.9, 0, 1);
		output(-40, 50, "you will have two buttons on your screen, one for trees and the other for targets.");
		output(-40, 45, "you can choose between these two object to build your level.");
		output(-40, 40, "be careful you can build up to 10 objects but you must have at least two target on each level.");

		glTranslatef(7, 12, 60);
		glRotatef(-90, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 1.5, 0);
		glVertex3f(1, 0, 0);
		glEnd();
		glRotatef(90, 0, 0, 1);
		glTranslatef(-7, -12, -60);

		glTranslatef(-5, 12, 60);
		glRotatef(90, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 1.5, 0);
		glVertex3f(1, 0, 0);
		glEnd();
		glRotatef(-90, 0, 0, 1);
		glTranslatef(5, -12, -60);
	}

	else if (GuideScreen == 2) {

		glClearColor(0.5, 1, 1, 1);

		glTranslatef(-4, 31.5, 70);
		buttons();
		glTranslatef(4, -31.5, -70);
		glColor3f(0, 0, 0);
		Tree_ = Treestr;
		glRasterPos3f(-4, 31.5, 71);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *Tree_); while (*(++Tree_));

		glTranslatef(-7, 20, 60);
		Tree();
		glTranslatef(7, -20, -60);

		glTranslatef(4, 31.5, 70);
		buttons();
		glTranslatef(-4, -31.5, -70);
		glColor3f(0, 0, 0);
		Target_ = Targetstr;
		glRasterPos3f(3.2, 31.5, 71);
		do glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *Target_); while (*(++Target_));

		glTranslatef(5, 25, 70);
		target();
		glTranslatef(-5, -25, -70);

		output(-45, 52, "you have to press on each button in order to generate your object.");
		output(-10, 15, "once you have generated your object you move it with keyboard by pressing");
		output(-10, 10, "W, A, S, D");
		output(-10, 0, "to lock in your position just press ENTER");
		output(-10, -10, "press R to restart your level");
		output(30, -10, "press HERE to go back to start screen");

		glTranslatef(-5, 12, 60);
		glRotatef(90, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 1.5, 0);
		glVertex3f(1, 0, 0);
		glEnd();
		glRotatef(-90, 0, 0, 1);
		glTranslatef(5, -12, -60);
	}
}

//Simulator of how to place object on the space
void Tutorial()
{
	//print on tree button  (Tree)
	glTranslatef(-2.5, 5.5, -6);
	buttons();
	glTranslatef(2.5, -5.5, 6);
	glColor3f(r, g, b);
	text(Treestr, Tree_, -3, 5.8, -5.8);

	//print on target button  (Target)
	glTranslatef(1, 5.5, -6);
	buttons();
	glTranslatef(-1, -5.5, 6);
	glColor3f(r1, g1, b1);
	text(Targetstr, Target_, 0.5, 5.8, -5.8);

	floor();
	moveObject = false;
	pressEnter == false;
	pressMouse = true;
	if (order > 0)
	{
		if (r == 1) {
			r = g = b = 0;
		}
		else {
			r = g = b = 1;
		}

		treeOB.x = 0;
		treeOB.y = 0;
		treeOB.z = -12;
		order--;
	}

	if (order == 0)
	{
		glTranslatef(treeOB.x, treeOB.y, treeOB.z);
		Tree();
		glTranslatef(-treeOB.x, -treeOB.y, -treeOB.z);
		pressMouse = false;
		pressEnter = false;
		moveObject = true;

		if (treeOB.x > -10)
		{
			treeOB.x -= speed;
			delay(0.3);
		}
		else if (treeOB.x == -10 && treeOB.z > -25)
		{
			treeOB.z -= speed;
			delay(0.3);
		}
		else
			order--;
	}

	if (order == -1 || order == -2)
	{
		moveObject = false;
		pressMouse = false;
		pressEnter = true;
		order--;
	}

	if (order == -3)
	{

		tree = ((Object*)realloc(tree, sizeof(Object) * (treeind + 1)));
		if (tree == NULL)  exit(1);
		tree[treeind] = treeOB;
		order--;
	}

	if (order <= -4)
	{

		for (int index = 0; index < (treeind + 1); index++)
		{
			glTranslatef(tree[index].x, tree[index].y, tree[index].z);
			Tree();
			glTranslatef(-tree[index].x, -tree[index].y, -tree[index].z);
		}

	}

	if (order <= -4 && order > -14)
	{

		if (r1 == 1) {
			r1 = g1 = b1 = 0;
		}
		else {
			r1 = g1 = b1 = 1;
		}
		pressEnter = false;
		moveObject = false;
		pressMouse = true;

		targetOB.x = 0;
		targetOB.y = 0;
		targetOB.z = -12;

		order--;
	}

	if (order == -14)
	{
		glTranslatef(targetOB.x, targetOB.y, targetOB.z);
		target();
		glTranslatef(-targetOB.x, -targetOB.y, -targetOB.z);
		pressEnter = false;
		pressMouse = false;
		moveObject = true;

		if (targetOB.x < 7) {
			targetOB.x += speed;
			delay(0.3);
		}
		else if (targetOB.x == 7 && targetOB.z > -20) {
			targetOB.z -= speed;
			delay(0.3);
		}
		else
			order--;
	}

	if (order == -15 || order == -16)
	{
		pressMouse = false;
		moveObject = false;
		pressEnter == true;
		delay(1);
		order--;
	}

	if (order == -17)
	{
		Target = ((Object*)realloc(Target, sizeof(Object) * (targetind + 1)));
		if (Target == NULL) exit(1);
		Target[targetind] = targetOB;
		order--;
	}

	if (order <= -18)
	{
		for (int index = 0; index < (targetind + 1); index++)// draw all targets
		{
			glTranslatef(Target[index].x, Target[index].y, Target[index].z);
			target();
			glTranslatef(-Target[index].x, -Target[index].y, -Target[index].z);
		}
	}

	if (order == -18)
	{
		Sleep(2000);
		tree = Target = NULL;
		screen = 1;
		order = 10;
	}
	if (pressMouse == true)
	{
		delay(0.8);
		strcpy(tutstr1, "PRESS WHERE IT GLOWS TO CREATE TREE");
		if (order <= -4 && order > -14)
			strcpy(tutstr1, "PRESS WHERE IT GLOWS TO CREATE TARGET");
		glColor3f(0, 0, 0);
		text(tutstr1, tutstr1, 1.5, 3, -4);

	}
	if (moveObject == true)
	{
		strcpy(tutstr1, "MOVE OBJECT WITH KEYBOARD (W,A,S,D)");
		text(tutstr1, tutstr1, 2, 3, -4);
	}

	if (pressEnter == true)
	{
		strcpy(tutstr1, "PRESS ENTER TO SAVE OBJECT LOCATION");
		text(tutstr1, tutstr1, 2, 3, -4);
		delay(1);
	}
	glutPostRedisplay();
}
//display score table 
void ScoreTable()
{
	glColor3f(0, 0, 0);
	output(-5, 60, "Scores");
	output(40, -10, "Back");

	// draw special diamond to the first place player (Highest score)
	glTranslatef(-23, 42, 30);
	glColor3f(1, 0, 0);
	glRotatef(rotscore, 0, 1, 0);
	glutSolidOctahedron();
	glRotatef(-rotscore, 0, 1, 0);
	glTranslatef(23, -42, -30);

	if (rotscore < 360)
		rotscore += 0.1;
	else
		rotscore = 0;

	// sort players according to their score
	typedef std::function<bool(std::pair<std::string, string>, std::pair<std::string, string>)> Comparator;

	// Defining a lambda function to compare two pairs. It will compare two pairs using second field
	Comparator compFunctor =
		[](std::pair<std::string, string> elem1, std::pair<std::string, string> elem2)
	{
		return stoi(elem1.second) > stoi(elem2.second);
	};

	std::set<std::pair<std::string, string>, Comparator> setOfWords(
		ScoresTable.begin(), ScoresTable.end(), compFunctor);

	int x = -65, y = 50;
	char tmpstr[20], tmp2str[20];

	// print all registered players
	for (std::pair<std::string, string> element : setOfWords)
	{
		if (y < 0)
		{
			x += 35;
			y = 50;
		}
		strcpy(tmpstr, element.first.c_str());
		glColor3f(1, 0, 0);
		output(x, y, tmpstr);
		strcpy(tmp2str, element.second.c_str());
		glColor3f(1, 0, 0);
		output(x + 20, y, tmp2str);
		y -= 5;
	}
}
// load/new game screen display
void neworloadScreen()
{
	glColor3f(0, 0, 0);
	output(-25, 60, "Welcome To ARCHER MASTER 3D !!");
	output(-35, 50, "  New Game");
	output(-35, 45, "  Load Game");
	output(-35, 40, "  Exit Game 'Esc'");
	output(20, -15, "Copy Right to WALAA & HAYTHAM Inc.");
	output(-10, 25, "Choose your color");
	output(-45, 5, "Default");

	//choose color of bow
	glBegin(GL_QUADS);
	glColor3f(0.4, 0.3, 0);
	glVertex2d(-45, 10);
	glVertex2d(-38, 10);
	glVertex2d(-38, 17);
	glVertex2d(-45, 17);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0, 0.6, 1);
	glVertex2d(-30, 10);
	glVertex2d(-23, 10);
	glVertex2d(-23, 17);
	glVertex2d(-30, 17);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.7, 0, 0);
	glVertex2d(-15, 10);
	glVertex2d(-8, 10);
	glVertex2d(-8, 17);
	glVertex2d(-15, 17);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.7, 0.2, 1);
	glVertex2d(0, 10);
	glVertex2d(7, 10);
	glVertex2d(7, 17);
	glVertex2d(0, 17);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1, 0.5, 0);
	glVertex2d(15, 10);
	glVertex2d(22, 10);
	glVertex2d(22, 17);
	glVertex2d(15, 17);
	glEnd();

	if (fileflag == 1)
		output(30, 10, "No Levels Found");

}
// transition between levels screen
void NextLevel()
{
	glClearColor(0.74, 0.05, 0.1, 1);
	glColor3f(0, 0, 0);
	output(-25, 60, "Well done you finished the level");
	output(-35, 50, "Go to next level");
	output(-35, 40, "Exit");
}
// finish game screen
void FinishGameScreen()
{
	glClearColor(0.3, 1, 0.2, 1);
	glColor3f(0, 0, 0);
	output(-25, 60, "Good job you finished all levels");
	output(-35, 40, "Exit 'ESC'");
}
// draw rain
void DrawRain()
{

	int Rx = -5;
	glTranslatef(Rx, Ry - 2, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 2), 2);

	Rx++;

	glTranslatef(Rx, Ry - 1, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 1), 2);

	Rx++;

	glTranslatef(Rx, Ry - 3, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 3), 2);

	Rx++;

	glTranslatef(Rx, Ry - 5, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 5), 2);

	Rx++;

	glTranslatef(Rx, Ry, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -Ry, 2);

	Rx++;

	glTranslatef(Rx, Ry - 4, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 4), 2);

	Rx++;

	glTranslatef(Rx, Ry - 7, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 7), 2);

	Rx++;

	glTranslatef(Rx, Ry, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -Ry, 2);

	Rx++;

	glTranslatef(Rx, Ry - 5, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 5), 2);

	Rx++;

	glTranslatef(Rx, Ry - 4, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -(Ry - 4), 2);

	Rx++;

	glTranslatef(Rx, Ry, -2);
	glBegin(GL_LINES);
	glColor3f(0.41, 0.62, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -0.5, 0);
	glEnd();
	glTranslatef(-Rx, -Ry, 2);

	Ry -= 0.01;
	if (Ry < -4)
		Ry = 6;
}

// display messages in level
void PrintonScreen()
{
	char Arrow[17] = "Arrows Left : ", * A;
	_itoa(ArrowStock, &Arrow[14], 10);
	glColor3f(1, 0, 0);
	A = Arrow;
	glRasterPos3f(-20, 0, -4);
	do glutBitmapCharacter(font, *A); while (*(++A));
	glutPostRedisplay();

	//draw board
	glColor3f(0.75, 1, 0);
	glTranslatef(-20, 0, -5);

	glRotatef(60, 0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 1, 0);
	glVertex3f(3, 1, 0);
	glVertex3f(3, -1, 0);
	glVertex3f(-1, -1, 0);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(-0.7, 0.5, -0.1);
	glVertex3f(-0.2, 0.5, -0.1);
	glVertex3f(-0.2, -1.5, -0.1);
	glVertex3f(-0.7, -1.5, -0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.7, 0.5, -0.1);
	glVertex3f(2.2, 0.5, -0.1);
	glVertex3f(2.2, -1.5, -0.1);
	glVertex3f(2.7, -1.5, -0.1);
	glEnd();
	glRotatef(-60, 0, 1, 0);
	glTranslatef(20, 0, 5);

	char Score_[15] = "Score : ", * score_;
	_itoa(score, &Score_[8], 10);
	glColor3f(1, 0, 0);
	score_ = Score_;
	glRasterPos3f(18, 0.5, -4);
	do glutBitmapCharacter(font, *score_); while (*(++score_));
	glutPostRedisplay();

	if (strOfUserLevel[0] != '3')
	{
		char time[15] = "Time : ", * time1;
		_itoa(t, &time[7], 10);
		glColor3f(1, 0, 0);
		time1 = time;
		glRasterPos3f(18, -0.5, -4);
		do glutBitmapCharacter(font, *time1); while (*(++time1));
		glutPostRedisplay();
	}

	if (strOfUserLevel[0] == '3')
	{
		char Resistance[15] = "Wind : ", * wind;
		_itoa(randomResistance, &Resistance[7], 10);
		glColor3f(1, 0, 0);
		wind = Resistance;
		glRasterPos3f(18, -0.5, -4);
		do glutBitmapCharacter(font, *wind); while (*(++wind));
		glutPostRedisplay();
	}

	if (birdhit == true)
	{
		char hit[35] = "You hit a bird -25 points ", * hit_;
		glColor3f(1, 0, 0);
		hit_ = hit;
		glRasterPos3f(2, 3, -4);
		do glutBitmapCharacter(font, *hit_); while (*(++hit_));
		glutPostRedisplay();
		if (msgtime + 2 < t) {
			msgtime = 0;
			birdhit = false;
		}
	}

	for (int index = 0; index < TargetArraylen; index++)
	{

		if (Target[index].hit == true && hiTarMSGYvissible)
		{
			glColor3f(1, 0, 0);
			_itoa(points, &hitargetMSG[1], 10);
			text(hitargetMSG, hitargetMSG_, 2, hitargetMSGY, -4);
			hitargetMSGY += 0.005;
			if (hitargetMSGY > 10)
			{
				hiTarMSGYvissible = false;
				hitargetMSGY = 3;
			}
		}
	}

	//draw board
	glColor3f(0.75, 1, 0);
	glTranslatef(20, 0, -5);

	glRotatef(-60, 0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-1, 1, 0);
	glVertex3f(3, 1, 0);
	glVertex3f(3, -1, 0);
	glVertex3f(-1, -1, 0);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(-0.7, 0.5, -0.1);
	glVertex3f(-0.2, 0.5, -0.1);
	glVertex3f(-0.2, -1.5, -0.1);
	glVertex3f(-0.7, -1.5, -0.1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(2.7, 0.5, -0.1);
	glVertex3f(2.2, 0.5, -0.1);
	glVertex3f(2.2, -1.5, -0.1);
	glVertex3f(2.7, -1.5, -0.1);
	glEnd();
	glRotatef(60, 0, 1, 0);
	glTranslatef(-20, 0, 5);
}
// mouse click in all screens
void mouse(int btn, int state, int x, int y)
{
	if (screen != 6)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (screen == 1)//start screen
			{
				if ((x >= 402 && x <= 458) && (y <= 210 && y >= 197))//login				
					screen = 2;

				if ((x >= 402 && x <= 483) && (y <= 255 && y >= 240))//register				
					screen = 3;

				if ((x >= 402 && x <= 460) && (y <= 300 && y >= 285))//guide
				{
					GuideScreen = 0;
					screen = 10;
				}

				if ((x >= 402 && x <= 466) && (y >= 328 && y <= 344))
				{
					OpenFile(8);
					screen = 12;
				}

				if ((x >= 402 && x <= 480) && (y >= 370 && y <= 388))
				{
					screen = 14;
				}

				if ((x >= 402 && x <= 445) && (y >= 416 && y <= 431))
					exit(1);
			}


			if (screen == 2)//login screen
			{
				if ((x >= 403 && x <= 611) && (y <= 258 && y >= 242))
				{
					pass = true;
					name = false;
				}
				if ((x >= 403 && x <= 575) && (y <= 214 && y >= 196))
				{
					pass = false;
					name = true;
				}
				if ((x >= 403 && x <= 452) && (y <= 301 && y >= 284))
				{
					glRasterPos2f(-17, 50);
					strcpy(strOfUserName, "");
					indexOfUserName = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');

					glRasterPos2f(-13, 45);
					strcpy(strOfUserNamepass, "");
					indexOfUserNamePass = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
					screen = 1;
				}
			}

			if (screen == 3)//register screen
			{

				if ((x >= 403 && x <= 611) && (y <= 258 && y >= 242))
				{
					pass = true;
					name = false;
				}
				if ((x >= 403 && x <= 575) && (y <= 214 && y >= 196))
				{
					pass = false;
					name = true;
				}
				if ((x >= 403 && x <= 452) && (y <= 301 && y >= 284))
				{
					glRasterPos2f(-17, 50);
					strcpy(strOfUserName, "");
					indexOfUserName = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');

					glRasterPos2f(-13, 45);
					strcpy(strOfUserNamepass, "");
					indexOfUserNamePass = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
					screen = 1;
				}
			}

			if (screen == 4)
			{
				if (x >= 414 && x <= 526 && y >= 195 && y <= 210) // new game button
				{
					screenflag = 1;
					screen = 13;
					return;
				}

				if (x >= 414 && x <= 526 && y >= 239 && y <= 258)// load game button
				{
					for (int rows = 0; rows < WorldZ; rows++)
					{
						for (int cols = 0; cols < WorldX; cols++)
						{
							Game[rows][cols] = '0';
						}
					}
					try {
						OpenFile(4);
					}
					catch (int z) //מנסה לעשות טעינה שלב כאשר לא קיים שלב
					{
						screen = z;
						fileflag = 1;
						return;
					}

					if (strOfUserLevel[0] == '1')
						ArrowStock = targetind + 8;
					if (strOfUserLevel[0] == '2')
						ArrowStock = targetind + 7;
					else
						ArrowStock = targetind + 5;

					screenflag = 1;
					fileflag = 0;
					screen = 6;
					return;
				}

				//choose color of bow in play screen
				if (x >= 298 && x <= 371 && y >= 502 && y <= 562)
				{
					BowR = 0.4;
					BowG = 0.3;
					BowB = 0;
				}

				if (x >= 455 && x <= 528 && y >= 502 && y <= 562)
				{
					BowR = 0;
					BowG = 0.6;
					BowB = 1;
				}

				if (x >= 612 && x <= 685 && y >= 502 && y <= 562)
				{
					BowR = 0.7;
					BowG = 0;
					BowB = 0;
				}

				if (x >= 768 && x <= 841 && y >= 502 && y <= 562)
				{
					BowR = 0.7;
					BowG = 0.2;
					BowB = 1;
				}

				if (x >= 924 && x <= 999 && y >= 502 && y <= 562)
				{
					BowR = 1;
					BowG = 0.5;
					BowB = 0;
				}
			}

			if (screen == 5)//build level by player
			{
				if (targBehindTree == 0)//check if their is no target behind tree
				{
					if (buildGamebtn == 1)
					{
						if (objectnum > 0)
						{
							if ((x >= 486 && x <= 647) && (y >= 33 && y <= 83)) {
								if (treeind == *buildGameMAX - AtLeastTarget)//check if there is atleast two targets -> if no then display message
									AtLeastTargFLAG = 1;

								Treebtn = 1;
								Targetbtn = 0;
								treeOB.x = 0;
								treeOB.y = 0;
								treeOB.z = -12;
								treeOB.bounDirection = 0;
								return;
							}

							else if ((x >= 768 && x <= 929) && (y >= 30 && y <= 86)) {
								Treebtn = 0;
								Targetbtn = 1;
								targetOB.x = 0;
								targetOB.y = 0;
								targetOB.z = -12;
								targetOB.bounDirection = 0;
								return;
							}
						}
					}
				}
			}

			if (screen == 7)//transition between levels screen
			{
				if (x >= 403 && x <= 561 && y >= 194 && y <= 213)
				{
					screen = 13;
				}
				if (x >= 402 && x <= 443 && y >= 280 && y <= 298)
				{
					exit(0);
				}
			}

			if (screen == 9)//error screen display
			{
				if ((x >= 403 && x <= 450) && (y <= 258 && y >= 240))
				{
					glRasterPos2f(-17, 50);
					strcpy(strOfUserName, "");
					indexOfUserName = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');

					glRasterPos2f(-13, 45);
					strcpy(strOfUserNamepass, "");
					indexOfUserNamePass = 0;
					glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
					screen = 1;
				}
			}

			if (screen == 10)//guide screen to users
			{
				if ((x >= 1016 && x <= 1068) && (y >= 791 && y <= 851) && GuideScreen == 1)
					GuideScreen = 2;

				if (GuideScreen == 0 && (x >= 1016 && x <= 1068) && (y >= 791 && y <= 851))
					GuideScreen = 1;

				if ((x >= 535 && x <= 591) && (y >= 791 && y <= 851) && GuideScreen == 1)
					GuideScreen = 0;

				if ((x >= 535 && x <= 591) && (y >= 791 && y <= 851) && GuideScreen == 2)
					GuideScreen = 1;

				if ((x >= 1080 && x <= 1450) && (y >= 723 && y <= 743) && GuideScreen == 2)
					screen = 1;
			}

			if (screen == 12) //score screen
			{
				if ((x >= 1186 && x <= 1234) && (y >= 726 && y <= 740))
					screen = 1;
			}

			if (screen == 13)//object quantity choose screen
			{
				if ((x >= 558 && x <= 570) && (y <= 242 && y >= 232) && (ObjNumUser > 2))
					ObjNumUser -= 1;

				if ((x >= 735 && x <= 752) && (y <= 248 && y >= 230) && (ObjNumUser < 10))
					ObjNumUser += 1;
			}
		}
	}

	if (screen == 6)
	{
		if (ArrowThrow == false && (AirplaneX == -15 || AirplaneX > 20))
		{
			if (btn == GLUT_LEFT_BUTTON)
			{
				leftclick = (state == GLUT_DOWN);
				glutMotionFunc(mouse);
				glutPassiveMotionFunc(mouse);
				glutPostRedisplay();
			}
			if (state == GLUT_UP)
			{
				LineZ = 0;
				time_ = 0;

				if (ArrowPower >= 0.01) {
					Arrowposz = cz;
					ArrowThrow = true;
					//printf("1) ArrowPower=%f\n", ArrowPower);
					TimeofFlight = Time_of_Flight();
					if (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2') {
						Projectile(1);
					}
					else if (strOfUserLevel[0] == '3')
					{
						ProjectileResistance(1);
					}
					ArrowStock--;
				}
				glutPostRedisplay();
			}
		}
	}
	glutPostRedisplay();
}

// play sound
//currently disabled because I lost the .wav files
void sound(int playsound)
{
	/*
	if (playsound == 1)
		PlaySound("holdclick.wav", NULL, SND_FILENAME | SND_ASYNC);

	if (playsound == 2)//מוזיקה תחילת משחק
		PlaySound("intro.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	if (playsound == 3)//מוזיקה שמגיע מטוס
		PlaySound("Planeflying.wav", NULL, SND_FILENAME | SND_ASYNC);

	if (playsound == 0)//הפסקת המוזיקה
		PlaySound(NULL, 0, 0);
		*/
	glutPostRedisplay();
}
// screen to choose number of objects
void ObjectNumScreen()
{
	glColor3f(1, 1, 1);
	output(-25, 60, "INSERT OBJECT'S NUMBER");
	output(-25, 30, "PRESS ENTER TO START THE GAME");
	glTranslatef(0, 25, 85);

	glTranslatef(0, 0, -15);
	glTranslatef(-2, 3, 0);
	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(2, 0, 0);
	glVertex3f(-2, 2, 0);
	glVertex3f(-2, 0, 0);
	glVertex3f(2, 0, 0);
	glVertex3f(2, 2, 0);
	glEnd();
	glTranslatef(2, -3, 0);

	_itoa(ObjNumUser, &strObjNum[0], 10);
	glColor3f(1, 1, 1);
	text(strObjNum, strObjNum_, -2, 3.7, 0.2);

	text(charMinus, charMinus_, -3.5, 3.7, 0.2);
	text(charplus, charplus_, -0.5, 3.7, 0.2);

	glTranslatef(0, 0, 15);
}
// moving forward or backwards according to user behavior
void TimerActions()
{
	if (strOfUserLevel[0] == '1')
	{
		if (t <= 3)
		{
			strcpy(tutstr1, "MOVE AROUND AND SHOOT WITH MOUSE");
			text(tutstr1, tutstr1_, 2, 4.5, -4);
		}

		if (tmpTimer >= 5)
		{
			for (int index = 0; index < TargetArraylen; index++)
			{
				if (Target[index].hit == true)//if player hit atleast one target then move rest of targets backwards
				{
					for (int index1 = 0; index1 < TargetArraylen; index1++)
						if (Target[index1].z > -25)
							Target[index1].z -= 5;
					Target[index].hit = false;
				}
				tmpTimer = 0;
			}
		}
	}

	else if (strOfUserLevel[0] == '2')
	{
		for (int index = 0; index < TargetArraylen; index++)
		{
			if (Target[index].hit == true)
			{
				ArrowHitforTimer++;
				Target[index].hit = false;
			}
			if (ArrowHitforTimer == 0 && tmpTimer >= 5)//if 5 seconds passed and player didn't hit any target then move targets forward
			{
				for (int index1 = 0; index1 < TargetArraylen; index1++)
					if (Target[index1].z < -7)
						Target[index1].z += 2;
				tmpTimer = 0;
			}
		}
	}
}
//draw all game
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0, -25, -85);

	//music for starting screen
	if (screen == 1 || screen == 2 || screen == 3 || screen == 4 || screen == 10 || screen == 12 || screen == 9)
	{
		if (playcnt == 0) {
			sound(2);
			playcnt++;
			if (playcnt > 1)
				playcnt = 1;
		}
	}
	else if (screenflag == 1) {
		playcnt = 0;
		sound(0);
		screenflag = 0;
	}

	//screen = 1 ; start screen
	if (screen == 1)
	{
		glClearColor(0.3, 0.7, 0.5, 1);
		startscreen();
	}

	//screen = 2 ; login screen
	else if (screen == 2)
	{
		glClearColor(0.4, 0.5, 0.5, 1);
		loginscreen();
	}

	//screen = 3 ; register screen
	else if (screen == 3)
	{
		glClearColor(0.7, 0.3, 0.1, 1);
		registerscreen();
	}

	//screen = 4 ; load or new game screen
	else if (screen == 4)
	{
		glClearColor(0.3, 0.5, 0.2, 1);
		neworloadScreen();
	}

	// screen = 5 ; new game - create game
	else if (screen == 5)
	{
		glTranslatef(0, 25, 85);
		glClearColor(0.470, 0.619, 0.890, 0.0);
		glTranslatef(0, 0, 0);
		gluLookAt(0, 0.5, 5, 0, 0.5, 4, 0, 1, 0);

		if (gamearrayflag == 1)
		{
			for (int rows = 0; rows < WorldZ; rows++) {
				for (int cols = 0; cols < WorldX; cols++)
				{
					Game[rows][cols] = '0';
				}
			}
			gamearrayflag = 0;
			treeind = targetind = 0;
		}
		buildGamebtn = 1;
		floor();
		buildGame();

		if (objectnum == 0)
		{
			if (targetind >= AtLeastTarget)
			{
				OpenFile(3);
				buildGamebtn = 0;

				if (strOfUserLevel[0] == '1')
					ArrowStock = TargetArraylen + 8;
				if (strOfUserLevel[0] == '2')
					ArrowStock = TargetArraylen + 9;
				else
					ArrowStock = TargetArraylen + 7;

				gamearrayflag = 1;
				LevelMalloc = 1;
				screen = 6;
				t = 0;
				return;
			}
		}
	}

	//screen = 6 ; start level
	else if (screen == 6)
	{
		glTranslatef(0, 25, 85);

		if (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2')
			glClearColor(0.470, 0.619, 0.890, 0.0);

		else
			glClearColor(0, 0, 0.5, 0);

		glutWarpPointer(xscreen / 2, yscreen / 2); //keep pointer of mouse centered
		glutSetCursor(GLUT_CURSOR_NONE); //make cursor of mouse invisible
		glTranslatef(0, 0, 0);

		if (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2')
		{
			tmpTimer += 0.001;
			t += 0.001;
		}

		gluLookAt(eyeX, eyeY, eyeZ,
			eyeX + cx, cy, eyeZ + cz,
			0.0f, 1.0f, 0.0f);

		floor();

		if (gamearrayflag == 1) {
			OpenFile(4);
			LevelMalloc = 1;
			gamearrayflag = 0;
			if (strOfUserLevel[0] == '1')
				ArrowStock = TargetArraylen + 8;
			if (strOfUserLevel[0] == '2')
				ArrowStock = TargetArraylen + 9;
			else
				ArrowStock = TargetArraylen + 7;
		}

		if (strOfUserLevel[0] == '1' && ArrowHits >= 2)
		{
			Bird();
			movebirds();
		}

		if (ArrowStock < 6)
		{
			if (AirplaneX < 20) {
				Plane();
				movingPlane();

				if (playcnt == 0)
					sound(3);
				playcnt++;
				if (playcnt > 1)
					playcnt = 1;
			}
			else {
				sound(0);
				playcnt = 0;
			}
			Box();
		}

		buildLevel();
		CylinderofArrows();

		if (strOfUserLevel[0] == '1' || strOfUserLevel[0] == '2')
			Sun();

		if (strOfUserLevel[0] == '3')
		{
			Moon();
			init_Clouds();
		}

		PrintonScreen();

		if (!leftclick) {
			Arrowposx = eyeX + cx;
			if (ArrowThrow == false) {
				Arrowposz = eyeZ + cz - 1;
			}
		}

		init_Bow();
		init_Arrows();
		holdclick();
		TimerActions();

		if (strOfUserLevel[0] == '3')
		{
			DrawRain();
		}


		if (ArrowStock >= 0 && ArrowHits == TargetArraylen)
		{
			free(tree);
			free(Target);
			tree = Target = NULL;

			// if user finished level 1 in less than 30 seconds then his remaining time is multiplied by 5 and added to the score
			if (strOfUserLevel[0] == '1')
			{
				if (t <= 30)
					score += (30 - t) * 5;
			}

			// if user finished level 2 in less than 45 seconds then his remaining time is multiplied by 5 and added to the score
			else if (strOfUserLevel[0] == 2) {
				if (t <= 45)
					score += (30 - t) * 5;
			}

			t = 0;

			Sleep(1000);
			if (strOfUserLevel[0] == '3')
			{
				screen = 8;
				return;
			}
			ArrowHits = 0;
			OpenFile(5);
			OpenFile(6);
			gamearrayflag = 1;
			LevelMalloc = 1;
			screen = 7;
			//objectnum = *buildGameMAX;
			return;
		}
	}

	//screen = 7; Go to next level??
	else if (screen == 7)
	{
		glutSetCursor(GLUT_CURSOR_INHERIT);
		NextLevel();
	}

	//screen = 8; Last screen
	else if (screen == 8)
	{
		FinishGameScreen();
	}

	//screen = 9; wrong password
	else if (screen == 9)
	{
		for (int i = 0; i < 20; i++)
		{
			strOfUserName[i] = NULL;
			strOfUserNamepass[i] = NULL;
		}
		glClearColor(0.2, 0.2, 0.4, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		indexOfUserName = 0;
		indexOfUserNamePass = 0;
		output(-25, 60, "ARCHER MASTER 3D !!");
		output(-35, 50, "User Already Exists OR Password Incorrect !! ");
		output(-35, 45, "Back");
		output(20, 0, "Copy Right to WALAA & HAYTHAM Inc.");
	}

	//screen = 10; guide - how to play
	else if (screen == 10)
	{
		Guide();
	}

	//screen = 11; reset level if arrowstock = 0
	else if (screen == 11)
	{
		ResetLevel(1);
	}

	//screen = 12; ScoresTable
	else if (screen == 12)
	{
		glClearColor(0, 0.9, 1, 1);
		ScoreTable();
	}

	// screen = 13; choose objects quantity
	else if (screen == 13)
	{
		glClearColor(0, 0, 0, 1);
		ObjectNumScreen();
	}

	//screen = 14; Tutorial how to build the game
	else if (screen == 14)
	{

		glTranslatef(0, 25, 85);
		glClearColor(0.470, 0.619, 0.890, 0.0);
		glTranslatef(0, 0, 0);
		gluLookAt(0, 0.5, 5, 0, 0.5, 4, 0, 1, 0);

		Tutorial();
	}

	glutSwapBuffers();
}
// initialize OpenGL parameters
void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.5, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);

	// Lighting parameters

	GLfloat mat_ambdif[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess[] = { 80.0f };
	GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

	glClearColor(0.470, 0.619, 0.890, 0.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambdif);	// set both amb and diff components
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);		// set specular
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);		// set shininess
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);		// set light "position", in this case direction
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);		// active material changes by glColor3f(..)

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, grassPic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassPicW, grassPicH, 0, GL_RGB, GL_UNSIGNED_BYTE, picBytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


}
//main function
int main(int argc, char* argv[])
{
	picBytes = loadTexture("grass.ppm", &grassPicW, &grassPicH);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);	// RGB display, double-buffered, with Z-Buffer
	glutInitWindowSize(xscreen, yscreen);
	glutCreateWindow("ARCHER MASTER 3D");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouse);
	glutFullScreen();
	glutIdleFunc(draw);

	std::thread timer(delay, 0.001);//time thread
	std::thread main(draw);//thread for main function

	init();

	//join threads
	main.join();
	timer.join();

	srand(time(0));

	if (fileflag == 2) {
		FILE* file = fopen("scores", "a");
		fclose(file);
		fileflag = 0;
	}


	glutMainLoop();							// Start the main event loop
}


