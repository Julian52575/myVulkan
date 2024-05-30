##
## EPITECH PROJECT, 2024
## Vulkan experience
## File description:
## make file
##

NAME = myvulkan.out

CFLAGS = -std=c++17 -O3 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SRC = src/main.cpp \
	src/myvulkan/*.cpp \
	src/myvulkan/window/*.cpp

all:
	g++ $(CFLAGS) -o $(NAME) $(SRC) $(LDFLAGS) -DNDEBUG
debug:
	g++ $(CFLAGS) -o $(NAME) $(SRC) $(LDFLAGS) -g

fclean:
	rm $(NAME) || true

re: fclean all 

run: re
	./$(NAME)

run_debug: fclean debug
	valgrind -s ./$(NAME)
	

.Phony: all debug fclean re run run_debug
