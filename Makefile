# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/03 22:58:14 by rtomishi          #+#    #+#              #
#    Updated: 2021/12/23 17:39:50 by rtomishi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = webserv

SRC =	./main.cpp \
		./server/Socket.cpp \
		./server/RequestParser.cpp \
		./server/Response.cpp \
		./server/Util_func.cpp \
		./config/Config.cpp \
		./config/ConfigBase.cpp \
		./config/ConfigUtils.cpp \
		./config/Location.cpp \
		./config/Server.cpp

OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra -I ./server -I ./config

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