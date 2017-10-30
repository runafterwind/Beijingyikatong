#
### makearm@163.com Makefile
#
#############
CROSS		= /usr/local/arm_linux_4.2/bin/arm-linux-
SRCDIRS		= ./ \
                  ./include\
                  ./bzdes\
                  ./Display\
                  ./input\
                  ./gui \
                  ./mmc \
 	          ./sound \
		 ./Errorlog \
		 ./psam \
		./update \
		./net

ASFLAGS		=
CFLAGS		= -Wall -w
CXXFLAGS	=
LDFLAGS		=
ARFLAGS		=
OCFLAGS		= 
ODFLAGS		=


INCDIRS		= -I ./include 
LIBDIRS 	= -L ./lib 
LIBS 		= -lpthread -lm -ldl
#
### You shouldn't need to change anything below this point.
#
##
#AS			= $(CROSS)as
CC			= $(CROSS)gcc
CXX			= $(CROSS)g++
LD			= $(CROSS)gcc
AR			= $(CROSS)ar
OC			= $(CROSS)objcopy
OD			= $(CROSS)objdump
RM			= -rm -fr


NAME		:= $(notdir $(CURDIR))
SFILES		:= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s))
CFILES		:= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
RMFILES		:= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*~))
RMFILEO		:= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.o))

OBJS 		:= $(SFILES:.s=.o) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
DEPS		:= $(OBJS:.o=.d)
VPATH		:= $(SRCDIRS)

.PHONY: all rebuild clean

all:
	@$(MAKE) $(NAME)

rebuild:
	@$(MAKE) clean
	@$(MAKE) $(NAME)

$(NAME):	$(OBJS)
	@echo test linking ...
#	$(LD) $(LDFLAGS) $(LIBDIRS) -o $@ $^ $(LIBS) 
#	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBDIRS)
	$(LD) $(LDFLAGS) -o armok $^ $(LIBS) $(LIBDIRS)
#	@tar -zcvf armok.tar.gz armok
#	cp armok /tftpboot/ 
%.o:	%.s
	@echo assembling $< ...
	$(AS) $(ASFLAGS) $(INCDIRS) $< -o $@

%.o:	%.c
	@echo test compiling $< ...
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@
	
%.o:	%.cpp	
	@echo compiling $< ...
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@
	
%.d:	%.c
	@$(CC) $(CFLAGS) $(INCDIRS) -MM $^ -o $@.tmp
	@sed 's,$(basename $(notdir $@)).o[ :]*,$(@:.d=.o) $@ : ,g' $@.tmp > $@
	@$(RM) $@.tmp
    	
%.d:	%.cpp
	@$(CXX) $(CXXFLAGS) $(INCDIRS) -MM $^ -o $@.tmp
	@sed 's,$(basename $(notdir $@)).o[ :]*,$(@:.d=.o) $@ : ,g' $@.tmp > $@
	@$(RM) $@.tmp

ifeq (mach/mach,)
-include $(DEPS)
endif
	
clean:
	@$(RM) $(OBJS) $(DEPS) $(NAME) $(RMFILES) $(RMFILEO)
	@echo clean completed

	
