# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/03 22:58:14 by rtomishi          #+#    #+#              #
#    Updated: 2022/01/18 21:54:06 by rtomishi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = webserv

MAIN = ./main.cpp
SRC += $(wildcard config/*.cpp)
SRC += $(wildcard server/*.cpp)

OBJ = $(SRC:.cpp=.o)
MAIN_OBJ = $(MAIN:.cpp=.o)

CONFIGTEST_NAME = webserv_t
CONFIGTEST_MAIN = ./config/test/main.cpp
CONFIGTEST_OBJ = $(CONFIGTEST_MAIN:.cpp=.o)

# -std=c++98 sample.cc sample_test.cc -o test -L/usr/local/lib -pthread -lgtest -lgtest_main
# -std=c++98 -L/usr/local/lib -pthread -lgtest -lgtest_main
GTEST_NAME = gtest_exe
GTEST_SRC = $(wildcard gtest/*.cpp)
GTEST_OBJ = $(GTEST_SRC:.cpp=.o)

CXXFLAGS = -Wall -Werror -Wextra -I ./server -I ./config

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

test: CXXFLAGS += -pthread
test: fclean $(OBJ) $(CONFIGTEST_OBJ) $(GTEST_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(CONFIGTEST_OBJ) -o $(CONFIGTEST_NAME)
	$(CXX) -I ./gtest $(CXXFLAGS) $(OBJ) $(GTEST_OBJ) -o $(GTEST_NAME) -L/usr/local/lib -lgtest -lgtest_main
	./$(GTEST_NAME)
	rm -rf $(GTEST_NAME)
	$(MAKE) fclean

.PHONY: all clean fclean re config_t
