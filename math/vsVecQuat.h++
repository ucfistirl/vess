// File vsVecQuat.h++

#ifndef VS_VEC_QUAT_HPP
#define VS_VEC_QUAT_HPP

#include "vsMatrix.h++"
#include "vsQuat.h++"

class vsVecQuat
{
public:

    vsVector    vector;
    vsQuat      quat;

		vsVecQuat();
		~vsVecQuat();

    void	clear();

    vsMatrix	getAsMatrix();
    void	setFromMatrix(vsMatrix theMatrix);
};

#endif
