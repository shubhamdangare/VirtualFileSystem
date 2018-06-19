#define _CRT_SECURE_NO_NOWARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define CURRENT 1
#define END 2
#define START 0

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

typedef struct superblock{

        int TotalInodes;
        int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;   

typedef struct inode{

        char FileName[50];
        char *Buffer;
        int InodeNumber;
        int FileSize;
        int FileType;
        int FileActualSize;
        int LinkCount;
        int ReferenceCout;
        int permission;

        struct inode *next;
}INODE,*PINODE,**PPINODE;


typedef struct filetable{

    int readoffset;
    int writeoffset;
    int cout;
    int mode;
    PINODE  ptrinode;
} FILETABLE,*PFILETABLE;    


typedef struct ufdt{
        PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;


// Input	:	char *name	: It accepts name of file from user
// Output	:	It returns file descriptor of file which is of type intger
// Description	:	It is used to get file descriptor from the file name

int GetFDFromName(char * name){

    int i=0;
    while(i<50){
        if(UFDTArr[i].ptrfiletable!=NULL){
            if(_stricmp(UFDTArr[i].ptrfiletable->ptrinode->FileName,name)==0)
                break;
        }
        i++;
    }
    if(i==50)
        {
            return -1;
        }
    else{
            return i;
    }
}

// Input	:	char *name	: It accepts name of file from user
// Output	:	It returns address of inode structure
// Description	:	It is used to get address of inode structure from file name

PINODE Get_Inode(char *name)
{

    PINODE temp = head;
    if(name==NULL){
        return NULL;
    }

    while(temp!=NULL){
        if(strcmp(name,temp->FileName)==0)
            {
                    break;
            }
            temp = temp->next;
    }

    return temp;
}

///   this create the entry for inode table and store the that inode in last of head /first node
// Description	:	It is used to create DILB
void CreateDILB(){

	int i=1;
    PINODE newn= NULL;
    PINODE temp = head;
     
     while(i<=MAXINODE){

         newn = (PINODE)malloc(sizeof(INODE));
         newn->LinkCount = newn->ReferenceCout = 0;
         newn->FileType = newn->FileSize = 0;
         newn->Buffer = NULL;
		 newn->next = NULL;
         newn->InodeNumber = i;
         if(temp == NULL){
             head = newn;
             temp = head;
         }
         else{
             temp->next =  newn;
             temp = temp->next;
         }
     

    i++;
	 }
}

//  It create NULL entry for SuperBLock and set maxmima number of 50 block and count to 50f free block
void InitaliseSuperBlock(){

    int i=0;
    while(i<50){
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }
    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInodes  = MAXINODE;
}

// it print the detail of inode on screen
void ls_file(){
    PINODE temp = head;
    
    if(SUPERBLOCKobj.FreeInodes == MAXINODE){
        printf("Error: there are no files");
    }

    printf("\n filename\t Inode number \t filse size \t linkcount");
    printf("------------------------------------------------------");
    while(temp != NULL){
        if(temp->FileType != 0){
            printf("%s\t\t%d\t\t%d\t\t%d",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp->next;
    }

      printf("------------------------------------------------------");
}

// decrese the refeence count and set offset to 0 of all files
void CloseAllFiles(){
    int i=0;
    while(i<50){
        if(UFDTArr[i].ptrfiletable!= NULL){
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCout)--;
        }
        i++;
    }



}


int CloseFileByName(char *name){

    int i=0;
    i = GetFDFromName(name);
    if(i==-1){
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCout)--;
}

// 	char *name	:	It accepts name of file from user
//			int permission	:	It accepts which permission should given to file	
// It returns file descriptor of file which is of type integer
// It is used to create a file
int CreateFile(char *name,int permission){
        int i=0;
        PINODE  temp = head;
        if((name==NULL) || (permission==0) || (permission>3)){
            return -1;
        }
        if(SUPERBLOCKobj.FreeInodes==0){
            return -2;
        }
        if(Get_Inode(name)!=NULL){
            return -3;
        }

        (SUPERBLOCKobj.FreeInodes)--;

        while(temp!=NULL){
                if(temp->FileType==0)
                        break;
                 temp = temp->next;
        }

        while(i<50){
                if(UFDTArr[i].ptrfiletable==NULL)
                    break;
                i++;
        }

        UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
        
        if(UFDTArr[i].ptrfiletable == NULL){
			return -4;}

        UFDTArr[i].ptrfiletable->cout = 1;
        UFDTArr[i].ptrfiletable->mode = permission;
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;

        UFDTArr[i].ptrfiletable->ptrinode = temp;

        strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
        UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
        UFDTArr[i].ptrfiletable->ptrinode->ReferenceCout = 1;
        UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
        UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
        UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
        UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
        UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);
        memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);
        
        return i;
        
        
}
// it give the information of Inode structure of filename which is passed to it
int stat_file(char *name){
        PINODE temp = head;
            if(name==NULL){ 
                    return -1;
            }

            while(temp!=NULL){
                    if(strcmp(name,temp->FileName)==0){
                        break;
                    }
                    temp = temp->next;
            }

            if(temp == NULL){
                return -2;
            }

            printf("----------Statistical Information about file are-----------");
            printf("file name: %s\n",temp->FileName);
            printf("file INode number: %d\n",temp->InodeNumber);
            printf("file Size: %d\n",temp->FileSize);
            printf("file Actual Size: %d\n",temp->FileActualSize);
            printf("file LInkcount: %d\n",temp->LinkCount);
            printf("file Refernecout: %d\n",temp->ReferenceCout);

            if(temp->permission==1){
                    printf("File permission is read only\n");
            }
            else 
                if(temp->permission==2){
                    printf("File permission is write\n");
            }
            else 
                if(temp->permission==3){
                    printf("File permission is Read and write\n");
            }
            printf("----------------------------------------------------------------\n");

        return 0;
}

// gives detail of inode form number in UFDtarr[]
int fstat_file(int fd){
        PINODE temp = head;
            if(fd<0){ 
                    return -1;
            }
            if(UFDTArr[fd].ptrfiletable == NULL){
                    return -2;
            }
            temp = UFDTArr[fd].ptrfiletable->ptrinode;


            printf("----------Statistical Information about file are-----------");
            printf("file name: %s\n",temp->FileName);
            printf("file INode number: %d\n",temp->InodeNumber);
            printf("file Size: %d\n",temp->FileSize);
            printf("file Actual Size: %d\n",temp->FileActualSize);
            printf("file LInkcount: %d\n",temp->LinkCount);
            printf("file Refernecout: %d\n",temp->ReferenceCout);

            if(temp->permission==1){
                    printf("File permission is read only\n");
            }
            else 
                if(temp->permission==2){
                    printf("File permission is write\n");
            }
            else 
                if(temp->permission==3){
                    printf("File permission is Read and write\n");
            }
            printf("----------------------------------------------------------------\n");

        return 0;
}

// It remove the entry of adress of file table
//char *name	: It accepts name of file from user
// Integer
// It is used to remove the file

int rm_File(char *name){
        int fd=0;
        fd = GetFDFromName(name);
        if(fd==-1)
            return -1;
        
        (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;


        if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount== 0){

                UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
                free(UFDTArr[fd].ptrfiletable);
        }
        UFDTArr[fd].ptrfiletable = NULL;
        (SUPERBLOCKobj.FreeInodes)++;
} 


int truncate_File(char *name){
    int fd = GetFDFromName(name);
    if(fd==-1)
    {
        return -1;

    }
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset =0 ;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize =0 ;
    
}


int WriteFile(int fd, char *arr,int isize){
    if( ( (UFDTArr[fd].ptrfiletable->mode)!=WRITE) && ((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))
    {
        return -1;

    }
    if( ( (UFDTArr[fd].ptrfiletable->ptrinode->permission)!=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=READ+WRITE))
    {
        return -1;

    }
    if ((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFILESIZE){
            return -2;

		//UFDTArr[fd].ptrfiletable->ptrinode->Buffer = (char *)realloc(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,10);
		//UFDTArr[fd].ptrfiletable->ptrinode->FileSize = MAXFILESIZE + 1024;

    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)
    {
        return -3;
    }
    
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;   

    
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

    return isize;
}


int OpenFile(char * name,int mode){
    int i=0;
    PINODE temp = NULL;
    if(name==NULL|| mode <= 0){
        return -1;
    }
    temp = Get_Inode(name);
    if(temp== NULL){
        return -1;
    }
    if(temp->permission < mode){
        return -3;
    }


    while(i<50){
        if(UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable == NULL){
        return -1;
    }
    UFDTArr[i].ptrfiletable->cout = 1;
    UFDTArr[i].ptrfiletable->mode = mode;
    
    if(mode == READ + WRITE){
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if(mode == READ){
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if(mode == WRITE){
        UFDTArr[i].ptrfiletable->writeoffset =0;
    }

    UFDTArr[i].ptrfiletable->ptrinode = temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCout)++;
    
    return i;
}

//		int fd		:	It accepts file descriptor of file 
//			char *arr	:	It is a address of empty buffer
//			int isize	:	It accepts how many bytes to read
// It returns how many bytes are read successfully
// It is used to read file


int ReadFile(int fd,char *arr,int isize){

        int read_size = 0;
        if(UFDTArr[fd].ptrfiletable==NULL){
            return -1;
        }
        if(UFDTArr[fd].ptrfiletable->mode!=READ && UFDTArr[fd].ptrfiletable->mode!=READ + WRITE){
            return -2;
        }
        
        if((UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ) && (UFDTArr[fd].ptrfiletable->ptrinode->permission!=READ + WRITE)){
            return -3;
        }
        if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR){
            return -4;
        }

        read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);

        if(read_size < isize){

            strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset) ,read_size);
            UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset) + read_size; 
        }
        else{

            strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset) ,isize);
            UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset) + isize; 
            
        }

        return isize;
}



