// M3DGeometryTransform
/*
Copyright (c) 2009-2023, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __M3D_GEOMETRY_PIPELINE
#define __M3D_GEOMETRY_PIPELINE


#include "M3DMatrixStack.h"

class M3DGeometryTransform
	{
	public:
        M3DGeometryTransform(void) {
                            _mModelView = NULL;
                            _mProjection = NULL;
                                }

        inline void SetModelViewMatrixStack(M3DMatrixStack& mModelView) { _mModelView = &mModelView; }
        inline void SetProjectionMatrixStack(M3DMatrixStack& mProjection) { _mProjection = &mProjection; }

        inline M3DMatrixStack& mvStack(void) { return *_mModelView; }
        inline M3DMatrixStack& pjStack(void) { return *_mProjection; }

        inline void SetMatrixStacks(M3DMatrixStack& mModelView, M3DMatrixStack& mProjection) {
			_mModelView = &mModelView;
			_mProjection = &mProjection;
			}

		const M3DMatrix44f& GetModelViewProjectionMatrix(void)
			{
			m3dMatrixMultiply44(_mModelViewProjection, _mProjection->GetMatrix(), _mModelView->GetMatrix());
			return _mModelViewProjection;
			}

		inline const M3DMatrix44f& GetModelViewMatrix(void) { return _mModelView->GetMatrix(); }
		inline const M3DMatrix44f& GetProjectionMatrix(void) { return _mProjection->GetMatrix(); }

		const M3DMatrix33f& GetNormalMatrix(bool bNormalize = false)
			{
			const M3DMatrix44f& modelView = GetModelViewMatrix();

			// Build the inverse-transpose of the upper-left 3x3 so normals
			// remain correct under non-uniform scale.
			const float a = modelView[0];
			const float d = modelView[1];
			const float g = modelView[2];
			const float b = modelView[4];
			const float e = modelView[5];
			const float h = modelView[6];
			const float c = modelView[8];
			const float f = modelView[9];
			const float i = modelView[10];

			const float c00 = (e * i) - (f * h);
			const float c01 = -((d * i) - (f * g));
			const float c02 = (d * h) - (e * g);
			const float c10 = -((b * i) - (c * h));
			const float c11 = (a * i) - (c * g);
			const float c12 = -((a * h) - (b * g));
			const float c20 = (b * f) - (c * e);
			const float c21 = -((a * f) - (c * d));
			const float c22 = (a * e) - (b * d);

			const float det = (a * c00) + (b * c01) + (c * c02);

			if(m3dCloseEnough(det, 0.0f, 0.000001f))
				m3dExtractRotationMatrix33(_mNormalMatrix, modelView);
			else
				{
				const float invDet = 1.0f / det;
				_mNormalMatrix[0] = c00 * invDet;
				_mNormalMatrix[1] = c10 * invDet;
				_mNormalMatrix[2] = c20 * invDet;
				_mNormalMatrix[3] = c01 * invDet;
				_mNormalMatrix[4] = c11 * invDet;
				_mNormalMatrix[5] = c21 * invDet;
				_mNormalMatrix[6] = c02 * invDet;
				_mNormalMatrix[7] = c12 * invDet;
				_mNormalMatrix[8] = c22 * invDet;
				}

			if(bNormalize) {
				m3dNormalizeVector3(&_mNormalMatrix[0]);
				m3dNormalizeVector3(&_mNormalMatrix[3]);
				m3dNormalizeVector3(&_mNormalMatrix[6]);
				}

			return _mNormalMatrix;
			}

	protected:
		M3DMatrix44f	_mModelViewProjection;
		M3DMatrix33f	_mNormalMatrix;

        M3DMatrixStack*  _mModelView;
        M3DMatrixStack* _mProjection;
};

#endif
