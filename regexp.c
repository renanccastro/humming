/* headers necessários */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

/* recebe como parâmetro a expressão regular e o input para
 * * tentar casar */
int er_match(char *regex, char* string)
{
    /* aloca espaço para a estrutura do tipo regex_t */
    regex_t reg;

    /* compila a ER passada em argv[1]
     * * em caso de erro, a função retorna diferente de zero */
    if (regcomp(&reg , regex, REG_EXTENDED|REG_NOSUB|REG_ICASE) != 0) {
        fprintf(stderr,"erro regcomp\n");
        exit(1);
    }
    /* tenta casar a ER compilada (&reg) com a entrada (argv[2])
     * * se a função regexec retornar 0 casou, caso contrário não */
    if ((regexec(&reg, string, 0, (regmatch_t *)NULL, 0)) == 0)
        return 1;
    else
        return 0;
}
