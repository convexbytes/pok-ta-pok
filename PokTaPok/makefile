FLAGS = -c
EXECUTABLE_NAME = poktapok
OBJS = agentResponse.o \
	client.o \
	gameCommand.o \
	gameData.o \
	gameObject.o \
	gameParameter.o \
	gilAgent.o \
	localizationEngine.o \
	localizationMethods.o \
	main.o \
	microParser.o \
	observation.o \
	obsHandler.o \
	parser.o \
	poktapokAgent.o \
	serializer.o \
	udpSocket.o \
	montecarloLocalization.o
poktapok : $(OBJS)
	g++ $(OBJS) -lpthread -o $(EXECUTABLE_NAME)

clean:
	rm $(OBJS)

agentResponse.o: \
	gameCommand.cpp
	g++ $(FLAGS) agentResponse.cpp

client.o:\
	udpSocket.cpp \
	gameData.cpp \
	agentResponse.cpp \
	parser.cpp \
	localizationEngine.cpp \
	microParser.cpp \
	udpSocket.cpp \
	serializer.cpp \
	gilAgent.cpp
	g++ $(FLAGS) -pthread client.cpp

gameCommand.o:\
	observation.cpp
	g++ $(FLAGS) gameCommand.cpp

gameData.o:\
	gameParameter.cpp \
	gameObject.cpp \
	obsHandler.cpp \
	observation.cpp \
	parser.cpp
	g++ $(FLAGS) gameData.cpp

gameObject.o:
	g++ $(FLAGS) gameObject.cpp

gameParameter.o:
	g++ $(FLAGS) gameParameter.cpp
	
gilAgent.o:\
	agentResponse.cpp \
	gameData.cpp \
	observation.cpp 
	g++ $(FLAGS) gilAgent.cpp

localizationEngine.o:\
	gameData.cpp 
	g++ $(FLAGS) localizationEngine.cpp
	
localizationMethods.o:
	g++ $(FLAGS) localizationMethods.cpp

main.o:\
	client.cpp
	g++ $(FLAGS) main.cpp

microParser.o:
	g++ $(FLAGS) microParser.cpp

montecarloLocalization.o:\
	gameObject.o
	g++ $(FLAGS) montecarloLocalization.cpp

observation.o:\
	gameObject.cpp
	g++ $(FLAGS) observation.cpp
	
obsHandler.o:\
	observation.cpp \
	gameObject.cpp 
	g++ $(FLAGS) obsHandler.cpp

parser.o:\
	gameData.cpp \
	microParser.cpp \
	obsHandler.cpp
	g++ $(FLAGS) parser.cpp
	
poktapokAgent.o:
	g++ $(FLAGS) poktapokAgent.cpp

serializer.o:\
	observation.cpp \
	gameCommand.cpp 
	g++ $(FLAGS) serializer.cpp

udpSocket.o:
	g++ $(FLAGS) udpSocket.cpp
