/* gfxMathHelper.inl - OpenGL ES Logo Demo */

/*
 * Copyright (c) 2014-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
14sep15,yat  Add support for Mesa GPU DRI (US24710)
01apr15,yat  Resolve defects from static analysis run (US50633)
10mar14,yat  Modified for VxWorks 7 release from demo provided by kka
*/

/* includes */

/* defines */

#define NUM_SIDESINTRIANGLE 3
#define NUM_ELEMENTSININDEX 3
#define NUM_TRIANGLESINEXTRUDEDRECT 2
#define NUM_ELEMENTSINORIGVERTEX 3
#define NUM_ELEMENTSINNEWVERTEX 4
#define NUM_ELEMENTSINVERTEXNORMAL 7
#define VERTICESNORMAL_STRIDE NUM_ELEMENTSINVERTEXNORMAL
#define VERTEXVECTOR4THELEM 1.0

static void mathHelperMultiply
    (
    GLfloat *m,
    const GLfloat *n
    )
    {
    GLfloat tmp[16];
    const GLfloat *row, *column;
    div_t d;
    int i, j;

    for (i = 0; i < 16; i++)
        {
        tmp[i] = 0;
        d = div(i, 4);
        row = n + d.quot * 4;
        column = m + d.rem;
        for (j = 0; j < 4; j++)
            tmp[i] += row[j] * column[j * 4];
        }
    bcopy((char *) &tmp, (char *) m, sizeof(tmp));
    }

/*
 * Calculates a rotation matrix based on angleX and angleY. 
 * m is a 4x4 matrix.
 */
static void mathHelperRotateRollPitch
    (
    GLfloat *m,
    GLfloat radianX,
    GLfloat radianY
    )
    {
    GLfloat sinX, sinY, cosX, cosY;

    sinX = (GLfloat)sin ((double)radianX);
    sinY = (GLfloat)sin ((double)radianY);
    cosX = (GLfloat)cos ((double)radianX);
    cosY = (GLfloat)cos ((double)radianY);

    GLfloat r[16];
    r[0] = cosY;
    r[1] = 0.0;
    r[2] = -sinY;
    r[3] = 0.0;

    r[4] = sinX*sinY;
    r[5] = cosX;
    r[6] = sinX*cosY;
    r[7] = 0.0;

    r[8] = sinY*cosX;
    r[9] = -sinX;
    r[10] = cosX*cosY;
    r[11] = 0.0;

    r[12] = 0.0;
    r[13] = 0.0;
    r[14] = 0.0;
    r[15] = 1.0;

    mathHelperMultiply(m, r);
    }

/*
 * Adds the first, second, and third elements by x, y, and z respectively.
 * Stride is the number of GLfloats in a row.
 */
static void mathHelperTranslateStride
    (
    GLfloat *m,
    int numRows,
    GLubyte stride,
    GLfloat x,
    GLfloat y,
    GLfloat z
    )
    {
    int i;
    for (i = 0; i < numRows; i++)
        {
        m[i * stride] = m[i * stride] + x;
        m[i * stride + 1] = m[i * stride + 1] + y;
        m[i * stride + 2] = m[i * stride + 2] + z;
        }
    }

/*
 * Multiplies the first, second, and third elements by xScale, yScale, and zScale respectively.
 * Stride is the number of GLfloats in a row.
 */
static void mathHelperScaleStride
    (
    GLfloat *m,
    int numRows,
    GLubyte stride,
    GLfloat xScale, 
    GLfloat yScale,
    GLfloat zScale
    )
    {
    int i;
    for (i = 0; i < numRows; i++)
        {
        m[i * stride] = m[i * stride] * xScale;
        m[i * stride + 1] = m[i * stride + 1] * yScale;
        m[i * stride + 2] = m[i * stride + 2] * zScale;
        }
    }

/*
 * Calculates the normalized normal of the triangle formed by vertexXyz*
 * and puts
 * the normal xyz values into normal.
 */
