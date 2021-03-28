TARGET = vksnake
CPPFILES = src/external/VkBootstrap.cpp src/renderer/VertexInput.cpp src/renderer/VulkanPipeline.cpp src/renderer/ReactangleShape.cpp src/renderer/VulkanRenderer.cpp src/SnakeBody.cpp src/Board.cpp src/Game.cpp src/vksnake.cpp
OBJS = $(CPPFILES:.cpp=.o)
CXXFLAGS = -Wall -pedantic -I./include -I./include/external -I./include/renderer -O2
LDFLAGS = -ldl -lSDL2 -lvulkan -s

all: $(TARGET)
	glslc -o vertexshader.spv src/shaders/vertexshader.vert
	glslc -o fragmentshader.spv src/shaders/fragmentshader.frag

%.o: %.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $<

$(TARGET): $(OBJS) $(SPVS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f *.spv
	rm -f $(OBJS)
	rm -f $(TARGET)
