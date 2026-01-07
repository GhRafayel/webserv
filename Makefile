SRC =	src/main.cpp \
		src/My_server.cpp \
		src/StringUtils.cpp  \
		src/Server.cpp \
		src/Location.cpp \
		src/Request.cpp \
		src/Response.cpp \
		src/Client.cpp \
		src/Get.cpp \
		src/Post.cpp \
		src/Delete.cpp \
		src/ConfigPars.cpp

OBJ_FOLDER = output
OBJ = $(SRC:%.cpp=$(OBJ_FOLDER)/%.o)
DEP = $(OBJ:.o=.d)

NAME = webserv
C	 = c++
FLAG = -Wall -Wextra -Werror -std=c++98 -MMD  -g3
COMP = $(C) $(FLAG)

all : create-dirs $(NAME)

create-dirs :
	mkdir -p $(OBJ_FOLDER)/src

$(NAME) : $(OBJ)
	$(COMP) -o $(NAME) $(OBJ)

$(OBJ_FOLDER)/%.o : %.cpp 
	$(COMP) -c $< -o $@

clean : 
	rm -rf $(OBJ_FOLDER) 

fclean : clean 
	rm -f $(NAME) rm -f $(DEBUG)

re : fclean all

-include $(DEP)

.PHONY: all clean fclean re create-dirs