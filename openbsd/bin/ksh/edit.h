/*	$OpenBSD: src/bin/ksh/edit.h,v 1.9 2011/05/30 17:14:35 martynas Exp $	*/

/* NAME:
 *      edit.h - globals for edit modes
 *
 * DESCRIPTION:
 *      This header defines various global edit objects.
 *
 * SEE ALSO:
 *
 *
 * RCSid:
 *      $From: edit.h,v 1.2 1994/05/19 18:32:40 michael Exp michael $
 *
 */

/* some useful #defines */
#ifdef EXTERN
# define I__(i) = i
#else
# define I__(i)
# define EXTERN extern
# define EXTERN_DEFINED
#endif

#define	BEL		0x07

/* tty driver characters we are interested in */
typedef struct {
	int erase;
	int kill;
	int werase;
	int intr;
	int quit;
	int eof;
} X_chars;

EXTERN __thread X_chars edchars;

/* x_cf_glob() flags */
#define XCF_COMMAND	BIT(0)	/* Do command completion */
#define XCF_FILE	BIT(1)	/* Do file completion */
#define XCF_FULLPATH	BIT(2)	/* command completion: store full path */
#define XCF_COMMAND_FILE (XCF_COMMAND|XCF_FILE)

/* edit.c */
int	x_getc(void);
void	x_flush(void);
void	x_putc(int);
void	x_puts(const char *);
bool	x_mode(bool);
int	promptlen(const char *, const char **);
int	x_do_comment(char *, int, int *);
void	x_print_expansions(int, char *const *, int);
int	x_cf_glob(int, const char *, int, int, int *, int *, char ***, int *);
int	x_longest_prefix(int , char *const *);
int	x_basename(const char *, const char *);
void	x_free_words(int, char **);
int	x_escape(const char *, size_t, int (*)(const char *, size_t));
/* emacs.c */
int	x_emacs(char *, size_t);
void	x_init_emacs(void);
void	x_emacs_keys(X_chars *);
/* vi.c */
int	x_vi(char *, size_t);


#ifdef DEBUG
# define D__(x) x
#else
# define D__(x)
#endif

/* This lot goes at the END */
/* be sure not to interfere with anyone else's idea about EXTERN */
#ifdef EXTERN_DEFINED
# undef EXTERN_DEFINED
# undef EXTERN
#endif
#undef I__
/*
 * Local Variables:
 * version-control:t
 * comment-column:40
 * End:
 */
