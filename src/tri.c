#include <stdbool.h>
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
    v2_t insideTex[3];
    v2_t outsideTex[3];
    int numInsidePoints = 0;
    int numOutsidePoints = 0;

    bool p0Inside = v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[0]);
    if (p0Inside)
    {
        insidePoints[numInsidePoints] = inTri.p[0];
        insideTex[numInsidePoints] = inTri.t[0];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[0];
        insideTex[numOutsidePoints] = inTri.t[0];
        numOutsidePoints++;
    }

    bool p1Inside = v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[1]);
    if (p1Inside)
    {
        insidePoints[numInsidePoints] = inTri.p[1];
        insideTex[numInsidePoints] = inTri.t[1];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[1];
        insideTex[numOutsidePoints] = inTri.t[1];
        numOutsidePoints++;
    }

    bool p2Inside = v3_isInsideHalfspace(planePoint, planeNormal, inTri.p[2]);
    if (p2Inside)
    {
        insidePoints[numInsidePoints] = inTri.p[2];
        insideTex[numOutsidePoints] = inTri.t[2];
        numInsidePoints++;
    }
    else
    {
        outsidePoints[numOutsidePoints] = inTri.p[2];
        insideTex[numOutsidePoints] = inTri.t[2];
        numOutsidePoints++;
    }

    if (numInsidePoints == 0)
    {
        return 0;
    }

    if (numInsidePoints == 1)
    {
        float t;

        outTris[0].p[0] = insidePoints[0];
        outTris[0].t[0] = insideTex[0];

        t = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[0]);
        outTris[0].p[1] = v3_interpolate(insidePoints[0], outsidePoints[0], t);
        outTris[0].t[1] = v2_interpolate(insideTex[0], outsideTex[0], t);

        t = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[1]);
        outTris[0].p[2] = v3_interpolate(insidePoints[0], outsidePoints[1], t);
        outTris[0].t[2] = v2_interpolate(insideTex[0], outsideTex[1], t);

        if (p1Inside)
        {
            // ensure correct winding and swap points 0 and 2
            v3_t tempP = outTris[0].p[0];
            v2_t tempT = outTris[0].t[0];
            outTris[0].p[0] = outTris[0].p[2];
            outTris[0].t[0] = outTris[0].t[2];
            outTris[0].p[2] = tempP;
            outTris[0].t[2] = tempT;
        }
        return 1;
    }

    if (numInsidePoints == 2)
    {
        float t;

        // tri 1
        outTris[0].p[0] = insidePoints[0];
        outTris[0].t[0] = insideTex[0];

        outTris[0].p[1] = insidePoints[1];
        outTris[0].t[1] = insideTex[1];

        t = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[0], outsidePoints[0]);
        outTris[0].p[2] = v3_interpolate(insidePoints[0], outsidePoints[0], t);
        outTris[0].t[2] = v2_interpolate(insideTex[0], outsideTex[0], t);

        // tri 2
        outTris[1].p[0] = insidePoints[1];
        outTris[1].t[0] = insideTex[1];

        t = v3_getPlaneIntersect(planePoint, planeNormal, insidePoints[1], outsidePoints[0]);
        outTris[1].p[1] = v3_interpolate(insidePoints[1], outsidePoints[0], t);
        outTris[1].t[1] = v2_interpolate(insideTex[1], outsideTex[0], t);

        outTris[1].p[2] = outTris[0].p[2];
        outTris[1].t[2] = outTris[0].t[2];

        if (!p1Inside)
        {
            // ensure correct winding and swap points 0 and 2
            v3_t tempP;
            v2_t tempT;

            tempP = outTris[0].p[0];
            tempT = outTris[0].t[0];
            outTris[0].p[0] = outTris[0].p[2];
            outTris[0].t[0] = outTris[0].t[2];
            outTris[0].p[2] = tempP;
            outTris[0].t[2] = tempT;

            tempP = outTris[1].p[0];
            tempT = outTris[1].t[0];
            outTris[1].p[0] = outTris[1].p[2];
            outTris[1].t[0] = outTris[1].t[2];
            outTris[1].p[2] = tempP;
            outTris[1].t[2] = tempT;
        }
        return 2;
    }

    outTris[0] = inTri;
    return 1;
}