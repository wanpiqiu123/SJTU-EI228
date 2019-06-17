//
// Created by Leo on 2019/6/11.
//
#include     "head.h"
/*********************************************************************/
int OpenDev(char *Dev)
{
    int fd = open( Dev, O_RDWR | O_NOCTTY );         //| O_NOCTTY | O_NDELAY
    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
        return -1;
    }
    else
        return fd;
}
/**
 *@brief  设置串口通信速率
 *@param  fd     类型 int  打开串口的文件句柄
 *@param  speed  类型 int  串口速度
 *@return  void
 */
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
                    B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,
                  19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
        if  (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0) {
                perror("tcsetattr fd1");
                return;
            }
            tcflush(fd,TCIOFLUSH);
        }
    }
}
/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄
 *@param  databits 类型  int 数据位   取值 为 7 或者8
 *@param  stopbits 类型  int 停止位   取值为 1 或者2
 *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 */
int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    if  ( tcgetattr( fd,&options)  !=  0) {
        perror("SetupSerial 1");
        return(FALSE);
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
            fprintf(stderr,"Unsupported data size/n"); return (FALSE);
    }
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;
        default:
            fprintf(stderr,"Unsupported parity/n");
            return (FALSE);
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
            fprintf(stderr,"Unsupported stop bits/n");
            return (FALSE);
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}

void signal(int nwrite, int fd, char ch){
    for (int i = 0; i < 50; ++i) {
        nwrite = write(fd, &ch, 1);
        usleep(1000);
    }
//    nwrite = write(fd, &ch, 1);
    printf("%c\t", ch);
    sleep(1);
//    usleep(100000);
    char stop='i';
    for (int i = 0; i < 50; ++i) {
        nwrite = write(fd, &stop, 1);
        usleep(1000);
    }
//    nwrite = write(fd, &stop, 1);
    printf("%c\n", stop);
    sleep(2);
//    usleep(500000);
}

void loop(int nwrite, int nread, int fd){
    char send,receive;
    for (int i = 0; i < 5; ++i) {
        send='a';
        nwrite = write(fd, &send, 1);
        printf("nwrite: %d\n write char: %c\n", nwrite, send);
        nread = read(fd, &receive, 1);
        printf("nread: %d\n read char: %c\n", nread, receive);

        sleep(1);

        send='i';
        nwrite = write(fd, &send, 1);
        printf("nwrite: %d\n write char: %c\n", nwrite, send);
        nread = read(fd, &receive, 1);
        printf("nread: %d\n read char: %c\n", nread, receive);

        sleep(1);

        send='c';
        nwrite = write(fd, &send, 1);
        printf("nwrite: %d\n write char: %c\n", nwrite, send);
        nread = read(fd, &receive, 1);
        printf("nread: %d\n read char: %c\n", nread, receive);

        sleep(1);

        send='i';
        nwrite = write(fd, &send, 1);
        printf("nwrite: %d\n write char: %c\n", nwrite, send);
        nread = read(fd, &receive, 1);
        printf("nread: %d\n read char: %c\n", nread, receive);

        sleep(1);
    }

}

//int main(int argc, char **argv)
//{
//
//    int fd;
//    int nread, nwrite;
//    char buff[16];
//    char buu = 'a';
//    char buu2=' ';
//    char *dev  = "/dev/tty.BT04-A-Port"; //serial_port name
////    char *dev = "/dev/tty.HC-06-DevB";
//    fd = OpenDev(dev);
//    set_speed(fd,9600);
//    if (set_Parity(fd,8,1,'N') == FALSE)
//    {
//        printf("Set Parity Error/n");
//        exit (0);
//    }
//    sleep(2);
////    int count=0;
////    while (count++<=20){
////        signal(nwrite,fd,'e');
////    }
//
//    while((read(fd, &buu2, 1))==1){
//        printf("buff: %c\n",buu2);
////        sleep(1);
//    }
//    loop(nwrite,nread,fd);
////    while(buu!='q') {
////        printf("output\n");
////        usleep(10000);
////        nwrite = write(fd, &buu, 1);
////        //buff[nread+1] = '\0';
////        printf("nwrite: %d\n write char: %c\n", nwrite, buu);
//////        sleep(1);
////        nread = read(fd, &buu2, 1);
////        printf("nread: %d\n read char: %c\n", nread, buu2);
////        std::cin>>buu;
////    }
//    close(fd);
//    //exit (0);
//}
//
