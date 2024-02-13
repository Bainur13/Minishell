/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vda-conc <vda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 11:45:55 by vda-conc          #+#    #+#             */
/*   Updated: 2024/02/13 10:09:12 by vda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_build_prompt(t_list **env)
{
	char	*hostname;
	char	*username;
	t_list	*pwd;
	char	*final_prompt;

	hostname = ft_extract_hostname(getenv("SESSION_MANAGER"));
	username = getenv("USER"); // TODO Change to get the USER VAR.
	pwd = ft_find_var(env, "$PWD");
	final_prompt = ft_strjoin(username, "@", 0);
	final_prompt = ft_strjoin(final_prompt, hostname, 1);
	final_prompt = ft_strjoin(final_prompt, ":", 1);
	final_prompt = ft_strjoin(final_prompt, "~", 1);
	final_prompt = ft_strjoin(final_prompt, trim_pwd(pwd->content), 1);
	final_prompt = ft_strjoin(final_prompt, "$ ", 1);
	free(hostname);
	return (final_prompt);
}

char	*ft_extract_hostname(char *session_manager)
{
	char	*end_of_hostname;
	char	*hostname;
	int		i;

	while (*session_manager != '/')
		session_manager++;
	session_manager++;
	end_of_hostname = session_manager;
	i = 0;
	while (i < 12)
	{
		i++;
		end_of_hostname++;
	}
	hostname = malloc((i + 1) * sizeof(char));
	i = 0;
	while (session_manager != end_of_hostname)
	{
		hostname[i] = *session_manager;
		session_manager++;
		i++;
	}
	hostname[i] = '\0';
	return (hostname);
}

char	*trim_pwd(char *pwd)
{
	char *home;

	home = getenv("HOME");
	while (*pwd == *home)
	{
		pwd++;
		home++;
	}
	return (pwd);
}