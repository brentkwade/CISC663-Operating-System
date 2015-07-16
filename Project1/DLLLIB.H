// Always follow naming convention for writing files: function_datastruct_type
#ifndef doublell_h
#define doublell_h

//program starts here
#include <stdlib.h>
#include<stdio.h>

struct node
{
	char* artist;
	char* album;
	char* song;
	char* date;
	int runtime;//in seconds
	struct node* next;
	struct node* prev;
};
struct node* head;//pointer to head - defined as global variable
char artist1[256];
char album1[256];
char title1[256];
char date1[30];
int runtime1;

struct node* getNewNode(char artist2[], char album2[], char song2[], char date2[], int runtime2)
{
	//int size=sizeof(artist2);
	//method to create a new node pointing empty both sides
	struct node* newNode = malloc(sizeof(newNode));//malloc returns address of the memory allocated
	//printf("Size of artist: %d\n",size);

	newNode->album=malloc(sizeof(album2));
	strcpy(newNode->album,album2);

	newNode->artist = malloc(sizeof(artist2));
	strcpy(newNode->artist,artist2);

	newNode->song=malloc(sizeof(song2));
	strcpy(newNode->song,song2);

	newNode->date=malloc(sizeof(date2));
	strcpy(newNode->date,date2);

	newNode->runtime=runtime2;
	//newNode->runtime=malloc(sizeof(rt2));
	//strcpy(newNode->runtime,rt2);

	//printf("1. After Assigining for Node Artist: %s",newNode->artist);
	newNode->next=NULL;
	newNode->prev=NULL;
	//printf("2. After Assigining for Node Artist: %s",newNode->artist);
	return newNode;//returning pointer to newly created node
}

void InsertAtHead(char artist2[], char album2[], char song2[], char date2[], int runtime2)
{
	struct node* newNode = getNewNode(artist2, album2, song2, date2, runtime2);//this will create new node pointing to 0 both sides
	//case if list is empty
	//printf("\nAt Node Insertion: %s",newNode->album);
	if(head	== NULL)
	{
		head=newNode;
		return;
	}
	head->prev=newNode;
	newNode->next=head;
	head=newNode;
}

void Delete(char arti[])
{
	struct node* temp = head;//(n-1)th node
	struct node* temp1 = temp->next;//(n)th node
	struct node* temp2 = temp1->next;//(n+1)th node
	int i,n=0;
	int cmpres, len;
	len=strlen(arti);
	//printf("length of the name to delete from the list: %d\n",len);
	//printf("Input string is: %s",arti);
	//printf("newNode->artist: %s",temp->artist);
	//traversing to find out if sting is found
	//using strncmp(str1,str2,compare_limit) function to compare two strings.

	while(temp != NULL)
	{
		cmpres=strncmp(temp->artist,arti,len);
		if(cmpres==0)
		{
			if(n==0)
			{
				head=temp->next;
				free(temp);
				return;
			}//if match is 1st element of list
			if(n==1)
			{
				temp1=head->next;
				temp2=temp1->next;
				head->next=temp2;
				temp2->prev=head;
				return;
			}
			else
			{
				temp=temp->prev;
				temp1=temp->next;//n
				temp2=temp1->next;//n+1
				temp->next=temp2;
				temp2->prev=temp;
				temp=temp->next;
			}
		}
		temp=temp->next;
		n++;
	}
}

