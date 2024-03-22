#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
// #include <select.h>

int set_speed(int fd, int speed){
    int   i;
    int   status;
    struct termios  Opt = {0};
    int speed_arr[] = { B230400, B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300 };
    int name_arr[] = { 230400, 115200, 38400,  19200,  9600,  4800,	2400,  1200,  300, 38400 };

    tcgetattr(fd, &Opt);

    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
            if  (speed == name_arr[i])
                    break;
    }

    if (i == sizeof(speed_arr) / sizeof(int))
            return -1;

    tcflush(fd, TCIOFLUSH);
    cfsetispeed(&Opt, speed_arr[i]);
    cfsetospeed(&Opt, speed_arr[i]);

    Opt.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    Opt.c_oflag  &= ~OPOST;   /*Output*/

    status = tcsetattr(fd, TCSANOW, &Opt);
    if  (status != 0) {
            printf("tcsetattr error");
            return -1;
    }
    tcflush(fd, TCIOFLUSH);
    return 0;
}

/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;

    if ( tcgetattr( fd,&options)  !=  0) {
            perror("SetupSerial 1");
            return -1;
    }
    options.c_cflag &= ~CSIZE;

    switch (databits) /*设置数据位数*/
    {
            case 7:
                    options.c_cflag |= CS7;
                    break;
            case 8:
                    options.c_cflag |= CS8;
                    break;
            default:
                    fprintf(stderr,"Unsupported data size\n");
                    return -1;
    }

    switch (parity)
    {
            case 'n':
            case 'N':
                    options.c_cflag &= ~PARENB;
                    options.c_iflag &= ~INPCK;
                    break;
            case 'o':
            case 'O':
                    options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
                    options.c_iflag |= INPCK;
                    break;
            case 'e':
            case 'E':
                    options.c_cflag |= PARENB;
                    options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
                    options.c_iflag |= INPCK;
                    break;
            case 'S':
            case 's':  /*as no parity*/
                    options.c_cflag &= ~PARENB;
                    options.c_cflag &= ~CSTOPB;break;
            default:
                    fprintf(stderr,"Unsupported parity\n");
                    return -1;
    }

    /* 设置停止位*/
    switch (stopbits)
    {
            case 1:
                    options.c_cflag &= ~CSTOPB;
                    break;
            case 2:
                    options.c_cflag |= CSTOPB;
                    break;
            default:
                    fprintf(stderr,"Unsupported stop bits\n");
                    return -1;
    }

    /* Set input parity option */
    if (parity != 'n')
            options.c_iflag |= INPCK;
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 150;
    options.c_cc[VMIN] = 0;
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
            perror("SetupSerial 3");
            return -1;
    }
    return 0;
}


int main(int argc, char const *argv[])
{
    int fd = 0;
        char uartName[20];
    char * uartNum = "5";
    if (argc > 1) {
        uartNum = argv[1];
    }
    sprintf(uartName, "/dev/ttyS%s", uartNum);
    printf("open device: %s\n", uartName);
    fd = open(uartName, O_RDWR);
    if (fd == -1) {
        printf("open %s error\n", uartName);
        return -1;
    }
    int baudRate = 115200;
    if (argc > 2) {
        baudRate = atoi(argv[2]);
    }
    set_speed(fd, baudRate);
    set_parity(fd, 8, 1, 'N');
    int len = 100;
    
    int pos = 0;
    char *str;
    while (1) {
        char c;
        while ((c = getchar())) {
            if (c == '\n')
                break;
            if (pos == 0) {
                str =  (char*)malloc(100);
                len = 100;
                
            }
            else if (pos == len) {
                char * temp = (char*)malloc(2*len);
                len *= 2;
                strcpy(temp, str);
                free(str);
                str = temp;
            }                                        
            str[pos] = c;    
            pos++;       
        }
        printf("len: %d\n",len);
        char * data = malloc((int)(len / 2) + 1);
        printf("data: %p\n",data);
        int dataPos = 0;
        char *p = strtok(str, " ");
        
        while(p)
    	{
            
        	printf("%s\n",p);
        	
            sscanf(p, "%02X", &(data[dataPos]));
            printf("data: %02X\n",data[dataPos]);
            dataPos += 1;
            p = strtok(NULL, " "); 
    	}
        int size = write(fd, data, dataPos);
        printf("write size: %d\n", size);
        free(data);
        free(str);
        pos = 0;
    }
    // unsigned char data[] = {0x55 ,0x55 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0x03 ,0xFD ,0xD4 ,0x14 ,0x01 ,0x17 ,0x00};
//     char data[] = "Hello\n";
    
    close(fd);
    return 0;
}
