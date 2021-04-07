///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////// CEYHUN IBOLAR /////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// 	  COMPUTER GRAPHICS  //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Angel.h"

using namespace std;

typedef vec4  color4;
typedef vec4  point4;


float               Starting_Point = 2.5;         // Defining the starting point of the sphere(cube) at the beginning of the animation
float               Position_X = -Starting_Point; // Defining X position of the sphere(cube) at the beginning of the animation
float               Position_Y = Starting_Point;  // Defining Y position of the sphere(cube) at the beginning of the animation
float               Gravity_Force = 0.0005;       // Grafity force which is used to make the ball fall down. This value is choosen with trials. 
const float         Friction_Force = 0.005;
float               Velocity_X = 0.003;
float               Velocity_Y = 0;


// Inýtial Values when the animation starts first

int Model = 0;         // This specifies which model will be initially choosed. In this case, Model = 1 means that the animation will start with sphere. If you change it to 0, it will start with cube.
int Initial_Color = 4; // The initla color is green


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// C U B E - V E R T I C E S ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const int NumVertices = 36;         //(6 faces)(2 triangles/face)(3 vertices/triangle)
point4 points[NumVertices];
color4 colors[NumVertices];

point4 vertices[8] = {              // Vertices of a unit cube centered at origin, sides aligned with axes
    point4(-0.5, -0.5,  0.5, 1.0),
    point4(-0.5,  0.5,  0.5, 1.0),
    point4(0.5,  0.5,  0.5, 1.0),
    point4(0.5, -0.5,  0.5, 1.0),
    point4(-0.5, -0.5, -0.5, 1.0),
    point4(-0.5,  0.5, -0.5, 1.0),
    point4(0.5,  0.5, -0.5, 1.0),
    point4(0.5, -0.5, -0.5, 1.0)
};


color4 vertex_colors[8] = {         // RGBA colors
    color4(1.0, 0.0, 0.0, 1.0),     // red
    color4(0.0, 0.0, 1.0, 1.0),     // blue
    color4(1.0, 1.0, 1.0, 1.0),     // white
    color4(1.0, 0.0, 1.0, 1.0),     // magenta
    color4(0.0, 1.0, 0.0, 1.0),     // green
    color4(0.0, 0.0, 0.0, 1.0),     // black
    color4(0.0, 1.0, 1.0, 1.0),     // cyan
    color4(1.0, 1.0, 0.0, 1.0),     // yellow

};


int Index = 0;
void
quad(int a, int b, int c, int d)
{
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[b]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[d]; Index++;
}
void
colorcube()                         // generate 12 triangles: 36 vertices

{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// S P H E R E - V E R T I C E S //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// This part is taken from append_example_7.cpp from Edward Angel book

const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;

const int NumVerticesSphere = 3 * NumTriangles;

point4 pointSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];

int IndexSphere = 0;
void
triangle(const point4& a, const point4& b, const point4& c)
{
    vec3 normal = normalize(cross(b - a, c - b));
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = a; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = b; IndexSphere++;
    normals[IndexSphere] = normal; pointSphere[IndexSphere] = c; IndexSphere++;
}
point4
unit(const point4& p)
{
    float len = p.x * p.x + p.y * p.y + p.z * p.z;
    point4 t;
    if (len > DivideByZeroTolerance) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}
void
divide_triangle(const point4& a, const point4& b,
    const point4& c, int count)
{
    if (count > 0) {
        point4 v1 = unit(a + b);
        point4 v2 = unit(a + c);
        point4 v3 = unit(b + c);
        divide_triangle(a, v1, v2, count - 1);
        divide_triangle(c, v2, v3, count - 1);
        divide_triangle(b, v3, v1, count - 1);
        divide_triangle(v1, v3, v2, count - 1);
    }
    else {
        triangle(a, b, c);
    }
}

void
tetrahedron(int count)
{
    point4 v[4] = {
        vec4(0.0, 0.0, 1.0, 1.0),
        vec4(0.0, 0.942809, -0.33333, 1.0),
        vec4(-0.816497, -0.471405, -0.33333, 1.0),
        vec4(0.816497, -0.471405, -0.33333, 1.0)
    };
    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
}


enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Xaxis;

GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLuint  ModelView, Projection, ColorShift;
GLuint vao[2];  // since we have 2 different models (cube and sphere), we need to define vao in size 2 => vao[2]
int Model_Selection[2] = { NumVertices,NumVerticesSphere };


void
init()
{
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");   // including the shaders


    colorcube();                            // the function defined for cube
    tetrahedron(NumTimesToSubdivide);       // the function defined for sphere
    glGenVertexArrays(2, vao);              // Defining 2 vertex arrays for Cube and Sphere.

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////// C U B E ////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    glBindVertexArray(vao[0]);      // Create a vertex array object
    GLuint buffer1;                  // Create and initialize a buffer object
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_ARRAY_BUFFER, buffer1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
    GLuint vPosition = glGetAttribLocation(program, "vPosition");     // Associate data with attribute variables in the shader
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////// S P H E R E ////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// -------------------------- Repeat the same for another vertex array object ------------------------------  ///
    glBindVertexArray(vao[1]);
    GLuint buffer2;
    glGenBuffers(1, &buffer2);
    glBindBuffer(GL_ARRAY_BUFFER, buffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointSphere) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pointSphere), pointSphere);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pointSphere), sizeof(normals), normals);
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    /// --------------------------------------------------------------------------------------------------------  ///


    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    Projection = glGetUniformLocation(program, "Projection");
    ColorShift = glGetUniformLocation(program, "ColorShift");

    glUseProgram(program);              // Set current program object

    glEnable(GL_DEPTH_TEST);            // Enable hiddden surface removal          
    glClearColor(1.0, 1.0, 1.0, 1.0);   // white background
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// D I S P L A Y  ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
display(void)
{
    glBindVertexArray(vao[Model]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 model_view;

    vec3 displacement(Position_X, Position_Y, 0.0);

    model_view = (Scale(0.3, 0.3, 0.3) * Translate(displacement) * RotateX(Theta[Xaxis]));

    glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);          // draws vertices
    glUniform4fv(ColorShift, 1, vertex_colors[Initial_Color]);      // sets the color


    glDrawArrays(GL_TRIANGLES, 0, Model_Selection[Model]);
    glutSwapBuffers();
}


