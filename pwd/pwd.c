/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vda-conc <vda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 19:13:30 by udumas            #+#    #+#             */
/*   Updated: 2024/02/13 10:17:12 by vda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_pwd(void)
{
	char	*cwd;

	cwd = malloc(sizeof(char) * SIZE);
	if (cwd == NULL)
		return (perror("malloc() error"), 0);
	if (getcwd(cwd, SIZE) == NULL)
	{
		perror("getcwd() error");
		free(cwd);
		return (0);
	}
	printf("%s\n", cwd);
	free(cwd);
	return (1);
}
