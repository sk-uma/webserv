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

MAIN = ./main/main.cpp
SRC += $(wildcard config/*.cpp)
SRC += $(wildcard server/*.cpp)

OBJ = $(SRC:.cpp=.o)
MAIN_OBJ = $(MAIN:.cpp=.o)

CONFIGTEST_NAME = webserv_t
CONFIGTEST_MAIN = ./config/test/main.cpp
CONFIGTEST_OBJ = $(CONFIGTEST_MAIN:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra -I ./temp_config -I ./server -I ./config


STDLIB = -std=c++98

CXX = clang++ $(STDLIB)
#CXX = clang++ $(STDLIB) -g -fsanitize=leak

RM = rm -f

all: $(NAME) $(NAME0)

$(NAME): $(OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(MAIN_OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ) $(MAIN_OBJ)

fclean: clean
	$(RM) $(NAME) $(CONFIGTEST_NAME)

re: fclean all

test: $(OBJ) $(CONFIGTEST_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(CONFIGTEST_OBJ) -o $(CONFIGTEST_NAME)

.PHONY: all clean fclean re config_t