void
reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    mat4  projection;        // The projection matrix
    if (w <= h)
        projection = Ortho(-1.0, 1.0, -1.0 * ((GLfloat)h / (GLfloat)w), 1.0 * ((GLfloat)h / (GLfloat)w), -1.0, 1.0);
    else
        projection = Ortho(-1.0 * ((GLfloat)w / (GLfloat)h), 1.0 * ((GLfloat)w / (GLfloat)h), -1.0, 1.0, -1.0, 1.0);
    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
    glDrawArrays(GL_TRIANGLES, 0, Model_Selection[Model]);
    glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// K E Y B O A R D ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
keyboard(unsigned char key, int x, int y)
{
    if (key == 'H' | key == 'h') {  //help menu

        cout << "  Welcome to Bouncing Cube(Sphere) Program" << endl;
        cout << "  Choose the object type(Cube or Sphere), drawing mode (wireframe or solid) and color" << endl;
        cout << " \n " << endl;
        cout << " -Press right mouse button to reach menu" << endl;
        cout << " -Press Q(q) to exit" << endl;
        cout << " -Press I(i) to reset" << endl;
        cout << " -Press H(h) to get help" << endl;
        cout << " \n " << endl;
        cout << " Enjoy it....!" << endl;

    }

    if (key == 'q' | key == 'Q')  // exit if Q is pressed

        exit(0);

    if (key == 'i' | key == 'I') { // reset if I is pressed

        Position_X = -Starting_Point;
        Velocity_X = 0.003;
        Position_Y = Starting_Point;
        Velocity_Y = 0;
        Gravity_Force = 0.0005;
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// A N I MA T I O N - I N P U T S ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// In this part we are controlling the animation inputs. 
// At the end of each timer, the position is updated.
// Velocity in the x axis remains constant., in the y axis decreases.

void
timer(int t)
{
    Position_X += Velocity_X;

    if (Position_Y < -(2 * Starting_Point)) {
        Velocity_Y *= -1;
        Velocity_Y -= Friction_Force;
        if (Velocity_Y < 0) {
            Gravity_Force = 0;
            Velocity_Y = 0;
            Velocity_X = 0;
        }
    }
    Velocity_Y -= Gravity_Force;
    Position_Y += Velocity_Y;

    // for a better look animation we are also rotationg while bouncing
    Theta[Axis] += 2.0;
    if (Theta[Axis] > 360.0) {
        Theta[Axis] -= 360.0;
    }

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////     M E N U    ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
color_menu(int n) {
    Initial_Color = n;
}

void
p_menu(int n) {
    Model = n;
}

void
mode_menu(int n) {
    if (n == 0) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (n == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void
background_menu(int n)
{
    if (n == 0) {
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else if (n == 1) {
        glClearColor(0.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    else if (n == 2) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    else if (n == 3) {
        glClearColor(1.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    else if (n == 4) {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    else if (n == 5) {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    else if (n == 6) {
        glClearColor(0.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);}

    else if (n == 7) {
        glClearColor(1.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);}
}

void
mainMenu(int n) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////   m a i n - f u n c t i o n    ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ceyhun's Window");
    glewExperimental = GL_TRUE;
    glewInit();
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(5, timer, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int oMenu = glutCreateMenu(p_menu);       // creating object menu
    glutAddMenuEntry("Cube", 0);              // adding entries into object menu
    glutAddMenuEntry("Sphere", 1);


    int dMenu = glutCreateMenu(mode_menu);     // creating mode menu
    glutAddMenuEntry("Line Mode", 0);          // adding entries into mode menu

    glutAddMenuEntry("Solid Mode", 1);

    int cMenu = glutCreateMenu(color_menu);   // creating color menu

    glutAddMenuEntry("Red", 0);               // adding entries into color menu
    glutAddMenuEntry("Blue", 1);
    glutAddMenuEntry("White", 2);
    glutAddMenuEntry("Magenta", 3);
    glutAddMenuEntry("Green", 4);
    glutAddMenuEntry("Black", 5);
    glutAddMenuEntry("Cyan", 6);
    glutAddMenuEntry("Yellow", 7);


    int eMenu = glutCreateMenu(background_menu);
    glutAddMenuEntry("Red", 0);               // adding entries into color menu
    glutAddMenuEntry("Blue", 1);
    glutAddMenuEntry("White", 2);
    glutAddMenuEntry("Magenta", 3);
    glutAddMenuEntry("Green", 4);
    glutAddMenuEntry("Black", 5);
    glutAddMenuEntry("Cyan", 6);
    glutAddMenuEntry("Yellow", 7);


    glutCreateMenu(mainMenu);                // creating main menu
    glutAddSubMenu("Object Type", oMenu);
    glutAddSubMenu("Drawing Mode", dMenu);
    glutAddSubMenu("Color Type", cMenu);
    glutAddSubMenu("Background Color", eMenu);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////// T H E - E N D ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
