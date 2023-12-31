#include "Terrain.h"

// ������������ ������ Terrain
Terrain::Terrain(int widthIn, int heightIn, int stepSizeIn)
{
    width = widthIn;
    height = heightIn;
    stepSize = stepSizeIn;
    makeVertices(&vertices);
}

Terrain::Terrain() {
    width = 50;
    height = 50;
    stepSize = 10;
    makeVertices(&vertices);
}

// ����� getVAO ������� � ���������� VAO (Vertex Array Object) ��� ��������� ���������
unsigned int Terrain::getVAO() {
    // ������� VAO � VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // ����������� VAO � VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ��������� ����� ������� � ��������
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);

    // ������������� ��������� �� �������� ������ � ������
    // ������ ������� - ���������� xyz
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ������ ������� - ���������� ���������� uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ���������� VAO � VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// ����� getSize ���������� ���������� ������ � ������� vertices
int Terrain::getSize() {
    return vertices.size();
}

// ����� getVertices ���������� ������ ������
std::vector<float> Terrain::getVertices() {
    return vertices;
}

// ����� makeVertices ��������� ������ ������
void Terrain::makeVertices(std::vector<float>* vertices) {
    /* triangle a b c
         b
         | \
         a _ c


       triangle d f e
         f _ e
         \ |
           d

       c == d
       b == f
       Duplicate vertices but easier in long run! (tesselation and LOD)

      a = (x,y,z)
      b = (x, y+1)
      c = (x+1,y)

      d = (x+1,y)
      e = (x, y+1)
      f = (x+1,y+1)

       each vertex a, b,c, etc. will have 5 data:
       x y z u v
        */

        // ��������� ������ ������
    for (int y = 0; y < height - 1; y++) {
        float  offSetY = y * stepSize;
        for (int x = 0; x < width - 1; x++) {
            float offSetX = x * stepSize;
            makeVertex(offSetX, offSetY, vertices);  // a
            makeVertex(offSetX, offSetY + stepSize, vertices);  // b
            makeVertex(offSetX + stepSize, offSetY, vertices);   // c
            makeVertex(offSetX + stepSize, offSetY, vertices);  //d
            makeVertex(offSetX, offSetY + stepSize, vertices);  //e
            makeVertex(offSetX + stepSize, offSetY + stepSize, vertices);  //f
        }
    }
}

// ����� makeVertex ��������� ������� � ��������� ������������ � ����������� ������������ � ������ vertices
void Terrain::makeVertex(int x, int y, std::vector<float>* vertices) {
    // ��������� ��� ������� ��� �������� ���������
    double pn = perlin.noise(x, y, 0.5);

    // ��������� ���������� xyz � ������ ������
    vertices->push_back((float)x); //xPos
    //vertices->push_back(1.0f); //yPos - always 0 for now. Going to calculate this on GPU - can change to calclaue it here.
    vertices->push_back(pn);
    vertices->push_back((float)y); //zPos

    // ��������� ���������� ���������� uv � ������ ������
    vertices->push_back((float)x / (width * stepSize));
    vertices->push_back((float)y / (height * stepSize));
}

// ����� cycleOctaves ���������� ��� ������� ��� �������� ��������� ���� � �������� ����������� �����.
double Terrain::cycleOctaves(glm::vec3 pos, int numOctaves)
{
    float total = 0.0f;
    float maxAmp = 0.0f;

    float amp = 100.0f;
    float frequency = 0.005f;

    for (int i = 0; i < numOctaves; i++)
    {
        double x = pos.x * frequency;
        double y = pos.y * frequency;
        total += perlin.noise(x, y, 0.01) * amp;
        maxAmp += amp;
        frequency *= 2;
        amp /= 2;
    }
    return (total / maxAmp);
}