static void mathHelperCalculateNormal
    (
    GLfloat* vertexXyz1,
    GLfloat* vertexXyz2,
    GLfloat* vertexXyz3,
    GLfloat* normal
    )
    {
    GLfloat vector1[3];
    GLfloat vector2[3];
    int i;
    
    /* Calculate the vectors formed by the vertices */
    for (i=0; i<3; i++)
        {
        vector1[i] = vertexXyz3[i] - vertexXyz2[i];
        vector2[i] = vertexXyz3[i] - vertexXyz1[i];    
        }
    
    /* Calculate the cross product. */
    normal[0] =  - vector1[1]*vector2[2] + vector1[2] * vector2[1];
    normal[1] =  - vector1[2]*vector2[0] + vector1[0] * vector2[2];
    normal[2] =  - vector1[0]*vector2[1] + vector1[1] * vector2[0];

    /* Normalize the normal */
    GLfloat normalLengthSquare = 0.0f;
    for (i=0; i<3; i++)
        {
        normalLengthSquare += normal[i]*normal[i];
        }
    GLfloat normalLength = (GLfloat)sqrt((double)normalLengthSquare);
    if (normalLength == 0)
        {
        normalLength = 0.01f;
        }
    mathHelperScaleStride(normal, 1, 3, 1.0f/normalLength, 1.0f/normalLength, 1.0f/normalLength);
    }

/*
 * Creates a z-plane extrusion plane from a line in x-y space.  It adds7 indices
 * (2 triangles) to indicesNew, and 7*3 more vertices elements.  The vertices elements
 * consist of 7 floats:  A triplet of xyz positions, a 1.0 for the w component
 * of an XYZ1 vector, and a triplet of xyz normals.  Adding the 1.0 for all the 4th elements
 * because when transferring into shaders, the 4th element didn't seem to be set to a 1.0 automatically,
 * preventing me from using the 4th column in transformation matrices as a translation element.
 * 
 * Note:  The 4th column was added with a value of 1 because I thought that would
 * provide me with a way to translate the XYZ coordinates with a matrix multiplication.
 * However, that didn't seem to work, so it's a little wasted.
 * 
 * I've extruded the shape, but I haven't had time to put a back face to the shape.  So the shape
 * has a front face, side extruded faces, but no back face.
 * 
 * Parameters:
 *         origVertices - Matrix of the original vertices containing xyz positions only
 *      index1 - The index to one end of the line.  Original 2-d surface is counter-clockwise
 *          going from index1 to index2.
 *      index2 - The index of the other end of the line.  Original 2-d surface is counter-clockwise
 *          going from index1 to index2.
 *      lastVerticesIndex - The new vertex being added will be at this index. 
 *      verticesNew - Pointer to the new vertices array, at the location where values 
 *          are to be written.
 *      indicesNew - Pointer to the new indices array, at the location where values
 *          are to be written.
 *      extrusionLength - Push the line back into the negative z-axis by this amount.
 */
