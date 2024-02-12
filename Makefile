CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
RM			=	rm -rf

NAME		=	ircserv

RESET		=	\033[0m
GREEN		=	\033[32m
YELLOW		=	\033[33m
BLUE		=	\033[34m
RED			=	\033[31m

INC_DIR		=	./inc/
OBJ_DIR		=	./src/obj

SRCS		=	$(shell find src -type f -name "*.cpp")
INCS		=	$(shell find inc -type f -name "*.hpp")
OBJS		=	$(SRCS:src/%.cpp=src/obj/%.o)
HOBJS		=	$(INCS:inc/%.hpp)

all: $(NAME)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(NAME): $(OBJ_DIR) $(OBJS) $(HOBJS)
	@echo "$(YELLOW)>- Compiling... [$(NAME)] $(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)>- Finished! $(RESET)"

$(OBJ_DIR)%.o: src/%.cpp

	@mkdir -p $(shell dirname $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(INC_DIR)

clean:
	@echo "$(RED)>- Deleting... [$(NAME)] $(RESET)"
	@$(RM) $(OBJ_DIR) $(HOBJS)
	@echo "$(BLUE)>- Successfully! $(RESET)"

fclean : clean
	@$(RM) $(NAME)

re: fclean all

.PHONY : all clean fclean re