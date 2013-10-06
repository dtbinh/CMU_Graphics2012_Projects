/**
 * @file project.cpp
 * @brief Geometry project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "geometry/project.hpp"
#include "application/opengl.hpp"
#include "application/imageio.hpp"

/*
   A namespace declaration. All project files use this namespace.
   Add this declaration (and its closing) to all source/headers you create.
   Note that all #includes should be BEFORE the namespace declaration.
 */
namespace _462 {

// definitions of functions for the GeometryProject class

// constructor, invoked when object is allocated
GeometryProject::GeometryProject() { }

// destructor, invoked when object is de-allocated
GeometryProject::~GeometryProject() { }

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param mesh The mesh to be rendered and subdivided.
 * @param texture_filename The filename of the texture to use with the mesh.
 *  Is null if there is no texture data with the mesh or no texture filename
 *  was passed in the arguments, in which case textures should not be used.
 * @return true on success, false on error.
 */
bool GeometryProject::initialize( const Camera* camera, const MeshData* mesh, const char* texture_filename )
{
    this->mesh = *mesh;

	// Initialize
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glViewport(0, 0, 1024, 768);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_DEPTH_TEST);

	// Light and material
	GLfloat mat_specular[] = {0.5, 0.5, 0.5, 1};
	GLfloat mat_shininess[] = {1};
	GLfloat light_position[] = {2, 4.5, 5.5, 0.0};
	GLfloat white_light [] = {0.5, 0.5, 0.5, 1};
	GLfloat lmodel_ambient[] = {0.6, 0.6, 0.6, 2.0};
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	
	// Texture
	if(texture_filename)
	{
		glGenTextures(1, &texName);
		glBindTexture(GL_TEXTURE_2D, texName);

		int textureWidth, textureHeight;
		unsigned char* texturePixels = imageio_load_image(texture_filename, &textureWidth, &textureHeight);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturePixels);
		free(texturePixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	

    return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void GeometryProject::destroy()
{
  // TODO any cleanup code
}

/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see scene/camera.hpp
 */
void GeometryProject::render( const Camera* camera )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fov * 180 / PI, camera->aspect, camera->near_clip, camera->far_clip);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3 eye = camera->get_position();
	Vector3 target = camera->get_position() + camera->get_direction();
	gluLookAt(eye.x, eye.y, eye.z, 
		target.x, target.y, target.z, 
		camera->get_up().x, camera->get_up().y, camera->get_up().z);

	RenderMesh();
}

void GeometryProject::RenderMesh()
{
	glColor3f(1, 1, 1);


	GLfloat* vertext_array = new GLfloat[3 * mesh.num_vertices];
	GLfloat* normals_array = new GLfloat[3 * mesh.num_vertices];
	GLfloat* texcoord_array = new GLfloat[2 * mesh.num_vertices];
	for(int i=0; i<mesh.num_vertices; i++)
	{
		vertext_array[i * 3] = mesh.vertices[i].position.x;
		vertext_array[i * 3 + 1] = mesh.vertices[i].position.y;
		vertext_array[i * 3 + 2] = mesh.vertices[i].position.z;

		normals_array[i * 3] = mesh.vertices[i].normal.x;
		normals_array[i * 3 + 1] = mesh.vertices[i].normal.y;
		normals_array[i * 3 + 2] = mesh.vertices[i].normal.z;

		texcoord_array[i * 2] = mesh.vertices[i].texture_coord.x;
		texcoord_array[i * 2 + 1] = mesh.vertices[i].texture_coord.y;
	}

	GLuint* indices_array = new GLuint[3 * mesh.num_triangles];
	for(int i=0; i<mesh.num_triangles; i++)
	{
		indices_array[i * 3] = mesh.triangles[i].vertices[0];
		indices_array[i * 3 + 1] = mesh.triangles[i].vertices[1];
		indices_array[i * 3 + 2] = mesh.triangles[i].vertices[2];
	}


	glPushMatrix();
	
	glVertexPointer(3, GL_FLOAT, 0, vertext_array);
	glNormalPointer(GL_FLOAT, 0, normals_array);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoord_array);

	glDrawElements(GL_TRIANGLES, 3 * mesh.num_triangles, GL_UNSIGNED_INT, indices_array);

	glPopMatrix();
}

/**
 * Subdivide the mesh that we are rendering using Loop subdivison.
 */
void GeometryProject::subdivide()
{
    // TODO perform a single subdivision.
}



} /* _462 */

