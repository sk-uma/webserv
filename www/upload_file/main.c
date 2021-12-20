/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/19 08:34:40 by rtomishi          #+#    #+#             */
/*   Updated: 2020/11/19 23:20:35 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int		main(int argc, char **argv)
{
	int		fd;
	int		fd1;
	int		fd2;
	char	*gnl;
	int		i;

	i = 1;
	gnl = NULL;
	if (argc == 1)
	{
		int	ret = 1;
		fd = 0;
		while (ret > 0)
		{
			ret = get_next_line(fd, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret, BUFFER_SIZE, fd, i, gnl);
			i++;
		}

	}
	else if (argc == 2)
	{
		int	ret = 1;
		fd = open(argv[1], O_RDONLY);
		while (ret > 0)
		{
			ret = get_next_line(fd, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret, BUFFER_SIZE, fd, i, gnl);
			free(gnl);
			i++;
		}
		close(fd);
	}
	else if (argc == 3)
	{
		int ret = 1;
		int ret1 = 1;
		fd = open(argv[1], O_RDONLY);
		fd1 = open(argv[2], O_RDONLY);
		while (ret > 0 || ret1 > 0)
		{
			ret = get_next_line(fd, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret, BUFFER_SIZE, fd, i, gnl);
			free(gnl);
			ret1 =get_next_line(fd1, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret1, BUFFER_SIZE, fd1, i, gnl);
			free(gnl);
			i++;
		}
		close(fd);
		close(fd1);
	}
	else if (argc == 4)
	{
		int ret = 1;
		int ret1 = 1;
		int ret2 = 1;
		fd = open(argv[1], O_RDONLY);
		fd1 = open(argv[2], O_RDONLY);
		fd2 = open(argv[3], O_RDONLY);
		while (ret > 0 || ret1 > 0 || ret2 > 0)
		{
			ret = get_next_line(fd, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret, BUFFER_SIZE, fd, i, gnl);
			free(gnl);
			ret1 =get_next_line(fd1, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret1, BUFFER_SIZE, fd1, i, gnl);
			free(gnl);
			ret2 =get_next_line(fd2, &gnl);
			printf("ret:%d BUF_SIZE:%d fd:%d line_num:%d get_line:%s\n\n", ret2, BUFFER_SIZE, fd2, i, gnl);
			free(gnl);
			i++;
		}
		close(fd);
		close(fd1);
		close(fd2);
	}
	// while(1)
	// {}
	return (0);
}
