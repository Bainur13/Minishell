/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vda-conc <vda-conc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 11:27:01 by udumas            #+#    #+#             */
/*   Updated: 2024/03/04 11:21:09 by vda-conc         ###   ########.fr       */ 
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*take_path(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i]);
		i++;
	}
	return (NULL);
}

void	ft_free_char_tab(char **str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

char	*add_slash(char *cmd1)
{
	int		i;
	char	*cmd_slash;

	i = 0;
	while (cmd1[i])
		i++;
	cmd_slash = malloc(sizeof(char) * i + 2);
	i = 0;
	while (cmd1[i])
	{
		cmd_slash[i] = cmd1[i];
		i++;
	}
	cmd_slash[i] = '/';
	cmd_slash[i + 1] = '\0';
	return (cmd_slash);
}

char	*check_valid_command(char **cmd_split, char *path)
{
	char	**path_split;
	char	*temp;
	int		i;

	i = 0;
	if (path == NULL)
		return (NULL);
	path_split = ft_split(path + 5, ':');
	while (path_split[i])
	{
		temp = add_slash(path_split[i]);
		path = ft_strjoin(temp, cmd_split[0], 0);

		free(temp);
		if (access(path, F_OK | X_OK) == 0)
			return (ft_free_char_tab(path_split),
				path);
		free(path);
		i++;
	}
	path = NULL;
	return (ft_free_char_tab(path_split), path);
}

char	**redo_env(t_list *env)
{
	t_list	*env_node;
	char	*env_str;
	char	*temp;
	char	**split_env;

	env_node = env;
	while (env_node)
	{
		temp = ft_strjoin(env_node->var_name, "=", 0);
		env_str = ft_strjoin(env_str, temp, 0);
		free(temp);
		temp = ft_strjoin(env_str, env_node->content, 0);
		free(env_str);
		env_str = temp;
		temp = ft_strjoin(env_str, "\n", 0);
		free(env_str);
		env_str = temp;
		env_node = env_node->next;
	}
	split_env = ft_split(env_str, '\n');
	free(env_str);
	return (split_env);
}
int check_command(char **command, t_list *env_list)
{
	int exit_status;
	
	exit_status = 1871;
	
	if (ft_strncmp("env", command[0], 3) == 0)
		exit_status = ft_print_env(env_list);
	else if (ft_strncmp("unset", command[0], 5) == 0)
		exit_status = ft_unset(&env_list, command[1]);
	else if (ft_strncmp("export", command[0], 6) == 0)
		exit_status = ft_export(&env_list, command[1]);
	else if (ft_strncmp("exit", command[0], 4) == 0)
		exit_status = 1917;
	else if (ft_strncmp("cd", command[0] , 2) == 0)
	{
		if (command[1] == NULL)
			exit_status = ft_cd(NULL, &env_list);
		else
			exit_status = ft_cd(command[1], &env_list);
	}
	else if (ft_strncmp("pwd", command[0], 3) == 0)
		exit_status = ft_pwd();
	return (ft_free_char_tab(command), exit_status);
}

int	exec_command(char *command, char **env, t_list *env_list)
{
	char	*instruct;
	char 	**cmd_split;
	int 	exit_status;

	exit_status = check_command(ft_split(command, ' '), env_list);
	if (exit_status != 1871)
		exit(exit_status);
	cmd_split = ft_split(command, ' ');
	instruct = check_valid_command(cmd_split, take_path(env));
	if (instruct == NULL && access(cmd_split[0], F_OK | X_OK) == 0)
		instruct = cmd_split[0];
	if (instruct == NULL)
	{
		ft_putstr_fd(command, 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		return (ft_free_char_tab(cmd_split), 0);
	}
	else
	{
		execve(instruct, cmd_split, env);
		printf("execve error\n");
		exit(EXIT_FAILURE);
	}
	return (ft_free_char_tab(cmd_split), 0);
}
void	handle_error(int err, char *msg)
{
	if (err == -1)
	{
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

void	do_redirections(t_ast *command)
{
	t_token	*travel;
	int		fd_out;
	int		fd_in;

	travel = command->token->file_redir_in;
	fd_out = 1;
	fd_in = 0;

	while (travel)
	{
		if (fd_in != 0)
			close (fd_in);
		fd_in = open(travel->file_redir, O_RDONLY);
		travel = travel->next;
	}
	travel = command->token->file_redir_out;
	while (travel)
	{
		if (fd_out != 1)
			close (fd_out);
		fd_out = open(travel->file_redir, O_CREAT | O_WRONLY | O_TRUNC, 0644);
		handle_error(fd_out, travel->file_redir);
		travel = travel->next;
	}
	
	if (fd_out != 1)
		dup2(fd_out, 1);
	if (fd_in != 0)
		dup2(fd_in, 0);
}

int	exec_shell_command(t_ast *command, t_list *env_list, char **env)
{
	int	id;
	int	exit_status;
	char *command_str;
	
	command_str = build_command(command);
	exit_status = 1871;
	exit_status = check_command(ft_split(command_str, ' '), env_list);
	printf("exit_status: %d\n", exit_status);
	if (exit_status != 1871)
		return (exit_status);
	id = fork();
	if (id == -1)
	{
		perror("failure fork");
		return (-1);
	}
	if (id == 0)
	{
		do_redirections(command);
		exec_command(command_str, env, env_list);
		printf("execve error\n");
		exit(EXIT_FAILURE);
	}
	else
        waitpid(id, &exit_status, 0);
	return (exit_status);
}
