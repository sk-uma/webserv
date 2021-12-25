# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/03 22:58:14 by rtomishi          #+#    #+#              #
#    Updated: 2021/12/20 16:55:34 by rtomishi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = webserv

# SRC =	./main/main.cpp \
# 		./server/socket.cpp \
# 		./server/RequestParser.cpp \
# 		./server/Response.cpp \
# 		./server/util_func.cpp

SRC = ./main/main.cpp
SRC += $(wildcard config/*.cpp)
SRC += $(wildcard server/*.cpp)

OBJ = $(SRC:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra -I ./temp_config -I ./server -I ./config


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