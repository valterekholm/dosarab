#define __STDC_WANT_LIB_EXT1__  1

//TODO: Skriva ut alla tecken på en text fil också, med valbar bredd (tex 80, 120, 200, 300, 400 tecken (chars)).

//Inkluderar andra filer
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

//Definierar siffer-konstanter, med nyckelord
#define B_HOGER 1 //bindning till höger, tex b, tä, thä - men inte hamza
#define B_VANSTER 2 //b. till v som har t.ex. b, non, läm, men inte alif, r, z, d dh, waw

//waw 1 8 16 32

#define INITIAL 4 //Om det tecknet har en initial-form, som tex ba, tä, thä - men inte alif
#define FINAL 8 //Om tecknet har en slutord-form som flesta, b,t,th,dj,7,ch,s,sh,$,D,T,zh,3,gh,f,k,l,m,n,h,w,ija, alif maksora 

#define ENSAM 16 //har den ett ensamläge
#define MITTEN 32 //har den mittenläge (unikt grafiskt)

//En slags funktion genom "define" som tar ett argument (bind) och "returnerar" 1 eller 0 (sant/falskt)
//Ger 1 om en 7arf inte binds med föregående
#define IS_END_POINT(bind) (!(bind&B_VANSTER))?1:0

//En slags funktion
#define IS_SPACE(unicode)(8192==unicode)?1:0

//om denna 7arf (som nästkommande) "väntar" på föregående, dvs har högerbind (pos är index i harf-array 'bindelse')
//Som t.ex lam, djim, mim
#define IS_WAITING(pos) (1&_7bindelse[pos])?1:0

//de läggs ihop som t.ex gällande alif 32,16,1 = 49
//b : 1,2,4,8,16,32 = 63
//t : 1,2,4,8 = 63
//dj : 1,2,4,8,16,32
//tä marbota 1, 8, 16
//18694 alif maqsoura 1, 8, 16

//för pos 7arf, 0-3
static unsigned POSITION;

//
#define SET_POSITION(position) (position>=0&&position<4)?position:0

//Arabiska texter har kommatecken, och dessa har mellanrum på bägge sidor, inte som i europa... då endast mellanrum efter i.v.f.

//Kolla om tecken är komma, punkt, eller liknande //3078,6918,11776 (8192 är space)
#define IS_KOMMA(unicode)(unicode==3078)?1:0
#define IS_SEMI(unicode)(unicode==6918)?1:0
#define IS_PUNKT(unicode)(unicode==11776)?1:0

//Dos-skärm standard bredd = 80 tecken
#define DOS_WIDTH 80

//Gällande skrivning av mening nära vänsterkanten...
//Definiera gräns mellan ord korta (som flyttas och ej kapas) och ord långa (utmätta med dostecken)
//Att flytta innebär att ordet ej skrivs nära kanten utan flyttas till nästa rad i stället
//HÖGRE tal = mer marginal vänster

#define KALIMAH_SAGHIRAH 17

//När ord ändå bryts av, tas marginal på en 7arfs max bredd

#define _7ARF_MAX_WIDTH 5


//En datasamling som liknar objekt i obj.orien. programmering, men saknar medlems-funktioner
//En struct kan skickas till funktioner samt returneras från dem.
struct _7arf{
	short bindings;//både bindningar och lägen
	char widths[4];//ett teckens bredd 1 - 4 ... ensam init mellan slut
	char top_pos[4];//var översta delen är; 0 top (tex tä marb.), 1 (tex alif), 2, 3 (baslinje), 4 under bas

	short map[25];//max 5*5 från höger
	char pos;//TODO lagra 7arfens läge (ensam,init,mitten,slutet)
};


//ny
struct _7arf_dos_row{
	short area[400];
	char write_head;//var utskriften stannat
};

void set_7arf_Message(char *descr, short value);
int index_Of_7arf(short unicode, int arr_len, short * _unicode_arabics);
char get_7arf_Symbols(short unicode, struct _7arf *_7arf, char position);
//vilken 7arf och position, 0 ensam, 1 init, 2 mitten, 3 slutlig
void paint_7arf(struct _7arf _7arf, char position);
void _7arf_Info(struct _7arf _7arf);
void paint_7_Row();
//test med en 7arf, arg: en 7arf kopia, pos: vilket läge av 0 ensam, 1 initial, 2 mitten, 3 avslut
void paint_7_test(struct _7arf * _7arf, char pos);

int get_7uroufline_dos(char * buffer, struct _7arf_dos_row * dosarea, short startpos);
int put_symbol_dosarea(short x, short y, struct _7arf_dos_row * dosarea, short value);

/**Skriver ut en rad på skärm från ett (_dos_row) minne**/
int print_7urouf_row(struct _7arf_dos_row * dosarea);


//TODO: definiera hur långt ett ord som bryts ska vara : kalla det "_ordbryt" - och detta kopplat till gränsvärde för ordbryt
//TODO: En förläsare (_ordbryt 7urouf före tecken som byggs) som kollar om det kommer mellanrum, nära skärmens vänsterkant
// ... om det gör (skriv ut ordet - som är kortare än _ordbryt i längd), annars skriv ordet på två rader (brutet).

