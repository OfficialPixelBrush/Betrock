class DummyMesh {
    public:
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Vertex> waterVertices;
        std::vector<GLuint> waterIndices;
        Chunk* chunk;
        DummyMesh(Chunk* chunk, std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
                    std::vector<Vertex>& waterVertices, std::vector<GLuint>& waterIndices) {
            this->chunk = chunk;
            this->name = name;
            this->vertices = vertices;
            this->indices = indices;
            this->waterVertices = waterVertices;
            this->waterIndices = waterIndices;
        }
};