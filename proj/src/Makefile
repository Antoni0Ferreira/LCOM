# name of the program (Minix service)
PROG=proj 

# source code files to be compiled
SRCS = main.c video.c keyboard.c mouse.c utils.c timer.c UI.c rtc.c file.c player.c astro.c game.c boost.c shots.c points.c cursor.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D__LCOM_OPTIMIZED__ 

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
