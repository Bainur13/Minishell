/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vda-conc <vda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 12:43:40 by vda-conc          #+#    #+#             */
/*   Updated: 2024/03/08 14:20:31 by vda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_print_env(t_list *env)
{
	if (env == NULL)
		return (0);
	while (env)
	{
		if (env->env_print == 1)
		{
			printf("%s", (char *)env->var_name);
			printf("=");
			printf("%s\n", (char *)env->content);
		}
		env = env->next;
	}
	return (1);
}

t_list	*ft_convert_env(char **env)
{
	int		i;
	t_list	*env_list;
	t_list	*tmp_new;

	i = 0;
	env_list = NULL;
	while (env[i])
	{
		tmp_new = ft_lstnew((char *)env[i], 1, 1);
		if (!tmp_new)
			return (NULL);
		ft_lstadd_back(&env_list, tmp_new);
		i++;
	}
	ft_lstadd_back(&env_list, ft_exit_variable());
	return (env_list);
}

t_list	*ft_exit_variable(void)
{
	t_list	*exit_variable;

	exit_variable = malloc(sizeof(t_list));
	if (!exit_variable)
		return (NULL);
	exit_variable->var_name = "?";
	exit_variable->content = "0";
	exit_variable->env_print = 1;
	exit_variable->next = NULL;
	exit_variable->prev = NULL;
	return (exit_variable);
}
void ft_update_xstatus(t_list **env, int x_status)
{
    t_list *curr;

    curr = *env;
    while (curr)
    {
        if (curr->var_name[0] == '?' && !curr->var_name[1])
        {
            curr->content = ft_itoa(x_status);
            break;
        }
        curr = curr->next;
    }
}