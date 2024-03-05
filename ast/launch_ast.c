/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launch_ast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: udumas <udumas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 08:56:17 by udumas            #+#    #+#             */
/*   Updated: 2024/03/05 11:49:08 by udumas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	launch_ast(char *input, t_list *env_list)
{
	int		exit_status;
	t_ast	*ast;

	exit_status = 0;
	ast = NULL; 
	create_ast_list(&ast, ft_lexer(input, &env_list));	
	//read_ast(ast, 0);
	if (!ast)
	{
		printf("Memory error\n");
		return (-1);
	}
	exit_status = launch_ast_recursive(ast, env_list);
	ft_free_ast(ast);
	return (exit_status);
}

int	launch_ast_recursive(t_ast *ast, t_list *env_list)
{
	int		exit_status;

	exit_status = 0;
	if (ast == NULL)
		return (0);
	else if (is_and(ast->token->token) == 1 && launch_ast_recursive(ast->left, env_list) == 0)
		exit_status = launch_ast_recursive(ast->right, env_list);
	else if (is_or(ast->token->token) == 1 && launch_ast_recursive(ast->left, env_list) != 0)
		exit_status = launch_ast_recursive(ast->right, env_list);
	else if (ast->token->type == 3 && is_pipe(ast->token->token) == 1)
		exit_status = create_redirection(ast, env_list);
	else if (ast->token->type == 0)
		exit_status = exec_shell_command(ast, env_list, redo_env(env_list));
	return (exit_status);
}

char	*build_command(t_ast *node)
{
	t_token	*travel;
	char	*command;
	
	travel = node->token;
	command = ft_strdup(node->token->token);
	travel = travel->next;
	while (travel != NULL)
	{
		command = ft_strjoin(command, " ", 1);
		command = ft_strjoin(command, travel->token, 1);
		travel = travel->next;
	}
	return (command);
}
void	do_pipe_redirections(t_ast *command, int fd[2])
{
	t_token	*travel;
	int		fd_out;
	int		fd_in;

	travel = command->token->file_redir_in;
	fd_out = fd[0];
	fd_in = fd[1];
	while (travel)
	{
		if (fd_in != fd[1])
			close (fd_in);
		fd_in = open(travel->file_redir, O_RDONLY);
		travel = travel->next;
	}
	travel = command->token->file_redir_out;
	while (travel)
	{
		if (fd_out != fd[0])
			close (fd_out);
		fd_out = open(travel->file_redir, O_CREAT | O_WRONLY | O_TRUNC, 0644);
		handle_error(fd_out, travel->file_redir);
		travel = travel->next;
	}
	printf("fd_out = %d\n", fd_out);
	printf("fd_in = %d\n", fd_in);
	printf ("fd[0] = %d\n", fd[0]);
	printf ("fd[1] = %d\n", fd[1]);
	if (fd_out != fd[0])
		dup2(fd_out, 1);
	if (fd_in != fd[1])
		dup2(fd_in, 0);
	if (fd_out == fd[0])
		close(fd[0]);
	if (fd_in == fd[1])
	{
		dup2(fd[1], 1);
		close(fd[1]);
	}
}

int	last_pipe(char **env, t_ast *command, int fd_out, t_list *env_list)
{
	int	id;
	int exit_status;
	char	*command_str;
	
	command_str = build_command(command);
	id = fork();
	if (id == 0)
	{
		do_pipe_redirections(command, (int[2]){0, 1});
		exec_command(command_str, env, env_list);
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(id, &exit_status, 0);
		if (fd_out != 1)
			close(fd_out);
	}
	while (wait(NULL) > 0)
		continue ;
	free(command_str);
	return (exit_status);
}

int	create_redirection(t_ast *node, t_list *env_list)
{
	int	exit_status;
    int saved_stdin = dup(0); 
	
	if (is_pipe(node->left->token->token) == 1)
        exit_status = left_pipe(node, env_list);
    else if (is_pipe(node->right->token->token) == 1)
        exit_status = right_pipe(node, env_list);
    else
    {
        exit_status = pipe_chain(redo_env(env_list), node->left, env_list);
        exit_status = last_pipe(redo_env(env_list), node->right, 1, env_list);
    }
	dup2(saved_stdin, 0);
	close(saved_stdin);
	return (exit_status);
}


int right_pipe(t_ast *node, t_list *env_list)
{
    t_ast   *travel;
    int     exit_status;
    travel = node;
    
    while (is_pipe(travel->right->token->token) == 1)
    {
        exit_status = pipe_chain(redo_env(env_list), node->left, env_list);
        travel = travel->left;
    }
    exit_status = pipe_chain(redo_env(env_list), node->left, env_list);
	last_pipe(redo_env(env_list), node->right, 1, env_list);
    return (exit_status);
}

int left_pipe(t_ast *node, t_list *env_list)
{
    t_ast   *travel;
    int     exit_status;
	
    travel = node;
    while (is_pipe(travel->left->token->token) == 1)
	{
        travel = travel->left;
	}
    exit_status = pipe_chain(redo_env(env_list), travel->left, env_list);
    while (travel != node)
    {
        exit_status = pipe_chain(redo_env(env_list), travel->right, env_list);
		travel = travel->daddy;
    }
    exit_status = last_pipe(redo_env(env_list), node->right, 1, env_list);
	
    return (exit_status);
}



int	pipe_chain(char **env, t_ast *command, t_list *env_list)
{
	int	fd[2];
	int	id;
	int	exit_status;

	if (pipe(fd) == -1)
	{
		perror("pipe failure");
		return (-1);
	}
	id = fork();
	if (id == -1)
	{
		perror("failure fork");
		return (-1);
	}
	if (id == 0)
	{
		do_pipe_redirections(command, fd);
		exec_command(build_command(command), env, env_list);
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(id, &exit_status, 0);
		dup2(fd[0], 0);
		close(fd[0]);
		close (fd[1]);
	}
	return (exit_status);
}
