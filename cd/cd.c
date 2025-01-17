/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: udumas <udumas@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 15:48:58 by udumas            #+#    #+#             */
/*   Updated: 2024/02/28 18:48:34 by udumas           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int    ft_cd (char *path, t_list **env)
{
    int status;
    
    // if (path == NULL)
    //     go_home(env);
    if (ft_set_pwd(env) == 0)
        return (0);
    status = old_pwd_use(path, env);
    if (status != 1)
        return (status);
    status = back_home(path, env);
    if (status != 1)
        return (status);
    if (chdir(path) == -1)
        {
            if (CDPATH_FIND(path, env) == 0)
                return (0);
            printf("cd: %s: No such file or directory\n", path);
            return (-1);
        }
        else
        {
            ft_replace_pwd(env);
            if (ft_find_var(env, "$PWD") == NULL)
                return (perror("PWD not created: error"), -1);
        }
    return (0);
}

int old_pwd_use(char *path, t_list **env)
{
    t_list *old_pwd;
    char *old_pwd_content;

    if (path == NULL)
        return (1);
    while (*path == ' ')
        (*path)++;
    if (ft_strncmp(path, "-", 1) == 0)
    {
        old_pwd = ft_find_var(env, "$OLDPWD");
        if (old_pwd == NULL || old_pwd->content == NULL)
        {
            printf("cd: OLDPWD not set\n");
            return (-1);
        }
        else
        {
            old_pwd_content = ft_strdup(old_pwd->content);
            if (ft_cd(old_pwd->content, env) == 0)
                printf("%s\n", old_pwd_content);
            free(old_pwd_content);
            return (0);
        }
    }
    return (1);
}

int back_home(char *path, t_list **env)
{
    t_list *home;
    
    if (path == NULL)
        return (1);
    while (*path == ' ')
        (*path)++;
    if (ft_strncmp(path, "~", 1) == 0)
    {
        home = ft_find_var(env, "$HOME");
        if (home == NULL || home->content == NULL)
        {
            printf("cd: HOME not set\n");
            return (-1);
        }
        else
        {
            if (ft_cd(home->content, env) == 0)
                return (0);
            return(-1);
        }
    }
    return (1);
}

int CDPATH_FIND(char *path, t_list **env)
{
    t_list *CDPATH;
    char **CDPATH_split;
    char *temp;
    int i;

    CDPATH = ft_find_var(env, "$CDPATH");
    if (CDPATH == NULL || CDPATH->content == NULL)
        return (-1);
    CDPATH_split = ft_split(CDPATH->content, ':');
    i = 0;
    while (CDPATH_split[i])
    {
        temp = add_slash(CDPATH_split[i++]);
        temp = ft_strjoin(temp, path, 1);
        if (chdir(temp) == 0)
        {
            ft_replace_pwd(env);
            printf("%s\n", ft_find_var(env, "$PWD")->content);
            free(temp);
            ft_free_char_tab(CDPATH_split);
            return (0);
        }
        free(temp);
    }
    return (free(CDPATH_split), -1);   
}