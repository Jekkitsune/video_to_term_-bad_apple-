/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fparis <fparis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 16:32:52 by fparis            #+#    #+#             */
/*   Updated: 2024/04/19 17:32:47 by fparis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "MacroLibX/includes/mlx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libft/headers/libft.h"
#include <sys/types.h>
#include <sys/wait.h>

int	get_color(int start_x, int start_y, int limit, void *mlx, void *img)
{
	int	nb_white;
	int	nb_black;
	int	x;
	int	y;

	nb_black = 0;
	nb_white = 0;
	y = start_y;
	while (y < start_y + limit)
	{
		x = start_x;
		while (x < start_x + limit)
		{
			if ((unsigned int)mlx_get_image_pixel(mlx, img, x, y) - 0xFF000000 > (unsigned int)(0xFFFFFF / 2))
				nb_white++;
			else
				nb_black++;
			x++;
		}
		y++;
	}
	if (nb_black > nb_white)
		return (1);
	return (0);
}

int	get_limit(int longu, int larg)
{
	int	max;

	max = larg;
	if (longu > larg)
		max = longu;
	if (max / 50 == 0)
		return (max);
	return (max / 50);
}

int	img_to_line(char *path, int fd, void *mlx, void *img, int longueur, int size_max)
{
	int		x;
	int		y;
	char	to_write;
	int		limit;

	limit = longueur / size_max;
	if (longueur / size_max == 0)
		limit = longueur;
	y = 0;
	while (y + limit <= longueur - (longueur % size_max))
	{
		x = 0;
		while (x + limit <= longueur - (longueur % size_max))
		{
			to_write = '0' + get_color(x, y, limit, mlx, img);
			write(fd, &to_write, 1);
			x += limit;
		}
		y += limit;
	}
	to_write = '\n';
	write(fd, &to_write, 1);
}

int	video_to_line(char *name)
{
	int		fd;
	void	*mlx;
	int		longueur;
	int		i;
	char	*path;
	int		i_nb;
	void	*img;

	mlx = mlx_init();
	fd = open("t", O_RDWR | O_CREAT | O_TRUNC, 0644);
	img = mlx_png_file_to_image(mlx, name, &longueur, NULL);
	mlx_destroy_image(mlx, img);
	path = malloc(ft_strlen(name) + 1);
	ft_strlcpy(path, name, ft_strlen(name) + 1);
	i_nb = ft_strlen(path) - 5;
	i = 0;
	while (!access(path, F_OK | R_OK))
	{
		img = mlx_png_file_to_image(mlx, path, NULL, NULL);
		img_to_line(path, fd, mlx, img, longueur, 100);
		mlx_destroy_image(mlx, img);
		i++;
		path[i_nb] = i % 10 + '0';
		path[i_nb - 1] = i % 100 / 10 + '0';
		path[i_nb - 2] = i % 1000 / 100 + '0';
		path[i_nb - 3] = i % 10000 / 1000 + '0';
	}
	free(path);
	close(fd);
	return (longueur);
}

void	ft_sleep(char *time)
{
	int		pid;
	char	**cmd;

	//usleep(100000);
	//return ;
	cmd = malloc(sizeof(char *) * 3);
	cmd[0] = "/bin/sleep";
	cmd[1] = time;
	cmd[2] = NULL;
	pid = fork();
	if (!pid)
	{
		execve(cmd[0], cmd, NULL);
		exit(0);
	}
	else
		waitpid(pid, NULL, 0);
	free(cmd);
}

void	line_reader(char *path, int size)
{
	int		i;
	int		fd;
	char	*line;

	fd = open(path, O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		ft_sleep("0.03333");
		printf("\033c");
		i = 0;
		while (line[i])
		{
			if (i % size == 0)
				printf("\n");
			if (line[i] == '1')
				printf("#");
			else
				printf(" ");
			i++;
		}
		free(line);
		line = get_next_line(fd);
		if (!line)
			return ;
	}
	close(fd);
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("first argument: txt path\nsecond argument: size\n");
		return (1);
	}
	//video_to_line("bamieux/frame_0001.png");
	line_reader(argv[1], ft_atoi(argv[2]));

	// int fd = open("t", O_RDWR | O_CREAT | O_TRUNC, 0644);
	// void *mlx = mlx_init();
	// img_to_line("bamieux/frame_0200.png", fd, 8, mlx);
}
