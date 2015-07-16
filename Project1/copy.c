#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
char buffer;
int src,dst;
int main( int argc,char *argv[] )
{    
    int n;
    if(argc!=3)
    {
       	//fprint(stderr,"Insufficient Parameters"); 
		write(STDOUT_FILENO,"prgm1 <sourcefile> <destination file>\n",50);
        exit(1);
    }
    else
    {
        src=open(argv[1],O_RDONLY, 0644);
        if(src==-1)
        {
            perror("INVALID SOURCE");
            exit(0);
        }
        else
        {
            dst=open(argv[2],O_WRONLY | O_CREAT , 0641);
            while((n=read(src,&buffer,1)) != -1)
            {
                    write( dst, &buffer, 1 );
					n++;
            }
            write(STDOUT_FILENO, "FILES COPIED\n" , 15);
            close(src);
            close(dst);
			exit(0);            
        }
    }
    return 0;
}