//int fd		:	It accepts file descriptor of file
//int size	:	It accepts how many bytes to be shift
//	int from	:	It accepts from where to shift
// It returns how many bytes are shift

int LseekFile(int fd,int size,int from){
    if(fd<0 || from>2){
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE)){

        if(from == CURRENT){
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)){
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0){
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;

        }

        else if(from == START){
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)){
                return -1;
            }
            if(size<0){
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }

        else if(from == END){
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE){
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size < 0)){
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }


    else if(UFDTArr[fd].ptrfiletable->mode == WRITE){

            if(from == CURRENT){

            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE ){
                return -1;
            }
            
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0){
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
                
            

            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
         
        }

        else if(from == START){
            if(size > MAXFILESIZE){
                return -1;
            }
            if(size<0){
                return -1;
            }
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            (UFDTArr[fd].ptrfiletable->writeoffset)= size;
        }

        else if(from == END){
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE){
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0){
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }

    }
}



void man(char *name){
    if(name==NULL) 
        return;
    if(_stricmp(name,"create")==0){
        printf("Descrition :used to create new regukar file\n");
        printf("usage :create File_name Permission\n");
    }
    else  if(_stricmp(name,"read")==0){
        printf("Descrition :used to read data for regukar file\n");
        printf("usage :read File_name No of byte to read\n");
    }

    else  if(_stricmp(name,"write")==0){
        printf("Descrition :used to write data for regukar file\n");
        printf("usage :write File_name after this enter the data that we want to write\n");
    }
    else  if(_stricmp(name,"ls")==0){
        printf("Descrition :used to display all information of file\n");
        printf("usage :ls\n");
    }
    else  if(_stricmp(name,"stat")==0){
        printf("Descrition :used to display all information of file\n");
        printf("usage :stat File_name\n");
    }


    else  if(_stricmp(name,"fstat")==0){
       printf("Descrition :used to display all information of file\n");
        printf("usage :stat File_description\n");
    }
    else  if(_stricmp(name,"truncate")==0){
        printf("Descrition :used to remove data from file\n");
        printf("usage :truncate file a\name\n");
    }
    else  if(_stricmp(name,"open")==0){
        printf("Descrition :used to open thr file\n");
        printf("usage :open File_name mode\n");
    }
    else  if(_stricmp(name,"close")==0){
        printf("Descrition :used to close the file\n");
        printf("usage :close File_name \n");
    }

    else  if(_stricmp(name,"closeall")==0){
        printf("Descrition :used to close all the file\n");
        printf("usage :closeall File_name \n");
    }
    else  if(_stricmp(name,"lseek")==0){
        printf("Descrition :used to change file offset\n");
        printf("usage : lseek file_name change in offset Start point \n");
    }
    else  if(_stricmp(name,"rm")==0){
        printf("Descrition :used to delete the file\n");
        printf("usage :rm file name \n");
    }
    else{
        printf("ERROR: no manual entry avaliable");
    }
    
}




