FLAGS = -c
EXECUTABLE_NAME = poktapok
SOURCE_DIR = src/
BIN_DIR_NAME = bin
BIN_DIR = $(BIN_DIR_NAME)/
VPATH = src/
OBJS = 	$(BIN_DIR)gameCommand.o \
	$(BIN_DIR)gameObject.o \
	$(BIN_DIR)gameParameter.o \
	$(BIN_DIR)localizationAdapter.o \
	$(BIN_DIR)localizationMethods.o \
	$(BIN_DIR)main.o \
	$(BIN_DIR)microParser.o \
	$(BIN_DIR)sensor.o \
	$(BIN_DIR)sensorHandler.o \
	$(BIN_DIR)parser.o \
	$(BIN_DIR)serializer.o \
	$(BIN_DIR)udpSocket.o \
	$(BIN_DIR)worldModel.o \
	$(BIN_DIR)state.o \
	$(BIN_DIR)montecarloLocalization.o \
	$(BIN_DIR)geometry.o \
	$(BIN_DIR)ability.o


$(BIN_DIR)poktapok : $(OBJS) $(BIN_DIR)poktapokAgent.o $(BIN_DIR)client.o 
	g++ $(OBJS) $(BIN_DIR)poktapokAgent.o $(BIN_DIR)client.o  -lpthread  -lrt -o $(BIN_DIR)$(EXECUTABLE_NAME)

clean:
	rm $(BIN_DIR)*

$(BIN_DIR)gameCommand.o:\
	$(SOURCE_DIR)gameCommand.h \
	$(SOURCE_DIR)gameCommand.cpp
	echo "checking for bin directory..."
	if [ -d bin ]; then echo continue; else mkdir bin; fi
	g++ $(FLAGS) $(SOURCE_DIR)gameCommand.cpp -o $@
	
$(BIN_DIR)client.o:\
	$(SOURCE_DIR)client.h \
	$(SOURCE_DIR)client.cpp
	g++ $(FLAGS) -pthread $(SOURCE_DIR)client.cpp -o $@

$(BIN_DIR)gameObject.o:
	g++ $(FLAGS) $(SOURCE_DIR)gameObject.cpp -o $@

$(BIN_DIR)gameParameter.o: \
	$(SOURCE_DIR)gameParameter.h \
	$(SOURCE_DIR)gameParameter.cpp
	g++ $(FLAGS) $(SOURCE_DIR)gameParameter.cpp -o $@
	
$(BIN_DIR)poktapokAgent.o:\
	$(SOURCE_DIR)poktapokAgent.h \
	$(SOURCE_DIR)poktapokAgent.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)poktapokAgent.cpp -o $@

$(BIN_DIR)localizationAdapter.o: \
	$(SOURCE_DIR)localizationAdapter.h \
	$(SOURCE_DIR)localizationAdapter.cpp
	g++ $(FLAGS) $(SOURCE_DIR)localizationAdapter.cpp -o $@
	
$(BIN_DIR)localizationMethods.o: \
	$(SOURCE_DIR)localizationMethods.h \
	$(SOURCE_DIR)localizationMethods.cpp
	g++ $(FLAGS) $(SOURCE_DIR)localizationMethods.cpp -o $@

$(BIN_DIR)main.o:\
	$(SOURCE_DIR)main.cpp
	g++ $(FLAGS) $(SOURCE_DIR)main.cpp -o $@

$(BIN_DIR)microParser.o:\
	$(SOURCE_DIR)microParser.h \
	$(SOURCE_DIR)microParser.cpp
	g++ $(FLAGS) $(SOURCE_DIR)microParser.cpp -o $@

$(BIN_DIR)montecarloLocalization.o:\
	$(SOURCE_DIR)montecarloLocalization.h \
	$(SOURCE_DIR)montecarloLocalization.cpp
	g++ $(FLAGS) $(SOURCE_DIR)montecarloLocalization.cpp -o $@

$(BIN_DIR)sensor.o:\
	$(SOURCE_DIR)sensor.h \
	$(SOURCE_DIR)sensor.cpp
	g++ $(FLAGS) $(SOURCE_DIR)sensor.cpp -o $@
	
$(BIN_DIR)sensorHandler.o:\
	$(SOURCE_DIR)sensorHandler.h \
	$(SOURCE_DIR)sensorHandler.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)sensorHandler.cpp -o $@

$(BIN_DIR)parser.o:\
	$(SOURCE_DIR)parser.h \
	$(SOURCE_DIR)parser.cpp
	g++ $(FLAGS) $(SOURCE_DIR)parser.cpp -o $@
	

$(BIN_DIR)serializer.o:\
	$(SOURCE_DIR)serializer.h \
	$(SOURCE_DIR)serializer.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)serializer.cpp -o $@

$(BIN_DIR)udpSocket.o: \
	$(SOURCE_DIR)udpSocket.h \
	$(SOURCE_DIR)udpSocket.cpp
	g++ $(FLAGS) $(SOURCE_DIR)udpSocket.cpp -o $@


$(BIN_DIR)worldModel.o: \
	$(SOURCE_DIR)worldModel.h \
	$(SOURCE_DIR)worldModel.cpp 
	g++ $(FLAGS) $(SOURCE_DIR)worldModel.cpp -o $@

$(BIN_DIR)state.o: state.h state.cpp
	g++ $(FLAGS) $(SOURCE_DIR)state.cpp -o $@

$(BIN_DIR)geometry.o: \
	$(SOURCE_DIR)geometry.h \
	$(SOURCE_DIR)geometry.cpp
	g++ $(FLAGS) $(SOURCE_DIR)geometry.cpp -o $@

$(BIN_DIR)ability.o: \
	$(SOURCE_DIR)ability.cpp \
	$(SOURCE_DIR)ability.h
	g++ $(FLAGS) $(SOURCE_DIR)ability.cpp -o $@
