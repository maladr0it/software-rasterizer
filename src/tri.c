#include "tri.h"
#include "console.h"

v3_t tri_getNormal(tri_t tri)
{
    v3_t v01 = v3_sub(tri.p[1], tri.p[0]);
    v3_t v02 = v3_sub(tri.p[2], tri.p[0]);
    return v3_normalize(v3_cross(v01, v02));
}

int tri_clipAgainstPlane(tri_t *outTris, v3_t planePoint, v3_t planeNormal, tri_t inTri)
{
    v3_t insidePoints[3];
    v3_t outsidePoints[3];
    int numInsidePoints = 0;
    int numOutsidePoints = 0;

    if (v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[0]))
    {
        insidePoints[numInsidePoints] = inTri.p[0];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[0];
        numOutsidePoints++;
    }

    if (v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[1]))
    {
        insidePoints[numInsidePoints] = inTri.p[1];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[1];
        numOutsidePoints++;
    }

    if (v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[2]))
    {
        insidePoints[numInsidePoints] = inTri.p[2];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[2];
        numOutsidePoints++;
    }

    if (numInsidePoints == 0)
    {
        return 0;
    }

    if (numInsidePoints == 1)
    {
        outTris[0].p[0] = insidePoints[0];
        outTris[0].p[1] = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[0]);
        outTris[0].p[2] = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[1]);
        return 1;
    }

    if (numInsidePoints == 2)
    {
        outTris[0].p[0] = insidePoints[0];
        outTris[0].p[1] = insidePoints[1];
        outTris[0].p[2] = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[0]);

        outTris[1].p[0] = insidePoints[1];
        outTris[1].p[1] = outTris[0].p[2];
        outTris[1].p[2] = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[1], outsidePoints[0]);
        return 2;
    }

    outTris[0] = inTri;
    return 1;
}