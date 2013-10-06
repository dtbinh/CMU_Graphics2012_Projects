/**
* @file project.cpp
* @brief OpenGL project
*
* @author H. Q. Bovik (hqbovik)
* @bug Unimplemented
*/

#include "opengl/project.hpp"

// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

	// definitions of functions for the OpenglProject class

	// constructor, invoked when object is created
	OpenglProject::OpenglProject()
	{
		// TODO any basic construction or initialization of members
		// Warning: Although members' constructors are automatically called,
		// ints, floats, pointers, and classes with empty contructors all
		// will have uninitialized data!
	}

	// destructor, invoked when object is destroyed
	OpenglProject::~OpenglProject()
	{
		// TODO any final cleanup of members
		// Warning: Do not throw exceptions or call virtual functions from deconstructors!
		// They will cause undefined behavior (probably a crash, but perhaps worse).
	}

	/**
	* Initialize the project, doing any necessary opengl initialization.
	* @param camera An already-initialized camera.
	* @param scene The scene to render.
	* @return true on success, false on error.
	*/
	bool OpenglProject::initialize( Camera* camera, Scene* scene )
	{
		// copy scene
		this->scene = *scene;

		// TODO opengl initialization code and precomputation of mesh/heightmap

		// OpenGL initialization
		glClearColor(0, 0, 0, 0);
		glClearDepth(1.0);

		glShadeModel(GL_SMOOTH);

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
		glEnable(GL_NORMALIZE);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, 1024, 768);
		return true;
	}

	/**
	* Clean up the project. Free any memory, etc.
	*/
	void OpenglProject::destroy()
	{
		// TODO any cleanup code, e.g., freeing memory
	}

	/**
	* Perform an update step. This happens on a regular interval.
	* @param dt The time difference from the previous frame to the current.
	*/
	void OpenglProject::update( real_t dt )
	{
		// update our heightmap
		scene.heightmap->update( dt );

	}

	/**
	* Clear the screen, then render the mesh using the given camera.
	* @param camera The logical camera to use.
	* @see math/camera.hpp
	*/
	void OpenglProject::render( const Camera* camera )
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

		RenderHeightMap();
	}


	void OpenglProject::CalculateNormals(Vector3 normals[160][160], int xResolution, int zResolution, float* vertex_array)
	{
		for(int ix = 0; ix < xResolution; ix++)
			for(int iz = 0; iz < zResolution; iz++)
			{
				int count = 0;
				Vector3 total = Vector3(0, 0, 0);

				Vector3 normal;
				if( GetNormalFromIDs(ix - 1, iz -1,ix -1,iz, ix, iz, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				if( GetNormalFromIDs(ix - 1, iz -1,ix,iz, ix, iz-1, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				if( GetNormalFromIDs(ix, iz -1,ix,iz, ix+1, iz, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				if( GetNormalFromIDs(ix-1, iz,ix,iz+1, ix, iz, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				if( GetNormalFromIDs(ix, iz,ix,iz+1, ix+1, iz+1, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				if( GetNormalFromIDs(ix, iz,ix+1,iz+1, ix+1, iz, xResolution, zResolution, normal, vertex_array) )
				{
					total += normal;
					count++;
				}

				normals[ix][iz] = total / count * -1;
			}
	}

	void OpenglProject::RenderHeightMap()
	{
		GLfloat mat_ambient[] = {0.2, 0.2, 1.0, 1};
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_ambient);
		glPushMatrix();

		glTranslatef(scene.heightmap_position.position.x, scene.heightmap_position.position.y,scene.heightmap_position.position.z);
		Vector3 axis; real_t angle;
		scene.heightmap_position.orientation.to_axis_angle(&axis, &angle);
		glRotatef(angle * 180 / PI, axis.x, axis.y, axis.z);
		glScalef(scene.heightmap_position.scale.x, scene.heightmap_position.scale.y, scene.heightmap_position.scale.z);
		
		int xResolution = 160;
		int zResolution = 160;

		float* vertex_array = new float[xResolution * zResolution * 3];
		for(int ix =0; ix<xResolution; ix++)
			for(int iz = 0; iz < zResolution; iz++)
			{
				float fx = ix * 2.0f / xResolution - 1.0f;
				float fz =  iz * 2.0f / zResolution - 1.0f;
				Vector2 vec = Vector2(fx, fz);
				float fy = scene.heightmap->compute_height(vec);
				vertex_array[(ix * zResolution + iz) * 3 ] = fx;
				vertex_array[(ix * zResolution + iz) * 3 + 1] = fy;
				vertex_array[(ix * zResolution + iz) * 3 + 2] = fz;
			}

		Vector3 normals[160][160];
		CalculateNormals(normals, xResolution, zResolution, vertex_array);

		float* normal_array = new float[xResolution * zResolution * 3];
		for(int ix =0; ix<xResolution; ix++)
			for(int iz = 0; iz < zResolution; iz++)
			{
				normal_array[(ix * zResolution + iz) * 3 ] = normals[ix][iz].x;
				normal_array[(ix * zResolution + iz) * 3 + 1] = normals[ix][iz].y;
				normal_array[(ix * zResolution + iz) * 3 + 2] = normals[ix][iz].z;
			}

		

		GLuint* indices_array = new GLuint[(xResolution - 1) * (zResolution - 1) * 3 * 2];

		int index = 0;
		for(int ix = 0; ix < xResolution-1; ix++)
		{
			for(int iz = 0; iz < zResolution-1; iz++)
			{
				indices_array[index++] = ix * zResolution + iz; 
				indices_array[index++] = ix * zResolution + iz + 1;
				indices_array[index++] = (ix+1) * zResolution + iz; 
			}
		}

		for(int ix = 1; ix < xResolution; ix++)
		{	for(int iz = 1; iz < zResolution; iz++)
			{
				indices_array[index++] = ix * zResolution + iz; 
				indices_array[index++] = ix * zResolution + iz - 1;
				indices_array[index++] = (ix-1) * zResolution + iz; 
			}
		}
		glVertexPointer(3, GL_FLOAT, 0, vertex_array);
		glNormalPointer(GL_FLOAT, 0, normal_array);
		glDrawElements(GL_TRIANGLES, 3 * (xResolution - 1) * (zResolution - 1) * 2 , GL_UNSIGNED_INT, indices_array);

		glPopMatrix();
	}

	bool OpenglProject::GetNormalFromIDs(int idx1, int idz1, int idx2, int idz2, int idx3, int idz3,
		int xResolution, int zResolution, Vector3& normal, float* vertex_array)
	{
		if( idx1 >= 0 && idx1 < xResolution
			&& idx2 >= 0 && idx2 < xResolution
			&& idx3 >= 0 && idx3 < xResolution
			&& idz1 >= 0 && idz1 < zResolution
			&& idz2 >= 0 && idz2 < zResolution
			&& idz3 >= 0 && idz3 < zResolution )
		{


			float fx1 = vertex_array[ ( idx1 * zResolution + idz1) * 3];
			float fy1 = vertex_array[ ( idx1 * zResolution + idz1) * 3 + 1];
			float fz1 = vertex_array[ ( idx1 * zResolution + idz1) * 3 + 2];

			float fx2 = vertex_array[ ( idx2 * zResolution + idz2) * 3];
			float fy2 = vertex_array[ ( idx2 * zResolution + idz2) * 3 + 1];
			float fz2 = vertex_array[ ( idx2 * zResolution + idz2) * 3 + 2];

			float fx3 = vertex_array[ ( idx3 * zResolution + idz3) * 3];
			float fy3 = vertex_array[ ( idx3 * zResolution + idz3) * 3 + 1];
			float fz3 = vertex_array[ ( idx3 * zResolution + idz3) * 3 + 2];

			normal = ComputeNormal(Vector3(fx1, fy1, fz1), 
				Vector3(fx2, fy2, fz2),
				Vector3(fx3, fy3, fz3));
			normal = normalize(normal);
			return true;
		}

		return false;
	}

	Vector3 OpenglProject::ComputeNormal(Vector3 v1, Vector3 v2, Vector3 v3)
	{
		Vector3 e1 = v1 - v2;
		Vector3 e2 = v3 - v2;
		Vector3 normal = cross(e1, e2);
		normal = normalize(normal);
		return normal;
	}

	void OpenglProject::RenderMesh()
	{
		GLfloat mat_ambient[] = {1.0, 0.2, 0.2, 1};
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_ambient);
		glPushMatrix();
		glTranslatef(scene.mesh_position.position.x, scene.mesh_position.position.y,scene.mesh_position.position.z);
		Vector3 axis; real_t angle;
		scene.mesh_position.orientation.to_axis_angle(&axis, &angle);
		glRotatef(angle * 180 / PI, axis.x, axis.y, axis.z);
		glScalef(scene.mesh_position.scale.x, scene.mesh_position.scale.y, scene.mesh_position.scale.z);
		
		glBegin(GL_TRIANGLES);
		for(int i=0; i<scene.mesh.num_triangles; i++)
		{
			int id1 = scene.mesh.triangles[i].vertices[0];
			int id2 = scene.mesh.triangles[i].vertices[1];
			int id3 = scene.mesh.triangles[i].vertices[2];

			glColor3f(1.0, 0.5, 0.5);
			glNormal3f(scene.mesh.normals[id1].x, scene.mesh.normals[id1].y,scene.mesh.normals[id1].z);
			glVertex3f(scene.mesh.vertices[id1].x,scene.mesh.vertices[id1].y, scene.mesh.vertices[id1].z); 
			glNormal3f(scene.mesh.normals[id2].x, scene.mesh.normals[id2].y,scene.mesh.normals[id2].z);
			glVertex3f(scene.mesh.vertices[id2].x,scene.mesh.vertices[id2].y, scene.mesh.vertices[id2].z);
			glNormal3f(scene.mesh.normals[id3].x, scene.mesh.normals[id3].y,scene.mesh.normals[id3].z);
			glVertex3f(scene.mesh.vertices[id3].x,scene.mesh.vertices[id3].y, scene.mesh.vertices[id3].z); 
		}
		glEnd();
		glPopMatrix();
	}

	void MeshData::CalculateNormals()
	{
		normals = new Vector3[num_vertices];

		for(int i=0; i<num_vertices; i++)
		{
			normals[i].x = normals[i].y = normals[i].z = 0;
		}

		int* times = new int[num_vertices];
		memset(times, 0, num_vertices);

		for(int i=0; i<num_triangles; i++)
		{
			int id1 = triangles[i].vertices[0];
			int id2 = triangles[i].vertices[1];
			int id3 = triangles[i].vertices[2];

			Vector3 e1 = vertices[id1] - vertices[id2];
			Vector3 e2 = vertices[id3] - vertices[id2];
			Vector3 normal = cross(e1, e2);
			normal = normalize(normal);

			normals[id1] += normal;
			normals[id2] += normal;
			normals[id3] += normal;

			times[id1] += 1;
			times[id2] += 1;
			times[id3] += 1;
		}

		for(int i=0; i<num_vertices; i++)
		{
			if(times[i] != 0)
				normals[i] /= times[i];
		}
	}

} /* _462 */

