#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXLEN 512

// 定义静态变量
static char selfName[MAXLEN];

// 打印错误信息，退出程序
void
exits (char *exitStr, int exitCode)
{
	printf ("%s", exitStr);
	exit (exitCode);
}

// 在字符串首位插入"-"字符
char *
minusInsert (char *srcStr)
{
	char *dstStr = (char *)malloc (MAXLEN);
	memset (dstStr, 0, MAXLEN);
	strcpy (dstStr, "-");
	strcat (dstStr, srcStr);
	return dstStr;
}

// 修辑相对启动器的参数
char *
argConvert (char *arg)
{
	char *trimArgs = (char *)malloc (MAXLEN);
	memset (trimArgs, 0, MAXLEN);

	char head[2] = "";
	strcpy (trimArgs, arg);
	strncpy (head, trimArgs, 1);

	while (!strcmp (" ", head))
	{
		strcpy (trimArgs, trimArgs + 1);
		strncpy (head, trimArgs, 1);
	}

	strcat (trimArgs, " -");

	return trimArgs;
}

// 返回指定路径和程序名
char *
name (int c, char *s)
{
	char *trimName = (char *)malloc (MAXLEN);
	memset (trimName, 0, MAXLEN);

	strcpy (trimName, s);

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
			{//合并"被调用文件的路径"
			char execPath[MAXLEN] = "";
			strcpy (execPath, name ('d', selfName));
			strcat (execPath, trimName);
			strcpy (trimName, execPath);
			return trimName; break;
			}
		default:
			exits ("an error occured on name ()function\n", 1);
			break;
	}
	return s;
}

int
main (int argc, char *argv[])
{
	// 初始化变量
	char cmd[MAXLEN] = "";
	char arg[MAXLEN] = "";
	memset (selfName, 0, MAXLEN);

	// 获取自身绝对路径和自身名称
	if (-1 == readlink ("/proc/self/exe", selfName, MAXLEN))
		exits ("an error occured, CAN NOT get selfName !\n", 1);

	//////////////////////////////////////////////////
	// 防止双击软链接时，在软链接所在目录生成配置文件
	if ( chdir (name ('d', selfName) ) )
		exits ( "CAN NOT change work directory !\n", 1);

	// 检查是否存在同名且后缀为".ptr"的文件
	if (!access (name ('r', selfName), 0))
	{	// 是，读取该文件第一行并检查内容是否正确
		FILE *fp;
		fp = fopen (name ('r', selfName), "r");
		fgets (cmd, MAXLEN, fp);
		fgets (arg, MAXLEN, fp);
		fclose (fp);

		// 去除被调用文件的"路径和参数"的末尾换行符
		strtok (cmd, "\n");
		strtok (arg, "\n");

		if (!strcmp (cmd, arg))
			memset (arg, 0, MAXLEN);
	}
	else
	{
		printf ("CAN NOT find resource:\n--->>>\t%s.ptr", selfName);
		FILE *fp;
		fp = fopen (name ('r', selfName), "w");
		fprintf (fp, "%s", "project by icyomik");
		fclose (fp);
		exit (1);
	}

	// 如果是波浪线结尾，则打开终端查看被调用程序的输出和对其进行输入
	char tail[2] = "";
	strcpy (tail, cmd + strlen (cmd)- 1);
	if (!strcmp ("~", tail))
	{
		char tmp[MAXLEN] = "";
		strncpy (tmp, cmd, strlen (cmd)- 1);
		strcpy (cmd, tmp);
	}

	// 检查"被调用文件"是否存在，并执行相应操作
	strcpy (cmd, name ('t', cmd));
	if (!access (cmd, 0))
	{	// 被调用文件存在
		char cmdPath[MAXLEN] = "";
		strcpy (cmdPath, cmd);
		dirname (cmdPath);

		printf ("changing directory to :\n--->>>\t%s\n", cmdPath);
		if (chdir (cmdPath))
			exits ("CAN NOT change directory !\n", 1);

		if (!fork ())
		{
			// 被调用文件不可执行或不可读，则设置被调用文件的权限
			if (access (cmd, 1) || access (cmd, 4))
				if (chmod (cmd, 0755))
					exits ("CAN NOT change the file's permission", 1);

			// 子进程执行被调用文件
			if (!strcmp ("~", tail))
			{
				char *args[3];

				args[0] = "xterm";
				args[1] = "-e";
				args[2] = cmd;
				strcat (args[2], " ");
				strcat (args[2], arg);
				execvp (args[0], args);
				// 退出子进程
				exit (0);
			}
			else
			{
				char tmp[MAXLEN];
				char *args[MAXLEN];
				char *seps= " -";
				char *ARG = argConvert (arg);
				char *buf = strstr (ARG, seps);

				int i = 1;
				while (buf != NULL)
				{
					strncpy (tmp, ARG, buf - ARG);
					tmp[buf - ARG] = 0;
					args[i] = minusInsert (tmp);
					ARG = buf + strlen (seps);
					buf = strstr (ARG, seps);
					i += 1;
				}

				args[0] = name ('b', cmd);
				args[1] = args[1] + 1;
				execvp (cmd, args);
				// 退出子进程
				exit (0);
			}
		}
		// 退出父进程
		exits ("********** PROGRAM EXIT NORMALLY **********\n", 0);
	}
	else if (access (cmd, 0))
	{	// 被调用文件不存在，打印错误信息后退出程序
		printf ("the file below CAN NOT be found !\n");
		printf ("--->>>\t%s\n", cmd);
		exit (1);
	}

	return 0;
}
