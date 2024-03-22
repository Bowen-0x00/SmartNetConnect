#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <ell/ell.h>

#include "cmd.h"

#define COLOR_OFF	"\x1B[0m"
#define COLOR_BLUE	"\x1B[0;34m"

#define CMD_ARGS_MAX	20

static int sigfd_setup(void)
{
	sigset_t mask;
	int fd;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGTERM);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		l_error("Failed to set signal mask");
		return -1;
	}

	fd = signalfd(-1, &mask, 0);
	if (fd < 0) {
		l_error("Failed to create signal descriptor");
		return -1;
	}

	return fd;
}

static void print_prompt(void)
{
	printf(COLOR_BLUE "[mesh]" COLOR_OFF "# ");
	fflush(stdout);
}

static void  process_cmdline(char *input_str, uint32_t len)
{
	char *cmd, *arg, *parse_arg;
	char *args[CMD_ARGS_MAX];
	int argc = 0, i;

	/*
	 * If user enter CTL + d, program will read an EOF and len is zero.
	 */
	if (!len) {
		l_info("empty command");
		goto done;
	}

	if (!strlen(input_str))
		goto done;

	if (input_str[0] == '\n' || input_str[len - 1] == '\r')
		input_str[len - 1] = '\0';

	cmd = strtok_r(input_str, " ", &arg);
	if (!cmd)
		goto done;

	if (arg) {
		int len = strlen(arg);

		if (len > 0 && arg[len - 1] == ' ')
			arg[len - 1] = '\0';

		for (argc = 0; argc < CMD_ARGS_MAX; argc++) {
			parse_arg = strtok_r(NULL, " ", &arg);
			if (!parse_arg)
				break;

			args[argc] = parse_arg;
		}
	}

	for (i = 0; cmd_table[i].cmd; i++) {
		if (strcmp(cmd, cmd_table[i].cmd))
			continue;

		if (cmd_table[i].func)
        {
            if(cmd_table[i].arg_cnt == argc)
            {
			    cmd_table[i].func(argc, args);
            }
            else
            {
                l_error("%s Unexpected argc: %d != arg_cnt %d,Please see help!\n",cmd,argc,cmd_table[i].arg_cnt);
            }
			goto done;
		}
	}

	if (strcmp(cmd, "help")) {
		l_info("Invalid command");
		goto done;
	}

	l_info("Available commands:");
	for (i = 0; cmd_table[i].cmd; i++) {
		if (cmd_table[i].desc)
			l_info("\t%-20s\t\t%-15s", cmd_table[i].cmd,
							cmd_table[i].desc);
	}

done:
	return;
}

static void stdin_read_handler(int fd)
{
	ssize_t read;
	size_t len = 0;
	char *line = NULL;

	read = getline(&line, &len, stdin);
	if (read < 0)
		return;

	if (read <= 1) {
		print_prompt();
		return;
	}

	line[read - 1] = '\0';

	process_cmdline(line, strlen(line) + 1);
}

static void fifo_read_handler(int fd)
{
    char fifo_r[1024];
    size_t len = 0;

    if((len = read(fd,fifo_r,sizeof(fifo_r))) < 0)
    {
        if(errno == EAGAIN)
        {
            l_info("fifo read no_data\n");
        }
        else
        {
            l_info("fifo read %s",strerror(errno));
        }
    }
    else
    {
        fifo_r[len] = '\0';
        l_info("fifo get buf len =%d,str =%s",len,fifo_r);
        process_cmdline(fifo_r, len+1);
    }
}

int main(void)
{
	int sigfd;
	struct pollfd pfd[3];
    const char *fn = "/tmp/mesh_io";
    char fifo_r[1024] ={'\0'};
    uint32_t fifo_r_len = 0;
	printf("test aw_mesh package in Tina\n");

	l_log_set_stderr();
	sigfd = sigfd_setup();
	if (sigfd < 0)
		return -1;
    ///freopen("/etc/lib/bluetooth/mesh/cmd","r",stdin);
	pfd[0].fd = sigfd;
	pfd[0].events = POLLIN | POLLHUP | POLLERR;
	pfd[1].fd = fileno(stdin);
    if(pfd[1].fd <= 0)
    {
        l_info("pfd[1].fd  =%d,%s\n",pfd[1].fd,strerror(errno));
    }
	pfd[1].events = POLLIN | POLLHUP | POLLERR;
    if(access(fn,F_OK) == -1)
    {
        if(mkfifo(fn,0777) < 0)
        {
            l_info("mkfifio fail,%s\n",strerror(errno));
        }
    }
    pfd[2].fd = open(fn,O_RDONLY | O_NONBLOCK);
    l_info("%s,fd =%d,%s",fn,pfd[2].fd,strerror(errno));
    pfd[2].events = POLLIN | POLLHUP | POLLERR;


    aw_mesh_dbg_cfg_set();
    aw_mesh_init(aw_mesh_get_event_cb());

	print_prompt();

	while (!__main_terminated) {
		pfd[0].revents = 0;
		pfd[1].revents = 0;
        pfd[2].revents = 0;

		if (poll(pfd, 3, -1) == -1) {
			if (errno = EINTR)
				continue;
			l_error("Poll error: %s", strerror(errno));
			return -1;
		}

		if (pfd[0].revents & (POLLHUP | POLLERR)) {
			l_error("Poll rdhup or hup or err");
			return -1;
		}

		if (pfd[1].revents & (POLLHUP | POLLERR)) {
			l_error("Poll rdhup or hup or err");
			return -1;
		}

		if (pfd[2].revents & (POLLHUP | POLLERR)) {
            close(pfd[2].fd);
            pfd[2].fd = open(fn,O_RDONLY | O_NONBLOCK);
			l_error("Poll rdhup or hup or err");
			continue;
		}

		if (pfd[0].revents & POLLIN) {
			struct signalfd_siginfo si;
			ssize_t ret;


			ret = read(pfd[0].fd, &si, sizeof(si));
			if (ret != sizeof(si))
				return -1;
			switch (si.ssi_signo) {
			case SIGINT:
			case SIGTERM:
				__main_terminated = 1;
				break;
			}
		}


		if (pfd[1].revents & POLLIN)
			stdin_read_handler(pfd[1].fd);

		if (pfd[2].revents & POLLIN)
        {
            fifo_read_handler(pfd[2].fd);
        }
        //fflush(stdin);
        //l_info("goto sleep\n");
        //usleep(1000000);
        //l_info("sleep timeout\n");
	}

	aw_mesh_deinit();

	return 0;
}