int main(){

	char *ptr = NULL;
    int ret =0 ,fd =0,count =0;
    char command[4][80],str[80],arr[1024];
	printf("hello");

    InitaliseSuperBlock();

    CreateDILB();
	printf("hello");

        while(1)

		{
			
			fflush(stdin);
                strcpy_s(str,"");
                printf("\n Virtual File System:>");
                fgets(str,80,stdin);
                count = sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

                    if(count==1){
                        if(_stricmp(command[0],"ls")==0){
                            ls_file();
                        }
                        else if(_stricmp(command[0],"closeall")==0){
                            CloseAllFiles();
                            printf("all file closed\n");
                            continue;
                        }
                        else if(_stricmp(command[0],"clear")==0){
                            system("cls");
                            continue;
                        }
                        else if(_stricmp(command[0],"help")==0){

                            //    DisplayHelp();
                            continue;
                        }

                        else if(_stricmp(command[0],"exit")==0){
                            
                            printf("Termonating the Marvellous File System\n");
                            break;
                        }
                        else{
                            printf("ERROR:Command not found\n");
                            continue;
                        }
                    }
                    else if(count==2){
                            if(_stricmp(command[0],"stat")==0){
                                ret = stat_file(command[1]);
                                if(ret==-1){
                                    printf("ERROR:Incorrect parameters\n");
                                }
                                
                                if(ret==-2){
                                    printf("ERROR:there is no such file\n");
                                }
                                continue;
                            }
                            else if(_stricmp(command[0],"fstat")==0){
                                ret = fstat_file(atoi(command[1]));
                                if(ret==-1){
                                    printf("ERROR:Incorrect parameters\n");
                                }
                                
                                if(ret==-2){
                                    printf("ERROR:there is no such file\n");
                                }
                                continue;
                            }
                            else if(_stricmp(command[0],"close")==0){
                                ret = CloseFileByName(command[1]);
                                
                                if(ret==-1){
                                    printf("ERROR:there is no such file\n");
                                }
                                continue;
                            }

                            else if(_stricmp(command[0],"rm")==0){
                                ret = rm_File(command[1]);
                                
                                if(ret==-1){
                                    printf("ERROR:there is no such file\n");
                                }
                                continue;
                            }
                            
                            else if(_stricmp(command[0],"man")==0){
                                man(command[1]);
                            }

                            else if(_stricmp(command[0],"write")==0){
                                    fd = GetFDFromName(command[1]);

                                
                                if(fd==-1){
                                    printf("ERRO:incorrect parameters\n");
                                    continue;
                                }
                                printf("Enter the Data:\n");
                                scanf("%[^\n]s",arr);

                                ret = strlen(arr);
                                if(ret==0){
                                     printf("ERRO:incorrect parameters\n");
                                    continue;
                                }
                                ret = WriteFile(fd,arr,ret);
                                if(ret==-1){
                                    printf("ERROR:Permission deneied\n");
                                }
                                
                                if(ret==-2){
                                    printf("ERROR:there is so sufficent memory to write\n");
                                }
                                if(ret == -3){
                                    printf("ERROR: its is not regular file\n");
                                }
                            }

                            else if(_stricmp(command[0],"truncate")==0)
                            {
                                ret = truncate_File(command[1]);
                                if(ret == -1){
                                    printf("ERROR: Incorrect parameters\n");
                                }
                            }
                            else{
                                 printf("ERROR: Incorrect parameters\n");
                                continue;
                            }
                         }

                    else if(count == 3){

                            if(_stricmp(command[0],"create")==0){
                                ret =  CreateFile(command[1],atoi(command[2]));
                                if(ret >=0){
                                    printf("File is Succesfully crated with dercriptot :%d\n",ret);
                                }
                                if(ret==-1){
                                    printf("ERROR:Permission deneied\n");
                                }
                                
                                if(ret==-2){
                                    printf("ERROR:there is no inodes\n");
                                }
                                if(ret == -3){
                                    printf("ERROR: file alread exist\n");
                                }
                                if(ret==-4){
                                    printf("ERROR: Memory allicate Failure\n");

                                }
                                continue;
                            }

                            
                           else if(_stricmp(command[0],"open")==0){
                                ret =  OpenFile(command[1],atoi(command[2]));
                                if(ret >=0){
                                    printf("File is Succesfully opened with dercriptot :%d\n",ret);
                                }
                                if(ret==-1){
                                    printf("ERROR:incorrect parameters\n");
                                }
                                
                                if(ret==-2){
                                    printf("ERROR:file not present\n");
                                }
                                if(ret == -3){
                                    printf("ERROR: permission deneid\n");
                                }
                                
                                continue;
                            }


                         else if(_stricmp(command[0],"read")==0){
                                ret =  GetFDFromName(command[1]);

                                if(fd==-1){
                                    printf("ERROR:incorrect parameters\n");
                                }

                                ptr = (char*)malloc(sizeof(atoi(command[2]))+1);

                                if(ptr== NULL){
                                    printf("ERROR:MEmory allocation failure\n");
                                    continue;
                                }
                                ret = ReadFile(fd,ptr,atoi(command[2]));
                                if(ret==-1){
                                    printf("ERROR: file not exixting\n");
                                }
                                if(ret==-2){
                                    printf("ERROR: permission deneid\n");
                                }
                                if(ret == -3){
                                    printf("ERROR: reached at enf of file\n");
                                }
                                if(ret==-4){
                                    printf("ERROR : It is not regular\n");
                                }
                                if(ret==0){
                                    printf("ERROR: File empty");
                                }

                                if(ret>0){
                                    _write(2,ptr,ret);
                                }
                                continue;
                            }
                            else {
                                printf("\n ERROR :coMMnad not found \n");
                                continue;
                            }
                    }


                    else if(count == 4){

                        if(_stricmp(command[0],"lseek")==0){
                            fd = GetFDFromName(command[1]);
                            if(fd==-1){
                                printf("ERROR : Incorrect parameters\n");
                                continue;
                            }
                            ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
                            if(ret == -1){
                                printf("ERROR:unabble to perform Lseek\n");
                            }
                  
                        }
                        else{
                            printf("\n ERRor :commaind not fouund\n");
                            continue;
                        }
                    }
                    else{
                        printf("\n ERRor :commaind not fouund\n");
                        continue;
                    }

        }
	


    return 0;
}