static void mathHelperExtrudeLineWithNormal
    (
    const GLfloat* origVertices,
    GLubyte index1, 
    GLubyte index2, 
    GLuint lastVerticesNumber,
    GLfloat* verticesNew,
    GLubyte* indicesNew,
    GLfloat extrusionLength
    )
    {
    GLubyte* indicesNewAssigner;
    int i;

    /* In counterclockwise order */
    static const GLubyte frontVertexIndexDelta1 = 0;
    static const GLubyte backVertexIndexDelta1 = 1;
    static const GLubyte backVertexIndexDelta2 = 2;
    static const GLubyte frontVertexIndexDelta2 = 3;
    
    GLfloat* origFrontVertexXyz1= (GLfloat*)origVertices + index1*NUM_ELEMENTSINORIGVERTEX;
    GLfloat* origFrontVertexXyz2 = (GLfloat*)origVertices + index2*NUM_ELEMENTSINORIGVERTEX;
    
    /* Add four new vertices. */
    
    /* The front vertices are a copy of the original */
    bcopy((char*)(origFrontVertexXyz1), 
            (char*)(verticesNew + frontVertexIndexDelta1*VERTICESNORMAL_STRIDE), 
            sizeof(GLfloat)*3);
    bcopy((char*)(origFrontVertexXyz2), 
            (char*)(verticesNew + frontVertexIndexDelta2*VERTICESNORMAL_STRIDE), 
            sizeof(GLfloat)*3);
   
    /* The back vertices are a copy of the original but with a modified Z-position. */
    bcopy((char*)(origFrontVertexXyz1), 
            (char*)(verticesNew + backVertexIndexDelta1*VERTICESNORMAL_STRIDE), 
            sizeof(GLfloat)*3);
    *(verticesNew + backVertexIndexDelta1*VERTICESNORMAL_STRIDE + 2) = -extrusionLength;
    bcopy((char*)origFrontVertexXyz2, 
            (char*)(verticesNew + backVertexIndexDelta2*VERTICESNORMAL_STRIDE), 
            sizeof(GLfloat)*3);
    *(verticesNew + backVertexIndexDelta2*VERTICESNORMAL_STRIDE + 2) = -extrusionLength;

    /* Calculate the normalized normal and add it to the normal xyz of verticesNew, which 
     * is the 5th, 6th, and 7th element of each vertex.  */
    mathHelperCalculateNormal(
            verticesNew, 
            verticesNew + VERTICESNORMAL_STRIDE,
            verticesNew + VERTICESNORMAL_STRIDE * 2,
            verticesNew + 4); /* Put in first vertex. */

    *(verticesNew + 3) = VERTEXVECTOR4THELEM; /*Set 4th element of first vertex to 1.0.  This is w. */
    for (i=1; i<4; i++)
        {
        /* Copy the normal value from the first vertex into the other 3 vertices.*/
        bcopy((char*)(verticesNew + 4), 
                (char*)(verticesNew + VERTICESNORMAL_STRIDE*i + 4), 
                3*sizeof(GLfloat));
        /* Set the 4th element to be 1.0, which is the w of an XYZ1 vector. */
        *(verticesNew + VERTICESNORMAL_STRIDE * i + 3) = VERTEXVECTOR4THELEM;
        }

    indicesNewAssigner = indicesNew;
    /* First triangle in counter-clockwise direction. */
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + frontVertexIndexDelta1);
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + backVertexIndexDelta1);
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + backVertexIndexDelta2);

    /* Second triangle. */
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + backVertexIndexDelta2);
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + frontVertexIndexDelta2);
    *(indicesNewAssigner++) = (GLubyte)(lastVerticesNumber + frontVertexIndexDelta1);
}

/*
 *From origVertices matrix containing a 3 x numRows matrix, put the values into newVertices
 * which is a 6 x numRows matrix.  The first 3 elements are xyz elements from origVertices.
 * The last 3 elements of each row is a (0,0,1) vector representing the normal.
 */
static void mathHelperAddNormalZ
    (
    const GLfloat* origVertices,
    GLfloat* newVertices,
    GLuint numRows
    )
    {
    int i, j;

    GLfloat* newVerticesTemp = newVertices;
    GLfloat* origVerticesTemp = (GLfloat*)origVertices;
    for (i=0; i<numRows; i++)
        {
        /* Duplicate the first 3 elements. */
        for (j=0; j<3; j++)
            {
            *(newVerticesTemp++) = *(origVerticesTemp++);
            }
        /* Set the w element. */
        *(newVerticesTemp++) = VERTEXVECTOR4THELEM;
        /* For the last 3 elements, put a (0,0,1) vector.
         * 
         */
        *(newVerticesTemp++) = 0.0f;
        *(newVerticesTemp++) = 0.0f;
        *(newVerticesTemp++) = 1.0f;
        }
    }

/*
 * This function specifically extrudes shapes with angles less than 180 degrees. 
 * (No L-shapes).  It creates a new vertices matrix at verticesNewPtr.  The new 
 * vertices matrix has 6 elements for each vertex: a triplet of xyz position and
 * a triplet of xyz normalized normals.  The number of new vertices
 * is returned in numNewVerticesPtr.  The size of the new vertices matrix would be
 * *numNewVerticesPtr x 6 * sizeof(GLfloat).
 * 
 * It creates a new indices matrix at indicesNewPtr.  
 * 
 * The extrusion does not create a back plane.  It only creates sides.
 * 
 * Parameters:
 *      verticesOrig - The original 2-D vertices matrix with z = 0.
 *      indicesOrig  - The original 2-D indices of each shape.  All shapes
 *                      must be formed by triangles with a point on the first vertex.
 *                      For 1st and last triangle, first and second sides are external sides. 
 *                      For all other triangles, first side only is an external side.
 *      numSides     - An array giving the number of sides for each shape. 
 *      numShapes    - The number of shapes.  numSides must have numShapes elements.
 *      extrusionLength - Extrude into negative z-axis by that much.
 *      verticesNewPtr  - Pointer to the array holding new vertices.
 *      indicesNewPtr - Pointer to the array holding the new indices.
 *      numNewVerticesPtr - Number of new vertices.
 *      indicesNewSizePtr  - Size of the new indices matrix. 
 *                      
  
 * original image.
 */
