// relative-starter
// 
// Copyright 2010 icyomik <iCyOMiK&GMAIL>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.

#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXLEN 512

//////////////////////////////////////////////////
// 定义静态变量
static char selfName[MAXLEN];

//////////////////////////////////////////////////
// 打印错误信息，退出程序
void
exits (char *exitStr, int exitCode)
{
	printf ("%s", exitStr);
	exit (exitCode);
}

//////////////////////////////////////////////////
// 返回指定路径和程序名
char *
name (int c, char *s)
{
	char *trimName = (char *)malloc( MAXLEN );
	memset (trimName, 0, MAXLEN);

	char CH[3] = "";
	strcpy (trimName, s);
	strtok (trimName, "\n");

	switch (c)
	{
		case 'b':
			trimName = basename (trimName);
			return trimName; break;
		case 'd':
			trimName = dirname (trimName);
			strcat (trimName, "/");
			return trimName; break;
		case 'r':
			trimName = basename (trimName);
			strcat (trimName, ".ptr");
			return trimName; break;
		case 't':
			// 检查被调用文件的路径是否正确
			strncpy (CH, trimName, 2);

			if ( !strcmp(CH, "..") )
			{	// 双点开头报错，程序退出
				exits ("an error occured, path CAN NOT begin with deuce !\n", 1);
				break;
			}
			else if ( !strcmp(CH, "./") || !strcmp(CH, "!/") )
			{
				if ( !strcmp(CH, "./") )
				{	// 单点斜杠开头将"./"去除，再合并"被调用文件的路径"
					strcpy (trimName, trimName + 2);
					char    execPath[MAXLEN] = "";
					strcpy (execPath, name('d', selfName));
					strcat (execPath, trimName);
					strcpy (trimName, execPath);
				}
				else
				{	// 叹号斜杠开头将"!"去除，再设置其为绝对路径调用
					strcpy (trimName, trimName + 1);
				}
			}
			else
			{	// 不是单点斜杠、叹号斜杠开头则直接合并"被调用文件的路径"
				char execPath[MAXLEN] = "";
				strcpy (execPath, name('d', selfName));
				strcat (execPath, trimName);
				strcpy (trimName, execPath);
			}

			strtok (trimName, "\n");
			return trimName; break;
		default:
			exits ("an error occured on name() function\n", 1);
			break;
	}
	return s;
}

//////////////////////////////////////////////////
// 在字符串首位插入"-"字符
char *
minusInsert (char *srcStr)
{
	char   *dstStr = (char *)malloc( MAXLEN );
	memset (dstStr, 0, MAXLEN);
	strcpy (dstStr, "-");
	strcat (dstStr, srcStr);
	strtok (dstStr, "\n");
	return  dstStr;
}

//////////////////////////////////////////////////
// 修辑相对启动器的参数
char *
argConvert (char *arg)
{
	char *trimArgs = (char *)malloc( MAXLEN );
	memset (trimArgs, 0, MAXLEN);

	char CH[2] = "";
	strcpy (trimArgs, arg);
	strncpy(CH, trimArgs, 1);

	while ( !strcmp(" ", CH) )
	{
		strcpy (trimArgs, trimArgs + 1);
		strncpy(CH, trimArgs, 1);
	}

	strcat (trimArgs, " -");
	strtok (trimArgs, "\n");

	return trimArgs;
}

//////////////////////////////////////////////////
// 把相对启动器的参数写入RC文件
void
writeCtrlToRC (char *ctrl)
{
	FILE *fp;
	char additional[]="\
\n\
###########################################\n\
# First  Line Control File Which To Start #\n\
# Second Line Control Args Add To Command #\n\
# --------------------------------------- #\n\
#   Example 1 :                           #\n\
#  'me/omik' almost equal to './me/omik'  #\n\
#   except for the example below          #\n\
#      1st-line  !/omik.sh                #\n\
#      1st-line  ./!/omik.sh              #\n\
# --------------------------------------- #\n\
#   Example 2 :                           #\n\
#   use parameter to load the executable  #\n\
#   file which is in the relative path    #\n\
#      1st-line  me/omik.sh               #\n\
#      2nd-line  --help --all             #\n\
# --------------------------------------- #\n\
#   Example 3 :                           #\n\
#   use parameter to load the executable  #\n\
#   file which is in the absolute path    #\n\
#      1st-line  !/usr/bin/me.pl          #\n\
#      2nd-line  --run --verbose          #\n\
# --------------------------------------- #\n\
#   Example 4 :                           #\n\
#   use 'gnome-terminal' or 'konsole'     #\n\
#   to load the executable file           #\n\
#      1st-line  ./icyomik.py~            #\n\
###########################################\n\
\n\
*******************************************\n\
* Author   -->>  Jiang Jun [CN]           *\n\
* Project  -->>  relative-starter         *\n\
* Website  -->>  http://bit.ly/bT9BLZ     *\n\
* Summary  -->>  run script or program    *\n\
* Version  -->>  0.08.18-2010 ( GPLv3 )   *\n\
*******************************************\n\
";
	fp = fopen ( name('r', selfName), "w" );
	fprintf (fp, "%s\n%s", ctrl, additional);
	fclose (fp);
}

//////////////////////////////////////////////////
// 打印程序的关于信息
void
printAbout()
{
	printf
	("\
*******************************************\n\
* Author   -->>  Jiang Jun [CN]           *\n\
* Project  -->>  relative-starter         *\n\
* Website  -->>  http://bit.ly/bT9BLZ     *\n\
* Summary  -->>  run script or program    *\n\
* Version  -->>  0.08.18-2010 ( GPLv3 )   *\n\
*******************************************\n"
	);
}