int main(void){

	//unicode kod-värden för grafiska element
	int chc[]={
		170,179,180,185,186,187,188,191,192,193,
		194,195,196,197,200,201,202,203,204,205,
		206,217,218,219,220,221,223,245,250,255,
		432,433,434,494,498,504,509,510};

	char* _7urouf[]={"alif","ba","ta","tha","djiim","7aa", "cha", "dal", "dhal", "ra",
		"zin","siin", "shiin", "saad", "daad", "taa", "zha", "ayn", "ghayn", "faa",
		"qaaf", "kaaf",	"laam", "miim", "nuun", "ha", "waaw", "yaa", "hamza", "ta-marbouta",
		"alif-hamza","alif-hamza-i", "alif-meddah","alif-maqsura","arab-komma","arab-semiko","[space]","arab-punkt"};

	//beskriv resp. 7arf - bindelse (h/v) och unika positioner   ---obs en fattas...
	short _7bindelse[]={49,63,63,63,63,63,63,25,25,25,
		25,63,63,63,63,63,63,63,63,63,
		63,63,63,63,63,63,25,63,16,25,
		49,5,49,25,16,16,16};

	short _unicode_arabics[]={
		9990,10246,10758,11014,11270,11526,11782,12038,12294,12550,
		12806,13062,13318,13574,13830,14086,14342,14598,14854,16646,
		16902,17158,17414,17670,17926,18182,18438,18950,8454,10502,
		8966,9478,8710,18694,3078,6918,8192,11776};

	//lagrar text beskrivning av 7arfs bindning-egenskap
	char description[150];

	//en rad 7urouf
	struct _7arf_dos_row dosarea;
	memset(dosarea.area,0,sizeof(short)*400);
	dosarea.write_head=0;

	//array för lagring (uppdelat i rader) av grafisk representation av 7arf
	//short map[25];//max 5*5
	struct _7arf _7arf;
	memset(_7arf.map,0,sizeof(short)*25);

	printf("_7urouf\n\n");
	for(int i=0;i<28;i++)
	printf("[%d]%s (%d), ", i, _7urouf[i], _7bindelse[i]);

	printf("\n\nCharcodes och tecken\n-------------------\n\n");

	//symboler
	for(int i=0;i<sizeof(chc)/sizeof(int);i++){
		printf("[%d]%c, ", chc[i], chc[i]);
	}

	FILE *fp;

	//Filen som öppnas
	char filnamn[]="hadith1.txt";//unicode_test.txt

	char _7urouffil[]="7urouf.txt";

	int len=0, len2=0;

	char * buffer;//Med kopia av unicode textfil

	//alfabet lista
	char * alfa_list;

	//printf("Enter the name of file you wish to see\n");
	//gets(file_name);

	printf("\n%s\n", filnamn);

	fp = fopen(filnamn,"r"); // read mode

	if( fp == NULL )
		{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	//ftell

	fseek(fp, 0, SEEK_END);

	len = ftell(fp);
	fclose(fp);
	printf("Total size of %s = %d bytes\n", filnamn, len);

	//fread...

	fp = fopen(filnamn,"r"); // read mode

	if( fp == NULL )
		{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	printf("The contents of %s file (in digits) are :\n", filnamn);

	/* Read and display data */
	buffer = (char *) malloc(len+1);
	fread(buffer, len+1, 1, fp);
	printf("\nbuffer: %s\n\n", buffer);
	fclose(fp);

	//-slut fread

	//definiera koder för alla 7urouf

	fp = fopen(_7urouffil,"r"); // read mode

	if( fp == NULL )
		{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	len2 = ftell(fp);

	printf("Total size of %s = %d bytes\n", _7urouffil, len2);
	printf("The contents of %s file (in digits) are :\n", _7urouffil);

	/* Read and display data */
	alfa_list = (char *) malloc(len2+1);
	fread(alfa_list, len2+1, 1, fp);
	printf("\nalfa_list: %s\n\n", alfa_list);
	fclose(fp);

	printf("From file \'%s\':\n\n", filnamn);

	for(int i=0;i<len+1;i++){
		//printf("%d\n", buffer[i]);
	}

	short temp, arr_len, next;

	short prev_7arf=0; //ska lagra föregående tecken, för att kunna avgöra om det ska bindas

	char pos_val=0;

	char Row_Is_Finninshed=0;//När en rad har kapats i slutet, för flera hela ord, och raden skrivs ut, och anses avklarad (1/0)

	printf("\n%d\n", sizeof(_unicode_arabics)/sizeof(int));

	arr_len = sizeof(_unicode_arabics)/sizeof(short);

	for(int i=0;i<len+1;i+=2){
		temp=buffer[i];
		temp=(temp << 8)|buffer[i+1];

		next=buffer[i+2];
		next=(next << 8)|buffer[i+3];

		char is_start=0;

		char is_end_p=0;

		Row_Is_Finninshed=0;

		//printf("%d", temp); //unicode

		//jämför temp mot array _unicode_arabics samt _7urouf
		//Ska öppnas bara en gång
		for(int j=0;j<arr_len;j++){
			if(temp==_unicode_arabics[j]){

				//printf(" - %s - ", _7urouf[j]); //transkribt av 7arf
				if(IS_SPACE(temp)){
					//printf("[ ]");
				}

				set_7arf_Message(description, _7bindelse[j]);//hämta 7arfens bindelse(r)

				//printf(" - %s - ", description);

				//Kolla om 7arf står i initialläge
				if(IS_END_POINT(_7bindelse[index_Of_7arf(prev_7arf, arr_len, &_unicode_arabics[0])])){
					is_start=1;
					//printf(" s ");
					if(IS_SPACE(next)){
					is_end_p=1;
					}
				}
				else{
					is_start=0; //om is_start är 0 och andra är 1 ska det vara slutpos... och även om nästkommande 7rf är space

					if(IS_END_POINT(_7bindelse[index_Of_7arf(temp, arr_len, &_unicode_arabics[0])])){
						is_end_p=1;
						//printf("-x-");			
					}
					//nästa 7arf är space el. likn...
					else if(IS_SPACE(next)){
						is_end_p=1;
					}//todo kolla, behövs det kollas om nästa 7arf har högerbind
					else{
						is_end_p=0;
					}

				}
				//Kombinationer: 1 1 (ensam) 1 0 (initial) 0 1 (slut) 0 0 mellanform
				
				//printf(" - %d %d %d - ", (is_end_p), (is_start), ((is_end_p<<1)|is_start));


				if(((is_end_p<<1)|is_start)==0){
					pos_val=2;
				}
				else if(((is_end_p<<1)|is_start)==1){
					pos_val=1;
				}
				else if(((is_end_p<<1)|is_start)==2){
					pos_val=3;
				}
				else if(((is_end_p<<1)|is_start)==3){
					pos_val=0;
				}
				
				//printf(" - %d - ", pos_val);

				//'omskapa' en 7arf
	
				memset(_7arf.map,0,sizeof(short)*25);

				get_7arf_Symbols(temp,&_7arf,pos_val);

				_7arf.pos=pos_val;
				
				
				//_7arf_Info(_7arf);

				//skriva till dosarea... ett tecken
	
				short counter=0; short x, y =0;
	
				for(y=(short)_7arf.top_pos[pos_val];y<5;y++){
		
					for(x=(short)_7arf.widths[pos_val];x>0;x--){/*utgå fr högra sidan*/
			
						put_symbol_dosarea(x,y,&dosarea,_7arf.map[counter]);
						counter++;
					}
				}//tecken skrivs ut

				//stega fram write_head

				dosarea.write_head+=_7arf.widths[pos_val];

				//Om nära kanten
				if(dosarea.write_head > DOS_WIDTH - KALIMAH_SAGHIRAH){
					//if(IS_KOMMA(next)){printf("is_komma next");}
					if(IS_SPACE(temp) && (IS_KOMMA(next)==0) && (IS_SEMI(next)==0)){ //tecken på g är space och nästkommande ej är komma el likn.
					//ordet ska flyttas ned
						print_7urouf_row(&dosarea);
						memset(dosarea.area,0,sizeof(short)*400);
						dosarea.write_head=0;
						Row_Is_Finninshed=1;
						break;
					}
					if(Row_Is_Finninshed==1)break;
				}
				if(Row_Is_Finninshed==1)break;
			}
			if(Row_Is_Finninshed==1)break;
		}//Slut ett tecken gått med
		prev_7arf=temp;
				
		//Rad är fylld?	
		if(dosarea.write_head > DOS_WIDTH-_7ARF_MAX_WIDTH){//TODO: byt till next-7arf-width
			print_7urouf_row(&dosarea);
			memset(dosarea.area,0,sizeof(short)*400);
			dosarea.write_head=0;
		}

		//printf("\n");
	} // -slut genomgång av arabisk text fil

	//test genererar skärmbild

	//get_7uroufline_dos(buffer, &dosarea, 6);
	print_7urouf_row(&dosarea);

	//TODO: att kunna skriva ut flera tecken efter varran

	printf("\nTryck Enter foer att fortsätta\n");
	getchar();
	//putchar(chc[0]);

	printf("Test med alif:\n\n");

	if(get_7arf_Symbols(9990,&_7arf, 0)==1){
		_7arf_Info(_7arf);
	}

	paint_7_Row();
	struct _7arf _7arf2;
	//11014,11270,11526,11782

	//en loop för att rita alla 7ourouf
	for(int x=0;x<38;x++){
	memset(_7arf2.map,0,sizeof(short)*25);
	get_7arf_Symbols(_unicode_arabics[x],&_7arf2,2);
	//_7arf_Info(_7arf2);
	paint_7_test(&_7arf2, 2);
	}

	/**
	 // en förversion av loopaden innan
	memset(_7arf2.map,0,sizeof(short)*25);
	get_7arf_Symbols(10502,&_7arf2,3);//9990
	_7arf_Info(_7arf2);
	paint_7_test(&_7arf2, 3);
	**/


	/**printf("\nTryck Enter foer att avsluta\n");
	getchar();**/
	return 0;
}


//!main


//en återanvänd str och ett bit-kombi-värde
void set_7arf_Message(char *descr, short value)
{
	char has_Started=0;

	if(value&1){
		strcpy(descr, "högerbind");
		has_Started=1;
	}
	if(value&2){
		if(has_Started)strcat(descr, ", vänsterbind");
		else strcpy(descr, "vänsterbind");
		has_Started=1;
	}
	if(value&4){
		if(has_Started)strcat(descr, ", initial");
		else strcpy(descr, "initial");
		has_Started=1;
	}
	if(value&8){
		if(has_Started)strcat(descr, ", final");
		else strcpy(descr, "final");
		has_Started=1;
	}
	if(value&16){
		if(has_Started)strcat(descr, ", ensam");
		else strcpy(descr, "ensam");
		has_Started=1;
	}
	if(value&32){
		if(has_Started)strcat(descr, ", mitten.");
		else strcpy(descr, "mitten");
	}
    
}


/** Returnerar värdet på index där unicode återfinns i array

unicode - kod på tecken
arr_len - längd på array att söka i
_unicode_arabics - array med unicode koder

**/
int index_Of_7arf(short unicode, int arr_len, short * _unicode_arabics){

		for(int j=0;j<arr_len;j++){
			if(unicode==_unicode_arabics[j]){

			return j;
			}
		}
	return -1;
}

//generera ngn 7arf, return 1=ok, return -1=fel, arg: unicode: sifferkod, _7arf struct för 7arf, position i text : ensam, början, mitten, slutet
char get_7arf_Symbols(short unicode, struct _7arf *_7arf, char position){

	//struct _7arf alif;

	/**
9990,10246,10758,11014,11270,11526,11782,12038,12294,12550,12806,13062,13318,
	13574,13830,14086,14342,14598,14854,16646,16902,17158,17414,17670,17926,18182,18438,18950,8454,10502,
	8966,9478,8710,18694,3078,6918,8192
	**/


	//Gör position åtkomlig utanför denna funktion med variabeln POSITION

	if(unicode==9990){
		//alif
		_7arf->widths[0]=2;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;
		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;
		_7arf->bindings=49;

		if(position==0 || position==1){

		_7arf->map[0]=255;//symbolkod ¦
		_7arf->map[1]=186;
		_7arf->map[2]=255;
		_7arf->map[3]=186;
		_7arf->map[4]=255;
		_7arf->map[5]=186;

			POSITION=1;

		return 1;
		}
		else if(position==2 || position==3){

		_7arf->map[0]=255;//symbolkod ¦
		_7arf->map[1]=186;
		_7arf->map[2]=255;
		_7arf->map[3]=186;//symbolkod ¦
		_7arf->map[4]=255;
		_7arf->map[5]=200;

			POSITION=3;

		return 1;
		}
		else return -1;
	}
	else if(unicode==10246){//b

		_7arf->widths[0]=3;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=3;

		_7arf->bindings=63;

		switch(position){
			case 0://fr v
				_7arf->map[0]=200;
				_7arf->map[1]=205;
				_7arf->map[2]=188;
				_7arf->map[3]=255;
				_7arf->map[4]=223;
				POSITION=0;
				break;
			case 1:
				_7arf->map[0]=205;
				_7arf->map[1]=188;
				_7arf->map[2]=255;
				_7arf->map[3]=223;
				POSITION=1;
				break;
			case 2:
				_7arf->map[0]=205;
				_7arf->map[1]=202;
				_7arf->map[2]=255;
				_7arf->map[3]=223;
				POSITION=2;
				break;
			case 3:
				_7arf->map[0]=200;
				_7arf->map[1]=205;
				_7arf->map[2]=202;
				_7arf->map[3]=255;
				_7arf->map[4]=223;
				POSITION=3;
				break;

			default:
				break;
		}

		return 1;
	}

	else if(unicode==10758){//t

		_7arf->widths[0]=3;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=250;
				_7arf->map[1]=250;
				_7arf->map[2]=255;
				_7arf->map[3]=200;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=221;
				_7arf->map[2]=205;
				_7arf->map[3]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=221;
				_7arf->map[2]=205;
				_7arf->map[3]=202;
				break;
			case 3:
				_7arf->map[0]=250;
				_7arf->map[1]=250;
				_7arf->map[2]=255;
				_7arf->map[3]=200;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	else if(unicode==11014){//th

		_7arf->widths[0]=3;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=193;
				_7arf->map[2]=255;
				_7arf->map[3]=200;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 1:
				_7arf->map[0]=193;
				_7arf->map[1]=188;
				break;
			case 2:
				_7arf->map[0]=193;
				_7arf->map[1]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=193;
				_7arf->map[2]=255;
				_7arf->map[3]=200;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	else if(unicode==11270){	//11270 djim

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=200;
				_7arf->map[7]=223;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=255;
				_7arf->map[7]=223;
				_7arf->map[8]=255;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=255;
				_7arf->map[7]=223;
				_7arf->map[8]=255;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=200;
				_7arf->map[7]=223;
				_7arf->map[8]=188;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	else if(unicode==11526){	//11270 7aa2

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;

				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	else if(unicode==11782){	//11782 cha

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;
				_7arf->map[6]=201;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;
				_7arf->map[6]=205;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;
				_7arf->map[6]=205;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;
				_7arf->map[6]=201;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	//12038
	else if(unicode==12038){	//däl

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=186;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=186;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=186;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=186;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	else if(unicode==12294){	//dhäl

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			default:
				;
				break;
		}

		return 1;
	}
	//ra 12550
	else if(unicode==12550){

		_7arf->widths[0]=2;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=3;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=255;
				_7arf->map[3]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=255;
				_7arf->map[3]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=204;
				_7arf->map[2]=255;
				_7arf->map[3]=188;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=204;
				_7arf->map[2]=255;
				_7arf->map[3]=188;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	//12806 z
	else if(unicode==12806){

		_7arf->widths[0]=1;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=220;
				_7arf->map[1]=186;
				_7arf->map[2]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=186;
				_7arf->map[4]=255;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=204;
				_7arf->map[4]=255;
				_7arf->map[5]=188;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=204;
				_7arf->map[4]=255;
				_7arf->map[5]=188;
				break;
			default:
				;
				break;
		}

		return 1;
	}
	//13062 s
	else if(unicode==13062){

		_7arf->widths[0]=5;
		_7arf->widths[1]=4;
		_7arf->widths[2]=3;
		_7arf->widths[3]=5;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=3;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=204;
				_7arf->map[3]=202;
				_7arf->map[4]=188;
				_7arf->map[5]=200;
				_7arf->map[6]=205;
				_7arf->map[7]=188;
				_7arf->map[8]=255;
				_7arf->map[9]=255;
				break;
			case 1:
				_7arf->map[0]=205;
				_7arf->map[1]=202;
				_7arf->map[2]=202;
				_7arf->map[3]=188;
				break;
			case 2:
				_7arf->map[0]=202;
				_7arf->map[1]=202;
				_7arf->map[2]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=204;
				_7arf->map[3]=202;
				_7arf->map[4]=202;
				_7arf->map[5]=200;
				_7arf->map[6]=205;
				_7arf->map[7]=188;
				_7arf->map[8]=255;
				_7arf->map[9]=255;
				break;
			default:
				;
				break;
		}

		return 1;
	}
	//13318 sh...

	else if(unicode==13318){

		_7arf->widths[0]=5;
		_7arf->widths[1]=4;
		_7arf->widths[2]=3;
		_7arf->widths[3]=5;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=193;
				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=204;
				_7arf->map[8]=202;
				_7arf->map[9]=188;
				_7arf->map[10]=200;
				_7arf->map[11]=205;
				_7arf->map[12]=188;
				_7arf->map[13]=255;
				_7arf->map[14]=255;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=193;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				_7arf->map[6]=202;
				_7arf->map[7]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=193;
				_7arf->map[2]=255;
				_7arf->map[3]=202;
				_7arf->map[4]=202;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=193;
				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=204;
				_7arf->map[8]=202;
				_7arf->map[9]=202;
				_7arf->map[10]=200;
				_7arf->map[11]=205;
				_7arf->map[12]=188;
				_7arf->map[13]=255;
				_7arf->map[14]=255;
				break;
			default:
				;
				break;
		}

		return 1;
	}
	//13574 saad

	else if(unicode==13574){

		_7arf->widths[0]=5;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=5;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=205;
				_7arf->map[4]=187;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=206;
				_7arf->map[8]=205;
				_7arf->map[9]=188;
				_7arf->map[10]=200;
				_7arf->map[11]=205;
				_7arf->map[12]=188;
				_7arf->map[13]=255;
				_7arf->map[14]=255;
				break;
			case 1:
				_7arf->map[0]=201;
				_7arf->map[1]=205;
				_7arf->map[2]=187;
				_7arf->map[3]=206;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=201;
				_7arf->map[1]=205;
				_7arf->map[2]=187;
				_7arf->map[3]=206;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=205;
				_7arf->map[4]=187;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=206;
				_7arf->map[8]=205;
				_7arf->map[9]=202;
				_7arf->map[10]=200;
				_7arf->map[11]=205;
				_7arf->map[12]=188;
				_7arf->map[13]=255;
				_7arf->map[14]=255;
				break;
			default:
				;
				break;
		}

		return 1;
	}


//daad
	else if(unicode==13830){

		_7arf->widths[0]=5;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=5;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=220;
				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=201;
				_7arf->map[8]=205;
				_7arf->map[9]=187;
				_7arf->map[10]=255;
				_7arf->map[11]=255;
				_7arf->map[12]=206;
				_7arf->map[13]=205;
				_7arf->map[14]=188;
				_7arf->map[15]=200;
				_7arf->map[16]=205;
				_7arf->map[17]=188;
				_7arf->map[18]=255;
				_7arf->map[19]=255;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=187;
				_7arf->map[6]=206;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=187;
				_7arf->map[6]=206;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=220;
				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=201;
				_7arf->map[8]=205;
				_7arf->map[9]=187;
				_7arf->map[10]=255;
				_7arf->map[11]=255;
				_7arf->map[12]=206;
				_7arf->map[13]=205;
				_7arf->map[14]=202;
				_7arf->map[15]=200;
				_7arf->map[16]=205;
				_7arf->map[17]=188;
				_7arf->map[18]=255;
				_7arf->map[19]=255;
				break;
			default:
				;
				break;
		}

		return 1;
	}

	//14086 "6" ta
	else if(unicode==14086){

		_7arf->widths[0]=3;
		_7arf->widths[1]=4;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=186;
				_7arf->map[1]=255;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=187;

				_7arf->map[6]=204;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=255;
				_7arf->map[3]=255;

				_7arf->map[4]=255;
				_7arf->map[5]=204;
				_7arf->map[6]=205;
				_7arf->map[7]=187;

				_7arf->map[8]=205;
				_7arf->map[9]=206;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 2:
				_7arf->map[0]=186;
				_7arf->map[1]=255;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=187;

				_7arf->map[6]=206;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=186;
				_7arf->map[1]=255;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=187;

				_7arf->map[6]=204;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//14342 zha
	else if(unicode==14342){

		_7arf->widths[0]=3;
		_7arf->widths[1]=4;
		_7arf->widths[2]=4;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=220;
				_7arf->map[3]=255;

				_7arf->map[4]=255;
				_7arf->map[5]=204;
				_7arf->map[6]=205;
				_7arf->map[7]=187;

				_7arf->map[8]=205;
				_7arf->map[9]=204;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=186;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=187;

				_7arf->map[6]=206;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=220;
				_7arf->map[3]=255;

				_7arf->map[4]=255;
				_7arf->map[5]=204;
				_7arf->map[6]=205;
				_7arf->map[7]=187;

				_7arf->map[8]=205;
				_7arf->map[9]=206;
				_7arf->map[10]=205;
				_7arf->map[11]=202;
				break;
			case 3:
				_7arf->map[0]=186;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=187;

				_7arf->map[6]=204;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//14598 3jn
	else if(unicode==14598){

		_7arf->widths[0]=3;
		_7arf->widths[1]=4;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=201;
				_7arf->map[1]=205;
				_7arf->map[2]=255;

				_7arf->map[3]=204;
				_7arf->map[4]=205;
				_7arf->map[5]=255;

				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=205;
				_7arf->map[3]=255;

				_7arf->map[4]=205;
				_7arf->map[5]=206;
				_7arf->map[6]=205;
				_7arf->map[7]=255;

				_7arf->map[8]=255;
				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=205;
				_7arf->map[2]=203;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=498;
				_7arf->map[2]=255;

				_7arf->map[3]=201;
				_7arf->map[4]=202;
				_7arf->map[5]=205;

				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//14854 gh
	else if(unicode==14854){

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=255;

				_7arf->map[6]=204;
				_7arf->map[7]=205;
				_7arf->map[8]=255;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=201;
				_7arf->map[4]=205;
				_7arf->map[5]=255;

				_7arf->map[6]=205;
				_7arf->map[7]=205;
				_7arf->map[8]=255;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;
				_7arf->map[4]=205;
				_7arf->map[5]=203;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=255;
				_7arf->map[4]=498;
				_7arf->map[5]=255;

				_7arf->map[6]=201;
				_7arf->map[7]=202;
				_7arf->map[8]=205;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//16646 f
	else if(unicode==16646){

		_7arf->widths[0]=4;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=4;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;

				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=201;
				_7arf->map[7]=187;

				_7arf->map[8]=200;
				_7arf->map[9]=205;
				_7arf->map[10]=202;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=205;
				_7arf->map[7]=202;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=205;
				_7arf->map[7]=202;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=220;
				_7arf->map[3]=255;

				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=201;
				_7arf->map[7]=187;

				_7arf->map[8]=200;
				_7arf->map[9]=205;
				_7arf->map[10]=202;
				_7arf->map[11]=202;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//q
	else if(unicode==16902){

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=46;
				_7arf->map[2]=46;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=255;
				_7arf->map[7]=200;
				_7arf->map[8]=185;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=46;
				_7arf->map[2]=46;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=205;
				_7arf->map[7]=202;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=46;
				_7arf->map[2]=46;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=205;
				_7arf->map[7]=202;
				_7arf->map[8]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=46;
				_7arf->map[2]=46;

				_7arf->map[3]=255;
				_7arf->map[4]=201;
				_7arf->map[5]=187;

				_7arf->map[6]=255;
				_7arf->map[7]=200;
				_7arf->map[8]=206;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//17158 K
	else if(unicode==17158){

		_7arf->widths[0]=4;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=4;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=201;

				_7arf->map[4]=255;
				_7arf->map[5]=115;//s
				_7arf->map[6]=255;
				_7arf->map[7]=186;

				_7arf->map[8]=200;
				_7arf->map[9]=205;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			case 1:
				_7arf->map[0]=201;
				_7arf->map[1]=205;
				_7arf->map[2]=205;

				_7arf->map[3]=200;
				_7arf->map[4]=187;
				_7arf->map[5]=255;

				_7arf->map[6]=205;
				_7arf->map[7]=188;
				_7arf->map[8]=255;
				break;
			case 2:
				_7arf->map[0]=201;
				_7arf->map[1]=205;
				_7arf->map[2]=205;

				_7arf->map[3]=200;
				_7arf->map[4]=187;
				_7arf->map[5]=255;

				_7arf->map[6]=205;
				_7arf->map[7]=202;
				_7arf->map[8]=205;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=201;

				_7arf->map[4]=255;
				_7arf->map[5]=115;//s
				_7arf->map[6]=255;
				_7arf->map[7]=186;

				_7arf->map[8]=200;
				_7arf->map[9]=205;
				_7arf->map[10]=205;
				_7arf->map[11]=202;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//17414 L
	else if(unicode==17414){

		_7arf->widths[0]=4;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=255;
				_7arf->map[3]=201;

				_7arf->map[4]=255;
				_7arf->map[5]=255;
				_7arf->map[6]=255;
				_7arf->map[7]=186;

				_7arf->map[8]=255;
				_7arf->map[9]=255;
				_7arf->map[10]=255;
				_7arf->map[11]=186;

				_7arf->map[12]=200;
				_7arf->map[13]=205;
				_7arf->map[14]=205;
				_7arf->map[15]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=255;
				_7arf->map[3]=186;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=186;
				_7arf->map[2]=255;
				_7arf->map[3]=186;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=186;

				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;

				_7arf->map[6]=255;
				_7arf->map[7]=255;
				_7arf->map[8]=204;

				_7arf->map[9]=200;
				_7arf->map[10]=205;
				_7arf->map[11]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//17670 M
	else if(unicode==17670){

		_7arf->widths[0]=2;
		_7arf->widths[1]=3;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=_7arf->top_pos[1]=_7arf->top_pos[2]=_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=204;
				_7arf->map[3]=188;
				_7arf->map[4]=186;
				_7arf->map[5]=255;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=202;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=202;
				_7arf->map[3]=202;
				break;
			case 3:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=204;
				_7arf->map[3]=202;
				_7arf->map[4]=186;
				_7arf->map[5]=255;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//17926 N
	else if(unicode==17926){

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=2;
		_7arf->top_pos[2]=2;
		_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=205;
				_7arf->map[4]=205;
				_7arf->map[5]=202;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=220;
				_7arf->map[2]=255;
				_7arf->map[3]=255;
				_7arf->map[4]=255;
				_7arf->map[5]=202;
				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//18182 H
	else if(unicode==18182){

		_7arf->widths[0]=2;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=2;
		_7arf->top_pos[2]=2;
		_7arf->top_pos[3]=1;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=200;
				_7arf->map[3]=188;
				break;
			case 1:
				_7arf->map[0]=201;
				_7arf->map[1]=203;
				_7arf->map[2]=187;
				_7arf->map[3]=202;
				_7arf->map[4]=202;
				_7arf->map[5]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=205;
				_7arf->map[4]=206;
				_7arf->map[5]=206;
				_7arf->map[6]=255;
				_7arf->map[7]=200;
				_7arf->map[8]=188;
				break;
			case 3:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=200;
				_7arf->map[3]=185;
				_7arf->map[4]=255;
				_7arf->map[5]=200;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//18438 Waw
	else if(unicode==18438){

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=2;
		_7arf->top_pos[2]=2;
		_7arf->top_pos[3]=2;

		_7arf->bindings=57;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=255;
				_7arf->map[4]=200;
				_7arf->map[5]=185;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=255;
				_7arf->map[4]=200;
				_7arf->map[5]=185;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 2:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=255;
				_7arf->map[4]=200;
				_7arf->map[5]=206;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=187;
				_7arf->map[3]=255;
				_7arf->map[4]=200;
				_7arf->map[5]=206;
				_7arf->map[6]=255;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//18950 ya
	else if(unicode==18950){

		_7arf->widths[0]=4;
		_7arf->widths[1]=2;
		_7arf->widths[2]=1;
		_7arf->widths[3]=4;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=3;
		_7arf->top_pos[2]=3;
		_7arf->top_pos[3]=2;

		_7arf->bindings=63;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=205;
				_7arf->map[4]=200;
				_7arf->map[5]=205;
				_7arf->map[6]=188;
				_7arf->map[7]=255;
				_7arf->map[8]=255;
				_7arf->map[9]=46;
				_7arf->map[10]=46;
				_7arf->map[11]=255;
				break;
			case 1:
				_7arf->map[0]=205;
				_7arf->map[1]=188;
				_7arf->map[2]=221;
				_7arf->map[3]=255;
				break;
			case 2:
				_7arf->map[0]=202;
				_7arf->map[1]=221;
				break;
			case 3:
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=187;
				_7arf->map[4]=200;
				_7arf->map[5]=205;
				_7arf->map[6]=188;
				_7arf->map[7]=200;
				_7arf->map[8]=255;
				_7arf->map[9]=46;
				_7arf->map[10]=46;
				_7arf->map[11]=255;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//8454 hamzah
	else if(unicode==8454){

		_7arf->widths[0]=2;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=2;
		_7arf->top_pos[2]=2;
		_7arf->top_pos[3]=2;

		_7arf->bindings=48;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=204;
				_7arf->map[3]=187;
				break;
			case 1:

				break;
			case 2:

				break;
			case 3:
				_7arf->map[0]=201;
				_7arf->map[1]=187;
				_7arf->map[2]=204;
				_7arf->map[3]=187;
				break;
			default:
				;
				break;
		}
		return 1;
	}

	//10502 tä marbota

	else if(unicode==10502){

		_7arf->widths[0]=2;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=1;
		_7arf->top_pos[1]=1;
		_7arf->top_pos[2]=1;
		_7arf->top_pos[3]=0;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=221;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=187;
				_7arf->map[4]=200;
				_7arf->map[5]=188;
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				_7arf->map[0]=221;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=187;
				_7arf->map[4]=200;
				_7arf->map[5]=185;
				_7arf->map[6]=255;
				_7arf->map[7]=200;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//hamza med alif 8966
	else if(unicode==8966){

		_7arf->widths[0]=1;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=1;

		_7arf->top_pos[0]=1;
		_7arf->top_pos[1]=1;
		_7arf->top_pos[2]=1;
		_7arf->top_pos[3]=1;

		_7arf->bindings=49;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=245;
				_7arf->map[1]=186;
				_7arf->map[2]=186;
				break;
			case 1:
				_7arf->map[0]=245;
				_7arf->map[1]=186;
				_7arf->map[2]=186;
				break;
			case 2:
				_7arf->map[0]=245;//?
				_7arf->map[1]=186;
				_7arf->map[2]=200;
				break;
			case 3:
				_7arf->map[0]=245;
				_7arf->map[1]=186;
				_7arf->map[2]=200;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//9478 alif-hamza-i

	else if(unicode==9478){

		_7arf->widths[0]=1;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=1;

		_7arf->top_pos[0]=1;
		_7arf->top_pos[1]=1;
		_7arf->top_pos[2]=1;
		_7arf->top_pos[3]=1;

		_7arf->bindings=49;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=186;
				_7arf->map[1]=186;
				_7arf->map[2]=245;
				break;
			case 1:
				_7arf->map[0]=186;
				_7arf->map[1]=186;
				_7arf->map[2]=245;
				break;
			case 2:
				break;
			case 3://?
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//8710 alif meddah
	else if(unicode==8710){

		_7arf->widths[0]=2;
		_7arf->widths[1]=2;
		_7arf->widths[2]=2;
		_7arf->widths[3]=2;

		_7arf->top_pos[0]=1;
		_7arf->top_pos[1]=1;
		_7arf->top_pos[2]=1;
		_7arf->top_pos[3]=1;

		_7arf->bindings=49;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=126;
				_7arf->map[2]=255;
				_7arf->map[3]=186;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				break;
			case 1:
				_7arf->map[0]=255;
				_7arf->map[1]=126;
				_7arf->map[2]=255;
				_7arf->map[3]=186;
				_7arf->map[4]=255;
				_7arf->map[5]=186;
				break;
			case 2:
				break;
			case 3://?
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//18694 alif maqsoura 1, 8, 16
	else if(unicode==18694){

		_7arf->widths[0]=3;
		_7arf->widths[1]=0;
		_7arf->widths[2]=0;
		_7arf->widths[3]=4;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=1;
		_7arf->top_pos[2]=1;
		_7arf->top_pos[3]=3;

		_7arf->bindings=25;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				_7arf->map[1]=201;
				_7arf->map[2]=205;
				_7arf->map[3]=255;
				_7arf->map[4]=200;
				_7arf->map[5]=187;
				_7arf->map[6]=200;
				_7arf->map[7]=205;
				_7arf->map[8]=188;
				break;
			case 1:
				break;
			case 2:
				break;
			case 3://?
				_7arf->map[0]=255;
				_7arf->map[1]=255;
				_7arf->map[2]=201;
				_7arf->map[3]=205;
				_7arf->map[4]=200;
				_7arf->map[5]=205;
				_7arf->map[6]=188;
				_7arf->map[7]=255;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//arab-komma
	//3078
	else if(unicode==3078){

		_7arf->widths[0]=1;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=1;

		_7arf->top_pos[0]=3;
		_7arf->top_pos[1]=3;
		_7arf->top_pos[2]=3;
		_7arf->top_pos[3]=3;

		_7arf->bindings=0;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=192;
				break;
			case 1:
				_7arf->map[0]=192;
				break;
			case 2:
				_7arf->map[0]=192;
				break;
			case 3://?
				_7arf->map[0]=192;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//,6918
	//arabi semikolon
	else if(unicode==6918){

		_7arf->widths[0]=1;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=1;

		_7arf->top_pos[0]=2;
		_7arf->top_pos[1]=2;
		_7arf->top_pos[2]=2;
		_7arf->top_pos[3]=2;

		_7arf->bindings=0;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=46;
				_7arf->map[1]=192;
				break;
			case 1:
				_7arf->map[0]=46;
				_7arf->map[1]=192;
				break;
			case 2:
				_7arf->map[0]=46;
				_7arf->map[1]=192;
				break;
			case 3://?
				_7arf->map[0]=46;
				_7arf->map[1]=192;
				break;
			default:
				;
				break;
		}
		return 1;
	}


	//punkt
	else if(unicode==11776){

		_7arf->widths[0]=1;
		_7arf->widths[1]=1;
		_7arf->widths[2]=1;
		_7arf->widths[3]=1;

		_7arf->top_pos[0]=3;
		_7arf->top_pos[1]=3;
		_7arf->top_pos[2]=3;
		_7arf->top_pos[3]=3;

		_7arf->bindings=0;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=220;
				break;
			case 1:
				_7arf->map[0]=220;
				break;
			case 2:
				_7arf->map[0]=220;
				break;
			case 3://?
				_7arf->map[0]=220;
				break;
			default:
				;
				break;
		}
		return 1;
	}
	//8192 space
	else if(unicode==8192){

		_7arf->widths[0]=3;
		_7arf->widths[1]=3;
		_7arf->widths[2]=3;
		_7arf->widths[3]=3;

		_7arf->top_pos[0]=3;
		_7arf->top_pos[1]=3;
		_7arf->top_pos[2]=3;
		_7arf->top_pos[3]=3;

		_7arf->bindings=0;

		POSITION=SET_POSITION(position);

		switch(position){
			case 0:
				_7arf->map[0]=255;
				break;
			case 1:
				_7arf->map[0]=255;
				break;
			case 2:
				_7arf->map[0]=255;
				break;
			case 3://?
				_7arf->map[0]=255;
				break;
			default:
				;
				break;
		}
		return 1;
	}


	else return -1;
}

//struct for 7arf och siffra för läget; 0 ensam, 1 init, 2 mitten, 3 avslutande
void paint_7arf(struct _7arf _7arf, char position){
	//short temp[5];

	printf("paint_7arf\n----------\n\n");
	//row by row
	for(int j=_7arf.top_pos[position];j<5;j++){
		for(int k=0;_7arf.widths[position];k++){
			//temp[k]=_7arf.map[k];
			printf("%c",_7arf.map[k]);
		}

		//draw line, at row j
		//printf("%s",temp);
		printf("\n");//todo gå till rätt x-pos / gör hela skärmbredden klar i en rad
	}
}

void _7arf_Info(struct _7arf _7arf){

	printf("_7arf_Info\n----------\n\n");
	printf("bindings: %d\n", _7arf.bindings);
	printf("width 1: %d, ", _7arf.widths[0]);
	printf("2: %d, ", _7arf.widths[1]);
	printf("3: %d, ", _7arf.widths[2]);
	printf("4: %d\n", _7arf.widths[3]);
	printf("top_pos 0: %d\n", _7arf.top_pos[0]);//var översta delen är; 0 top (tex ta marb.), 1 (tex alif), 2, 3 (baslinje), 4 under bas

	printf("top_pos 1: %d\n", _7arf.top_pos[1]);
	printf("top_pos 2: %d\n", _7arf.top_pos[2]);
	printf("top_pos 3: %d\n", _7arf.top_pos[3]);

	for(int i=0; i<25; i++)
		printf("[%d] %d,  ", i, _7arf.map[i]);//max 5*5 från höger

}

//En rad 7urouf, 80x5 unicode-tecken
void paint_7_Row(short *unicode_arabics){
	//en ruta för doskärm 5 tecken hög, 80 bred
	short test[400]={
	201,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,187,
	442,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,442,
	442,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,442,
	442,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,442,	
	200,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
	205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,188
	};

	printf("\n\npaint_7_test\n\n");
	for(int i=0;i<400;i++) printf("%c", test[i]);

	printf("\n\nslut test ---\n\n");
	//return 0;
}

void paint_7_test(struct _7arf * _7arf, char pos){

	//är den tom?
	int zcount=0;
	for(int x=0;x<25;x++){
		if(_7arf->map[x]==0)
			zcount++;

	}
	if(zcount==25)
		return;//Om hela teckenytan är nollställd/noll avbryts funktion

	int map_pos=0;//position i map, som är en dimensions array
	printf("\npaint_7_test\n************\ntop pos: %d, width: %d\n", _7arf->top_pos[pos], _7arf->widths[pos]);

	for(int i=(int)_7arf->top_pos[pos];i<5;i++){
		for(int j=0;j<_7arf->widths[pos];j++){
			printf("%c", _7arf->map[map_pos]);
			map_pos++;
		}
		//baslinje---
		if(i==3) printf(" ---");
		printf("\n");
	}
}

//få en minnesyta på 80x5 tecken, fylld med symboler för 7urouf

//buffer - data fr textfil (unicode)

//dosarea - tecken på 80x5

//startpos - tecken i ursprungsfilen

//return slutpos (efter en rad) i fil som lästs

//TODO:fattas info om 7arfs position (0-3)

int get_7uroufline_dos(char * buffer, struct _7arf_dos_row * dosarea, short startpos){

	struct _7arf test7;
	int counter=0;
	short read_from=startpos;
	short _7arfkod=0;
	short next=0;

	printf("\nget_7uroufline_dos\nsizeof buffer = %d\n", sizeof(&buffer));

	do{
		//printf("\n%d\n", (buffer[read_from]<<8)|buffer[read_from+1]);
		_7arfkod = ((buffer[read_from]<<8)|buffer[read_from+1]);

		next=buffer[read_from+2];
		next=(next << 8)|buffer[read_from+3];

		memset(test7.map,0,sizeof(short)*25);
		//Todo: måste ha rätt position
	
		get_7arf_Symbols(_7arfkod,&test7,2);

		read_from+=2;
		counter+=1;
	}while(dosarea->write_head<34); //bredd per rad...

	//printf("\ndosarea->write_head:%d\n", dosarea->write_head);

	return 0;
}

//sätta färg på minne (koordinater utgår från top höger)
//Används
int put_symbol_dosarea(short x, short y, struct _7arf_dos_row * dosarea, short value){
	
	//printf(" - psd - %d %d %d (write_head: %d) - ", x, y, value, dosarea->write_head);
	//ställa in x-pos efter write_head

	x=80-x;

	if(dosarea->write_head>0){
		x-=dosarea->write_head;
	}

	dosarea->area[y*80+x]=value;
	return 0;
}


int print_7urouf_row(struct _7arf_dos_row * dosarea){
	//printf("\nprint_7urouf_row... write_head=%d\n\n", dosarea->write_head);
	for(int i=0;i<400;i++) printf("%c", dosarea->area[i]);
	//printf("\n--------p7rd write_head=%d\n\n", dosarea->write_head);
	return 1;
}