static int mathHelperExtrudeShape
    (
    const GLfloat *verticesOrig,
    const GLubyte *indicesOrig, 
    const GLuint *numSides,
    const GLuint numShapes, 
    GLfloat extrusionLength, 
    GLfloat **verticesNewPtr,
    GLubyte **indicesNewPtr,
    unsigned int *numNewVerticesPtr,
    unsigned int *indicesNewSizePtr) 
    {
    int i, j; 
    GLuint numOldVertices = 0;
    GLuint numIndices = 0;

    GLfloat* verticesNew;
    GLubyte* indicesNew;

    *numNewVerticesPtr = 0;
    *indicesNewSizePtr = 0;

    for (i=0; i<numShapes; i++)
        {
        numOldVertices += numSides[i];
        numIndices += 3 + 3*(numSides[i]-3);
        }

    /* There are five times as many vertices.  Each vertex is part of 3 planes, and 
     * each vertex is duplicated at the back.  The back vertex is part of 2 planes. */
    *numNewVerticesPtr = numOldVertices*5;
    
    /* The number of triangles added is the (sum of numSides) * 2, since each side extrudes a rectangle into
     * the z-plane.   The sum of numSides is numVertices. 
     */
    *indicesNewSizePtr = ( (numIndices + (3*numOldVertices*2)));
    
    verticesNew = (GLfloat*)calloc ((*numNewVerticesPtr)*NUM_ELEMENTSINVERTEXNORMAL, sizeof(GLfloat));
    if (verticesNew == NULL)
        {
        return 1;
        }

    indicesNew = (GLubyte*)calloc (*indicesNewSizePtr, sizeof(GLubyte));
    if (indicesNew == NULL)
        {
        free (verticesNew);
        return 1;
        }

    *verticesNewPtr = verticesNew;
    *indicesNewPtr = indicesNew;
    
    /* First, augment the original vertices matrix with normal values.  */
    mathHelperAddNormalZ(verticesOrig, verticesNew, numOldVertices);

    /* Create indices for the sides. */
    /* First, duplicate the original set of indices. */
    bcopy ((char*)indicesOrig, (char*)indicesNew, sizeof(GLubyte)*numIndices);
    int twoDTriangleNumber = 0; /* Triangles on the 2-D surface */
    GLuint lastVerticesNumber = numOldVertices;
    
    /* The starting point for new vertices to be added by extrusion. */
    GLfloat* verticesEmptyLocationPtr = verticesNew + numOldVertices * NUM_ELEMENTSINVERTEXNORMAL;
    GLubyte* indicesEmptyLocationPtr = indicesNew + numIndices;

    for (i=0; i<numShapes; i++ )
        {
        int numTriangles = 1 + (numSides[i] - 3);
        for (j=0; j<numTriangles; j++)
            {
            /* Each triangle in the shape is described by 3 indices. 
             * First side is between first and second indices.*/
            /* First side in a shape is always external side, so extrude it by making 2 triangles. */
            int firstIndexInIndicesArrayInTriangle = twoDTriangleNumber*3;  
            mathHelperExtrudeLineWithNormal(verticesOrig, indicesOrig[firstIndexInIndicesArrayInTriangle],
                    indicesOrig[firstIndexInIndicesArrayInTriangle + 1],
                    lastVerticesNumber, 
                    verticesEmptyLocationPtr,
                    indicesEmptyLocationPtr,
                    extrusionLength);

            /* 7 indices and 4 vertices are added with the extrusion of a line. */
            lastVerticesNumber += 4; /* 4 vertices are added. */
            verticesEmptyLocationPtr += 4*NUM_ELEMENTSINVERTEXNORMAL; /*4 vertices, each vertex has 6 elements:  xyz position and xyz normal */
            indicesEmptyLocationPtr += NUM_ELEMENTSININDEX*NUM_TRIANGLESINEXTRUDEDRECT;

            /* For the first and last sides in a shape, also extrude the second side, which is between
             * second and third indices. */
            if (j == 0 || j == numTriangles-1)
                {
                mathHelperExtrudeLineWithNormal(verticesOrig, indicesOrig[firstIndexInIndicesArrayInTriangle + 1],
                        indicesOrig[firstIndexInIndicesArrayInTriangle + 2],
                        lastVerticesNumber, 
                        verticesEmptyLocationPtr,
                        indicesEmptyLocationPtr,
                        extrusionLength);
                /* 6 indices and 4 vertices are added with the extrusion of a line. */
                lastVerticesNumber += 4; /* 4 vertices are added. */
                verticesEmptyLocationPtr += 4*NUM_ELEMENTSINVERTEXNORMAL; /*4 vertices, each vertex has 6 elements:  xyz position and xyz normal */
                indicesEmptyLocationPtr += NUM_ELEMENTSININDEX*NUM_TRIANGLESINEXTRUDEDRECT;
                }
            /* If the shape has only one triangle, then extrude the third side. */
            if (numTriangles == 1)
                {
                mathHelperExtrudeLineWithNormal(verticesOrig, indicesOrig[firstIndexInIndicesArrayInTriangle + 2],
                        indicesOrig[firstIndexInIndicesArrayInTriangle],
                        lastVerticesNumber, 
                        verticesEmptyLocationPtr,
                        indicesEmptyLocationPtr,
                        extrusionLength);
                /* 6 indices and 4 vertices are added with the extrusion of a line. */
                lastVerticesNumber += 4; /* 4 vertices are added. */
                verticesEmptyLocationPtr += 4*NUM_ELEMENTSINVERTEXNORMAL; /*4 vertices, each vertex has 6 elements:  xyz position and xyz normal */
                indicesEmptyLocationPtr += NUM_ELEMENTSININDEX*NUM_TRIANGLESINEXTRUDEDRECT;
                } /* If numTriangles == 1 */
            twoDTriangleNumber++;
            } /* for j = 0 to numTriangles */
        }/* for i = 0 to numShapes */

    return 0;
    }/* mathHelperExtrudeShape */