//////////////////////////////////////////////////
// 打印程序的使用说明
void
printReadme (char *self)
{
	printf
	("\
usage:\n\
  %s [-cmd \"COMMAND\" [-arg \"ARGUMENTS\"]]\n\
     COMMAND:\n\
       [./|!/][dir0/][...][dirN/]file[~]\n\
     ARGUMENTS:\n\
       THE FILE'S (WHICH ABOVE) ARGUMENTS\n\
  Read \"%s\" For More Information\n", self, name('r', selfName)
	);

	char *ctrl = (char *)malloc( MAXLEN * 2 );
	strcat
	(ctrl, "\
!/usr/bin/zenity\n\
--info \
--text=\"THIS IS A TEST FOR ACALL ,\\n LOAD WITH ARGUMENTS !\" \
--title=\"project by icyomik\""
	);

	writeCtrlToRC (ctrl);
	exit (0);
}

//////////////////////////////////////////////////
// 主函数
int
main (int argc, char *argv[])
{
	//////////////////////////////////////////////////
	// 初始化变量
	int readme = 0;
	char cmd[MAXLEN] = "";
	char arg[MAXLEN] = "";
	memset (selfName, 0, MAXLEN);

	//////////////////////////////////////////////////
	// 获取自身绝对路径和自身名称
	if ( -1 == readlink( "/proc/self/exe", selfName, MAXLEN ) )
		exits ("an error occured, CAN NOT get selfName !\n", 1);

	//////////////////////////////////////////////////
	// 防止双击软链接时，在软链接所在目录生成配置文件
	if ( chdir (name ('d', selfName) ) )
		exits ( "CAN NOT change work directory !\n", 1);

	//////////////////////////////////////////////////
	// 检查参数，有则将其写入同名且后缀为".ptr"的文件中
	if ( argv[1] != NULL && !strcmp(argv[1], "-cmd") )
	{
		char *ctrl = (char *)malloc( MAXLEN * 2 );
		if ( argv[2] != NULL )
			strcat (ctrl, argv[2]);

		if ( argv[3] != NULL && !strcmp (argv[3], "-arg") )
			if ( argv[4] != NULL )
			{
				strcat ( ctrl, "\n" );
				strcat ( ctrl, argv[4] );
			}

		writeCtrlToRC (ctrl);
	}

	//////////////////////////////////////////////////
	// 检查是否存在同名且后缀为".ptr"的文件
	if  ( !access ( name('r', selfName), 0 ) )
	{	// 是，读取该文件第一行并检查内容是否正确
		FILE *fp;
		fp = fopen ( name('r', selfName), "r" );
		fgets (cmd, MAXLEN, fp);
		fgets (arg, MAXLEN, fp);
		fclose (fp);

		// 去除被调用文件的路径和参数的末尾换行符
		strtok (cmd, "\n");
		strtok (arg, "\n");

		if ( strlen(cmd) < 2 )
			readme = 1;

		if ( !strcmp(cmd, arg) )
			memset (arg, 0, MAXLEN);
	}
	else
	{	// 否，允许打印使用说明
		readme = 1;
	}

	//////////////////////////////////////////////////
	// 如果变量readMe值为1则打印使用说明，并且退出程序
	readme == 1 ? printReadme(argv[0]) : printAbout() ;

	//////////////////////////////////////////////////
	// 如果是波浪线结尾，则打开终端查看被调用程序的输出和对其进行输入
	char CH[2] = "";
	strcpy( CH, cmd + strlen(cmd) - 1 );
	if ( !strcmp("~", CH) )
	{
		char tmp[MAXLEN] = "";
		strncpy( tmp, cmd, strlen(cmd) - 1 );
		strcpy ( cmd, tmp );
	}

	//////////////////////////////////////////////////
	// 检查"被调用文件"是否存在，并执行相应操作
	strcpy ( cmd, name('t', cmd) );
	if ( !access(cmd, 0) )
	{	// 被调用文件存在
		char cmdPath[MAXLEN] = "";
		strcpy (cmdPath, cmd);
		dirname (cmdPath);

		printf ("changing directory to :\n--->>>\t%s\n", cmdPath);
		if ( chdir(cmdPath) )
			exits ( "CAN NOT change directory !\n", 1);

		if ( !fork() )
		{
			// 被调用文件不可执行或不可读，设置被调用文件的权限
			if ( access(cmd,1) || access(cmd,4) )
				if ( chmod (cmd, 0755) )
					exits ("CAN NOT change the file's permission", 1);

			// 子进程执行被调用文件
			if ( !strcmp("~", CH) )
			{
				char *args[MAXLEN];

				args[0] = "xterm";
				args[1] = "-e";
				args[2] = cmd;
				strcat (args[2]," ");
				strcat (args[2],arg);
				execvp (args[0],args);
				// 退出子进程
				exit (0);
			}
			else
			{
				char *seps= " -";
				char *ARG = argConvert(arg);

				char  tmp [MAXLEN];
				char *args[MAXLEN];
				char *buf = strstr(ARG, seps);

				int i = 1;
				while ( buf != NULL )
				{
					strncpy(tmp, ARG, buf - ARG);
					tmp [buf - ARG] = 0;
					args[i] = minusInsert(tmp);
					ARG = buf + strlen(seps);
					buf = strstr (ARG, seps);
					i += 1;
				}

				args[0] = name('b', cmd);
				args[1] = args[1] + 1;
				execvp ( cmd, args );
				// 退出子进程
				exit (0);
			}
		}
		// 退出父进程
		exits ( "********** PROGRAM EXIT NORMALLY **********\n", 0 );
	}
	else if ( access(cmd, 0) )
	{	// 被调用文件不存在，打印错误信息后退出程序
		printf( "the file below CAN NOT be found !\n" );
		printf( "--->>>\t%s\n", cmd );
		exit(1);
	}

	return 0;
}
