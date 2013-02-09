FLAGS = -c 
EXECUTABLE_NAME = poktapok
SOURCE_DIR = src/
BIN_DIR_NAME = bin
BIN_DIR = $(BIN_DIR_NAME)/
VPATH = src/
OBJS = $(BIN_DIR)agentResponse.o \
	$(BIN_DIR)gameCommand.o \
	$(BIN_DIR)gameData.o \
	$(BIN_DIR)gameObject.o \
	$(BIN_DIR)gameParameter.o \
	$(BIN_DIR)localizationEngine.o \
	$(BIN_DIR)localizationMethods.o \
	$(BIN_DIR)main.o \
	$(BIN_DIR)microParser.o \
	$(BIN_DIR)observation.o \
	$(BIN_DIR)obsHandler.o \
	$(BIN_DIR)parser.o \
	$(BIN_DIR)serializer.o \
	$(BIN_DIR)udpSocket.o \
	$(BIN_DIR)montecarloLocalization.o


$(BIN_DIR)poktapok : $(OBJS) $(BIN_DIR)gilAgent.o $(BIN_DIR)client.o 
	g++ $(OBJS) $(BIN_DIR)gilAgent.o $(BIN_DIR)client.o  -lpthread -o $(BIN_DIR)$(EXECUTABLE_NAME)

clean:
	rm $(BIN_DIR)*

$(BIN_DIR)agentResponse.o: \
	$(SOURCE_DIR)gameCommand.cpp
	echo "checking for bin directory..."
	if [ -d bin ]; then echo continue; else mkdir bin; fi
	g++ $(FLAGS) $(SOURCE_DIR)agentResponse.cpp -o $@

$(BIN_DIR)client.o:\
	$(SOURCE_DIR)udpSocket.cpp \
	$(SOURCE_DIR)gameData.cpp \
	$(SOURCE_DIR)agentResponse.cpp \
	$(SOURCE_DIR)parser.cpp \
	$(SOURCE_DIR)localizationEngine.cpp \
	$(SOURCE_DIR)microParser.cpp \
	$(SOURCE_DIR)udpSocket.cpp \
	$(SOURCE_DIR)serializer.cpp \
	$(SOURCE_DIR)gilAgent.cpp
	g++ $(FLAGS) -pthread $(SOURCE_DIR)client.cpp -o $@

$(BIN_DIR)gameCommand.o:\
	$(SOURCE_DIR)observation.cpp
	g++ $(FLAGS) $(SOURCE_DIR)gameCommand.cpp -o $@

$(BIN_DIR)gameData.o:\
	$(SOURCE_DIR)gameParameter.cpp \
	$(SOURCE_DIR)gameObject.cpp \
	$(SOURCE_DIR)obsHandler.cpp \
	$(SOURCE_DIR)observation.cpp \
	$(SOURCE_DIR)parser.cpp
	g++ $(FLAGS) $(SOURCE_DIR)gameData.cpp -o $@

$(BIN_DIR)gameObject.o:
	g++ $(FLAGS) $(SOURCE_DIR)gameObject.cpp -o $@

$(BIN_DIR)gameParameter.o:
	g++ $(FLAGS) $(SOURCE_DIR)gameParameter.cpp -o $@
	
$(BIN_DIR)gilAgent.o:\
	$(SOURCE_DIR)agentResponse.cpp \
	$(SOURCE_DIR)gameData.cpp \
	$(SOURCE_DIR)observation.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)gilAgent.cpp -o $@

$(BIN_DIR)localizationEngine.o:\
	$(SOURCE_DIR)gameData.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)localizationEngine.cpp -o $@
	
$(BIN_DIR)localizationMethods.o:
	g++ $(FLAGS) $(SOURCE_DIR)localizationMethods.cpp -o $@

$(BIN_DIR)main.o:\
	$(SOURCE_DIR)client.cpp
	g++ $(FLAGS) $(SOURCE_DIR)main.cpp -o $@

$(BIN_DIR)microParser.o:
	g++ $(FLAGS) $(SOURCE_DIR)microParser.cpp -o $@

$(BIN_DIR)montecarloLocalization.o:\
	$(SOURCE_DIR)gameObject.cpp
	g++ $(FLAGS) $(SOURCE_DIR)montecarloLocalization.cpp -o $@

$(BIN_DIR)observation.o:\
	$(SOURCE_DIR)gameObject.cpp
	g++ $(FLAGS) $(SOURCE_DIR)observation.cpp -o $@
	
$(BIN_DIR)obsHandler.o:\
	$(SOURCE_DIR)observation.cpp \
	$(SOURCE_DIR)gameObject.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)obsHandler.cpp -o $@

$(BIN_DIR)parser.o:\
	$(SOURCE_DIR)gameData.cpp \
	$(SOURCE_DIR)microParser.cpp \
	$(SOURCE_DIR)obsHandler.cpp
	g++ $(FLAGS) $(SOURCE_DIR)parser.cpp -o $@
	

$(BIN_DIR)serializer.o:\
	$(SOURCE_DIR)observation.cpp \
	$(SOURCE_DIR)gameCommand.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)serializer.cpp -o $@

$(BIN_DIR)udpSocket.o:
	g++ $(FLAGS) $(SOURCE_DIR)udpSocket.cpp -o $@

########### Para compilar el entrenamiento ################

TRAINING_SRC_DIR = training_src/

$(BIN_DIR)primeraTrainer :\
	$(OBJS) \
	$(BIN_DIR)trainerClient.o  \
	$(BIN_DIR)primeraTrainer.o
	g++ $(OBJS) $(BIN_DIR)trainerClient.o $(BIN_DIR)primeraTrainer.o -lpthread -o $@

$(BIN_DIR)primeraApprentice : $(OBJS) $(BIN_DIR)primeraApprentice.o  $(BIN_DIR)primeraAppClient.o 	$(BIN_DIR)primeraTrainer.o
	g++ $(OBJS)  $(BIN_DIR)primeraApprentice.o  $(BIN_DIR)primeraAppClient.o $(BIN_DIR)primeraTrainer.o -lpthread -o $@

$(BIN_DIR)primeraAppClient.o : 
	g++ $(TRAINING_SRC_DIR)primeraAppClient.cpp -c -o $@

$(BIN_DIR)primeraTrainer.o :
	g++ $(TRAINING_SRC_DIR)primeraTrainer.cpp -c -o $@

$(BIN_DIR)primeraApprentice.o :	\
	$(BIN_DIR)gameCommand.o \
	$(BIN_DIR)primeraTrainer.o
	g++ $(TRAINING_SRC_DIR)primeraApprentice.cpp -c -o $@

	
$(BIN_DIR)trainerClient.o : $(OBJS)
	g++ $(TRAINING_SRC_DIR)trainerClient.cpp -c -o $@

