/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#include "QuadricDecimationMesh.h"

const QuadricDecimationMesh::VisualizationMode QuadricDecimationMesh::QuadricIsoSurfaces =
    NewVisualizationMode("Quadric Iso Surfaces");

void QuadricDecimationMesh::Initialize() {
    // Allocate memory for the quadric array
    size_t numVerts = mVerts.size();
    mQuadrics.reserve(numVerts);
    std::streamsize width = std::cerr.precision();  // store stream precision
    for (size_t i = 0; i < numVerts; i++) {

        // Compute quadric for vertex i here
        mQuadrics.push_back(createQuadricForVert(i));

        // Calculate initial error, should be numerically close to 0

        Vector3<float> v0 = mVerts[i].pos;
        Vector4<float> v(v0[0], v0[1], v0[2], 1);
        Matrix4x4<float> m = mQuadrics.back();

        float error = v * (m * v);
        // std::cerr << std::scientific << std::setprecision(2) << error << " ";
    }
    std::cerr << std::setprecision(width) << std::fixed;  // reset stream precision

    // Run the initialize for the parent class to initialize the edge collapses
    DecimationMesh::Initialize();
}

/*! \lab2 Implement the computeCollapse here */
/*!
 * \param[in,out] collapse The edge collapse object to (re-)compute,
 * DecimationMesh::EdgeCollapse
 */
void QuadricDecimationMesh::computeCollapse(EdgeCollapse *collapse) {
    // Compute collapse->position and collapse->cost here
    // based on the quadrics at the edge endpoints

	// Get the two vertices
	size_t indx1 = e(collapse->halfEdge).vert;
    size_t indx2 = e(e(collapse->halfEdge).pair).vert;

	// Get the quadric for each vertex
    Matrix4x4<float> Q1 = mQuadrics.at(indx1);
    Matrix4x4<float> Q2 = mQuadrics.at(indx2);

	Matrix4x4<float> Q = Q1 + Q2;
    
	/* Create a matrix Qeq from Q (to be used in the equation)
	 * and set the fourth row to [0 0 0 1] */
	Matrix4x4<float> Qeq = Q;
    
	Qeq(3, 0) = 0.0f;
    Qeq(3, 1) = 0.0f;
    Qeq(3, 2) = 0.0f;
    Qeq(3, 3) = 1.0f;



    std::cerr << "computeCollapse in QuadricDecimationMesh not implemented.\n";
}

/*! After each edge collapse the vertex properties need to be updated */
void QuadricDecimationMesh::updateVertexProperties(size_t ind) {
    DecimationMesh::updateVertexProperties(ind);
    mQuadrics[ind] = createQuadricForVert(ind);
}

/*!
 * \param[in] indx vertex index, points into HalfEdgeMesh::mVerts
 */
Matrix4x4<float> QuadricDecimationMesh::createQuadricForVert(size_t indx) const {

    float q[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    Matrix4x4<float> Q(q);

    std::vector<size_t> oneRing = HalfEdgeMesh::FindNeighborVertices(indx);

    for (int i = 0; i < oneRing.size(); i++) {
    
		Q += createQuadricForFace(i);
	}

    // The quadric for a vertex is the sum of all the quadrics for the adjacent
    // faces Tip: Matrix4x4 has an operator +=
    return Q;
}

/*!
 * \param[in] indx face index, points into HalfEdgeMesh::mFaces
 */
Matrix4x4<float> QuadricDecimationMesh::createQuadricForFace(size_t indx) const {

    // Calculate the quadric (outer product of plane parameters) for a face
    // here using the formula from Garland and Heckbert

    // (a,b,c)
    Vector3<float> position = v(e(f(indx).edge).vert).pos;

    // (x,y,z)
    Vector3<float> normal = f(indx).normal;

    /* Distance is calculated with dot product of position and normal,
     * and multiplied with -1.0 according to ax + by + cz = -d	*/
    float distance = (position * normal) * -1.0;

    /*
     *  Q = | (a*a) (a*b) (a*c) (a*d) |
     *		| (b*a) (b*b) (b*c) (b*d) |
     *		| (c*a) (c*b) (c*c) (c*d) |
     *		| (d*d) (a*d) (a*d) (d*d) |
     */
    float q[4][4] = {
        {normal[0] * normal[0], normal[0] * normal[1], normal[0] * normal[2], normal[0] * distance},
        {normal[1] * normal[0], normal[1] * normal[1], normal[1] * normal[2], normal[1] * distance},
        {normal[2] * normal[0], normal[2] * normal[1], normal[2] * normal[2], normal[2] * distance},
        {normal[0] * distance, normal[1] * distance, normal[2] * distance, distance * distance}};

    return Matrix4x4<float>(q);
}

void QuadricDecimationMesh::Render() {
    DecimationMesh::Render();

    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);

    if (mVisualizationMode == QuadricIsoSurfaces) {
        // Apply transform
        glPushMatrix();  // Push modelview matrix onto stack

        // Implement the quadric visualization here
        std::cout << "Quadric visualization not implemented" << std::endl;

        // Restore modelview matrix
        glPopMatrix();
    }
}
