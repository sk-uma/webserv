# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/03 22:58:14 by rtomishi          #+#    #+#              #
#    Updated: 2021/11/26 22:24:13 by rtomishi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = webserv

SRC =	./main.cpp \
		./socket.cpp \
		./RequestParser.cpp \
		./Response.cpp \
		./util_func.cpp

OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra

STDLIB = -std=c++98

CXX = clang++ $(STDLIB)
#CXX = clang++ $(STDLIB) -g -fsanitize=leak

RM = rm -f

all: $(NAME) $(NAME0)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re