/*
 * Calculates the perspective matrix formed by a Frustrum and 
 * puts it into projMatrix. The viewed area is a frustrum from
 * z = -nearZ to z=-farZ, with the near plane spanning between
 * x = -nearPlaneHalfWidth to nearPlaneHalfWidth, and y = -nearPlaneHalfHeight
 * to y = nearPlaneHalfHeight.
 * 
 * http://www.songho.ca/opengl/gl_projectionmatrix.html 
 */
static void mathHelperPerspectiveMatrix
    (
    GLfloat* projMatrix, 
    GLfloat nearZ,
    GLfloat farZ,
    GLfloat nearPlaneHalfWidth,
    GLfloat nearPlaneHalfHeight
    )
    {
    bzero((void*)projMatrix, 16 * sizeof(GLfloat));
    
    projMatrix[0] = nearZ / nearPlaneHalfWidth;
    projMatrix[5] = nearZ / nearPlaneHalfHeight;
    projMatrix[10] = -(farZ + nearZ)/(farZ - nearZ);
    projMatrix[14] = -2.0f*farZ*nearZ / (farZ - nearZ);
    projMatrix[11] = -1.0f;
    }

/*
 * Provides a matrix that casts a shadow on the floor at y=-yfloor.
 */
static void mathHelperShadowCastMatrix
    (
    GLfloat* shadowMatrix,
    GLfloat* lightVector,
    GLfloat yfloor
    )
    {
    bzero((void*)shadowMatrix, 16 * sizeof(GLfloat));
    
    shadowMatrix[0] = 1.0f;
    shadowMatrix[1] = -lightVector[0]/lightVector[1];
    shadowMatrix[2] = 0.0f;
    shadowMatrix[3] = yfloor * lightVector[0]/lightVector[1];
    
    shadowMatrix[4] = 0.0f;
    shadowMatrix[5] = 0.0f;
    shadowMatrix[6] = 0.0f;
    shadowMatrix[7] = yfloor;
    
    shadowMatrix[8] = 0.0f;
    shadowMatrix[9] = -lightVector[2] / lightVector[1];
    shadowMatrix[10] = 1.0f;
    shadowMatrix[11] = yfloor * lightVector[2]/lightVector[1];
    
    shadowMatrix[12] = 0.0f;
    shadowMatrix[13] = 0.0f;
    shadowMatrix[14] = 0.0f;
    shadowMatrix[15] = 1.0f;
    }
