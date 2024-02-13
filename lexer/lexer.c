/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vda-conc <vda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 16:34:27 by vda-conc          #+#    #+#             */
/*   Updated: 2024/02/13 10:08:17 by vda-conc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_list	*ft_lexer(char *input, t_list **env)
{
	char	*expanded_input;
	char	**tokens;
	t_token	*listed_tokens;

	listed_tokens = NULL;
	expanded_input = ft_expand(input, env);
	tokens = ft_token_split(expanded_input);
	ft_print_tokens(tokens);
	listed_tokens = ft_convert_tokens(tokens);
	ft_print_token_list(&listed_tokens);
	return (NULL);
}

t_token	*ft_convert_tokens(char **tokens)
{
	int		i;
	t_ttype	type;
	char	*previous_token;
	t_ttype	previous_type;
	t_token	*tokens_list;

	i = 0;
	previous_token = NULL;
	tokens_list = NULL;
	while (tokens[i])
	{
		if (i == 0)
		{
			if (tokens[i] && tokens[i][0] && !ft_is_operator(tokens[i][0]))
				type = COMMAND;
		}
		else
			type = ft_define_ttype(tokens[i], previous_token);
		ft_tokenlstadd_back(&tokens_list, ft_tokenlstnew(tokens[i], type));
		previous_token = tokens[i];
		previous_type = type;
		i++;
	}
	return (tokens_list);
}

t_ttype	ft_define_ttype(char *token, char *previous_token)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	if (!token)
		return (0);
	if (ft_is_operator(token[i]))
		return (OPERATOR);
	if (token[i] == 34 || token[i] == 39)
		i++;
	if (ft_is_option(token))
		return (OPTION);
	if (previous_token)
	{
		if (previous_token[j] == 34 || previous_token[j] == 39)
			j++;
		if (previous_token[j] == '>' || previous_token[j] == '<')
			return (PATH_FILE);
		if (previous_token[j] == 38 || previous_token[j] == 124)
			return (COMMAND);
	}
	return (CMD_ARG);
}

int	ft_is_file(char *token)
{
	int	i;

	i = 0;
	while (token[i])
	{
		if (token[i] == '.')
			return (1);
		i++;
	}
	return (0);
}

int	ft_is_option(char *token)
{
	int	i;

	i = 0;
	if (token[0] != '-')
		return (0);
	else
		i++;
	while (token[i])
	{
		if (!ft_isalnum(token[i]))
			return (0);
		i++;
	}
	return (1);
}