void print()
{
	char *c;
	int i,len;
	struct node* temp = head;

	while(temp != NULL)
	{
		//Artist printing
		c=malloc(sizeof(temp->artist));
		c=temp->artist;
		len=strlen(temp->artist);
		printf("|ARTIST: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//Album printing
		c=malloc(sizeof(temp->album));
		c=temp->album;
		len=strlen(temp->album);
		printf("|ALBUM: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//song printing
		c=malloc(sizeof(temp->song));
		c=temp->song;
		len=strlen(temp->song);
		printf("|SONG: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//date printing
		c=malloc(sizeof(temp->date));
		c=temp->date;
		len=strlen(temp->date);
		printf("|DATE: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//runtime printing
		printf("|RUNTIME: %d\n",temp->runtime);
		printf("\n============\n");
		temp=temp->next;
	}
	printf("\n----------------------------------------------");
}

void reversePrint()
{
	struct node* temp=head;
	char *c;
	int i,len;
	if(temp==NULL)
		return;

	while(temp -> next != NULL)
	{
		temp=temp->next;
	}
	printf("\nReverse Print\n\n");
	while(temp != NULL)
	{
		//Artist printing
		c=malloc(sizeof(temp->artist));
		c=temp->artist;
		len=strlen(temp->artist);
		printf("|ARTIST: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//Album printing
		c=malloc(sizeof(temp->album));
		c=temp->album;
		len=strlen(temp->album);
		printf("|ALBUM: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//song printing
		c=malloc(sizeof(temp->song));
		c=temp->song;
		len=strlen(temp->song);
		printf("|SONG: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//date printing
		c=malloc(sizeof(temp->date));
		c=temp->date;
		len=strlen(temp->date);
		printf("|DATE: ");
		for(i=0;i<len;i++)
			printf("%c",c[i]);
		//runtime printing
		printf("|RUNTIME: %d",temp->runtime);
		printf("\n\n============");
		temp=temp->prev;
	}
}

void getDataFromUser()
{
	char tmp[256];
	printf("PLEASE ENTER DATA FOR THE FOLLOWING FIELDS\n");
	//fgets(tmp,256,stdin);
	//printf("At user input: %s",tmp);

	//printf("%s",artist1);
	printf("ALBUM: ");
	//fgets(album1,256,stdin);
	scanf("%s",album1);
	//printf("%s",album1);
	printf("ARTIST: ");
	//fgets(artist1,256,stdin);
	scanf("%s",artist1);
	printf("TITLE: ");
	//fgets(title1,256,stdin);
	scanf("%s",title1);
	printf("DATE: ");
	//fgets(date1,256,stdin);
	scanf("%s",date1);
	printf("RUNTIME(in seconds): ");
	scanf("%d",&runtime1);
	//fgets(runtime1,10,stdin);
	InsertAtHead(artist1,album1,title1,date1,runtime1);
}

void menuPrint()
{
	int choice,num,pos;
	int deci,rectrack;
	char arti[256];
	printf("FOLLOWING OPERATIONS CAN BE PERFORMED ON THIS LIST: \n");
	printf("1. Insert Data\n2. Delete Data\n3. Print Data\n4. Reverse Print Data\n");
	printf("PLEASE ENTER YOUR CHOICE: ");
	scanf("%d",&choice);
	printf("\n---------------------------------------------\n");
	switch(choice)
	{
		case 1:
			getDataFromUser();
			printf("\nNew List is: \n");
			print();
			printf("\n----------------------------------------------\n");
			printf("\n1. Operate Again\n2. Exit\nPlease enter your choice: ");scanf("%d",&deci);
			printf("\n----------------------------------------------\n");
			if(deci==1)
			{
				rectrack++;
				menuPrint();
			}
			else
			{
				exit(0);
			}
		case 2:
			//printf("\nCurrent List is: \n");print();

			printf("\nEnter The Name of Artist to Delete: ");
			scanf("%s",arti);
			Delete(arti);
			printf("\nNew List is		: ");print();
			printf("\n---------------------------------------------\n");
			printf("\n1. Operate Again\n2. Exit\nPlease Enter your choice: ");scanf("%d",&deci);
			if(deci==1)
			{
				menuPrint();
			}
			else exit(0);
		case 3:
			printf("\nList is: \n");
			print();
			printf("\n1. Operate Again\n2. Exit\n Please Enter your choice: ");scanf("%d",&deci);
			printf("\n---------------------------------------------\n");
			if(deci==1)
			{
				menuPrint();
			}
			else exit(0);

		case 4:
			printf("\nList is is: \n");
			reversePrint();
			printf("\n1. Operate Again\n2. Exit\nPlease enter your choice: ");scanf("%d",&deci);
			printf("\n---------------------------------------------\n");
			if(deci==1)
			{
				menuPrint();
			}
			else exit(0);
	}

}

#endif